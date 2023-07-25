/*
==============================================================================
Copyright (C) 2023 - zsliu98
This file is part of ZLEComp

ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
==============================================================================
*/

#include "computer.h"

namespace Computer {
    template<typename FloatType>
    FloatType Computer<FloatType>::eval(FloatType x) {
        if (x <= threshold - kneeW) {
            return x;
        } else if (x >= threshold + kneeW) {
            return x / ratio + (1 - 1 / ratio) * threshold;
        } else if (x < threshold) {
            return static_cast<FloatType>(c1->operator()(x));
        } else {
            return static_cast<FloatType>(c2->operator()(x));
        }
    }

    template<typename FloatType>
    void Computer<FloatType>::interpolate() {
        std::vector<double> X1 = {threshold - kneeW, threshold};
        std::vector<double> Y1 = {threshold - kneeW, threshold - kneeD * 0.75 * kneeW * (1 - 0.5 / ratio - 0.5)};
        c1 = std::make_unique<tk::spline>(X1, Y1, tk::spline::cspline_hermite, false,
                                          tk::spline::first_deriv, 1,
                                          tk::spline::first_deriv, static_cast<double>(kneeS + (1 - kneeS) / ratio));
        std::vector<double> X2 = {threshold, threshold + kneeW / ratio};
        std::vector<double> Y2 = {threshold - kneeD * 0.75 * kneeW * (1 - 0.5 / ratio - 0.5), threshold - kneeW};
        c2 = std::make_unique<tk::spline>(X2, Y2, tk::spline::cspline_hermite, false,
                                          tk::spline::first_deriv, static_cast<double>(kneeS + (1 - kneeS) / ratio),
                                          tk::spline::first_deriv, static_cast<double>(1 / ratio));
    }

    template
    class Computer<float>;

    template
    class Computer<double>;
} // Computer