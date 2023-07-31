// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "rms_tracker.h"

namespace detector {

    template<typename FloatType>
    RMSTracker<FloatType>::~RMSTracker() {
        loudnessBuffer.clear();
    }

    template<typename FloatType>
    void RMSTracker<FloatType>::prepare(const juce::dsp::ProcessSpec &spec) {
        reset();
        secondPerBuffer = static_cast<FloatType>(spec.maximumBlockSize) / static_cast<FloatType>(spec.sampleRate);
    }

    template<typename FloatType>
    void RMSTracker<FloatType>::reset() {
        loudnessBuffer.clear();
        mLoudness = 0;
        iLoudness = 0;
        numBuffer = 0;
    }

    template<typename FloatType>
    void RMSTracker<FloatType>::setMomentarySize(size_t mSize) {
        sizeToReset.store(juce::jmax(mSize, size_t(1)));
        isSizeReset.store(true);
    }

    template<typename FloatType>
    void RMSTracker<FloatType>::toSetMomentarySize() {
        while (loudnessBuffer.size() > sizeToReset.load()) {
            mLoudness -= loudnessBuffer.front();
            loudnessBuffer.pop_front();
        }
        loudnessBuffer.set_capacity(sizeToReset.load());
    }

    template<typename FloatType>
    void RMSTracker<FloatType>::process(const juce::AudioBuffer<FloatType> &buffer) {
        if (isSizeReset.load()) {
            isSizeReset.store(false);
            toSetMomentarySize();
        }
        FloatType _ms = 0;
        for (auto channel = 0; channel < buffer.getNumChannels(); channel++) {
            auto data = buffer.getReadPointer(channel);
            for (auto i = 0; i < buffer.getNumSamples(); i++) {
                _ms += data[i] * data[i];
            }
        }

        _ms = _ms / static_cast<FloatType> (buffer.getNumSamples());

        if (loudnessBuffer.size() == loudnessBuffer.capacity()) {
            mLoudness -= loudnessBuffer.front();
        }

        loudnessBuffer.push_back(_ms);
        mLoudness += _ms;

        iLoudness += _ms;
        numBuffer += 1;
    }

    template
    class RMSTracker<float>;

    template
    class RMSTracker<double>;
} // detector