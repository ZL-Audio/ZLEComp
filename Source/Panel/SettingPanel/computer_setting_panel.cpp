// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "computer_setting_panel.h"

namespace zlpanel {
    ComputerSettingPanel::ComputerSettingPanel(juce::AudioProcessorValueTreeState &parameters,
                                               zlinterface::UIBase &base) {
        uiBase = &base;
        std::array<std::string, 2> rotarySliderID{zldsp::threshold::ID, zldsp::ratio::ID};
        attachSliders<zlinterface::RotarySliderComponent, 2>(*this, rotarySliderList, sliderAttachments, rotarySliderID,
                                                             parameters, base);

        std::array<std::string, 4> linearSliderID{zldsp::kneeW::ID, zldsp::kneeS::ID, zldsp::kneeD::ID,
                                                  zldsp::bound::ID};
        attachSliders<zlinterface::LinearSliderComponent, 4>(*this, linearSliderList, sliderAttachments, linearSliderID,
                                                             parameters, base);

        uiBase = &base;
    }

    ComputerSettingPanel::~ComputerSettingPanel() = default;

    void ComputerSettingPanel::paint(juce::Graphics &g) {
        auto bound = getLocalBounds().toFloat();
        uiBase->fillRoundedShadowRectangle(g, bound, 0.5f * uiBase->getFontSize(), {
                .blurRadius=0.25f, .mainColour=uiBase->getBackgroundColor()});
    }

    void ComputerSettingPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = uiBase->getRoundedShadowRectangleArea(bound, 0.5f * uiBase->getFontSize(), {.blurRadius=0.25f});

        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(6)), Track(Fr(3)), Track(Fr(3))};
        grid.templateColumns = {Track(Fr(1)), Track(Fr(1))};

        juce::Array<juce::GridItem> items;
        items.add(*thresholdSlider);
        items.add(*ratioSlider);
        items.add(*kneeWSlider);
        items.add(*kneeSSlider);
        items.add(*kneeDSlider);
        items.add(*boundSlider);
        grid.items = items;

        grid.performLayout(bound.toNearestInt());
    }
} // zlpanel