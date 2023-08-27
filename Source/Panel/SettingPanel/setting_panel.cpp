// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "setting_panel.h"

namespace zlpanel {
    SettingPanel::SettingPanel(juce::AudioProcessorValueTreeState &parameters,
                               zlinterface::UIBase &base) :
            globalSettingPanel(parameters, base),
            computerSettingPanel(parameters, base),
            detectorSettingPanel(parameters, base) {
        addAndMakeVisible(globalSettingPanel);
        addAndMakeVisible(computerSettingPanel);
        addAndMakeVisible(detectorSettingPanel);

        uiBase = &base;
    }

    SettingPanel::~SettingPanel() = default;

    void SettingPanel::paint(juce::Graphics &) {}

    void SettingPanel::resized() {
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(1))};
        grid.templateColumns = {Track(Fr(20)), Track(Fr(20)), Track(Fr(29))};

        juce::Array<juce::GridItem> items;
        items.add(globalSettingPanel);
        items.add(computerSettingPanel);
        items.add(detectorSettingPanel);
        grid.items = items;

        grid.performLayout(getLocalBounds());
    }
} // zlpanel