// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "detector_attach.h"

namespace zlcontroller {
    template<typename FloatType>
    DetectorAttach<FloatType>::DetectorAttach(Controller<FloatType> &c,
                                              juce::AudioProcessorValueTreeState &parameters) {
        controller = &c;
        apvts = &parameters;
    }

    template<typename FloatType>
    DetectorAttach<FloatType>::~DetectorAttach() {
        for (auto &ID: IDs) {
            apvts->removeParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::initDefaultVs() {
        for (size_t i = 0; i < IDs.size(); ++i) {
            parameterChanged(IDs[i], defaultVs[i]);
        }
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<FloatType>(newValue);
        if (parameterID == zldsp::attack::ID) {
            controller->lDetector.setAttack(zldsp::attack::formatV(v));
            controller->rDetector.setAttack(zldsp::attack::formatV(v));
        } else if (parameterID == zldsp::release::ID) {
            controller->lDetector.setRelease(zldsp::release::formatV(v));
            controller->rDetector.setRelease(zldsp::release::formatV(v));
        } else if (parameterID == zldsp::aStyle::ID) {
            auto idx = static_cast<size_t>(v);
            controller->lDetector.setAStyle(idx);
            controller->rDetector.setAStyle(idx);
        } else if (parameterID == zldsp::rStyle::ID) {
            auto idx = static_cast<size_t>(v);
            controller->lDetector.setRStyle(idx);
            controller->rDetector.setRStyle(idx);
        } else if (parameterID == zldsp::smooth::ID) {
            controller->lDetector.setSmooth(v);
            controller->rDetector.setSmooth(v);
        }
        plotArrayReady.store(true);
    }

    template<typename FloatType>
    std::array<float, 200> DetectorAttach<FloatType>::getPlotArrayY() {
        std::array<float, 200> temp = plotArrayY;
        return temp;
    }

    template<typename FloatType>
    std::array<float, 200> DetectorAttach<FloatType>::getPlotArrayX() {
        std::array<float, 200> temp = plotArrayX;
        return temp;
    }

    template<typename FloatType>
    void DetectorAttach<FloatType>::calculatePlot() {
        const juce::GenericScopedLock<juce::SpinLock> scopedLock (plotLock);
        // init zldetector
        auto tempDetector = zldetector::Detector<FloatType>(controller->lDetector);
        auto x = FloatType(0), y = FloatType(1);
        // calculate attack plot
        FloatType deltaT = tempDetector.getAttack() / 100;
        tempDetector.setDeltaT(deltaT);
        tempDetector.setAttack(tempDetector.getAttack());
        for (size_t i = 0; i < 100; ++i) {
            plotArrayX[i] = static_cast<float>(x);
            plotArrayY[i] = static_cast<float>(y);
            x += deltaT;
            y = tempDetector.process(FloatType(0.01));
        }
        // calculate release plot
        deltaT = tempDetector.getRelease() / 100;
        tempDetector.setDeltaT(deltaT);
        tempDetector.setAttack(tempDetector.getRelease());
        for (size_t i = 100; i < 200; ++i) {
            plotArrayX[i] = static_cast<float>(x);
            plotArrayY[i] = static_cast<float>(y);
            x += deltaT;
            y = tempDetector.process(FloatType(1));
        }
        // finish
        plotArrayReady.store(false);
    }

    template
    class DetectorAttach<float>;

    template
    class DetectorAttach<double>;
}