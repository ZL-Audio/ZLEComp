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
          parameters(*this, nullptr, juce::Identifier("ZLECompParameters"),
                     ZLDsp::getParameterLayout()),
          controller(*this, parameters),
          controllerAttach(controller, parameters),
          detectorAttach(controller, parameters),
          computerAttach(controller, parameters){
    controllerAttach.initDefaultVs();
    controllerAttach.addListeners();
    detectorAttach.initDefaultVs();
    detectorAttach.addListeners();
    computerAttach.initDefaultVs();
    computerAttach.addListeners();
//    parameters.state.addChild (
//            { "uiState", { { "width", ZLInterface::WindowWidth }, { "height", ZLInterface::WindowHeight } }, {} }, -1, nullptr);
}

PluginProcessor::~PluginProcessor() {
}

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
    return 1;
//    return static_cast<int>(ZLDsp::presets.size());   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram() {
    return 0;
}

void PluginProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
    if (static_cast<size_t>(index) < ZLDsp::presets.size()) {
        juce::XmlDocument xmlDocument{ ZLDsp::presets[static_cast<size_t>(index)]};
        const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());
        parameters.replaceState(valueTreeToLoad);
    }
}

const juce::String PluginProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    if (static_cast<size_t>(index) < ZLDsp::presets.size()) {
        return ZLDsp::presetNames[static_cast<size_t>(index)];
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
//    return new PluginEditor(*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void PluginProcessor::getStateInformation(juce::MemoryBlock &destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
//    const auto presetFile = "/Volumes/Ramdisk/halfRMS.xml";
//    xml->writeTo(juce::File(presetFile));
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new PluginProcessor();
}