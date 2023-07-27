// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "fifo_audio_buffer.h"

namespace fixedBuffer {
    template<typename FloatType>
    FIFOAudioBuffer<FloatType>::FIFOAudioBuffer(int channels, int bufferSize):
            fifo(bufferSize) {
        buffer.setSize(channels, bufferSize);
    }

    template<typename FloatType>
    void FIFOAudioBuffer<FloatType>::clear() {
        fifo.reset();
        buffer.clear();
    }

    template<typename FloatType>
    void FIFOAudioBuffer<FloatType>::setSize(int channels, int bufferSize) {
        juce::ignoreUnused(channels);
        clear();
        fifo.setTotalSize(bufferSize);
        buffer.setSize(channels, bufferSize);
    }

    template<typename FloatType>
    void FIFOAudioBuffer<FloatType>::push(const FloatType **samples, int numSamples) {
        jassert (fifo.getFreeSpace() >= numSamples);
        int start1, size1, start2, size2;
        fifo.prepareToWrite(numSamples, start1, size1, start2, size2);
        if (size1 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.copyFrom(channel, start1, samples[channel], size1);
        if (size2 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.copyFrom(channel, start2, samples[channel] + size1, size2);
        fifo.finishedWrite(size1 + size2);
    }

    template<typename FloatType>
    void FIFOAudioBuffer<FloatType>::push(const juce::AudioBuffer<FloatType> &samples,
                                          int numSamples) {
        const int addSamples = numSamples < 0 ? samples.getNumSamples() : numSamples;
        jassert (fifo.getFreeSpace() >= addSamples);

        int start1, size1, start2, size2;
        fifo.prepareToWrite(addSamples, start1, size1, start2, size2);
        if (size1 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.copyFrom(channel, start1, samples.getReadPointer(channel, 0),
                                size1);
        if (size2 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.copyFrom(channel, start2, samples.getReadPointer(channel, size1),
                                size2);
        fifo.finishedWrite(size1 + size2);
    }

    template<typename FloatType>
    void FIFOAudioBuffer<FloatType>::pop(FloatType **samples, int numSamples) {
        jassert (fifo.getNumReady() >= numSamples);
        int start1, size1, start2, size2;
        fifo.prepareToRead(numSamples, start1, size1, start2, size2);
        if (size1 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                juce::FloatVectorOperations::copy(samples[channel],
                                                  buffer.getReadPointer(channel, start1),
                                                  size1);
        if (size2 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                juce::FloatVectorOperations::copy(samples[channel] + size1,
                                                  buffer.getReadPointer(channel, start2),
                                                  size2);
        fifo.finishedRead(size1 + size2);
    }

    template<typename FloatType>
    void
    FIFOAudioBuffer<FloatType>::pop(juce::AudioBuffer<FloatType> &samples, int numSamples) {
        const int readSamples = numSamples > 0 ? numSamples : samples.getNumSamples();
        jassert (fifo.getNumReady() >= readSamples);
        int start1, size1, start2, size2;
        fifo.prepareToRead(readSamples, start1, size1, start2, size2);
        if (size1 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                samples.copyFrom(channel, 0, buffer.getReadPointer(channel, start1), size1);
        if (size2 > 0)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                samples.copyFrom(channel, size1, buffer.getReadPointer(channel, start2),
                                 size2);
        fifo.finishedRead(size1 + size2);
    }

    template
    class FIFOAudioBuffer<float>;

    template
    class FIFOAudioBuffer<double>;
}