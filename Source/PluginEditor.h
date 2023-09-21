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

#include "PluginProcessor.h"
#include "Panel/main_panel.h"
#include "State/state_definitions.h"
#include "State/property.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::Value::Listener,
                     private juce::AudioProcessorValueTreeState::Listener,
                     private juce::AsyncUpdater {
public:
    explicit PluginEditor(PluginProcessor &p);

    ~PluginEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;

    void resized() override;

private:
    PluginProcessor &processorRef;
    zlstate::Property property;
    zlpanel::MainPanel mainPanel;
    juce::Value lastUIWidth, lastUIHeight;
    constexpr const static std::array IDs{zlstate::showComputer::ID, zlstate::showDetector::ID,
                                          zlstate::monitorSetting::ID,
                                          zlstate::uiStyle::ID,
                                          zlstate::windowW::ID, zlstate::windowH::ID};

    void valueChanged(juce::Value &) override;

    void parameterChanged(const juce::String &parameterID, float newValue) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
