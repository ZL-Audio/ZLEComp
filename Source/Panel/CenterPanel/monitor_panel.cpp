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
        processorRef = &p;
        processorRef->states.addParameterListener(zlstate::showMonitor::ID, this);
        isMonitorVisible.store(static_cast<bool>(*p.states.getRawParameterValue(zlstate::showMonitor::ID)));

        monitorSubPanel.setMonitorVisible(isMonitorVisible.load());
        addAndMakeVisible(monitorSubPanel);

        startTimerHz(callBackHz);
    }

    MonitorPanel::~MonitorPanel() {
        stopTimer();
        processorRef->states.removeParameterListener(zlstate::showMonitor::ID, this);
    }

    void MonitorPanel::paint(juce::Graphics &g) {
        g.setColour(zlinterface::BackgroundColor);
        g.fillRect(getLocalBounds());
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
            g.setFont(fontSize * zlinterface::FontLarge);

            g.setColour(zlinterface::TextInactiveColor);
            g.drawText("0",
                       juce::Rectangle<float>(
                               bound.getX() + bound.getWidth() + 0.125f * fontSize,
                               bound.getY(),
                               largePadding * fontSize, fontSize),
                       juce::Justification::centredLeft);

            g.drawText("-60",
                       juce::Rectangle<float>(
                               bound.getX() + bound.getWidth() + 0.1f * fontSize,
                               bound.getY() + bound.getHeight() - fontSize,
                               largePadding * fontSize, fontSize),
                       juce::Justification::centredLeft);

            float dashLengths[2] = {fontSize * .5f, fontSize * .5f};
            for (int i = 1; i < 6; ++i) {
                auto value = zlinterface::formatFloat(-static_cast<float>(i) * 10, 0);
                auto initialY = bound.getY() + static_cast<float>(i) / 6.f * bound.getHeight();
                g.setColour(zlinterface::TextInactiveColor);
                g.drawText(value,
                           juce::Rectangle<float>(
                                   bound.getX() + bound.getWidth() + 0.1f * fontSize,
                                   initialY - 0.5f * fontSize,
                                   largePadding * fontSize, fontSize),
                           juce::Justification::centredLeft);
                g.setColour(zlinterface::TextHideColor);
                g.drawDashedLine(juce::Line<float>(bound.getX(),
                                                   initialY,
                                                   bound.getX() + bound.getWidth(),
                                                   initialY),
                                 dashLengths, 2, fontSize * 0.1f);
            }
        }
    }

    void MonitorPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = zlinterface::getRoundedShadowRectangleArea(bound, 0.5f * fontSize, {.blurRadius=0.25f});
        bound = bound.withTrimmedLeft(
                fontSize * largePadding).withTrimmedBottom(
                fontSize * largePadding).withTrimmedRight(
                fontSize * largePadding).withTrimmedTop(
                fontSize * smallPadding);
        auto thickness = fontSize * 0.1f;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - 2 * thickness,
                                            bound.getHeight() - 2 * thickness);
        monitorSubPanel.setBounds(bound.toNearestInt());
    }

    void MonitorPanel::setFontSize(float fSize) {
        fontSize = fSize;
        monitorSubPanel.setFontSize(fSize);
    }

    void MonitorPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::showMonitor::ID) {
            isMonitorVisible.store(static_cast<bool>(newValue));
            triggerAsyncUpdate();
        }
    }

    void MonitorPanel::timerCallback() {
        monitorSubPanel.repaint();
    }

    void MonitorPanel::handleAsyncUpdate() {
        monitorSubPanel.setMonitorVisible(isMonitorVisible.load());
        if (isMonitorVisible.load()) {
            repaint();
            startTimerHz(callBackHz);
        } else {
            stopTimer();
            repaint();
        }
    }
} // zlpanel