// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_MONITOR_PANEL_H
#define ZLECOMP_MONITOR_PANEL_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "../../PluginProcessor.h"
#include "../../GUI/interface_definitions.h"
#include "../../DSP/Meter/meter.h"
#include "plot_panel.h"
#include "monitor_sub_panel.h"
#include <boost/circular_buffer.hpp>

namespace zlpanel {
    class MonitorPanel : public juce::Component, private juce::AsyncUpdater,
    public juce::AudioProcessorValueTreeState::Listener {
    public:
        explicit MonitorPanel(PluginProcessor &p, zlinterface::UIBase &base);

        ~MonitorPanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        MonitorSubPanel monitorSubPanel;
        std::unique_ptr<juce::VBlankAttachment> vBlankAttachment;
        auto static constexpr largePadding = 1.5f, smallPadding = 0.5f;
        PluginProcessor *processorRef;
        std::atomic<int> monitorSetting = zlstate::monitorSetting::defaultI;
        zlinterface::UIBase *uiBase;

        void handleAsyncUpdate() override;
    };

} // zlpanel

#endif //ZLECOMP_MONITOR_PANEL_H
