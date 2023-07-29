// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_FIXEDAUDIOBUFFER_H
#define ZLECOMP_FIXEDAUDIOBUFFER_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../dsp_defines.h"
#include "fifo_audio_buffer.h"

namespace fixedBuffer {
    template<typename FloatType>
    class FixedAudioBuffer {
    public:
        juce::AudioBuffer<FloatType> subBuffer;

        explicit FixedAudioBuffer(
                int subBufferSize = static_cast<int>(ZLDsp::segment::defaultV));

        void clear();

        void setSubBufferSize(int subBufferSize);

        void prepare(juce::dsp::ProcessSpec spec);

        void pushBuffer(juce::AudioBuffer<FloatType> &buffer);

        void pushBlock(juce::dsp::AudioBlock<FloatType> block);

        void popSubBuffer();

        void pushSubBuffer();

        void popBuffer(juce::AudioBuffer<FloatType> &buffer);

        void popBlock(juce::dsp::AudioBlock<FloatType> block);

        juce::AudioBuffer<FloatType> getSubBufferChannels(int channelOffset, int numChannels);

        juce::dsp::AudioBlock<FloatType> getSubBlockChannels(int channelOffset, int numChannels);

        inline auto isSubReady() {
            return inputBuffer.getNumReady() >= subBuffer.getNumSamples();
        }

        inline auto getMainSpec() { return mainSpec; }

        inline auto getSubSpec() { return subSpec; }

        inline auto getLatencySamples() { return subSpec.maximumBlockSize; }

    private:
        FIFOAudioBuffer<FloatType> inputBuffer, outputBuffer;
        juce::dsp::ProcessSpec subSpec, mainSpec;
    };
}

#endif //ZLECOMP_FIXEDAUDIOBUFFER_H
