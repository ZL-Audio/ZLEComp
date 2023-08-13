// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZL_LINEAR_SLIDER_LOOK_AND_FEEL_H
#define ZL_LINEAR_SLIDER_LOOK_AND_FEEL_H

#include "interface_definitions.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace zlinterface {
    class LinearSliderLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        explicit LinearSliderLookAndFeel() = default;

        void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                              float sliderPos, float minSliderPos, float maxSliderPos,
                              const juce::Slider::SliderStyle, juce::Slider &slider) override {
            juce::ignoreUnused(slider, minSliderPos, maxSliderPos);
            auto bound = juce::Rectangle<int>(x, y, width, height).toFloat();
            bound = zlinterface::getRoundedShadowRectangleArea(bound, fontSize * 0.5f, {});
            zlinterface::fillRoundedInnerShadowRectangle(g, bound, fontSize * 0.5f, {.blurRadius = 0.66f});

            juce::Path mask;
            mask.addRoundedRectangle(bound, fontSize * 0.5f);
            g.saveState();
            g.reduceClipRegion(mask);
            auto proportion = sliderPos / static_cast<float>(width);
            auto shadowBound = bound.withWidth(proportion * bound.getWidth());
            g.setColour(TextHideColor);
            g.fillRect(shadowBound);
            g.restoreState();
        }

        juce::Label *createSliderTextBox(juce::Slider &) override {
            auto *l = new juce::Label();
            l->setJustificationType(juce::Justification::centred);
            l->setInterceptsMouseClicks(false, false);
            return l;
        }

        juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override {
            auto localBounds = slider.getLocalBounds().toFloat();
            juce::Slider::SliderLayout layout;
            auto textBounds = localBounds;
            layout.textBoxBounds = textBounds.toNearestInt();
            layout.sliderBounds = slider.getLocalBounds();
            return layout;
        }

        void drawLabel(juce::Graphics &g, juce::Label &label) override {
            if (editable.load()) {
                g.setColour(TextColor);
            } else {
                g.setColour(TextInactiveColor);
            }
            auto labelArea{label.getLocalBounds().toFloat()};
            auto center = labelArea.getCentre();
            if (fontSize > 0) {
                g.setFont(fontSize * FontLarge);
            } else {
                g.setFont(labelArea.getHeight() * 0.6f);
            }
            g.drawSingleLineText(juce::String(label.getText()),
                                 juce::roundToInt(center.x + g.getCurrentFont().getHorizontalScale()),
                                 juce::roundToInt(center.y + g.getCurrentFont().getDescent()),
                                 juce::Justification::horizontallyCentred);
        }

        void setFontSize(float size) {
            fontSize = size;
        }

        void setEditable(bool f) {
            editable.store(f);
        }

        void setMouseOver(bool f) {
            mouseOver.store(f);
        }

    private:
        std::atomic<float> fontSize = 0.0f;
        std::atomic<bool> editable = true, mouseOver = false;
    };
}

#endif //ZL_LINEAR_SLIDER_LOOK_AND_FEEL_H
