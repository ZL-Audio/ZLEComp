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

namespace zlcontroller {
    template<typename FloatType>
    ComputerAttach<FloatType>::ComputerAttach(juce::AudioProcessor &processor,
                                              Controller<FloatType> &control,
                                              juce::AudioProcessorValueTreeState &parameters) {
        processorRef = &processor;
        c = &control;
        apvts = &parameters;
        isPlotReady.setValue(false);
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
        if (parameterID == zldsp::threshold::ID) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            c->lrComputer.setThreshold(v);
        } else if (parameterID == zldsp::ratio::ID) {
            c->lrComputer.setRatio(v);
        } else if (parameterID == zldsp::kneeW::ID) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            c->lrComputer.setKneeW(zldsp::kneeW::formatV(v));
        } else if (parameterID == zldsp::kneeS::ID) {
            c->lrComputer.setKneeS(v);
        } else if (parameterID == zldsp::kneeD::ID) {
            c->lrComputer.setKneeD(v);
        } else if (parameterID == zldsp::bound::ID) {
            c->lrComputer.setBound(v);
        }
        isPlotReady.setValue(!isPlotReady.getValue());
    }

    template<typename FloatType>
    void ComputerAttach<FloatType>::getPlotArray(std::vector<float> &x, std::vector<float> &y){
        for (size_t i = 0; i < 241; ++i) {
            x.push_back((static_cast<float>(i) - 240.f) * 0.25f);
            y.push_back(static_cast<float>(c->lrComputer.eval(x[i])));
        }
    }

    template
    class ComputerAttach<float>;

    template
    class ComputerAttach<double>;
}