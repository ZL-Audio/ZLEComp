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
                                      juce::AudioProcessorValueTreeState &parameters) :
            meterIn(processor), meterOut(processor), meterEnd(processor) {
        m_processor = &processor;
        apvts = &parameters;
        mainDelay.setMaximumDelayInSamples(96000);
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
        mainDelay.setMaximumDelayInSamples(static_cast<int>(spec.sampleRate));
        dryDelay.prepare(spec);
        dryDelay.setMaximumDelayInSamples(static_cast<int>(spec.sampleRate));
        sideGainDSP.prepare(spec);
        sideGainDSP.setRampDurationSeconds(0.1);
        outGainDSP.prepare(spec);
        outGainDSP.setRampDurationSeconds(0.1);
        meterIn.prepare(spec);
        meterOut.prepare(spec);
        meterEnd.prepare(spec);

        allBuffer.setSize(static_cast<int>(spec.numChannels * 2), static_cast<int>(spec.maximumBlockSize));
        dryBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
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

        // apply lookahead
        juce::AudioBuffer<FloatType> mainBuffer(m_processor->getBusBuffer(allBuffer, true, 0));
        auto mainBlock = juce::dsp::AudioBlock<FloatType>(mainBuffer);
        mainDelay.process(juce::dsp::ProcessContextReplacing<FloatType>(mainBlock));
        // add dry samples
        dryBuffer.makeCopyOf(mainBuffer, true);
        auto dryBlock = juce::dsp::AudioBlock<FloatType>(dryBuffer);
        dryDelay.process(juce::dsp::ProcessContextReplacing<FloatType>(dryBlock));
        meterIn.process(dryBlock);
        mixer.pushDrySamples(dryBlock);
        // apply over-sampling(up)
        auto allBlock = juce::dsp::AudioBlock<FloatType>(allBuffer);
        auto overSampledBlock = overSamplers[idxSampler]->processSamplesUp(allBlock);
        // ---------------- start sub buffer
        subBuffer.pushBlock(overSampledBlock);
        while (subBuffer.isSubReady()) {
            subBuffer.popSubBuffer();
            switch (structureStyle.load()) {
                case zldsp::sStyle::smooth:
                    smoothStyleProcess();
                    break;
                case zldsp::sStyle::clean:
                    cleanStyleProcess();
                    break;
            }
            subBuffer.pushSubBuffer();
        }
        subBuffer.popBlock(overSampledBlock);
        // ---------------- end sub buffer
        // apply over-sampling(down)
        overSamplers[idxSampler]->processSamplesDown(allBlock);
        juce::AudioBuffer<FloatType> outBuffer(m_processor->getBusBuffer(allBuffer, true, 0));
        auto outBlock = juce::dsp::AudioBlock<FloatType>(outBuffer);
        // mix wet samples
        meterOut.process(outBlock);
        mixer.mixWetSamples(outBlock);
        // apply out gain
        if (!byPass.load()) {
            outGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(outBlock));
        }
        meterEnd.process(outBlock);
        // check audit mode
        if (audit.load()) {
            m_processor->getBusBuffer(buffer, false, 0).makeCopyOf(
                    m_processor->getBusBuffer(allBuffer, true, 1), true);
        } else {
            m_processor->getBusBuffer(buffer, false, 0).makeCopyOf(
                    m_processor->getBusBuffer(allBuffer, true, 0), true);
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
        lTracker.reset();
        rTracker.reset();
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
        spec.numChannels = spec.numChannels / 2;
        spec.numChannels = spec.numChannels / 2;
        lTracker.prepare(spec);
        rTracker.prepare(spec);
        lDetector.prepare(spec);
        rDetector.prepare(spec);
        lGainDSP.prepare(spec);
        rGainDSP.prepare(spec);

        auto rampSeconds = double(spec.maximumBlockSize) / 4 / spec.sampleRate;
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
    void Controller<FloatType>::setByPass(bool f) {
        byPass.store(f);
    }

    template<typename FloatType>
    void Controller<FloatType>::setLatency() {
        if (!overSamplers[idxSampler.load()]) {
            return;
        }
        if (audit.load()) {
            m_processor->setLatencySamples(
                    static_cast<int>(overSamplers[idxSampler.load()]->getLatencyInSamples()) +
                    static_cast<int>(subBuffer.getLatencySamples() / std::pow(2, idxSampler.load())));
        } else {
            m_processor->setLatencySamples(
                    static_cast<int>(mainDelay.getDelay() + overSamplers[idxSampler.load()]->getLatencyInSamples()) +
                    static_cast<int>(subBuffer.getLatencySamples() / std::pow(2, idxSampler.load())));
        }
        dryDelay.setDelay(static_cast<float>(overSamplers[idxSampler.load()]->getLatencyInSamples()) +
                          static_cast<float>(subBuffer.getLatencySamples() / std::pow(2, idxSampler.load())));
    }

    template<typename FloatType>
    void Controller<FloatType>::setStructureStyleID(size_t idx) {
        const juce::GenericScopedLock<juce::CriticalSection> processLock(m_processor->getCallbackLock());
        structureStyle.store(idx);
        lDetector.reset();
        rDetector.reset();
        lGainDSP.reset();
        rGainDSP.reset();
        if (idx == zldsp::sStyle::smooth) {
            lDetector.setPhase(zldetector::Detector<FloatType>::gain);
            rDetector.setPhase(zldetector::Detector<FloatType>::gain);
        } else {
            lDetector.setPhase(zldetector::Detector<FloatType>::level);
            rDetector.setPhase(zldetector::Detector<FloatType>::level);
        }
    }

    template<typename FloatType>
    void Controller<FloatType>::smoothStyleProcess() {
        // calculate rms value
        lTracker.process(subBuffer.getSubBufferChannels(2, 1));
        rTracker.process(subBuffer.getSubBufferChannels(3, 1));
        // compute current loudness level
        FloatType l = lTracker.getMomentaryLoudness();
        FloatType r = rTracker.getMomentaryLoudness();
        FloatType lr = l + r;
        // perform stereo link
        l = link.load() * r + (1 - link.load()) * l;
        r = lr - l;
        // compute current gain
        l = lrComputer.process(l);
        r = lrComputer.process(r);
        // attack/release current gain
        l = lDetector.process(l);
        r = rDetector.process(r);
        // apply gain separately
        if (!byPass.load()) {
            lGainDSP.setGainLinear(l);
            rGainDSP.setGainLinear(r);
            auto lSubBlock = subBuffer.getSubBlockChannels(0, 1);
            lGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(lSubBlock));
            auto rSubBlock = subBuffer.getSubBlockChannels(1, 1);
            rGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(rSubBlock));
        }
    }

    template<typename FloatType>
    void Controller<FloatType>::cleanStyleProcess() {
        // calculate rms value
        lTracker.process(subBuffer.getSubBufferChannels(2, 1));
        rTracker.process(subBuffer.getSubBufferChannels(3, 1));
        // compute current loudness level
        FloatType l = lTracker.getMomentaryLoudness();
        FloatType r = rTracker.getMomentaryLoudness();
        // convert gain to linear domain
        l = juce::Decibels::decibelsToGain(l);
        r = juce::Decibels::decibelsToGain(r);
        // attack/release current gain
        l = lDetector.process(l);
        r = rDetector.process(r);
        // convert gain to db domain
        l = juce::Decibels::gainToDecibels(l);
        r = juce::Decibels::gainToDecibels(r);
        FloatType lr = l + r;
        // perform stereo link
        l = link.load() * r + (1 - link.load()) * l;
        r = lr - l;
        // compute current gain
        l = lrComputer.process(l);
        r = lrComputer.process(r);
        // apply gain separately
        if (!byPass.load()) {
            lGainDSP.setGainLinear(l);
            rGainDSP.setGainLinear(r);
            auto lSubBlock = subBuffer.getSubBlockChannels(0, 1);
            lGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(lSubBlock));
            auto rSubBlock = subBuffer.getSubBlockChannels(1, 1);
            rGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(rSubBlock));
        }
    }

    template
    class Controller<float>;

    template
    class Controller<double>;
}