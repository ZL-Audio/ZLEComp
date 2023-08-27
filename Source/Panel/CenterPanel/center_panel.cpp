// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "center_panel.h"

namespace zlpanel {

    CenterPanel::CenterPanel(PluginProcessor &p, zlinterface::UIBase &base) :
            plotPanel(p, base), monitorPanel(p, base) {
        processorRef = &p;
        openGLContext.attachTo(*this);

        uiBase = &base;

        processorRef->states.addParameterListener(zlstate::monitorSetting::ID, this);
        monitorSetting.store(static_cast<int>(*p.states.getRawParameterValue(zlstate::monitorSetting::ID)));

        addAndMakeVisible(monitorPanel);
        addAndMakeVisible(plotPanel);
        
        setSize(200, 100);
    }

    CenterPanel::~CenterPanel() {
        openGLContext.detach();
        processorRef->states.removeParameterListener(zlstate::monitorSetting::ID, this);
    }

    void CenterPanel::paint(juce::Graphics &g) {
        g.setColour(uiBase->getBackgroundColor());
        g.fillRect(getLocalBounds());
    }

    void CenterPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        if (monitorSetting.load() == zlstate::monitorSetting::hz30m ||
            monitorSetting.load() == zlstate::monitorSetting::hz60m) {
            monitorPanel.setBounds(bound.withTrimmedLeft(bound.getHeight()).toNearestInt());
        } else {
            monitorPanel.setBounds(bound.toNearestInt());
        }
        bound = bound.withWidth(bound.getHeight());
        plotPanel.setBounds(bound.toNearestInt());
    }

    void CenterPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlstate::monitorSetting::ID) {
            monitorSetting.store(static_cast<int>(newValue));
            triggerAsyncUpdate();
        }
    }

    void CenterPanel::handleAsyncUpdate() {
        resized();
    }
} // zlpanel