// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZL_BUTTON_COMPONENT_H
#define ZL_BUTTON_COMPONENT_H

#include "button_look_and_feel.h"
#include "name_look_and_feel.h"
#include "interface_definitions.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace zlinterface {
    class ButtonComponent : public juce::Component {
    public:
        explicit ButtonComponent(const juce::String &labelText, UIBase &base) :
                myLookAndFeel(base), nameLookAndFeel(base) {
            uiBase = &base;

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
            auto bound = getLocalBounds().toFloat();
            auto labelBound = bound.removeFromTop(labelHeight * bound.getHeight());
            label.setBounds(labelBound.toNearestInt());
            bound = bound.withSizeKeepingCentre(bound.getWidth(), bound.getWidth() * buttonRatio);
            button.setBounds(bound.toNearestInt());
        }

        void paint(juce::Graphics &g) override {
            juce::ignoreUnused(g);
        }

        juce::ToggleButton &getButton() { return button; }

        juce::Label &getLabel() { return label; }

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

        UIBase *uiBase;
    };
}

#endif //ZL_BUTTON_COMPONENT_H
