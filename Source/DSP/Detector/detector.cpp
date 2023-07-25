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
        xS += para * std::abs(funcs<FloatType>[style](std::abs(xS - target))) * sgn(xS - target);
        FloatType distance = xS * smooth.load() + target * (1 - smooth.load());
        xC += para * std::abs(funcs<FloatType>[style](distance)) * sgn(distance);
        return xC;
    }

    template<typename FloatType>
    void Detector<FloatType>::reset() {
        xC = 0;
        xS = 0;
    }

    template class Detector<float>;
    template class Detector<double>;
} // detector