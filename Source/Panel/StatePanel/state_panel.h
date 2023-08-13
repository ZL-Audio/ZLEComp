// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_STATE_PANEL_H
#define ZLECOMP_STATE_PANEL_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../PluginProcessor.h"
#include "../../GUI/button_component.h"
#include "../panel_definitions.h"
#include "logo_panel.h"

namespace zlpanel {

    class StatePanel : public juce::Component {
    public:
        explicit StatePanel(juce::AudioProcessorValueTreeState &parameters);

        ~StatePanel() override;

        void paint(juce::Graphics &) override;

        void resized() override;

        void setFontSize(float fSize);

    private:
        LogoPanel logoPanel;
        std::unique_ptr<zlinterface::ButtonComponent> showCButton, showDButton, showMButton;
        std::array<std::unique_ptr<zlinterface::ButtonComponent>*, 3> buttonList{&showCButton, &showDButton, &showMButton};

        juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;
    };

} // zlpanel

#endif //ZLECOMP_STATE_PANEL_H
