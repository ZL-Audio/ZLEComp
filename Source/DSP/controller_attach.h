// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_CONTROLLER_ATTACH_H
#define ZLECOMP_CONTROLLER_ATTACH_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp_definitions.h"
#include "controller.h"
#include "../State/state_definitions.h"


namespace zlcontroller {
    template<typename FloatType>
    class ControllerAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        explicit ControllerAttach(juce::AudioProcessor &processor,
                                  Controller<FloatType> &c,
                                  juce::AudioProcessorValueTreeState &parameters,
                                  juce::AudioProcessorValueTreeState &state_parameters);

        ~ControllerAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        juce::AudioProcessor *processorRef;
        Controller<FloatType> *controller;
        juce::AudioProcessorValueTreeState *apvts, *apvtsNA;
        constexpr const static std::array IDs{zldsp::outGain::ID, zldsp::mix::ID,
                                              zldsp::overSample::ID,
                                              zldsp::rms::ID, zldsp::lookahead::ID,
                                              zldsp::segment::ID,
                                              zldsp::audit::ID, zldsp::external::ID,
                                              zldsp::sideGain::ID, zldsp::link::ID,
                                              zldsp::byPass::ID, zldsp::sStyle::ID};

        constexpr const static std::array defaultVs{zldsp::outGain::defaultV, zldsp::mix::defaultV,
                                                    float(zldsp::overSample::defaultI),
                                                    zldsp::rms::defaultV, zldsp::lookahead::defaultV,
                                                    zldsp::segment::defaultV,
                                                    float(zldsp::audit::defaultV), float(zldsp::external::defaultV),
                                                    zldsp::sideGain::defaultV, zldsp::link::defaultV,
                                                    float(zldsp::byPass::defaultV),
                                                    float(zldsp::sStyle::defaultI)};
    };
}


#endif //ZLECOMP_CONTROLLER_ATTACH_H
