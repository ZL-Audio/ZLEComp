// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <BinaryData.h>
#include "State/dummy_processor.h"
#include "State/state_definitions.h"
#include "DSP/dsp_definitions.h"
#include "DSP/controller.h"
#include "DSP/controller_attach.h"
#include "DSP/detector_attach.h"
#include "DSP/computer_attach.h"

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor {
public:
    DummyProcessor dummyProcessor;
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState parametersNA;
    juce::AudioProcessorValueTreeState states;

    PluginProcessor();

    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    inline zlcontroller::ControllerAttach<float>& getControllerAttach() {
        return controllerAttach;
    }

    inline zlcontroller::DetectorAttach<float>& getDetectorAttach() {
        return detectorAttach;
    }

    inline zlcontroller::ComputerAttach<float>& getComputerAttach() {
        return computerAttach;
    }

    inline zlmeter::MeterSource<float> &getMeterIn() {
        return controller.meterIn;
    }

    inline zlmeter::MeterSource<float> &getMeterOut() {
        return controller.meterOut;
    }

    inline zlmeter::MeterSource<float> &getMeterEnd() {
        return controller.meterEnd;
    }

private:
    zlcontroller::Controller<float> controller;
    zlcontroller::ControllerAttach<float> controllerAttach;
    zlcontroller::DetectorAttach<float> detectorAttach;
    zlcontroller::ComputerAttach<float> computerAttach;
    std::atomic<int> programIndex = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};