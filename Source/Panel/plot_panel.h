// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_PLOT_PANEL_H
#define ZLECOMP_PLOT_PANEL_H

//#include <iostream>
//#include <fstream>
#include <span>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../PluginProcessor.h"
#include "../DSP/dsp_definitions.h"
#include "../DSP/computer_attach.h"
#include "../DSP/detector_attach.h"

namespace panel {
    float getPointX(juce::Rectangle<float> bound, float x, float xMin, float xMax);

    float getPointY(juce::Rectangle<float> bound, float y, float yMin, float yMax);

    void plotXY(juce::Graphics &g, juce::Rectangle<float> bound,
                std::span<const float> x, std::span<const float> y,
                float xMin, float xMax, float yMin, float yMax,
                float thickness);

//    inline static void writeArray(const std::string &fileName, std::vector<float> &x) {
//        std::ofstream myFile(fileName);
//        if (myFile.is_open()) {
//            for (const auto &xx: x) {
//                myFile << xx << ", ";
//            }
//            myFile << "\n";
//        }
//    }

    class ComputerPlotPanel : public juce::Component, public juce::AudioProcessorValueTreeState::Listener,
                              private juce::AsyncUpdater {
    public:
        explicit ComputerPlotPanel(PluginProcessor &p);

        ~ComputerPlotPanel() override;

        void paint(juce::Graphics &g) override;

        void setFontSize(float fSize);

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        zlcontroller::ComputerAttach<float> *computerAttach;
        float thickNess = 0.0f;
        PluginProcessor *processorRef;
        std::array<juce::String, 6> isComputerChangedParaIDs{zldsp::threshold::ID, zldsp::ratio::ID,
                                                             zldsp::kneeW::ID, zldsp::kneeD::ID,
                                                             zldsp::kneeS::ID, zldsp::bound::ID};
        std::array<juce::String, 1> isComputerChangedStateIDs{zlstate::showComputer::ID};
        std::atomic<bool> isComputerVisible = zlstate::showComputer::defaultV;

        void handleAsyncUpdate() override;

    };

    class DetectorPlotPanel : public juce::Component, public juce::AudioProcessorValueTreeState::Listener,
                              private juce::AsyncUpdater {
    public:
        explicit DetectorPlotPanel(PluginProcessor &p);

        ~DetectorPlotPanel() override;

        void paint(juce::Graphics &g) override;

        void setFontSize(float fSize);

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        zlcontroller::DetectorAttach<float> *detectorAttach;
        float thickNess = 0.0f;
        PluginProcessor *processorRef;
        std::array<juce::String, 5> isDetectorChangedParaIDs{zldsp::attack::ID, zldsp::release::ID,
                                                             zldsp::aStyle::ID, zldsp::rStyle::ID,
                                                             zldsp::smooth::ID};
        std::array<juce::String, 1> isDetectorChangedStateIDs{zlstate::showDetector::ID};
        std::atomic<bool> isDetectorVisible = zlstate::showDetector::defaultV;

        void handleAsyncUpdate() override;
    };
}
#endif //ZLECOMP_PLOT_PANEL_H
