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
#include "../../GUI/combo_box_component.h"
#include "../panel_definitions.h"
#include "logo_panel.h"
#include "ui_state_panel.h"
#include "process_state_panel.h"

namespace zlpanel {

    class StatePanel : public juce::Component {
    public:
        explicit StatePanel(PluginProcessor &p, zlinterface::UIBase &base);

        ~StatePanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        LogoPanel logoPanel;
        ProcessStatePanel processStatePanel;
        UIStatePanel uiStatePanel;

        zlinterface::UIBase *uiBase;
    };

} // zlpanel

#endif //ZLECOMP_STATE_PANEL_H
