// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "state_panel.h"

namespace zlpanel {
    StatePanel::StatePanel(PluginProcessor &p,
                           zlinterface::UIBase &base) :
                           logoPanel(p, base),
                           processStatePanel(p, base),
                           uiStatePanel(p, base){
        addAndMakeVisible(logoPanel);
        addAndMakeVisible(processStatePanel);
        addAndMakeVisible(uiStatePanel);
        uiBase = &base;
    }

    StatePanel::~StatePanel() = default;

    void StatePanel::paint(juce::Graphics &g) {
        juce::ignoreUnused(g);
    }

    void StatePanel::resized() {
        auto bound = getLocalBounds();
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(1))};
        grid.templateColumns = {Track(Fr(20)), Track(Fr(20)), Track(Fr(29))};

        juce::Array<juce::GridItem> items;
        items.add(logoPanel);
        items.add(processStatePanel);
        items.add(uiStatePanel);
        grid.items = items;

        grid.performLayout(bound);
    }
} // zlpanel