// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_COMPUTER_H
#define ZLECOMP_COMPUTER_H

#include "../dsp_defines.h"
#include "spline.h"

namespace computer {

    template<typename FloatType>
    class Computer {
    public:
        Computer() {
            interpolate();
        }

        FloatType eval(FloatType x);

        inline void setThreshold(FloatType v) {
            threshold.store(v);
            interpolate();
        }

        inline void setRatio(FloatType v) {
            ratio.store(v);
            interpolate();
        }

        inline void setKneeW(FloatType v) {
            kneeW.store(v);
            interpolate();
        }

        inline void setKneeD(FloatType v) {
            kneeD.store(v);
            interpolate();
        }

        inline void setKneeS(FloatType v) {
            kneeS.store(v);
            interpolate();
        }

        inline void setBound(FloatType v) {
            bound.store(v);
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
