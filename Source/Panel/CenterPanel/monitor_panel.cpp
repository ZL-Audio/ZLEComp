// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "monitor_panel.h"

namespace zlpanel {
    MonitorPanel::MonitorPanel(PluginProcessor &p) :
            monitorSubPanel(p) {
//        openGLContext.attachTo(*getTopLevelComponent());
        processorRef = &p;
        processorRef->states.addParameterListener(zlstate::showMonitor::ID, this);
        isMonitorVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showMonitor::ID)));

        monitorSubPanel.setMonitorVisible(isMonitorVisible.load());
        addAndMakeVisible(monitorSubPanel);

        startTimerHz(callBackHz);
    }

    MonitorPanel::~MonitorPanel() {
//        openGLContext.detach();
        stopTimer();
    }

    void MonitorPanel::paint(juce::Graphics &g) {
        if (isMonitorVisible.load()) {
            // draw boundary
            auto bound = getLocalBounds().toFloat();
            bound = zlinterface::fillRoundedShadowRectangle(g, bound, 0.5f * fontSize, {.blurRadius=0.25f});
            bound = bound.withTrimmedLeft(
                    fontSize * largePadding).withTrimmedBottom(
                    fontSize * largePadding).withTrimmedRight(
                    fontSize * largePadding).withTrimmedTop(
                    fontSize * smallPadding);
            g.setColour(zlinterface::TextInactiveColor);
            auto thickness = fontSize * 0.1f;
            g.drawRect(bound, thickness);
        }
    }

    void MonitorPanel::resized() {
//        image = image.rescaled(getWidth() * upScaling, getHeight() * upScaling);
        auto bound = getLocalBounds().toFloat();
        bound = zlinterface::getRoundedShadowRectangleArea(bound, 0.5f * fontSize, {.blurRadius=0.25f});
        bound = bound.withTrimmedLeft(
                fontSize * largePadding).withTrimmedBottom(
                fontSize * largePadding).withTrimmedRight(
                fontSize * largePadding).withTrimmedTop(
                fontSize * smallPadding);
        auto thickness = fontSize * 0.1f;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness,
                                            bound.getHeight() - thickness);
        monitorSubPanel.setBounds(bound.toNearestInt());
    }

    void MonitorPanel::setFontSize(float fSize) {
        fontSize = fSize;
        monitorSubPanel.setFontSize(fSize);
    }

    void MonitorPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::showMonitor::ID) {
            auto v = static_cast<bool>(newValue);
            isMonitorVisible.store(v);
            if (v) {
                startTimerHz(callBackHz);
            } else {
                stopTimer();
            }
            triggerAsyncUpdate();
        }
    }

    void MonitorPanel::timerCallback() {
        monitorSubPanel.repaint();
    }

    void MonitorPanel::handleAsyncUpdate() {
        monitorSubPanel.setMonitorVisible(isMonitorVisible.load());
        repaint();
    }
} // zlpanel