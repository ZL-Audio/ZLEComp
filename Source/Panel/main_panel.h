// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_MAIN_PANEL_H
#define ZLECOMP_MAIN_PANEL_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../PluginProcessor.h"
#include "../GUI/interface_definitions.h"
#include "StatePanel/state_panel.h"
#include "CenterPanel/center_panel.h"
#include "SettingPanel/setting_panel.h"
// #include "MeterPanel/meter_panel.h"

namespace zlpanel {

    class MainPanel : public juce::Component {
    public:
        explicit MainPanel(PluginProcessor &p);

        void attachOpenGL(juce::Component &component);

        ~MainPanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        zlinterface::UIBase uiBase;

        StatePanel statePanel;
        CenterPanel centerPanel;
        SettingPanel settingPanel;
//        MeterPanel meterPanel;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
    };

} // panel

#endif //ZLECOMP_MAIN_PANEL_H
