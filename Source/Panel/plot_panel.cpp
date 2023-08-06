// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "plot_panel.h"

namespace panel {
    float getPointX(juce::Rectangle<float> bound, float x, float xMin, float xMax) {
        return (x - xMin) / (xMax - xMin) * bound.getWidth() + bound.getX();
    }

    float getPointY(juce::Rectangle<float> bound, float y, float yMin, float yMax) {
        return (yMax - y) / (yMax - yMin) * bound.getHeight() + bound.getY();
    }

    void plotXY(juce::Graphics &g, juce::Rectangle<float> bound,
                std::span<const float> x, std::span<const float> y,
                float xMin, float xMax, float yMin, float yMax,
                float thickness) {
        juce::Path path;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness, bound.getHeight() - thickness);
        path.startNewSubPath(getPointX(bound, x[0], xMin, xMax), getPointY(bound, y[0], yMin, yMax));
        for (size_t i = 1; i < x.size(); ++i) {
            path.lineTo(getPointX(bound, x[i], xMin, xMax), getPointY(bound, y[i], yMin, yMax));
        }
        g.strokePath(path, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));
    }

    ComputerPlotPanel::ComputerPlotPanel(PluginProcessor &p) {
        computerAttach = &p.getComputerAttach();
        processorRef = &p;
        for (const auto & isComputerChangedParaID : isComputerChangedParaIDs) {
            processorRef->parameters.addParameterListener(isComputerChangedParaID, this);
        }
        for (const auto & isComputerChangedStateID : isComputerChangedStateIDs) {
            processorRef->states.addParameterListener(isComputerChangedStateID, this);
        }
        isComputerVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showComputer::ID)));
    }

    ComputerPlotPanel::~ComputerPlotPanel() {
        for (size_t i = 0; i < 6; ++i) {
            processorRef->parameters.removeParameterListener(isComputerChangedParaIDs[i], this);
        }
        for (const auto & isComputerChangedStateID : isComputerChangedStateIDs) {
            processorRef->states.removeParameterListener(isComputerChangedStateID, this);
        }
    }

    void ComputerPlotPanel::paint(juce::Graphics &g) {
        if (isComputerVisible.load()) {
            std::vector<float> x, y;
            computerAttach->getPlotArray(x, y);
            plotXY(g, getLocalBounds().toFloat(),
                   x, y, -60.f, 0.f, -60.f, 0.f,
                   thickNess);
        }
    }

    void ComputerPlotPanel::setFontSize(float fSize) {
        thickNess = fSize * 0.1f;
    }

    void ComputerPlotPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::showComputer::ID) {
            isComputerVisible.store(static_cast<bool>(newValue));
        }
        triggerAsyncUpdate();
    }

    void ComputerPlotPanel::handleAsyncUpdate() {
        repaint();
    }

    DetectorPlotPanel::DetectorPlotPanel(PluginProcessor &p) {
        detectorAttach = &p.getDetectorAttach();
        processorRef = &p;
        for (const auto & isDetectorChangedParaID : isDetectorChangedParaIDs) {
            processorRef->parameters.addParameterListener(isDetectorChangedParaID, this);
        }
        for (const auto & isDetectorChangedStateID : isDetectorChangedStateIDs) {
            processorRef->states.addParameterListener(isDetectorChangedStateID, this);
        }
        isDetectorVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showDetector::ID)));
    }

    DetectorPlotPanel::~DetectorPlotPanel() {
        for (const auto & isDetectorChangedParaID : isDetectorChangedParaIDs) {
            processorRef->parameters.removeParameterListener(isDetectorChangedParaID, this);
        }
        for (const auto & isDetectorChangedStateID : isDetectorChangedStateIDs) {
            processorRef->states.removeParameterListener(isDetectorChangedStateID, this);
        }
    }

    void DetectorPlotPanel::paint(juce::Graphics &g) {
        if (isDetectorVisible.load()) {
            std::vector<float> x, y;
            detectorAttach->getPlotArray(x, y);
            plotXY(g, getLocalBounds().toFloat(),
                   x, y, 0.f, *std::ranges::max_element(x), 0.1f, 1.f,
                   thickNess);
        }
    }

    void DetectorPlotPanel::setFontSize(float fSize) {
        thickNess = fSize * 0.1f;
    }

    void DetectorPlotPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::showDetector::ID) {
            isDetectorVisible.store(static_cast<bool>(newValue));
        }
        triggerAsyncUpdate();
    }

    void DetectorPlotPanel::handleAsyncUpdate() {
        repaint();
    }
}