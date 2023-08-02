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
#include "dsp_defines.h"
#include "controller.h"

namespace controller {
    template<typename FloatType>
    class ComputerAttach : public juce::AudioProcessorValueTreeState::Listener {
    public:
        explicit ComputerAttach(Controller<FloatType> &c,
                                juce::AudioProcessorValueTreeState &parameters);

        ~ComputerAttach() override;

        void initDefaultVs();

        void addListeners();

        void parameterChanged(const juce::String &parameterID, float newValue) override;

        inline bool isPlotArrayReady() { return plotArrayReady.load(); }

        std::array<FloatType, 121> getPlotArrayY();

        std::array<FloatType, 121> getPlotArrayX();

        void calculatePlot();

    private:
        Controller<FloatType> *controller;
        juce::AudioProcessorValueTreeState *apvts;
        constexpr const static std::array IDs{ZLDsp::threshold::ID, ZLDsp::ratio::ID,
                                              ZLDsp::kneeW::ID, ZLDsp::kneeD::ID,
                                              ZLDsp::kneeS::ID, ZLDsp::bound::ID};

        constexpr const static std::array defaultVs{ZLDsp::threshold::defaultV, ZLDsp::ratio::defaultV,
                                                    ZLDsp::kneeW::defaultV, ZLDsp::kneeD::defaultV,
                                                    ZLDsp::kneeS::defaultV, ZLDsp::bound::defaultV};
        std::atomic<bool> plotArrayReady = false;
        std::array<FloatType, 121> plotArrayX, plotArrayY;
        juce::SpinLock plotLock;


    };
}

#endif //ZLECOMP_COMPUTER_ATTACH_H
