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

    StatePanel::StatePanel(juce::AudioProcessorValueTreeState &parameters) : logoPanel() {
        addAndMakeVisible(logoPanel);
        std::array<std::string, 3> buttonID{zlstate::showComputer::ID, zlstate::showDetector::ID, zlstate::showMonitor::ID};
        attachButtons<zlinterface::ButtonComponent, 3>(*this, buttonList, buttonAttachments, buttonID, parameters);
    }

    StatePanel::~StatePanel() = default;

    void StatePanel::paint(juce::Graphics &) {}

    void StatePanel::resized() {
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(1))};
        grid.templateColumns = {Track(Fr(3)), Track(Fr(5)), Track(Fr(2)), Track(Fr(2)), Track(Fr(2))};
        grid.items = {
                juce::GridItem(logoPanel).withArea(1, 1),
                juce::GridItem(*showCButton).withArea(1, 3),
                juce::GridItem(*showDButton).withArea(1, 4),
                juce::GridItem(*showMButton).withArea(1, 5)
        };

        grid.performLayout(getLocalBounds());
    }

    void StatePanel::setFontSize(float fSize) {
        for (auto const &b: buttonList) {
            (*b)->setFontSize(fSize);
        }
    }

} // zlpanel