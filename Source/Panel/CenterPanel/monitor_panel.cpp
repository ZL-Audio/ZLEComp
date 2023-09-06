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
    MonitorPanel::MonitorPanel(PluginProcessor &p, zlinterface::UIBase &base) :
            monitorSubPanel(p, base) {
        processorRef = &p;
        processorRef->states.addParameterListener(zlstate::monitorSetting::ID, this);
        monitorSetting.store(static_cast<int>(*p.states.getRawParameterValue(zlstate::monitorSetting::ID)));

        uiBase = &base;

        addAndMakeVisible(monitorSubPanel);
        triggerAsyncUpdate();
    }

    MonitorPanel::~MonitorPanel() {
        processorRef->states.removeParameterListener(zlstate::monitorSetting::ID, this);
    }

    void MonitorPanel::paint(juce::Graphics &g) {
        g.setColour(uiBase->getBackgroundColor());
        g.fillRect(getLocalBounds());
        if (monitorSetting.load() != zlstate::monitorSetting::off) {
            // draw boundary
            auto bound = getLocalBounds().toFloat();
            bound = uiBase->fillRoundedShadowRectangle(g, bound, 0.5f * uiBase->getFontSize(),
                                                            {.blurRadius=0.25f});
            bound = bound.withTrimmedLeft(
                    uiBase->getFontSize() * largePadding).withTrimmedBottom(
                    uiBase->getFontSize() * largePadding).withTrimmedRight(
                    uiBase->getFontSize() * largePadding).withTrimmedTop(
                    uiBase->getFontSize() * smallPadding);
            g.setColour(uiBase->getTextInactiveColor());
            auto thickness = uiBase->getFontSize() * 0.1f;
            g.drawRect(bound, thickness);
            g.setFont(uiBase->getFontSize() * zlinterface::FontLarge);

            g.setColour(uiBase->getTextInactiveColor());
            g.drawText("0",
                       juce::Rectangle<float>(
                               bound.getX() + bound.getWidth() + 0.125f * uiBase->getFontSize(),
                               bound.getY(),
                               largePadding * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::centredLeft);

            g.drawText("-60",
                       juce::Rectangle<float>(
                               bound.getX() + bound.getWidth() + 0.1f * uiBase->getFontSize(),
                               bound.getY() + bound.getHeight() - uiBase->getFontSize(),
                               largePadding * uiBase->getFontSize(), uiBase->getFontSize()),
                       juce::Justification::centredLeft);

            float dashLengths[2] = {uiBase->getFontSize() * .5f, uiBase->getFontSize() * .5f};
            for (int i = 1; i < 6; ++i) {
                auto value = zlinterface::formatFloat(-static_cast<float>(i) * 10, 0);
                auto initialY = bound.getY() + static_cast<float>(i) / 6.f * bound.getHeight();
                g.setColour(uiBase->getTextInactiveColor());
                g.drawText(value,
                           juce::Rectangle<float>(
                                   bound.getX() + bound.getWidth() + 0.1f * uiBase->getFontSize(),
                                   initialY - 0.5f * uiBase->getFontSize(),
                                   largePadding * uiBase->getFontSize(), uiBase->getFontSize()),
                           juce::Justification::centredLeft);
                g.setColour(uiBase->getTextHideColor());
                g.drawDashedLine(juce::Line<float>(bound.getX(),
                                                   initialY,
                                                   bound.getX() + bound.getWidth(),
                                                   initialY),
                                 dashLengths, 2, uiBase->getFontSize() * 0.1f);
            }
        }
    }

    void MonitorPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = uiBase->getRoundedShadowRectangleArea(bound, 0.5f * uiBase->getFontSize(), {.blurRadius=0.25f});
        bound = bound.withTrimmedLeft(
                uiBase->getFontSize() * largePadding).withTrimmedBottom(
                uiBase->getFontSize() * largePadding).withTrimmedRight(
                uiBase->getFontSize() * largePadding).withTrimmedTop(
                uiBase->getFontSize() * smallPadding);
        auto thickness = uiBase->getFontSize() * 0.1f;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness,
                                            bound.getHeight() - thickness);
        monitorSubPanel.setBounds(bound.toNearestInt());
    }

    void MonitorPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::monitorSetting::ID) {
            monitorSetting.store(static_cast<int>(newValue));
            triggerAsyncUpdate();
        }
    }

    void MonitorPanel::handleAsyncUpdate() {
        auto idx = monitorSetting.load();
        if (idx == zlstate::monitorSetting::off) {
            monitorSubPanel.setMonitorVisible(false);
            vBlankAttachment.reset();
        } else {
            vBlankAttachment = std::make_unique<juce::VBlankAttachment>(&monitorSubPanel, [&]{monitorSubPanel.repaint();});
            monitorSubPanel.setMonitorVisible(true);
            repaint();
            if (idx == zlstate::monitorSetting::medium) {
                monitorSubPanel.setTimeInSecond(4.f);
            } else {
                monitorSubPanel.setTimeInSecond(7.f);
            }
        }
    }
} // zlpanel