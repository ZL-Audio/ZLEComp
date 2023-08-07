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

#include <juce_audio_processors/juce_audio_processors.h>
#include "../PluginProcessor.h"
#include "../GUI/interface_definitions.h"
#include "../DSP/Meter/meter.h"
#include "plot_panel.h"
#include <boost/circular_buffer.hpp>

namespace zlpanel {

    inline void plotY(juce::Graphics &g, juce::Rectangle<float> bound,
                      boost::circular_buffer<float> &y,
                      size_t xNum, float yMin, float yMax,
                      float thickness);

    class MonitorPanel : public juce::Component, private juce::Timer,
                         public juce::AudioProcessorValueTreeState::Listener {
    public:
        auto static constexpr timeInSeconds = 3;

        explicit MonitorPanel(PluginProcessor &p);

        ~MonitorPanel() override;

        void paint(juce::Graphics &g) override;

        void setFontSize(float fSize);

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        auto static constexpr largePadding = 1.5f, smallPadding = 0.5f;
        PluginProcessor *processorRef;
        std::atomic<bool> isMonitorVisible = zlstate::showMonitor::defaultV;
        zlmeter::MeterSource<float> *meterIn, *meterOut;
        float fontSize = 0.0f;
        boost::circular_buffer<float> rmsIn, rmsOut;
        void timerCallback() override;
    };

} // zlpanel

#endif //ZLECOMP_MONITOR_PANEL_H
