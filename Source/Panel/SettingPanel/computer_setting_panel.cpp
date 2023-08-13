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
    ComputerSettingPanel::ComputerSettingPanel(juce::AudioProcessorValueTreeState &parameters) {
        std::array<std::string, 2> rotarySliderID{zldsp::threshold::ID, zldsp::ratio::ID};
        attachSliders<zlinterface::RotarySliderComponent, 2>(*this, rotarySliderList, sliderAttachments, rotarySliderID,
                                                             parameters);

        std::array<std::string, 4> linearSliderID{zldsp::kneeW::ID, zldsp::kneeS::ID, zldsp::kneeD::ID,
                                                  zldsp::bound::ID};
        attachSliders<zlinterface::LinearSliderComponent, 4>(*this, linearSliderList, sliderAttachments, linearSliderID,
                                                             parameters);
    }

    ComputerSettingPanel::~ComputerSettingPanel() = default;

    void ComputerSettingPanel::paint(juce::Graphics &g) {
        auto bound = getLocalBounds().toFloat();
        zlinterface::fillRoundedShadowRectangle(g, bound, 0.5f * fontSize, {
                .blurRadius=0.25f, .mainColour=zlinterface::BackgroundColor});
    }

    void ComputerSettingPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = zlinterface::getRoundedShadowRectangleArea(bound, 0.5f * fontSize, {.blurRadius=0.25f});

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

    void ComputerSettingPanel::setFontSize(float fSize) {
        fontSize = fSize;
        for (auto const &s: rotarySliderList) {
            (*s)->setFontSize(fSize);
        }
        for (auto const &s: linearSliderList) {
            (*s)->setFontSize(fSize);
        }
    }
} // zlpanel