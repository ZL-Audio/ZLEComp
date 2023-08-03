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
#include "dsp_defines.h"
#include "controller.h"
#include "../State/state_defines.h"


namespace controller {
    template<typename FloatType>
    class ControllerAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        explicit ControllerAttach(juce::AudioProcessor &processor,
                                  Controller<FloatType> &c,
                                  juce::AudioProcessorValueTreeState &parameters);

        ~ControllerAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        juce::AudioProcessor *m_processor;
        Controller<FloatType> *controller;
        juce::AudioProcessorValueTreeState *apvts;
        constexpr const static std::array IDs{ZLDsp::outGain::ID, ZLDsp::mix::ID,
                                              ZLDsp::overSample::ID,
                                              ZLDsp::rms::ID, ZLDsp::lookahead::ID,
                                              ZLDsp::segment::ID,
                                              ZLDsp::audit::ID, ZLDsp::external::ID,
                                              ZLDsp::sideGain::ID, ZLDsp::link::ID};

        constexpr const static std::array defaultVs{ZLDsp::outGain::defaultV, ZLDsp::mix::defaultV,
                                                    float(ZLDsp::overSample::defaultI),
                                                    ZLDsp::rms::defaultV, ZLDsp::lookahead::defaultV,
                                                    ZLDsp::segment::defaultV,
                                                    float(ZLDsp::audit::defaultV), float(ZLDsp::external::defaultV),
                                                    ZLDsp::sideGain::defaultV, ZLDsp::link::defaultV};
    };
}


#endif //ZLECOMP_CONTROLLER_ATTACH_H
