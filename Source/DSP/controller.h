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
#include "dsp_defines.h"
#include "Computer/computer.h"
#include "Detector/detector.h"
#include "Detector/rms_tracker.h"
#include "FixedBuffer/fixed_audio_buffer.h"

namespace controller {

    template<typename FloatType>
    class Controller {
    public:
        explicit Controller(juce::AudioProcessor &processor,
                            juce::AudioProcessorValueTreeState &parameters);

        ~Controller();

        void prepare(juce::dsp::ProcessSpec spec);

        void reset();

        void process(juce::AudioBuffer<FloatType> &buffer);

        void setOutGain(FloatType v);

        void setMixProportion(FloatType v);

        void setOversampleID(size_t idx);

        void toSetOversampleID(size_t idx);

        void setRMSSize(FloatType v);

        void setLookAhead(FloatType v);

        void setSegment(FloatType v);

        void toSetSegment(FloatType v);

        void setLink(FloatType v);

        void setAudit(bool f);

        void setExternal(bool f);

    private:
        detector::Detector<FloatType> lDetector, rDetector;
        detector::RMSTracker<FloatType> lTracker, rTracker;
        computer::Computer<FloatType> lrComputer;

        std::array<std::unique_ptr<juce::dsp::Oversampling<FloatType>>, ZLDsp::overSample::overSampleNUM>
                overSamplers{};
        std::atomic<size_t> idxSampler, tempIdxSampler;
        std::atomic<bool> idxSamplerToReset = false;

        std::atomic<bool> audit, external;
        std::atomic<FloatType> link;
        juce::dsp::Gain<FloatType> sideGainDSP, outGainDSP, lGainDSP, rGainDSP;
        juce::dsp::DelayLine<FloatType> mainDelay;
        juce::dsp::DryWetMixer<FloatType> mixer;

        fixedBuffer::FixedAudioBuffer<FloatType> subBuffer;
        std::atomic<FloatType> subBufferSize;
        std::atomic<bool> subBufferSizeToReset = false;

        juce::dsp::ProcessSpec mainSpec = {44100, 512, 2};

        juce::AudioProcessor *m_processor;
        juce::AudioProcessorValueTreeState *apvts;

        juce::AudioBuffer<FloatType> allBuffer;
    };
}

namespace controller {
    template<typename FloatType>
    class ControllerAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        explicit ControllerAttach(Controller<FloatType> &c,
                                  juce::AudioProcessorValueTreeState &parameters);

        ~ControllerAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        Controller<FloatType> *controller;
        juce::AudioProcessorValueTreeState *apvts;
        constexpr const static std::array IDs{ZLDsp::outGain::ID, ZLDsp::mix::ID, ZLDsp::overSample::ID,
                                              ZLDsp::rms::ID, ZLDsp::lookahead::ID, ZLDsp::segment::ID,
                                              ZLDsp::link::ID, ZLDsp::audit::ID, ZLDsp::external::ID};
        constexpr const static std::array defaultVs{ZLDsp::outGain::defaultV, ZLDsp::mix::defaultV,
                                                    float(ZLDsp::overSample::defaultI),
                                                    ZLDsp::rms::defaultV, ZLDsp::lookahead::defaultV,
                                                    ZLDsp::segment::defaultV,
                                                    float(ZLDsp::link::defaultV), float(ZLDsp::audit::defaultV),
                                                    float(ZLDsp::external::defaultV)};
    };

} // controller

#endif //ZLECOMP_CONTROLLER_H
