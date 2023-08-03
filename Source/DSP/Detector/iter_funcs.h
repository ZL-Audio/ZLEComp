// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================



#ifndef ZLECOMP_ITER_FUNCS_H
#define ZLECOMP_ITER_FUNCS_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../dsp_defines.h"

namespace detector {
    enum iterType {
        classic, style1, style2, style3, style4, styleNUM
    };

    template<typename FloatType>
    static std::array<std::function<FloatType(FloatType)>, iterType::styleNUM> funcs = {
            [](FloatType x) { return x; },
            [](FloatType x) { return x * (FloatType(0.5) + (FloatType(1.5) - x) * x); },
            [](FloatType x) { return std::sin(x * juce::MathConstants<FloatType>::halfPi); },
            [](FloatType x) { return std::sin(x * juce::MathConstants<FloatType>::halfPi) - x; },
            [](FloatType x) { return x * (1 - x); }
    };

    template<typename FloatType>
    static std::array<FloatType, iterType::styleNUM> scales = {
            FloatType(11.375),
            FloatType(25.475),
            FloatType(7.35),
            FloatType(23.975),
            FloatType(16.025)
    };

    template<typename FloatType>
    static const std::array<FloatType, iterType::styleNUM> scales0 = {
            FloatType(9.1046),
            FloatType(15.9204),
            FloatType(5.9165),
            FloatType(17.6046),
            FloatType(11.4072)
    };

    template<typename FloatType>
    static const std::array<FloatType, iterType::styleNUM> scales1 = {
            FloatType(11.6414),
            FloatType(20.7262),
            FloatType(7.5334),
            FloatType(22.0533),
            FloatType(14.0133)
    };

    template<typename FloatType>
    static FloatType getScale(FloatType smooth) {
        auto proportion = (1 - smooth) * (1 - smooth);
        return scales0<FloatType>[ZLDsp::rStyle::defaultI] * proportion +
               scales1<FloatType>[ZLDsp::rStyle::defaultI] * (1 - proportion);
    }
}

#endif //ZLECOMP_ITER_FUNCS_H
