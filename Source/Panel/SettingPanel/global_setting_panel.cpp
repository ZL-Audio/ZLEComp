// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "global_setting_panel.h"

namespace zlpanel {
    GlobalSettingPanel::GlobalSettingPanel(juce::AudioProcessorValueTreeState & parameters, zlinterface::UIBase &base) {
        uiBase = &base;

        std::array<std::string, 2> rotarySliderID{zldsp::outGain::ID, zldsp::mix::ID};
        attachSliders<zlinterface::RotarySliderComponent, 2>(*this, rotarySliderList, sliderAttachments, rotarySliderID,
                                                             parameters, base);

        std::array<std::string, 3> linearSliderID{zldsp::rms::ID, zldsp::lookahead::ID, zldsp::segment::ID};
        attachSliders<zlinterface::LinearSliderComponent, 3>(*this, linearSliderList, sliderAttachments, linearSliderID,
                                                             parameters, base);

        std::array<std::string, 1> boxID{zldsp::overSample::ID};
        attachBoxes<zlinterface::ComboboxComponent, 1>(*this, boxList, boxAttachments, boxID, parameters, base);
    }

    GlobalSettingPanel::~GlobalSettingPanel()  = default;

    void GlobalSettingPanel::paint(juce::Graphics &g) {
        auto bound = getLocalBounds().toFloat();
        uiBase->fillRoundedShadowRectangle(g, bound, 0.5f * uiBase->getFontSize(),
                                           {.blurRadius=0.25f});
    }

    void GlobalSettingPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = uiBase->getRoundedShadowRectangleArea(bound, 0.5f * uiBase->getFontSize(), {.blurRadius=0.25f});

        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(6)), Track(Fr(3)), Track(Fr(3))};
        grid.templateColumns = {Track(Fr(1)), Track(Fr(1))};

        juce::Array<juce::GridItem> items;
        items.add(*gainSlider);
        items.add(*mixSlider);
        items.add(*oversampleBox);
        items.add(*rmsSlider);
        items.add(*lookaheadSlider);
        items.add(*segmentSlider);
        grid.items = items;

        grid.performLayout(bound.toNearestInt());
    }
} // zlpanel