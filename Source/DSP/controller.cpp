// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "controller.h"

namespace controller {
    template<typename FloatType>
    Controller<FloatType>::Controller(juce::AudioProcessor &processor,
                                      juce::AudioProcessorValueTreeState &parameters) {
        mainDelay.setDelay(0);
        m_processor = &processor;
        apvts = &parameters;

        mixer.setWetMixProportion(ZLDsp::mix::formatV(ZLDsp::mix::defaultV));
    }

    template<typename FloatType>
    Controller<FloatType>::~Controller() {
        reset();
    }

    template<typename FloatType>
    void Controller<FloatType>::prepare(const juce::dsp::ProcessSpec spec) {
        mainSpec = {spec.sampleRate, spec.maximumBlockSize, spec.numChannels};
        for (size_t i = 0; i < ZLDsp::overSample::overSampleNUM; ++i) {
            overSamplers[i] = std::make_unique<juce::dsp::Oversampling<FloatType>>(
                    spec.numChannels * 2, i,
                    juce::dsp::Oversampling<FloatType>::filterHalfBandFIREquiripple,
                    true, true);
            overSamplers[i]->initProcessing(spec.maximumBlockSize);
        }

        mainDelay.prepare(spec);
        sideGainDSP.prepare(spec);
        sideGainDSP.setRampDurationSeconds(0.1);
        outGainDSP.prepare(spec);
        outGainDSP.setRampDurationSeconds(0.1);

        allBuffer.setSize(static_cast<int>(spec.numChannels * 2), static_cast<int>(spec.maximumBlockSize));
        subBuffer.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels * 2});
        lTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        rTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        lDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        rDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
    }

    template<typename FloatType>
    void Controller<FloatType>::reset() {
        subBuffer.clear();
        lDetector.reset();
        rDetector.reset();
        lTracker.reset();
        rTracker.reset();
    }

    template<typename FloatType>
    void Controller<FloatType>::process(juce::AudioBuffer<FloatType> &buffer) {
        // prepare
        if (idxSamplerToReset.load()) {
            setOversampleID(tempIdxSampler.load());
            idxSamplerToReset.store(false);
        }
        if (subBufferSizeToReset.load()) {
            setSegment(subBufferSize.load());
            subBufferSizeToReset.store(false);
        }
        // copy buffer into allBuffer
        allBuffer.makeCopyOf(buffer, true);
        // copy side-chain into sideBuffer
        juce::AudioBuffer<FloatType> sideBuffer(m_processor->getBusBuffer(allBuffer, false, 1));
        if (external.load()) {
            sideBuffer.makeCopyOf(m_processor->getBusBuffer(buffer, false, 1), true);
        } else {
            sideBuffer.makeCopyOf(m_processor->getBusBuffer(buffer, false, 0), true);
        }
        // apply side gain
        auto sideBlock = juce::dsp::AudioBlock<FloatType>(sideBuffer);
        sideGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(sideBlock));
        // check audit mode
        if (audit.load()) {
            m_processor->getBusBuffer(buffer, false, 0).makeCopyOf(sideBuffer, true);
            return;
        }
        // apply lookahead
        juce::AudioBuffer<FloatType> mainBuffer(m_processor->getBusBuffer(buffer, false, 0));
        auto mainBlock = juce::dsp::AudioBlock<FloatType>(mainBuffer);
        mainDelay.process(juce::dsp::ProcessContextReplacing<FloatType>(mainBlock));
        // add dry samples
        mixer.pushDrySamples(mainBlock);
        // apply over-sampling(up)
        auto allBlock = juce::dsp::AudioBlock<FloatType>(allBuffer);
        auto overSampledBuffer = overSamplers[idxSampler]->processSamplesUp(allBlock);
        // ---------------- start sub buffer
        subBuffer.pushBlock(overSampledBuffer);
        while (subBuffer.isSubReady()) {
            subBuffer.popSubBuffer();
            // calculate rms value
            lTracker.process(subBuffer.getSubBufferChannels(2, 1));
            rTracker.process(subBuffer.getSubBufferChannels(3, 1));
            // compute current level
            FloatType lGain = lTracker.getMomentaryLoudness();
            FloatType rGain = rTracker.getMomentaryLoudness();
            // perform stereo link
            lGain = link.load() * rGain + (1 - link.load()) * lGain;
            rGain = link.load() * lGain + (1 - link.load()) * rGain;
            // compute current gain
            lGain = lrComputer.process(lGain);
            rGain = lrComputer.process(rGain);
            // attack/release current gain
            lGain = lDetector.process(lGain);
            rGain = rDetector.process(rGain);
            // apply gain separately
            lGainDSP.setGainLinear(lGain);
            rGainDSP.setGainLinear(rGain);
            auto lSubBlock = subBuffer.getSubBlockChannels(0, 1);
            lGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(lSubBlock));
            auto rSubBlock = subBuffer.getSubBlockChannels(1, 1);
            rGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(rSubBlock));
            subBuffer.pushSubBuffer();
        }
        subBuffer.popBlock(overSampledBuffer);
        // ---------------- end sub buffer
        // apply over-sampling(down)
        overSamplers[idxSampler]->processSamplesDown(allBlock);
        // mix wet samples
        mixer.mixWetSamples(allBlock.getSubsetChannelBlock(0, 2));
        // apply out gain
        juce::AudioBuffer<FloatType> outBuffer(m_processor->getBusBuffer(allBuffer, false, 0));
        auto outBlock = juce::dsp::AudioBlock<FloatType>(outBuffer);
        outGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(outBlock));
    }

    template<typename FloatType>
    void Controller<FloatType>::setOutGain(FloatType v) {
        outGainDSP.setGainDecibels(v);
    }

    template<typename FloatType>
    void Controller<FloatType>::setSideGain(FloatType v) {
        sideGainDSP.setGainDecibels(v);
    }

    template<typename FloatType>
    void Controller<FloatType>::setMixProportion(FloatType v) {
        mixer.setWetMixProportion(v);
    }

    template<typename FloatType>
    void Controller<FloatType>::setOversampleID(size_t idx) {
        idxSampler.store(idx);
    }

    template<typename FloatType>
    void Controller<FloatType>::toSetOversampleID(size_t idx) {
        tempIdxSampler.store(idx);
        idxSamplerToReset.store(true);
    }

    template<typename FloatType>
    void Controller<FloatType>::setRMSSize(FloatType v) {
        auto mSize = static_cast<size_t>(subBuffer.getSubSpec().sampleRate * v / subBuffer.getSubSpec().maximumBlockSize);
        lTracker.setMomentarySize(mSize);
        rTracker.setMomentarySize(mSize);
    }

    template<typename FloatType>
    void Controller<FloatType>::setLookAhead(FloatType v) {
        mainDelay.setDelay(static_cast<FloatType>(v * mainSpec.sampleRate));
    }

    template<typename FloatType>
    void Controller<FloatType>::setSegment(FloatType v) {
        subBuffer.setSubBufferSize(static_cast<int>(v * mainSpec.sampleRate));
    }

    template<typename FloatType>
    void Controller<FloatType>::toSetSegment(FloatType v) {
        subBufferSize.store(v);
        subBufferSizeToReset.store(true);
    }

    template<typename FloatType>
    void Controller<FloatType>::setLink(FloatType v) {
        link.store(v);
    }

    template<typename FloatType>
    void Controller<FloatType>::setAudit(bool f) {
        audit.store(f);
    }

    template<typename FloatType>
    void Controller<FloatType>::setExternal(bool f) {
        external.store(f);
    }

    template
    class Controller<float>;

    template
    class Controller<double>;
}

namespace controller {
    template<typename FloatType>
    ControllerAttach<FloatType>::ControllerAttach(Controller<FloatType> &c,
                                                  juce::AudioProcessorValueTreeState &parameters) {
        controller = &c;
        apvts = &parameters;
    }

    template<typename FloatType>
    ControllerAttach<FloatType>::~ControllerAttach() {
        for (auto &ID: IDs) {
            apvts->removeParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::initDefaultVs() {
        for (size_t i = 0; i < IDs.size(); ++i) {
            parameterChanged(IDs[i], defaultVs[i]);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<FloatType>(newValue);
        if (parameterID == ZLDsp::outGain::ID) {
            controller->setOutGain(v);
        } else if (parameterID == ZLDsp::mix::ID) {
            controller->setMixProportion(ZLDsp::mix::formatV(v));
        } else if (parameterID == ZLDsp::overSample::ID) {
            controller->toSetOversampleID(static_cast<size_t>(v));
        } else if (parameterID == ZLDsp::rms::ID) {
            controller->setRMSSize(ZLDsp::rms::formatV(v));
        } else if (parameterID == ZLDsp::lookahead::ID) {
            controller->setLookAhead(ZLDsp::segment::formatV(v));
        } else if (parameterID == ZLDsp::segment::ID) {
            controller->toSetSegment(ZLDsp::segment::formatV(v));
        } else if (parameterID == ZLDsp::audit::ID) {
            controller->setAudit(static_cast<bool>(v));
        } else if (parameterID == ZLDsp::external::ID) {
            controller->setExternal(static_cast<bool>(v));
        }
    }

    template
    class ControllerAttach<float>;

    template
    class ControllerAttach<double>;
}

namespace controller {
    template<typename FloatType>
    DetectorAttach<FloatType>::DetectorAttach(Controller<FloatType> &c,
                                                  juce::AudioProcessorValueTreeState &parameters) {
        controller = &c;
        apvts = &parameters;
    }

    template<typename FloatType>
    DetectorAttach<FloatType>::~DetectorAttach() {
        for (auto &ID: IDs) {
            apvts->removeParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::initDefaultVs() {
        for (size_t i = 0; i < IDs.size(); ++i) {
            parameterChanged(IDs[i], defaultVs[i]);
        }
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<FloatType>(newValue);
        if (parameterID == ZLDsp::sideGain::ID) {
            controller->setSideGain(v);
        } else if (parameterID == ZLDsp::attack::ID) {
            controller->lDetector.setAttack(ZLDsp::attack::formatV(v));
            controller->rDetector.setAttack(ZLDsp::attack::formatV(v));
        } else if (parameterID == ZLDsp::release::ID) {
            controller->lDetector.setRelease(ZLDsp::release::formatV(v));
            controller->rDetector.setRelease(ZLDsp::release::formatV(v));
        } else if (parameterID == ZLDsp::aStyle::ID) {
            auto idx = static_cast<size_t>(v);
            controller->lDetector.setAStyle(idx);
            controller->rDetector.setAStyle(idx);
        } else if (parameterID == ZLDsp::rStyle::ID) {
            auto idx = static_cast<size_t>(v);
            controller->lDetector.setRStyle(idx);
            controller->rDetector.setRStyle(idx);
        } else if (parameterID == ZLDsp::smooth::ID) {
            controller->lDetector.setSmooth(v);
            controller->rDetector.setSmooth(v);
        } else if (parameterID == ZLDsp::link::ID) {
            controller->setLink(v);
        }
    }

    template
    class DetectorAttach<float>;

    template
    class DetectorAttach<double>;
}

namespace controller {
    template<typename FloatType>
    ComputerAttach<FloatType>::ComputerAttach(Controller<FloatType> &c,
                                              juce::AudioProcessorValueTreeState &parameters) {
        controller = &c;
        apvts = &parameters;
    }

    template<typename FloatType>
    ComputerAttach<FloatType>::~ComputerAttach() {
        for (auto &ID: IDs) {
            apvts->removeParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::initDefaultVs() {
        for (size_t i = 0; i < IDs.size(); ++i) {
            parameterChanged(IDs[i], defaultVs[i]);
        }
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<FloatType>(newValue);
        if (parameterID == ZLDsp::threshold::ID) {
            controller->lrComputer.setThreshold(v);
        } else if (parameterID == ZLDsp::ratio::ID) {
            controller->lrComputer.setRatio(v);
        } else if (parameterID == ZLDsp::kneeW::ID) {
            controller->lrComputer.setKneeW(ZLDsp::kneeW::formatV(v));
        } else if (parameterID == ZLDsp::kneeS::ID) {
            controller->lrComputer.setKneeS(v);
        } else if (parameterID == ZLDsp::kneeD::ID) {
            controller->lrComputer.setKneeD(v);
        } else if (parameterID == ZLDsp::bound::ID) {
            controller->lrComputer.setBound(v);
        }
    }

    template
    class ComputerAttach<float>;

    template
    class ComputerAttach<double>;
}