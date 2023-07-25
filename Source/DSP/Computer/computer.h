/*
==============================================================================
Copyright (C) 2023 - zsliu98
This file is part of ZLEComp

ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
==============================================================================
*/

#ifndef ZLECOMP_COMPUTER_H
#define ZLECOMP_COMPUTER_H

#include "../dsp_defines.h"
#include "spline.h"

namespace Computer {

    template<typename FloatType>
    class Computer {
        Computer() {
            interpolate();
        }

        FloatType eval(FloatType x);

        void setThreshold(FloatType v) {
            threshold = v;
            interpolate();
        }

        void setRatio(FloatType v) {
            ratio = v;
            interpolate();
        }

        void setKneeW(FloatType v) {
            kneeW = v;
            interpolate();
        }

        void setKneeD(FloatType v) {
            kneeD = v;
            interpolate();
        }

        void setKneeS(FloatType v) {
            kneeS = v;
            interpolate();
        }

        void setBound(FloatType v) {
            bound = v;
        }

    private:
        std::atomic<FloatType> threshold = ZLDsp::threshold::defaultV, ratio = ZLDsp::ratio::defaultV;
        std::atomic<FloatType> kneeW = ZLDsp::kneeW::formatV(ZLDsp::kneeW::defaultV), kneeD = ZLDsp::kneeD::defaultV, kneeS = ZLDsp::kneeS::defaultV;
        std::atomic<FloatType> bound = ZLDsp::bound::defaultV;
        std::unique_ptr<tk::spline> c1, c2;

        void interpolate();
    };

} // Computer

#endif //ZLECOMP_COMPUTER_H
