// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "detector.h"

namespace detector {

    template<typename FloatType>
    Detector<FloatType>::Detector(const Detector<FloatType> &d) {
        setDeltaT(d.getDeltaT());
        setAStyle(d.getAStyle());
        setRStyle(d.getRStyle());
        setAttack(d.getAttack());
        setRelease(d.getRelease());
        setSmooth(d.getSmooth());
    }

    template<typename FloatType>
    FloatType Detector<FloatType>::process(FloatType target) {
        FloatType para = xC < target ? rPara.load() : aPara.load();
        size_t style = xC < target ? rStyle.load() : aStyle.load();
        FloatType distanceS = target - xS;
        FloatType distanceC = (xS - xC) * smooth.load() + (target - xC) * (1 - smooth.load());
        FloatType slopeS = juce::jmin(para * std::abs(funcs<FloatType>[style](std::abs(distanceS))), std::abs(distanceS));
        FloatType slopeC = juce::jmin(para * std::abs(funcs<FloatType>[style](std::abs(distanceC))), std::abs(distanceC));
        xS += slopeS * sgn(distanceS);
        xC += slopeC * sgn(distanceC);
        xS = juce::jmax(xS, FloatType(0.0001));
        xC = juce::jmax(xC, FloatType(0.0001));
        return xC;
    }

    template<typename FloatType>
    void Detector<FloatType>::reset() {
        xC = 1.0;
        xS = 1.0;
    }

    template<typename FloatType>
    void Detector<FloatType>::prepare(const juce::dsp::ProcessSpec &spec) {
        deltaT.store(static_cast<FloatType>(1 / spec.sampleRate));
        setAttack(getAttack());
        setRelease(getRelease());
    }

    template
    class Detector<float>;

    template
    class Detector<double>;
} // detector