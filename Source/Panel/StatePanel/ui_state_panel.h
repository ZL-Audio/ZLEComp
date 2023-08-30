// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_UI_STATE_PANEL_H
#define ZLECOMP_UI_STATE_PANEL_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../PluginProcessor.h"
#include "../../GUI/button_component.h"
#include "../../GUI/combo_box_component.h"
#include "../panel_definitions.h"

namespace zlpanel {

    class UIStatePanel : public juce::Component {
    public:
        explicit UIStatePanel(PluginProcessor &p, zlinterface::UIBase &base);

        ~UIStatePanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        std::unique_ptr<zlinterface::ButtonComponent> showCButton, showDButton, showMButton;
        std::array<std::unique_ptr<zlinterface::ButtonComponent> *, 2> buttonList{&showCButton, &showDButton};

        juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;

        std::unique_ptr<zlinterface::ComboboxComponent> monitorBox;
        std::array<std::unique_ptr<zlinterface::ComboboxComponent>*, 1> boxList{&monitorBox};

        juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> boxAttachments;

        zlinterface::UIBase *uiBase;
    };

} // zlpanel

#endif //ZLECOMP_UI_STATE_PANEL_H
