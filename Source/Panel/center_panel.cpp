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

    CenterPanel::CenterPanel(PluginProcessor &p) :
            plotPanel(p), monitorPanel(p) {
        processorRef = &p;
        addAndMakeVisible(monitorPanel);
        addAndMakeVisible(plotPanel);
        startTimerHz(10);
    }

    CenterPanel::~CenterPanel() {
        stopTimer();
    }

    void CenterPanel::paint(juce::Graphics &g) {
        juce::ignoreUnused(g);
    }

    void CenterPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        monitorPanel.setBounds(bound.toNearestInt());
        bound = bound.withWidth(bound.getHeight());
        plotPanel.setBounds(bound.toNearestInt());
    }

    void CenterPanel::setFontSize(float fSize) {
        fontSize = fSize;
        plotPanel.setFontSize(fSize);
        monitorPanel.setFontSize(fSize);
    }

    void CenterPanel::timerCallback() {
        repaint();
    }
} // zlpanel