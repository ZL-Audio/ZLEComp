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

namespace zlcontroller {
    template<typename FloatType>
    Controller<FloatType>::Controller(juce::AudioProcessor &processor,
                                      juce::AudioProcessorValueTreeState &parameters) {
        mainDelay.setMaximumDelayInSamples(48000);
        mainDelay.setDelay(0);
        m_processor = &processor;
        apvts = &parameters;

        mixer.setWetMixProportion(zldsp::mix::formatV(zldsp::mix::defaultV));
    }

    template<typename FloatType>
    Controller<FloatType>::~Controller() {
        reset();
    }

    template<typename FloatType>
    void Controller<FloatType>::prepare(const juce::dsp::ProcessSpec spec) {
        mainSpec = {spec.sampleRate, spec.maximumBlockSize, spec.numChannels};
        for (size_t i = 0; i < zldsp::overSample::overSampleNUM; ++i) {
            overSamplers[i] = std::make_unique<juce::dsp::Oversampling<FloatType>>(
                    spec.numChannels * 2, i,
                    juce::dsp::Oversampling<FloatType>::filterHalfBandFIREquiripple,
                    true, true);
            overSamplers[i]->initProcessing(spec.maximumBlockSize);
        }
        mixer.prepare(spec);

        mainDelay.prepare(spec);
        sideGainDSP.prepare(spec);
        sideGainDSP.setRampDurationSeconds(0.1);
        outGainDSP.prepare(spec);
        outGainDSP.setRampDurationSeconds(0.1);

        allBuffer.setSize(static_cast<int>(spec.numChannels * 2), static_cast<int>(spec.maximumBlockSize));
        reset();
        setOversampleID(idxSampler.load(), false);
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
        // copy buffer into allBuffer
        allBuffer.makeCopyOf(buffer, true);
        // copy side-chain into sideBuffer
        juce::AudioBuffer<FloatType> sideBuffer(m_processor->getBusBuffer(allBuffer, true, 1));
        if (!external.load()) {
            sideBuffer.makeCopyOf(m_processor->getBusBuffer(buffer, true, 0), true);
        }
        // apply side gain
        auto sideBlock = juce::dsp::AudioBlock<FloatType>(sideBuffer);
        sideGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(sideBlock));
        // check audit mode
        if (audit.load()) {
            m_processor->getBusBuffer(buffer, false, 0).makeCopyOf(sideBuffer, true);
        }
        // apply lookahead
        juce::AudioBuffer<FloatType> mainBuffer(m_processor->getBusBuffer(allBuffer, true, 0));
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
            FloatType lrGain = lGain + rGain;
            // perform stereo link
            lGain = link.load() * rGain + (1 - link.load()) * lGain;
            rGain = lrGain - lGain;
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
        if (!audit.load()) {
            m_processor->getBusBuffer(buffer, false, 0).makeCopyOf(
                    m_processor->getBusBuffer(allBuffer, true, 1), true);
        }
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
    void Controller<FloatType>::setOversampleID(size_t idx, bool useLock) {
        if (useLock) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(m_processor->getCallbackLock());
            toSetOversampleID(idx);
        } else {
            toSetOversampleID(idx);
        }
    }

    template<typename FloatType>
    void Controller<FloatType>::toSetOversampleID(size_t idx) {
        idxSampler.store(idx);
        auto rate = std::pow(2, idx);
        juce::dsp::ProcessSpec spec{mainSpec.sampleRate * rate,
                                    mainSpec.maximumBlockSize * static_cast<juce::uint32>(rate),
                                    mainSpec.numChannels};
        subBuffer.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels * 2});
        setSegment(segment.load(), false);
    }

    template<typename FloatType>
    void Controller<FloatType>::setRMSSize(FloatType v, bool useLock) {
        if (useLock) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(m_processor->getCallbackLock());
            toSetRMSSize(v);
        } else {
            toSetRMSSize(v);
        }
    }

    template<typename FloatType>
    void Controller<FloatType>::toSetRMSSize(FloatType v) {
        rmsSize.store(v);
        auto mSize = static_cast<size_t>(subBuffer.getSubSpec().sampleRate * v /
                                         subBuffer.getSubSpec().maximumBlockSize);
        mSize = juce::jmax(size_t(1), mSize);
        lTracker.setMomentarySize(mSize);
        rTracker.setMomentarySize(mSize);
    }

    template<typename FloatType>
    void Controller<FloatType>::setLookAhead(FloatType v) {
        mainDelay.setDelay(static_cast<float>(static_cast<int>(v * mainSpec.sampleRate)));
        setLatency();
    }

    template<typename FloatType>
    void Controller<FloatType>::setSegment(FloatType v, bool useLock) {
        if (useLock) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(m_processor->getCallbackLock());
            toSetSegment(v);
        } else {
            toSetSegment(v);
        }
    }

    template<typename FloatType>
    void Controller<FloatType>::toSetSegment(FloatType v) {
        segment.store(v);
        subBuffer.setSubBufferSize(juce::jmax(1, static_cast<int>(v * subBuffer.getMainSpec().sampleRate)));

        juce::dsp::ProcessSpec spec = subBuffer.getSubSpec();
        lTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 4});
        rTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 4});
        lDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 4});
        rDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 4});
        lGainDSP.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 4});
        rGainDSP.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 4});

        auto rampSeconds = double(spec.maximumBlockSize - 1) / 4 / spec.sampleRate;
        lGainDSP.setRampDurationSeconds(rampSeconds);
        rGainDSP.setRampDurationSeconds(rampSeconds);

        setRMSSize(rmsSize.load(), false);
        setLatency();
    }

    template<typename FloatType>
    void Controller<FloatType>::setLink(FloatType v) {
        link.store(v);
    }

    template<typename FloatType>
    void Controller<FloatType>::setAudit(bool f) {
        const juce::GenericScopedLock<juce::CriticalSection> scopedLock(m_processor->getCallbackLock());
        audit.store(f);
        setLatency();
    }

    template<typename FloatType>
    void Controller<FloatType>::setExternal(bool f) {
        external.store(f);
    }

    template<typename FloatType>
    void Controller<FloatType>::setLatency() {
        if (audit.load()) {
            m_processor->setLatencySamples(0);
        } else if (overSamplers[idxSampler.load()]) {
            m_processor->setLatencySamples(
                    static_cast<int>(mainDelay.getDelay() + overSamplers[idxSampler.load()]->getLatencyInSamples()) +
                    static_cast<int>(subBuffer.getLatencySamples() / std::pow(2, idxSampler.load())));
        }
    }

    template
    class Controller<float>;

    template
    class Controller<double>;
}