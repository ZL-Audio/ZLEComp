// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_CONTROLLER_H
#define ZLECOMP_CONTROLLER_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp_definitions.h"
#include "Computer/computer.h"
#include "Detector/detector.h"
#include "Detector/rms_tracker.h"
#include "FixedBuffer/fixed_audio_buffer.h"
#include "Meter/meter.h"

namespace zlcontroller {
    template<typename FloatType>
    class Controller {
    public:
        zldetector::Detector<FloatType> lDetector, rDetector;
        zldetector::RMSTracker<FloatType> lTracker, rTracker;
        zlcomputer::Computer<FloatType> lrComputer;
        zlmeter::MeterSource<FloatType> meterIn, meterOut, meterEnd;

        explicit Controller(juce::AudioProcessor &processor,
                            juce::AudioProcessorValueTreeState &parameters);

        ~Controller();

        void prepare(juce::dsp::ProcessSpec spec);

        void reset();

        void process(juce::AudioBuffer<FloatType> &buffer);

        void setOutGain(FloatType v);

        void setSideGain(FloatType v);

        void setMixProportion(FloatType v);

        void setOversampleID(size_t idx, bool useLock = true);

        void toSetOversampleID(size_t idx);

        void setRMSSize(FloatType v, bool useLock = true);

        void toSetRMSSize(FloatType v);

        void setLookAhead(FloatType v);

        void setSegment(FloatType v, bool useLock = true);

        void toSetSegment(FloatType v);

        void setLink(FloatType v);

        void setAudit(bool f);

        void setExternal(bool f);

        void setByPass(bool f);

    private:
        std::array<std::unique_ptr<juce::dsp::Oversampling<FloatType>>, zldsp::overSample::overSampleNUM>
                overSamplers{};
        std::atomic<size_t> idxSampler;

        std::atomic<bool> audit, external, byPass;
        std::atomic<FloatType> link;
        juce::dsp::Gain<FloatType> sideGainDSP, outGainDSP, lGainDSP, rGainDSP;
        juce::dsp::DelayLine<FloatType> mainDelay, dryDelay;
        juce::dsp::DryWetMixer<FloatType> mixer;

        fixedBuffer::FixedAudioBuffer<FloatType> subBuffer;
        std::atomic<FloatType> segment;
        std::atomic<FloatType> rmsSize;

        juce::dsp::ProcessSpec mainSpec = {44100, 512, 2};

        juce::AudioProcessor *m_processor;
        juce::AudioProcessorValueTreeState *apvts;

        juce::AudioBuffer<FloatType> allBuffer, dryBuffer;

        void setLatency();
    };
}

#endif //ZLECOMP_CONTROLLER_H
