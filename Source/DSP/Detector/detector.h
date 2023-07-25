// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_DETECTOR_H
#define ZLECOMP_DETECTOR_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../dsp_defines.h"
#include "iter_funcs.h"

namespace detector {

    template<typename FloatType>
    class Detector {
    public:
        Detector() = default;

        void reset();

        FloatType process(FloatType target);

    private:
        std::atomic<size_t> aStyle = ZLDsp::aStyle::defaultI, rStyle = ZLDsp::rStyle::defaultI;
        std::atomic<FloatType> aPara = 1000 / ZLDsp::attack::defaultV * scales<FloatType>[ZLDsp::aStyle::defaultI] / 44100;
        std::atomic<FloatType> rPara = 1000 / ZLDsp::release::defaultV * scales<FloatType>[ZLDsp::rStyle::defaultI] / 44100;
        std::atomic<FloatType> deltaT = FloatType(1 / 44100);
        std::atomic<FloatType> smooth = ZLDsp::smooth::defaultV;
        FloatType xC = 0, xS = 0;

        inline static FloatType sgn(FloatType val) {
            return (FloatType(0) < val) - (val < FloatType(0));
        }
    };

} // detector

#endif //ZLECOMP_DETECTOR_H