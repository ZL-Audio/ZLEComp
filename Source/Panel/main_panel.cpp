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

namespace zlpanel {
    MainPanel::MainPanel(PluginProcessor &p) :
            uiBase(),
            statePanel(p.states, uiBase),
            centerPanel(p, uiBase),
            settingPanel(p.parameters, uiBase) {
//            meterPanel(&p.getMeterIn(), &p.getMeterEnd()) {
        addAndMakeVisible(centerPanel);
        addAndMakeVisible(settingPanel);
        addAndMakeVisible(statePanel);
//        addAndMakeVisible(meterPanel);
    }

    MainPanel::~MainPanel() = default;

    void MainPanel::paint(juce::Graphics &g) {
        g.fillAll(uiBase.getBackgroundColor());
        auto bound = getLocalBounds().toFloat();
        float fontSize = bound.getHeight() * 0.0514f * 0.5f;
        bound = uiBase.fillRoundedShadowRectangle(g, bound, fontSize * 0.5f, {});
        uiBase.fillRoundedInnerShadowRectangle(g, bound, fontSize * 0.5f, {.blurRadius=0.45f, .flip=true});
    }

    void MainPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        auto fontSize = bound.getHeight() * 0.0514f * 0.45f;
        bound = zlinterface::getRoundedShadowRectangleArea(bound, fontSize * 0.5f, {});
        bound = zlinterface::getRoundedShadowRectangleArea(bound, fontSize * 0.5f, {});

        uiBase.setFontSize(fontSize);

        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(7)), Track(Fr(24)), Track(Fr(31))};
        grid.templateColumns = {Track(Fr(14)), Track(Fr(3))};

        grid.items = {
                juce::GridItem(statePanel).withArea(1, 1, 2, 3),
                juce::GridItem(settingPanel).withArea(2, 1, 3, 3),
                juce::GridItem(centerPanel).withArea(3, 1, 4, 3),
//                juce::GridItem(meterPanel).withArea(3, 2, 4, 3),
        };

        grid.performLayout(bound.toNearestInt());
    }
} // panel