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
    FloatType Detector<FloatType>::process(FloatType target) {
        FloatType para = xC < target ? rPara.load() : aPara.load();
        size_t style = xC < target ? rStyle.load() : aStyle.load();
        FloatType distanceS = target - xS;
        FloatType distanceC = (xS - xC) * smooth.load() + (target - xC) * (1 - smooth.load());
        FloatType slopeS = juce::jmin(para * std::abs(funcs<FloatType>[style](std::abs(distanceS))), distanceS);
        FloatType slopeC = juce::jmin(para * std::abs(funcs<FloatType>[style](std::abs(distanceC))), distanceC);
        xS += slopeS * sgn(distanceS);
        xC += slopeC * sgn(distanceC);
        return xC;
    }

    template<typename FloatType>
    inline void Detector<FloatType>::reset() {
        xC = 0;
        xS = 0;
    }

    template<typename FloatType>
    inline void Detector<FloatType>::prepare(const juce::dsp::ProcessSpec &spec) {
        deltaT.store(static_cast<FloatType>(1 / spec.sampleRate));
        reset();
    }

    template
    class Detector<float>;

    template
    class Detector<double>;
} // detector