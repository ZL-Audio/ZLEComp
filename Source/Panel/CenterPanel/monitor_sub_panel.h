// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_MONITOR_SUB_PANEL_H
#define ZLECOMP_MONITOR_SUB_PANEL_H

#include "juce_audio_processors/juce_audio_processors.h"
#include <juce_opengl/juce_opengl.h>
#include "../../PluginProcessor.h"
#include "../../GUI/interface_definitions.h"
#include "../../DSP/Meter/meter.h"
#include "plot_panel.h"
#include <boost/circular_buffer.hpp>

namespace zlpanel {

    inline juce::Point<float> plotY(juce::Graphics &g, juce::Rectangle<float> bound,
                                    boost::circular_buffer<float> &y,
                                    size_t xNum, float yMin, float yMax,
                                    float thickness, std::optional<juce::Point<float>> startPoint = std::nullopt);

    class MonitorSubPanel : public juce::Component, private juce::Timer {
    public:
        auto static constexpr callBackHz = 180, upScaling = 2;
        auto static constexpr dummySize = 20;

        explicit MonitorSubPanel(PluginProcessor &p, zlinterface::UIBase &base);

        ~MonitorSubPanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void setMonitorVisible(bool f);

        void setTimeInSecond(float v);

    private:
        PluginProcessor *processorRef;
        std::atomic<bool> isMonitorVisible = true;
        zlmeter::MeterSource<float> *meterIn, *meterOut, *meterEnd;
        boost::circular_buffer<float> rmsIn, rmsOut, rmsDiff, peakStart, peakEnd;
        float totalDeltaX = 0.f;
        std::atomic<float> timeInSeconds = 7;

        void timerCallback() override;

        juce::Image image;
        juce::Time previousTime;
        juce::Point<float> lastInEndPoint, lastOutEndPoint, lastDiffEndPoint;
        juce::CriticalSection processLock, imageLock;

        zlinterface::UIBase *uiBase;
    };

} // zlpanel

#endif //ZLECOMP_MONITOR_SUB_PANEL_H
