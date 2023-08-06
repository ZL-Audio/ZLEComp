// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "main_panel.h"

namespace panel {
    MainPanel::MainPanel(PluginProcessor &p) :
            uiState(),
            logoPanel(),
            computerPlotPanel(p) {
//        addAndMakeVisible(logoPanel);
        addAndMakeVisible(computerPlotPanel);
    }

    MainPanel::~MainPanel() = default;

    void MainPanel::paint(juce::Graphics &g) {
        g.fillAll(zlinterface::BackgroundColor);
        auto bound = getLocalBounds().toFloat();
        float fontSize = bound.getHeight() * 0.0514f;
        bound = zlinterface::fillRoundedShadowRectangle(g, bound, fontSize * 0.5f);
        zlinterface::fillRoundedInnerShadowRectangle(g, bound, fontSize * 0.5f,
                                                   fontSize * 0.15f,
                                                     true, true, true, true, true);
    }

    void MainPanel::resized() {
//        logoPanel.setBounds(getLocalBounds());
        computerPlotPanel.setBounds(getLocalBounds());
        computerPlotPanel.setFontSize( static_cast<float> (getLocalBounds().toFloat().getHeight()) * 0.0514f);
    }
} // panel