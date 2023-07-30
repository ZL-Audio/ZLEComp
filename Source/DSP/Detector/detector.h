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
#include <juce_dsp/juce_dsp.h>
#include "../dsp_defines.h"
#include "iter_funcs.h"

namespace detector {

    template<typename FloatType>
    class Detector {
    public:
        Detector() = default;

        void reset();

        void prepare(const juce::dsp::ProcessSpec &spec);

        FloatType process(FloatType target);

        inline void setAStyle(size_t idx) {
            aStyle.store(idx);
        }

        inline void setRStyle(size_t idx) {
            rStyle.store(idx);
        }

        inline void setAttack(FloatType v) {
            v = juce::jmin(v, FloatType(0.001));
            aPara.store(juce::jmin(scales<FloatType>[ZLDsp::aStyle::defaultI] / v * deltaT.load(), FloatType(1)));
        }

        inline void setRelease(FloatType v) {
            v = juce::jmin(v, FloatType(0.001));
            rPara.store(juce::jmin(scales<FloatType>[ZLDsp::rStyle::defaultI] / v * deltaT.load(), FloatType(1)));
        }

        inline void setSmooth(FloatType v) {
            smooth.store(v);
        }

    private:
        std::atomic<size_t> aStyle, rStyle;
        std::atomic<FloatType> aPara, rPara, smooth;
        std::atomic<FloatType> deltaT = FloatType(1 / 44100);
        FloatType xC = 0, xS = 0;

        inline static FloatType sgn(FloatType val) {
            return (FloatType(0) < val) - (val < FloatType(0));
        }
    };

} // detector

#endif //ZLECOMP_DETECTOR_H
