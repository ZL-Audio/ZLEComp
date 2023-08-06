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
#include "dsp_definitions.h"
#include "controller.h"

namespace zlcontroller {
    template<typename FloatType>
    class DetectorAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        constexpr const static size_t plotSize = 200;
        explicit DetectorAttach(Controller<FloatType> &c,
                                juce::AudioProcessorValueTreeState &parameters);

        ~DetectorAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

        void getPlotArray(std::vector<float> &x, std::vector<float> &y, FloatType target=FloatType(0.1));

    private:
        Controller<FloatType> *controller;
        juce::AudioProcessorValueTreeState *apvts;
        constexpr const static std::array IDs{zldsp::attack::ID, zldsp::release::ID,
                                              zldsp::aStyle::ID, zldsp::rStyle::ID,
                                              zldsp::smooth::ID};

        constexpr const static std::array defaultVs{zldsp::attack::defaultV, zldsp::release::defaultV,
                                                    float(zldsp::aStyle::defaultI), float(zldsp::rStyle::defaultI),
                                                    zldsp::smooth::defaultV};
    };
}

#endif //ZLECOMP_DETECTOR_ATTACH_H
