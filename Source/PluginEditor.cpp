// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor &p) :
        AudioProcessorEditor(&p), processorRef(p),
        mainPanel(p) {
    juce::ignoreUnused(processorRef);

    addAndMakeVisible(mainPanel);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizeLimits(zlstate::windowW::minV, zlstate::windowH::minV, zlstate::windowW::maxV, zlstate::windowH::maxV);
    getConstrainer()->setFixedAspectRatio(
            static_cast<float>(zlstate::windowW::defaultV) / static_cast<float>(zlstate::windowH::defaultV));
    setResizable(true, p.wrapperType != PluginProcessor::wrapperType_AudioUnitv3);
    lastUIWidth.referTo(p.states.getParameterAsValue(zlstate::windowW::ID));
    lastUIHeight.referTo(p.states.getParameterAsValue(zlstate::windowH::ID));
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());
    lastUIWidth.addListener(this);
    lastUIHeight.addListener(this);
}

PluginEditor::~PluginEditor() = default;

//==============================================================================
void PluginEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void PluginEditor::resized() {
    mainPanel.setBounds(getLocalBounds());
    lastUIWidth = getWidth();
    lastUIHeight = getHeight();
}

void PluginEditor::valueChanged (juce::Value&) {
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());
}