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

namespace zlpanel {
    float getPointX(juce::Rectangle<float> bound, float x, float xMin, float xMax) {
        x = juce::jlimit(xMin, xMax, x);
        return (x - xMin) / (xMax - xMin) * bound.getWidth() + bound.getX();
    }

    float getPointY(juce::Rectangle<float> bound, float y, float yMin, float yMax) {
        y = juce::jlimit(yMin, yMax, y);
        return (yMax - y) / (yMax - yMin) * bound.getHeight() + bound.getY();
    }

    void plotXY(juce::Graphics &g, juce::Rectangle<float> bound,
                std::span<const float> x, std::span<const float> y,
                float xMin, float xMax, float yMin, float yMax,
                float thickness, long numToPlots) {
        if (x.empty() || x.size() < y.size()) {
            return;
        }
        juce::Path path;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness, bound.getHeight() - thickness);
        path.startNewSubPath(getPointX(bound, x[0], xMin, xMax), getPointY(bound, y[0], yMin, yMax));
        if (numToPlots < 0) {
            numToPlots = static_cast<long>(x.size());
        }
        for (size_t i = 1; i < static_cast<size_t>(numToPlots); ++i) {
            path.lineTo(getPointX(bound, x[i], xMin, xMax), getPointY(bound, y[i], yMin, yMax));
        }
        g.strokePath(path, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));
    }

    ComputerPlotPanel::ComputerPlotPanel(PluginProcessor &p, zlinterface::UIBase &base) {
        computerAttach = &p.getComputerAttach();
        uiBase = &base;
        processorRef = &p;
        computerAttach->isPlotReady.addListener(this);
//        for (const auto &isComputerChangedParaID: isComputerChangedParaIDs) {
//            processorRef->parameters.addParameterListener(isComputerChangedParaID, this);
//        }
        for (const auto &isComputerChangedStateID: isComputerChangedStateIDs) {
            processorRef->states.addParameterListener(isComputerChangedStateID, this);
        }
        isComputerVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showComputer::ID)));
    }

    ComputerPlotPanel::~ComputerPlotPanel() {
//        for (const auto &isComputerChangedParaID: isComputerChangedParaIDs) {
//            processorRef->parameters.removeParameterListener(isComputerChangedParaID, this);
//        }
        computerAttach->isPlotReady.removeListener(this);
        for (const auto &isComputerChangedStateID: isComputerChangedStateIDs) {
            processorRef->states.removeParameterListener(isComputerChangedStateID, this);
        }
    }

    void ComputerPlotPanel::paint(juce::Graphics &g) {
        if (isComputerVisible.load()) {
            std::vector<float> x, y;
            computerAttach->getPlotArray(x, y);
            auto bound = getLocalBounds().toFloat();

            g.setColour(uiBase->getTextInactiveColor());
            g.setFont(uiBase->getFontSize() * zlinterface::FontLarge);
            g.drawText("0",
                       juce::Rectangle<float>(
                               bound.getX(), bound.getY() + smallPadding * uiBase->getFontSize(),
                               largePadding * 0.9f * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::topRight);
            g.drawText("-60",
                       juce::Rectangle<float>(
                               bound.getX(),
                               bound.getY() + bound.getHeight() - (1.f + largePadding) * uiBase->getFontSize(),
                               largePadding * 0.9f * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::bottomRight);

            auto threshold = static_cast<float>(computerAttach->getThreshold());
            g.drawText(zlinterface::formatFloat(threshold, 0),
                       juce::Rectangle<float>(
                               bound.getX(),
                               bound.getY() + smallPadding * uiBase->getFontSize() +
                               juce::jlimit(uiBase->getFontSize(), bound.getHeight() -
                                                                   (largePadding + smallPadding + 1) *
                                                                   uiBase->getFontSize(),
                                            (bound.getHeight() -
                                             (largePadding + smallPadding) * uiBase->getFontSize()) * threshold /
                                            (-60.f)) - uiBase->getFontSize() * 0.5f,
                               largePadding * 0.95f * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::centredRight);

            bound = bound.withTrimmedLeft(
                    uiBase->getFontSize() * largePadding).withTrimmedBottom(
                    uiBase->getFontSize() * largePadding).withTrimmedRight(
                    uiBase->getFontSize() * smallPadding).withTrimmedTop(
                    uiBase->getFontSize() * smallPadding);
            g.setColour(uiBase->getTextInactiveColor());
            g.drawRect(bound, uiBase->getFontSize() * 0.1f);

            bound = bound.withSizeKeepingCentre(bound.getWidth() - uiBase->getFontSize() * 0.1f,
                                                bound.getHeight() - uiBase->getFontSize() * 0.1f);

            float dashLengths[2] = {uiBase->getFontSize() * .5f, uiBase->getFontSize() * .5f};
            g.setColour(uiBase->getTextInactiveColor());
            g.drawDashedLine(juce::Line<float>(bound.getX(), bound.getY() + bound.getHeight(),
                                               bound.getX() + bound.getWidth(), bound.getY()),
                             dashLengths, 2, uiBase->getFontSize() * 0.1f);

            auto thresholdY = getPointY(bound, static_cast<float>(computerAttach->getThreshold()), -60.f, 0.f);
            g.drawDashedLine(juce::Line<float>(bound.getX(), thresholdY,
                                               bound.getX() + bound.getWidth(), thresholdY),
                             dashLengths, 2, uiBase->getFontSize() * 0.1f);

            g.setColour(uiBase->getTextColor());
            plotXY(g, bound,
                   x, y, -60.f, 0.f, -60.f, 0.f,
                   uiBase->getFontSize() * 0.125f);
        }
    }

    void ComputerPlotPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::showComputer::ID) {
            isComputerVisible.store(static_cast<bool>(newValue));
        }
        triggerAsyncUpdate();
    }

    void ComputerPlotPanel::valueChanged(juce::Value &value) {
        juce::ignoreUnused(value);
        triggerAsyncUpdate();
    }

    void ComputerPlotPanel::handleAsyncUpdate() {
        repaint();
    }

    DetectorPlotPanel::DetectorPlotPanel(PluginProcessor &p, zlinterface::UIBase &base) {
        detectorAttach = &p.getDetectorAttach();
        detectorAttach->isPlotReady.addListener(this);
        processorRef = &p;
        uiBase = &base;
//        for (const auto &isDetectorChangedParaID: isDetectorChangedParaIDs) {
//            processorRef->parameters.addParameterListener(isDetectorChangedParaID, this);
//        }
        for (const auto &isDetectorChangedStateID: isDetectorChangedStateIDs) {
            processorRef->states.addParameterListener(isDetectorChangedStateID, this);
        }
        isDetectorVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showDetector::ID)));
    }

    DetectorPlotPanel::~DetectorPlotPanel() {
//        for (const auto &isDetectorChangedParaID: isDetectorChangedParaIDs) {
//            processorRef->parameters.removeParameterListener(isDetectorChangedParaID, this);
//        }
        detectorAttach->isPlotReady.removeListener(this);
        for (const auto &isDetectorChangedStateID: isDetectorChangedStateIDs) {
            processorRef->states.removeParameterListener(isDetectorChangedStateID, this);
        }
    }

    void DetectorPlotPanel::paint(juce::Graphics &g) {
        if (isDetectorVisible.load()) {
            std::vector<float> x, y;
            detectorAttach->getPlotArray(x, y);
            auto xMax = x.back();
            auto yMinIndex = static_cast<size_t>(std::distance(std::begin(y),
                                                               std::min_element(std::begin(y), std::end(y))));

            auto bound = getLocalBounds().toFloat();
            g.setColour(uiBase->getTextInactiveColor());
            g.setFont(uiBase->getFontSize() * zlinterface::FontLarge);
            g.drawText("0",
                       juce::Rectangle<float>(
                               bound.getX(), bound.getY() + smallPadding * uiBase->getFontSize(),
                               largePadding * 0.9f * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::topRight);
            g.drawText("-20",
                       juce::Rectangle<float>(
                               bound.getX(),
                               bound.getY() + bound.getHeight() - (1.f + largePadding) * uiBase->getFontSize(),
                               largePadding * 0.95f * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::bottomRight);
            g.drawText(zlinterface::fixFormatFloat(xMax * 1000, 3),
                       juce::Rectangle<float>(
                               bound.getX() + bound.getWidth() -
                               (largePadding * 2 + smallPadding) * uiBase->getFontSize(),
                               bound.getY() + bound.getHeight() - largePadding * uiBase->getFontSize(),
                               largePadding * 2 * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::centredRight);
            g.drawText(zlinterface::fixFormatFloat(x[yMinIndex] * 1000, 3),
                       juce::Rectangle<float>(
                               bound.getX() +
                               juce::jlimit(largePadding * 0.5f * uiBase->getFontSize(),
                                            bound.getWidth() -
                                            (largePadding * 2.5f + smallPadding) * uiBase->getFontSize(),
                                            (bound.getWidth() - (largePadding + smallPadding) * uiBase->getFontSize()) *
                                            x[yMinIndex] / xMax),
                               bound.getY() + bound.getHeight() - largePadding * uiBase->getFontSize(),
                               largePadding * 2 * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::centred);

            bound = bound.withTrimmedLeft(
                    uiBase->getFontSize() * largePadding).withTrimmedBottom(
                    uiBase->getFontSize() * largePadding).withTrimmedRight(
                    uiBase->getFontSize() * smallPadding).withTrimmedTop(
                    uiBase->getFontSize() * smallPadding);
            g.setColour(uiBase->getTextInactiveColor());
            g.drawRect(bound, uiBase->getFontSize() * 0.1f);

            bound = bound.withSizeKeepingCentre(bound.getWidth() - uiBase->getFontSize() * 0.1f,
                                                bound.getHeight() - uiBase->getFontSize() * 0.1f);

            float dashLengths[2] = {uiBase->getFontSize() * .5f, uiBase->getFontSize() * .5f};
            g.setColour(uiBase->getTextInactiveColor());
            g.drawDashedLine(juce::Line<float>(getPointX(bound, x.front(), 0.f, xMax),
                                               getPointY(bound, y.front(), 0.1f, 1.f),
                                               getPointX(bound, x[yMinIndex], 0.f, xMax),
                                               getPointY(bound, y[yMinIndex], 0.1f, 1.f)),
                             dashLengths, 2, uiBase->getFontSize() * 0.1f);
            g.drawDashedLine(juce::Line<float>(
                                     getPointX(bound, x[yMinIndex], 0.f, xMax),
                                     getPointY(bound, y[yMinIndex], 0.1f, 1.f),
                                     getPointX(bound, x.back(), 0.f, xMax),
                                     getPointY(bound, y.back(), 0.1f, 1.f)),
                             dashLengths, 2, uiBase->getFontSize() * 0.1f);
            g.setColour(uiBase->getTextColor());
            plotXY(g, bound,
                   x, y, 0.f, xMax, 0.1f, 1.f,
                   uiBase->getFontSize() * 0.125f);
        }
    }

    void DetectorPlotPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::showDetector::ID) {
            isDetectorVisible.store(static_cast<bool>(newValue));
        }
        triggerAsyncUpdate();
    }

    void DetectorPlotPanel::valueChanged(juce::Value &value) {
        juce::ignoreUnused(value);
        triggerAsyncUpdate();
    }

    void DetectorPlotPanel::handleAsyncUpdate() {
        repaint();
    }

    PlotPanel::PlotPanel(PluginProcessor &p, zlinterface::UIBase &base) :
            computerPlotPanel(p, base),
            detectorPlotPanel(p, base) {
        processorRef = &p;
        uiBase = &base;
        for (const auto &isVisibleChangedStateID: isVisibleChangedStateIDs) {
            processorRef->states.addParameterListener(isVisibleChangedStateID, this);
        }
        isComputerVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showComputer::ID)));
        isDetectorVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showDetector::ID)));

        addAndMakeVisible(computerPlotPanel);
        addAndMakeVisible(detectorPlotPanel);
    }

    PlotPanel::~PlotPanel() {
        for (const auto &isVisibleChangedStateID: isVisibleChangedStateIDs) {
            processorRef->states.removeParameterListener(isVisibleChangedStateID, this);
        }
    }

    void PlotPanel::paint(juce::Graphics &g) {
        if (isComputerVisible.load() || isDetectorVisible.load()) {
            auto bound = getLocalBounds().toFloat();
            bound = bound.withWidth(bound.getHeight());
            uiBase->fillRoundedShadowRectangle(g, bound, 0.5f * uiBase->getFontSize(), {
                    .blurRadius=0.25f, .mainColour=uiBase->getBackgroundHideColor()});
        }
    }

    void PlotPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = bound.withWidth(bound.getHeight());
        bound = uiBase->getRoundedShadowRectangleArea(bound, 0.5f * uiBase->getFontSize(), {.blurRadius=0.25f});
        computerPlotPanel.setBounds(bound.toNearestInt());
        detectorPlotPanel.setBounds(bound.toNearestInt());
    }

    void PlotPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<bool>(newValue);
        if (parameterID == zlstate::showComputer::ID) {
            isComputerVisible.store(v);
            if (v) {
                processorRef->states.getParameter(zlstate::showDetector::ID)->setValueNotifyingHost(0.f);
            }
        } else if (parameterID == zlstate::showDetector::ID) {
            isDetectorVisible.store(v);
            if (v) {
                processorRef->states.getParameter(zlstate::showComputer::ID)->setValueNotifyingHost(0.f);
            }
        }
        triggerAsyncUpdate();
    }

    void PlotPanel::handleAsyncUpdate() {
        repaint();
    }
}