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
    StatePanel::StatePanel(juce::AudioProcessorValueTreeState &parameters,
                           zlinterface::UIBase &base) : logoPanel(base) {
        addAndMakeVisible(logoPanel);
        std::array<std::string, 2> buttonID{zlstate::showComputer::ID, zlstate::showDetector::ID};
        attachButtons<zlinterface::ButtonComponent, 2>(*this, buttonList, buttonAttachments, buttonID, parameters, base);

        std::array<std::string, 1> boxID{zlstate::monitorSetting::ID};
        attachBoxes<zlinterface::ComboboxComponent, 1>(*this, boxList, boxAttachments, boxID, parameters, base);

        uiBase = &base;
    }

    StatePanel::~StatePanel() = default;

    void StatePanel::paint(juce::Graphics &g) {
        auto bound = getLocalBounds().toFloat();
        bound = bound.withTrimmedLeft(bound.getWidth() * float(8) / float(14));
        uiBase->fillRoundedShadowRectangle(g, bound, 0.5f * uiBase->getFontSize(), {
                .blurRadius=0.25f, .mainColour=zlinterface::BackgroundColor});
    }

    void StatePanel::resized() {
        auto bound = getLocalBounds().toFloat();
        logoPanel.setBounds(bound.removeFromLeft(bound.getWidth() * float(3) / float(14)).toNearestInt());
        bound = bound.withTrimmedLeft(bound.getWidth() * float(5) / float(11));
        bound = uiBase->getRoundedShadowRectangleArea(bound, 0.5f * uiBase->getFontSize(), {.blurRadius=0.25f});

        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(1))};
        grid.templateColumns = {Track(Fr(1)), Track(Fr(1)), Track(Fr(1))};

        juce::Array<juce::GridItem> items;
        items.add(*showCButton);
        items.add(*showDButton);
        items.add(*monitorBox);
        grid.items = items;

        grid.performLayout(bound.toNearestInt());
    }

} // zlpanel