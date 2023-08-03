// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLINFLATOR_BUTTONCOMPONENT_H
#define ZLINFLATOR_BUTTONCOMPONENT_H

#include "button_look_and_feel.h"
#include "name_look_and_feel.h"
#include "interface_defines.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace interface {
    class ButtonComponent : public juce::Component {
    public:
        explicit ButtonComponent(const juce::String &labelText) {
            setLookAndFeel(&myLookAndFeel);
            button.setClickingTogglesState(true);
            button.setLookAndFeel(&myLookAndFeel);
            addAndMakeVisible(button);
            label.setText(labelText, juce::dontSendNotification);
            label.setLookAndFeel(&nameLookAndFeel);
            addAndMakeVisible(label);
        }

        ~ButtonComponent() override {
            setLookAndFeel(nullptr);
            button.setLookAndFeel(nullptr);
            label.setLookAndFeel(nullptr);
        }

        void resized() override {
            auto bounds = getLocalBounds();
            bounds.removeFromTop(int(0.1 * bounds.getHeight()));
            button.setBounds(0, (int) (labelHeight * (float) bounds.getHeight()), bounds.getWidth(),
                             (int) ((float) bounds.getWidth() * buttonRatio));
            label.setBoundsRelative(0.f, 0.0f, 1.f, labelHeight);
        }

        void paint(juce::Graphics &g) override {
            g.fillAll(BackgroundColor);
        }

        juce::ToggleButton &getButton() { return button; }

        juce::Label &getLabel() { return label; }

        void setFontSize(float size) {
            myLookAndFeel.setFontSize(size);
            nameLookAndFeel.setFontSize(size);
        }

        void setEditable(bool f) {
            myLookAndFeel.setEditable(f);
            nameLookAndFeel.setEditable(f);
            repaint();
        }

    private:
        ButtonLookAndFeel myLookAndFeel;
        NameLookAndFeel nameLookAndFeel;
        juce::ToggleButton button;
        juce::Label label;

        constexpr static float buttonHeight = 0.7f;
        constexpr static float labelHeight = 1.f - buttonHeight;
        constexpr static float buttonRatio = 0.45f;
    };
}

#endif //ZLINFLATOR_BUTTONCOMPONENT_H
