// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_SETTING_PANEL_H
#define ZLECOMP_SETTING_PANEL_H

#include "global_setting_panel.h"
#include "computer_setting_panel.h"
#include "detector_setting_panel.h"

namespace zlpanel {

    class SettingPanel : public juce::Component {
    public:
        explicit SettingPanel(juce::AudioProcessorValueTreeState &parameters);

        ~SettingPanel() override;

        void paint(juce::Graphics &) override;

        void resized() override;

        void setFontSize(float fSize);

    private:
        GlobalSettingPanel globalSettingPanel;
        ComputerSettingPanel computerSettingPanel;
        DetectorSettingPanel detectorSettingPanel;
    };

} // zlpanel

#endif //ZLECOMP_SETTING_PANEL_H
