// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_RMS_TRACKER_H
#define ZLECOMP_RMS_TRACKER_H

#include "tracker.h"

namespace detector {

    template<typename FloatType>
    class RMSTracker : Tracker<FloatType> {
    public:
        explicit RMSTracker() = default;

        ~RMSTracker() override;

        void prepareToPlay(const juce::dsp::ProcessSpec &spec) override;

        void reset() override;

        void setMomentarySize(size_t mSize) override;

        inline FloatType getBufferPeak() override {
            return juce::Decibels::gainToDecibels(peak);
        }

        inline FloatType getMomentaryLoudness() override {
            FloatType meanSquare = 0;
            if (loudness.size() > 0) {
                meanSquare = mLoudness / static_cast<FloatType>(loudness.size());
            }
            return juce::Decibels::gainToDecibels(meanSquare) * static_cast<FloatType>(0.5);
        }

        inline FloatType getIntegratedLoudness() override {
            FloatType meanSquare = 0;
            if (numBuffer > 0) {
                meanSquare = iLoudness / static_cast<FloatType>(numBuffer);
            }
            return secondPerBuffer * juce::Decibels::gainToDecibels(meanSquare) *
                   static_cast<FloatType>(0.5);
        }

        inline FloatType getIntegratedTotalLoudness() override {
            return getIntegratedLoudness() * static_cast<FloatType>(numBuffer);
        }

        void process(const juce::AudioBuffer<FloatType> &buffer) override;

    private:

        bool kWeight = false;
        size_t size = 0, numBuffer = 0;
        FloatType peak = 0, mLoudness = 0, iLoudness = 0;
        FloatType secondPerBuffer = 0.01;
        std::deque<FloatType> loudness;
        juce::AudioBuffer<FloatType> bufferCopy;
    };

} // detector

#endif //ZLECOMP_RMS_TRACKER_H