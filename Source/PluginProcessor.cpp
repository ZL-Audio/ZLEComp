// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
        : AudioProcessor(BusesProperties()
                                 .withInput("Input", juce::AudioChannelSet::stereo(),
                                            true)
                                 .withOutput("Output", juce::AudioChannelSet::stereo(),
                                             true)
                                 .withInput("Ext", juce::AudioChannelSet::stereo(),
                                            true)),
          dummyProcessor(),
          parameters(*this, nullptr,
                     juce::Identifier("ZLECompParameters"),
                     zldsp::getParameterLayout()),
          parametersNA(dummyProcessor, nullptr,
                       juce::Identifier("ZLECompParametersNA"),
                       zlstate::getNAParameterLayout()),
          states(dummyProcessor, nullptr,
                 juce::Identifier("ZLECompStates"),
                 zlstate::getParameterLayout()),
          controller(*this, parameters),
          controllerAttach(*this, controller, parameters, parametersNA),
          detectorAttach(controller, parameters),
          computerAttach(*this, controller, parameters) {
    controllerAttach.initDefaultVs();
    controllerAttach.addListeners();
    detectorAttach.initDefaultVs();
    detectorAttach.addListeners();
    computerAttach.initDefaultVs();
    computerAttach.addListeners();
}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
const juce::String PluginProcessor::getName() const {
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PluginProcessor::getNumPrograms() {
    return static_cast<int>(zlstate::preset::presetNUM);
}

int PluginProcessor::getCurrentProgram() {
    return programIndex.load();
}

void PluginProcessor::setCurrentProgram(int index) {
    programIndex.store(index);
    parametersNA.getParameter(zlstate::programIdx::ID)->setValueNotifyingHost(zlstate::programIdx::convertTo01(index));
    if (index < zlstate::preset::presetNUM) {
        juce::XmlDocument xmlDocument{zlstate::preset::xmls[static_cast<size_t>(index)]};
        const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());
        parameters.replaceState(valueTreeToLoad.getChildWithName("ZLECompParameters"));
    }
}

const juce::String PluginProcessor::getProgramName(int index) {
    if (index < zlstate::preset::presetNUM) {
        return zlstate::preset::names[static_cast<size_t>(index)];
    }
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String &newName) {
    juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto channels = static_cast<juce::uint32> (juce::jmin(getMainBusNumInputChannels(),
                                                          getMainBusNumOutputChannels()));
    juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32> (samplesPerBlock),
                                channels};
    controller.prepare(spec);
}

void PluginProcessor::releaseResources() {}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) {
        return false;
    }
    if (layouts.getChannelSet(true, 0) != layouts.getChannelSet(true, 1)) {
        return false;
    }
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) {
        return false;
    }
    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                   juce::MidiBuffer &midiMessages) {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormal;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    controller.process(buffer);
}

//==============================================================================
bool PluginProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *PluginProcessor::createEditor() {
    return new PluginEditor(*this);
}

//==============================================================================
void PluginProcessor::getStateInformation(juce::MemoryBlock &destData) {
    auto tempTree = juce::ValueTree("ZLECompParaState");
    tempTree.appendChild(parameters.copyState(), nullptr);
    tempTree.appendChild(parametersNA.copyState(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(tempTree.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName("ZLECompParaState")) {
        auto tempTree = juce::ValueTree::fromXml(*xmlState);
        parameters.replaceState(tempTree.getChildWithName(parameters.state.getType()));
        parametersNA.replaceState(tempTree.getChildWithName(parametersNA.state.getType()));
        programIndex.store(static_cast<int>(parametersNA.getRawParameterValue(zlstate::programIdx::ID)->load()));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new PluginProcessor();
}