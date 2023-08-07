// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_CENTER_PANEL_H
#define ZLECOMP_CENTER_PANEL_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../PluginProcessor.h"
#include "../GUI/interface_definitions.h"
#include "../State/state_definitions.h"
#include "plot_panel.h"
#include "monitor_panel.h"

namespace zlpanel {

    class CenterPanel : public juce::Component {
    public:
        explicit CenterPanel(PluginProcessor &p);

        ~CenterPanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void setFontSize(float fSize);

    private:
        PluginProcessor *processorRef;
        PlotPanel plotPanel;
        MonitorPanel monitorPanel;
        float fontSize = 0.0f;
    };

} // zlpanel

#endif //ZLECOMP_CENTER_PANEL_H
