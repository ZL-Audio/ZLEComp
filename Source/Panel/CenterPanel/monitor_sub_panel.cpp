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
        g.strokePath(path, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));
        return path.getCurrentPosition();
    }

    MonitorSubPanel::MonitorSubPanel(PluginProcessor &p) :
            image(juce::Image::ARGB, 100, 100, true) {
//        openGLContext.attachTo(*getTopLevelComponent());
        processorRef = &p;

        meterIn = &p.getMeterIn();
        meterOut = &p.getMeterOut();
        rmsIn.set_capacity(callBackHz * timeInSeconds * 3);
        rmsOut.set_capacity(callBackHz * timeInSeconds * 3);
        rmsDiff.set_capacity(callBackHz * timeInSeconds * 3);
        rmsIn.push_back(-60);
        rmsOut.push_back(-60);
        rmsDiff.push_back(0);
        startTimerHz(callBackHz);
        setSize(500, 500);
    }

    MonitorSubPanel::~MonitorSubPanel() {
//        openGLContext.detach();
        stopTimer();
    }

    void MonitorSubPanel::paint(juce::Graphics &g) {
        if (isMonitorVisible.load()) {
            // draw boundary
            auto bound = getLocalBounds().toFloat();
            auto thickness = fontSize * 0.125f;
            // calculate time difference
            auto currentTime = juce::Time::getCurrentTime();
            auto relativeTime = currentTime - previousTime;
            previousTime = currentTime;
            if (relativeTime.inDays() >= 1) {
                return;
            }
            // draw the old part
            auto tempImage = juce::Image(juce::Image::ARGB, image.getWidth(), image.getHeight(), true);
            auto tempG = juce::Graphics(tempImage);
            tempG.setOpacity(1.0f);
            auto deltaX = static_cast<float>(relativeTime.inSeconds() / timeInSeconds);
            auto area = image.getBounds().toFloat();
            deltaX = deltaX * area.getWidth();
//        auto oldArea = area.withTrimmedLeft(deltaX).toNearestInt();
//        deltaX = area.getWidth() - static_cast<float>(oldArea.getWidth());
            lastInEndPoint = lastInEndPoint.translated(-deltaX - thickness * 0.1f, 0);
            lastOutEndPoint = lastOutEndPoint.translated(-deltaX - thickness * 0.1f, 0);
            lastDiffEndPoint = lastDiffEndPoint.translated(-deltaX - thickness * 0.1f, 0);
//        auto oldImage = image.getClippedImage(oldArea);
//        tempG.drawImageAt(image, -deltaX, 0);
            tempG.drawImageTransformed(image, juce::AffineTransform::translation(-deltaX, 0));
            // draw the new part
            if (!rmsIn.empty()) {
                auto tempBound = image.getBounds().toFloat();
                tempBound = tempBound.withTrimmedLeft(
                        juce::jmax(tempBound.getWidth() - deltaX,// - upScaling * fontSize * 0.075f,
                                   0.f));
                tempG.setColour(zlinterface::TextHideColor);
                lastInEndPoint = plotY(tempG, tempBound,
                                       rmsIn,
                                       rmsIn.size(), -60.f, 0.f,
                                       thickness * upScaling * 0.6f, lastInEndPoint);
                tempG.setColour(zlinterface::TextColor);
                lastOutEndPoint = plotY(tempG, tempBound,
                                        rmsOut,
                                        rmsOut.size(), -60.f, 0.f,
                                        thickness * upScaling * 0.6f, lastOutEndPoint);
                tempG.setColour(juce::Colours::darkred);
                lastDiffEndPoint = plotY(tempG, tempBound,
                                         rmsDiff,
                                         rmsDiff.size(), -60.f, 0.f,
                                         thickness * upScaling, lastDiffEndPoint);
                while (rmsIn.size() > 0) {
                    rmsIn.pop_front();
                    rmsOut.pop_front();
                    rmsDiff.pop_front();
                }
            }
            // update image
            image = tempImage;
            image.duplicateIfShared();
            // draw image to panel
            g.setOpacity(1.0f);
//        g.drawImageAt(image, bound.toNearestInt().getX(), bound.toNearestInt().getY());
            g.drawImage(image, bound);
        }
    }

    void MonitorSubPanel::resized() {
        auto bound = getLocalBounds().toFloat();
        bound = bound.withSize(bound.getWidth() * upScaling, bound.getHeight() * upScaling);
        image = image.rescaled(bound.toNearestInt().getWidth(),
                               bound.toNearestInt().getHeight());
        bound = juce::Rectangle<float>(bound.getWidth(), bound.getHeight());
        lastInEndPoint = bound.getBottomRight();
        lastOutEndPoint = bound.getBottomRight();
        lastDiffEndPoint = bound.getTopRight();
//        image = image.rescaled(getWidth() * upScaling, getHeight() * upScaling);
    }

    void MonitorSubPanel::setFontSize(float fSize) {
        fontSize = fSize;
    }

    void MonitorSubPanel::setMonitorVisible(bool f) {
        isMonitorVisible.store(f);
    }

    void MonitorSubPanel::timerCallback() {
        auto num = meterIn->appendHistory(rmsIn);
        meterOut->appendHistory(rmsOut, num);
        for (size_t i = rmsIn.size() - num; i < rmsIn.size(); ++i) {
            rmsDiff.push_back(rmsOut[i] - rmsIn[i]);
        }
    }
} // zlpanel