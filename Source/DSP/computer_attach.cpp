// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "computer_attach.h"

namespace controller {
    template<typename FloatType>
    ComputerAttach<FloatType>::ComputerAttach(Controller<FloatType> &c,
                                              juce::AudioProcessorValueTreeState &parameters) {
        controller = &c;
        apvts = &parameters;
    }

    template<typename FloatType>
    ComputerAttach<FloatType>::~ComputerAttach() {
        for (auto &ID: IDs) {
            apvts->removeParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::initDefaultVs() {
        for (size_t i = 0; i < IDs.size(); ++i) {
            parameterChanged(IDs[i], defaultVs[i]);
        }
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<FloatType>(newValue);
        if (parameterID == ZLDsp::threshold::ID) {
            controller->lrComputer.setThreshold(v);
        } else if (parameterID == ZLDsp::ratio::ID) {
            controller->lrComputer.setRatio(v);
        } else if (parameterID == ZLDsp::kneeW::ID) {
            controller->lrComputer.setKneeW(ZLDsp::kneeW::formatV(v));
        } else if (parameterID == ZLDsp::kneeS::ID) {
            controller->lrComputer.setKneeS(v);
        } else if (parameterID == ZLDsp::kneeD::ID) {
            controller->lrComputer.setKneeD(v);
        } else if (parameterID == ZLDsp::bound::ID) {
            controller->lrComputer.setBound(v);
        }
        plotArrayReady.store(true);
    }

    template<typename FloatType>
    std::array<FloatType, 121> ComputerAttach<FloatType>::getPlotArrayY() {
        std::array<FloatType, 121> temp = plotArrayY;
        return temp;
    }

    template<typename FloatType>
    std::array<FloatType, 121> ComputerAttach<FloatType>::getPlotArrayX() {
        std::array<FloatType, 121> temp = plotArrayX;
        return temp;
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::calculatePlot() {
        const juce::GenericScopedLock<juce::SpinLock> scopedLock (plotLock);
        for (size_t i = 0; i < 241; ++i) {
            plotArrayX[i] = static_cast<FloatType>(i - 240) * FloatType(0.25);
            plotArrayY[i] = controller->lrComputer.process(plotArrayX[i]);
        }
        plotArrayReady.store(false);
    }

    template
    class ComputerAttach<float>;

    template
    class ComputerAttach<double>;
}