// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "detector_setting_panel.h"

namespace zlpanel {
    DetectorSettingPanel::DetectorSettingPanel(juce::AudioProcessorValueTreeState & parameters) {
        std::array<std::string, 3> rotarySliderID{zldsp::sideGain::ID, zldsp::attack::ID, zldsp::release::ID};
        attachSliders<zlinterface::RotarySliderComponent, 3>(*this, rotarySliderList, sliderAttachments, rotarySliderID,
                                                             parameters);

        std::array<std::string, 2> linearSliderID{zldsp::smooth::ID, zldsp::link::ID};
        attachSliders<zlinterface::LinearSliderComponent, 2>(*this, linearSliderList, sliderAttachments, linearSliderID,
                                                             parameters);

        std::array<std::string, 2> boxID{zldsp::aStyle::ID, zldsp::rStyle::ID};
        attachBoxes<zlinterface::ComboboxComponent, 2>(*this, boxList, boxAttachments, boxID, parameters);

        std::array<std::string, 2> buttonID{zldsp::external::ID, zldsp::audit::ID};
        attachButtons<zlinterface::ButtonComponent, 2>(*this, buttonList, buttonAttachments, buttonID, parameters);
    }

    DetectorSettingPanel::~DetectorSettingPanel() = default;

    void DetectorSettingPanel::paint(juce::Graphics &g) {
        auto bound = getLocalBounds().toFloat();
        zlinterface::fillRoundedShadowRectangle(g, bound, 0.5f * fontSize, {
                .blurRadius=0.25f, .mainColour=zlinterface::BackgroundColor});
    }

    void DetectorSettingPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = zlinterface::getRoundedShadowRectangleArea(bound, 0.5f * fontSize, {.blurRadius=0.25f});

        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(6)), Track(Fr(3)), Track(Fr(3))};
        grid.templateColumns = {Track(Fr(1)), Track(Fr(1)), Track(Fr(1))};

        juce::Array<juce::GridItem> items;
        items.add(*gainSlider);
        items.add(*attackSlider);
        items.add(*releaseSlider);
        items.add(*extButton);
        items.add(*aStyleBox);
        items.add(*rStyleBox);
        items.add(*auditButton);
        items.add(*smoothSlider);
        items.add(*linkSlider);
        grid.items = items;

        grid.performLayout(bound.toNearestInt());
    }

    void DetectorSettingPanel::setFontSize(float fSize) {
        fontSize = fSize;
        for (auto const &s: rotarySliderList) {
            (*s)->setFontSize(fSize);
        }
        for (auto const &s: linearSliderList) {
            (*s)->setFontSize(fSize);
        }
        for (auto const &c: boxList) {
            (*c)->setFontSize(fSize);
        }
        for (auto const &b: buttonList) {
            (*b)->setFontSize(fSize);
        }
    }
} // zlpanel