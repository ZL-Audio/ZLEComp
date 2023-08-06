// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_COMPUTER_ATTACH_H
#define ZLECOMP_COMPUTER_ATTACH_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp_definitions.h"
#include "controller.h"

namespace zlcontroller {
    template<typename FloatType>
    class ComputerAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        constexpr const static size_t plotSize = 121;

        explicit ComputerAttach(Controller<FloatType> &control,
                                juce::AudioProcessorValueTreeState &parameters);

        ~ComputerAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

        void getPlotArray(std::vector<float> &x, std::vector<float> &y);

        FloatType getThreshold() { return c->lrComputer.getThreshold(); }

    private:
        Controller<FloatType> *c;
        juce::AudioProcessorValueTreeState *apvts;
        constexpr const static std::array IDs{zldsp::threshold::ID, zldsp::ratio::ID,
                                              zldsp::kneeW::ID, zldsp::kneeD::ID,
                                              zldsp::kneeS::ID, zldsp::bound::ID};

        constexpr const static std::array defaultVs{zldsp::threshold::defaultV, zldsp::ratio::defaultV,
                                                    zldsp::kneeW::defaultV, zldsp::kneeD::defaultV,
                                                    zldsp::kneeS::defaultV, zldsp::bound::defaultV};
    };
}

#endif //ZLECOMP_COMPUTER_ATTACH_H
