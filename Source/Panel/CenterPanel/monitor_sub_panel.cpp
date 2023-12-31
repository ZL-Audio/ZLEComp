// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "monitor_sub_panel.h"

namespace zlpanel {
    juce::Point<float> plotY(juce::Graphics &g, juce::Rectangle<float> bound,
                             boost::circular_buffer<float> &y,
                             size_t xNum, float yMin, float yMax,
                             float thickness, std::optional<juce::Point<float>> startPoint) {
        if (y.empty()) {
            return {0.f, 0.f};
        }
        juce::Path path;
        if (startPoint == std::nullopt) {
            path.startNewSubPath(getPointX(bound, static_cast<float>(xNum - y.size()), 0, static_cast<float>(xNum - 1)),
                                 getPointY(bound, y[0], yMin, yMax));
            for (size_t i = 1; i < y.size(); i++) {
                path.lineTo(getPointX(bound, static_cast<float>(xNum - y.size() + i), 0, static_cast<float>(xNum - 1)),
                            getPointY(bound, y[i], yMin, yMax));
            }
        } else {
            path.startNewSubPath(startPoint->getX(), startPoint->getY());
            for (size_t i = 0; i < y.size(); i++) {
                path.lineTo(getPointX(bound, static_cast<float>(i + 1), 0, static_cast<float>(xNum)),
                            getPointY(bound, y[i], yMin, yMax));
            }

        }
        g.strokePath(path, juce::PathStrokeType(thickness, juce::PathStrokeType::beveled,
                                                juce::PathStrokeType::rounded));
        return path.getCurrentPosition();
    }

    MonitorSubPanel::MonitorSubPanel(PluginProcessor &p, zlinterface::UIBase &base) :
            image(juce::Image::ARGB, 100, 100, true) {
        processorRef = &p;
        uiBase = &base;

        const juce::GenericScopedLock<juce::CriticalSection> myScopedLock(p.getCallbackLock());
        meterIn = &p.getMeterIn();
        meterIn->resetHistory();
        meterOut = &p.getMeterOut();
        meterOut->resetHistory();
        meterEnd = &p.getMeterEnd();
        meterEnd->resetHistory();
        const juce::GenericScopedUnlock<juce::CriticalSection> myScopedUnlock(p.getCallbackLock());

        rmsIn.set_capacity(10 * 50);
        rmsOut.set_capacity(10 * 50);
        rmsDiff.set_capacity(10 * 50);
        peakStart.set_capacity(10 * 50);
        peakEnd.set_capacity(10 * 50);

        rmsIn.push_back(-60);
        rmsOut.push_back(-60);
        rmsDiff.push_back(0);
        peakStart.push_back(-60);
        peakEnd.push_back(-60);

        startTimerHz(callBackHz);
    }

    MonitorSubPanel::~MonitorSubPanel() {
        stopTimer();
    }

    void MonitorSubPanel::paint(juce::Graphics &g) {
        if (isMonitorVisible.load()) {
            auto thickness = uiBase->getFontSize() * 0.175f;
            // calculate time difference
            auto currentTime = juce::Time::getCurrentTime();
            auto relativeTime = currentTime - previousTime;
            previousTime = currentTime;
            if (relativeTime.inDays() >= 1) {
                return;
            }
            // draw the old part
            const juce::GenericScopedLock<juce::CriticalSection> imageScopedLock(imageLock);
            auto tempImage = juce::Image(juce::Image::ARGB, image.getWidth(), image.getHeight(), true);
            auto tempG = juce::Graphics(tempImage);
            tempG.setOpacity(1.0f);
            auto area = image.getBounds().toFloat();
            auto deltaX = static_cast<float>(relativeTime.inSeconds() / timeInSeconds);
            deltaX = deltaX * (area.getWidth() - dummySize);
            deltaX = static_cast<float>(juce::roundToInt(deltaX));
            lastInEndPoint = lastInEndPoint.translated(-deltaX, 0);
            lastOutEndPoint = lastOutEndPoint.translated(-deltaX, 0);
            lastDiffEndPoint = lastDiffEndPoint.translated(-deltaX, 0);
            tempG.drawImageAt(image, juce::roundToInt(-deltaX), 0);
            totalDeltaX += deltaX;
            // draw the new part
            if (!rmsIn.empty()) {
                auto tempBound = image.getBounds().toFloat();

                tempBound = tempBound.withTrimmedLeft(
                        juce::jmax(tempBound.getWidth() - dummySize - totalDeltaX, 0.f));
                totalDeltaX = 0.f;
                tempBound = tempBound.withTrimmedRight(dummySize);
                const juce::GenericScopedLock<juce::CriticalSection> processScopedLock (processLock);
                tempG.setColour(uiBase->getTextInactiveColor());
                lastInEndPoint = plotY(tempG, tempBound,
                                       peakStart,
                                       peakStart.size(), -60.f, 0.f,
                                       thickness * upScaling * 0.65f, lastInEndPoint);
                tempG.setColour(uiBase->getTextColor());
                lastOutEndPoint = plotY(tempG, tempBound,
                                        peakEnd,
                                        peakEnd.size(), -60.f, 0.f,
                                        thickness * upScaling * 0.65f, lastOutEndPoint);
                tempG.setColour(uiBase->getLineColor1());
                lastDiffEndPoint = plotY(tempG, tempBound,
                                         rmsDiff,
                                         rmsDiff.size(), -60.f, 0.f,
                                         thickness * upScaling, lastDiffEndPoint);
                rmsIn.clear();
                rmsOut.clear();
                rmsDiff.clear();
                peakStart.clear();
                peakEnd.clear();
            }
            // update image
            image = tempImage;
            image.duplicateIfShared();
            auto imageToDraw = image.getClippedImage(image.getBounds().withTrimmedRight(dummySize));
            // draw image to panel
            g.setOpacity(1.0f);
            g.drawImage(imageToDraw, getLocalBounds().toFloat());
        }
    }

    void MonitorSubPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = bound.withSize(bound.getWidth() * upScaling, bound.getHeight() * upScaling);
        const juce::GenericScopedLock<juce::CriticalSection> imageScopedLock(imageLock);
        image.clear(image.getBounds());
        image = image.rescaled(bound.toNearestInt().getWidth() + dummySize,
                               bound.toNearestInt().getHeight());
        lastInEndPoint = bound.getBottomRight();
        lastOutEndPoint = bound.getBottomRight();
        lastDiffEndPoint = bound.getTopRight();
    }

    void MonitorSubPanel::setMonitorVisible(bool f) {
        isMonitorVisible.store(f);
        if (f) {
            startTimerHz(callBackHz);
        } else {
            stopTimer();
        }
    }

    void MonitorSubPanel::setTimeInSecond(float v) {
        timeInSeconds.store(v);
    }

    void MonitorSubPanel::timerCallback() {
        const juce::GenericScopedLock<juce::CriticalSection> processScopedLock (processLock);
        auto num = meterIn->appendHistoryRMS(rmsIn);
        meterOut->appendHistoryRMS(rmsOut, num);
        for (size_t i = rmsIn.size() - num; i < rmsIn.size(); ++i) {
            rmsDiff.push_back(rmsOut[i] - rmsIn[i]);
        }
        meterIn->appendHistoryPeak(peakStart, num);
        meterEnd->appendHistoryPeak(peakEnd, num);
    }
} // zlpanel