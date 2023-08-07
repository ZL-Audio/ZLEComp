// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLINFLATOR_ROTARYSLIDERLOOKANDFEEL_H
#define ZLINFLATOR_ROTARYSLIDERLOOKANDFEEL_H

#include "interface_definitions.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace zlinterface {
    class RotarySliderLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        explicit RotarySliderLookAndFeel() = default;

        void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                              const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) override {
            juce::ignoreUnused(slider);
            // calculate values
            auto rotationAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
            auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
            bounds = bounds.withSizeKeepingCentre(diameter, diameter);
            // draw knob
            auto oldBounds = drawInnerShadowEllipse(g, bounds, fontSize * 0.5f, {});
            auto newBounds = drawShadowEllipse(g, oldBounds, fontSize * 0.5f, {});
            drawInnerShadowEllipse(g, newBounds, fontSize * 0.15f, {.flip=true});
            // draw arrow
            auto arrowUnit = (diameter - newBounds.getWidth()) * 0.5f;
            auto arrowBound = juce::Rectangle<float>(
                    -0.5f * arrowUnit + bounds.getCentreX() +
                    (0.5f * diameter - 0.5f * arrowUnit) * std::sin(rotationAngle),
                    -0.5f * arrowUnit + bounds.getCentreY() +
                    (0.5f * diameter - 0.5f * arrowUnit) * (-std::cos(rotationAngle)),
                    arrowUnit, arrowUnit);
            auto arrowStartBound = juce::Rectangle<float>(
                    -0.5f * arrowUnit + bounds.getCentreX() +
                    (0.5f * diameter - 0.5f * arrowUnit) * std::sin(rotaryStartAngle),
                    -0.5f * arrowUnit + bounds.getCentreY() +
                    (0.5f * diameter - 0.5f * arrowUnit) * (-std::cos(rotaryStartAngle)),
                    arrowUnit, arrowUnit);
            juce::Path mask;
            mask.addEllipse(bounds);
            mask.setUsingNonZeroWinding(false);
            mask.addEllipse(newBounds);
            g.saveState();
            g.reduceClipRegion(mask);
            drawShadowEllipse(g, arrowBound, fontSize * 0.5f,
                              {.fit=false, .drawBright=false, .drawDark=true});
            drawShadowEllipse(g, arrowStartBound, fontSize * 0.5f,
                              {.fit=false, .drawBright=false, .drawDark=true, .mainColour=TextHideColor});

            juce::Path filling;
            filling.addPieSegment(bounds, rotaryStartAngle, rotationAngle, 0);
            filling.setUsingNonZeroWinding(false);
            filling.addPieSegment(arrowStartBound, rotaryStartAngle, rotaryStartAngle + juce::MathConstants<float>::pi,
                                  0);
            g.setColour(TextHideColor);
            g.fillPath(filling);
            drawInnerShadowEllipse(g, arrowBound, fontSize * 0.15f, {.flip=true});
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
            auto textBounds = juce::Rectangle<float>(localBounds.getX() + 0.15f * localBounds.getWidth(),
                                                     localBounds.getY() + 0.15f * localBounds.getHeight(),
                                                     localBounds.getWidth() * 0.7f,
                                                     localBounds.getHeight() * 0.7f);
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
                g.setFont(fontSize * FontHuge);
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

    private:
        std::atomic<float> fontSize = 0.0f;
        std::atomic<bool> editable = true;
    };
}
#endif //ZLINFLATOR_ROTARYSLIDERLOOKANDFEEL_H