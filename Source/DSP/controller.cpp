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
        outGainDSP.prepare(spec);

        allBuffer.setSize(static_cast<int>(spec.numChannels * 2), static_cast<int>(spec.maximumBlockSize));
        subBuffer.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels * 2});
        lTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        rTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        lDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        rDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
    }

    template<typename FloatType>
    void Controller<FloatType>::reset() {
        lDetector.reset();
        rDetector.reset();
        lTracker.reset();
        rTracker.reset();
    }

    template<typename FloatType>
    void Controller<FloatType>::process(juce::AudioBuffer<FloatType> &buffer) {
        // copy data into sideBuffer
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
        // apply out gain
        juce::AudioBuffer<FloatType> outBuffer(m_processor->getBusBuffer(allBuffer, false, 0));
        auto outBlock = juce::dsp::AudioBlock<FloatType>(outBuffer);
        outGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(outBlock));
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
    void ControllerAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {

    }

    template
    class ControllerAttach<float>;

    template
    class ControllerAttach<double>;
} // controller