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

namespace controller {

    template<typename FloatType>
    class Controller {
    public:
        explicit Controller();

        ~Controller();

        void prepareToPlay(const juce::dsp::ProcessSpec &spec);

        void reset();

        void process(const juce::AudioBuffer<FloatType> &buffer);

    private:
        detector::Detector<FloatType> sideDetector;
        detector::RMSTracker<FloatType> sideTracker;
        computer::Computer<FloatType> sideComputer;

        std::array<std::unique_ptr<juce::dsp::Oversampling<FloatType>>, ZLDsp::overSample::overSampleNUM>
                overSamplers{};
        std::atomic<size_t> idxSampler = ZLDsp::overSample::defaultI;
        juce::dsp::Gain<FloatType> sideGain, outGain;
        juce::dsp::DelayLine<FloatType> mainDelay;
    };

} // controller

#endif //ZLECOMP_CONTROLLER_H
