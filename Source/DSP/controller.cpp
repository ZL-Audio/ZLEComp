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

        allBuffer.setSize(spec.numChannels * 2, spec.maximumBlockSize);
        subBuffer.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels * 2});
        lTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        rTracker.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        lDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
        rDetector.prepare({spec.sampleRate, spec.maximumBlockSize, spec.numChannels / 2});
    }

    template<typename FloatType>
    void Controller<FloatType>::process(const juce::AudioBuffer<FloatType> &buffer) {
        // copy data into sideBuffer
        juce::AudioBuffer<FloatType> sideBuffer(m_processor->getBusBuffer(allBuffer, false, 1));
        if (external.load()) {
            sideBuffer.makeCopyOf(m_processor->getBusBuffer(buffer, false, 1), true);
        } else {
            sideBuffer.makeCopyOf(m_processor->getBusBuffer(buffer, false, 0), true);
        }
        // apply side gain
        sideGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(sideBuffer));
        // check audit mode
        if (audit.load()) {
            m_processor->getBusBuffer(buffer, false, 0).makeCopyOf(sideBuffer, true);
            return;
        }
        // apply over-sampling(up)
        auto overSampledBuffer = overSamplers[idxSampler]->processSamplesUp(juce::dsp::AudioBlock<FloatType>(allBuffer));
        // ---------------- start sub buffer
        subBuffer.pushBuffer(overSampledBuffer);
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
            lGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(subBuffer.getSubBufferChannels(0, 1)));
            rGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(subBuffer.getSubBufferChannels(1, 1)));
            subBuffer.pushSubBuffer();
        }
        subBuffer.popBuffer(overSampledBuffer);
        // ---------------- end sub buffer
        // apply over-sampling(down)
        overSamplers[idxSampler]->processSamplesDown(juce::dsp::AudioBlock<FloatType>(allBuffer));
        // apply out gain
        juce::AudioBuffer<FloatType> outBuffer(m_processor->getBusBuffer(allBuffer, false, 0));
        outGainDSP.process(juce::dsp::ProcessContextReplacing<FloatType>(outBuffer));
    }
} // controller