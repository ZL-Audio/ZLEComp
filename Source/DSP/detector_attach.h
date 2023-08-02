// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_DETECTOR_ATTACH_H
#define ZLECOMP_DETECTOR_ATTACH_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp_defines.h"
#include "controller.h"

namespace controller {
    template<typename FloatType>
    class DetectorAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        explicit DetectorAttach(Controller<FloatType> &c,
                                juce::AudioProcessorValueTreeState &parameters);

        ~DetectorAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

        inline bool isPlotArrayReady() { return plotArrayReady.load(); }

        std::array<FloatType, 200> getPlotArrayY();

        std::array<FloatType, 200> getPlotArrayX();

        void calculatePlot();

    private:
        Controller<FloatType> *controller;
        juce::AudioProcessorValueTreeState *apvts;
        constexpr const static std::array IDs{ZLDsp::attack::ID, ZLDsp::release::ID,
                                              ZLDsp::aStyle::ID, ZLDsp::rStyle::ID,
                                              ZLDsp::smooth::ID};

        constexpr const static std::array defaultVs{ZLDsp::attack::defaultV, ZLDsp::release::defaultV,
                                                    float(ZLDsp::aStyle::defaultI), float(ZLDsp::rStyle::defaultI),
                                                    ZLDsp::smooth::defaultV};

        std::atomic<bool> plotArrayReady = false;
        std::array<FloatType, 200> plotArrayX, plotArrayY;
        juce::SpinLock plotLock;
    };
}

#endif //ZLECOMP_DETECTOR_ATTACH_H
