// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include <juce_audio_processors/juce_audio_processors.h>

namespace state {

    class DummyProcessor : public juce::AudioProcessor {
    public:

        DummyProcessor() = default;

        ~DummyProcessor() override = default;

        void prepareToPlay(double sampleRate, int samplesPerBlock) override {}

        void releaseResources() override {}

        bool isBusesLayoutSupported(const BusesLayout &layouts) const override { return false; }

        void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override {}

        juce::AudioProcessorEditor *createEditor() override { return nullptr; }

        bool hasEditor() const override { return false; }

        const juce::String getName() const override { return {}; }

        bool acceptsMidi() const override { return false; }

        bool producesMidi() const override { return false; }

        bool isMidiEffect() const override { return false; }

        double getTailLengthSeconds() const override { return 0.0; }

        int getNumPrograms() override { return 1; }

        int getCurrentProgram() override { return 0; }

        void setCurrentProgram(int index) override {}

        const juce::String getProgramName(int index) override { return {}; }

        void changeProgramName(int index, const juce::String &newName) override {}

        void getStateInformation(juce::MemoryBlock &destData) override {}

        void setStateInformation(const void *data, int sizeInBytes) override {}
    };
}