// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "process_state_panel.h"

namespace zlpanel {
    ProcessStatePanel::ProcessStatePanel(PluginProcessor &p, zlinterface::UIBase &base) {
        std::array<std::string, 1> buttonID{zldsp::byPass::ID};
        attachButtons<zlinterface::ButtonComponent, 1>(*this, buttonList, buttonAttachments, buttonID, p.parameters, base);

        uiBase = &base;
    }

    ProcessStatePanel::~ProcessStatePanel() = default;

    void ProcessStatePanel::paint(juce::Graphics &g) {
        auto bound = getLocalBounds().toFloat();
        uiBase->fillRoundedShadowRectangle(g, bound, 0.5f * uiBase->getFontSize(), {
                .blurRadius=0.25f, .mainColour=zlinterface::BackgroundColor});
    }

    void ProcessStatePanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = uiBase->getRoundedShadowRectangleArea(bound, 0.5f * uiBase->getFontSize(), {.blurRadius=0.25f});

        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(1))};
        grid.templateColumns = {Track(Fr(1)), Track(Fr(2)), Track(Fr(1))};

        grid.items = {
                juce::GridItem(*byPassButton).withArea(1, 2, 2, 3)
        };

        grid.performLayout(bound.toNearestInt());
    }
} // zlpanel