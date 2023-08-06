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
#include "../DSP/computer_attach.h"
#include "../DSP/detector_attach.h"
#include "../GUI/interface_definitions.h"
#include "logo_panel.h"
#include "plot_panel.h"

namespace panel {

    class MainPanel : public juce::Component {
    public:
        explicit MainPanel(PluginProcessor &p);

        ~MainPanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        LogoPanel logoPanel;
        ComputerPlotPanel computerPlotPanel;
        DetectorPlotPanel detectorPlotPanel;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
    };

} // panel

#endif //ZLECOMP_MAIN_PANEL_H
