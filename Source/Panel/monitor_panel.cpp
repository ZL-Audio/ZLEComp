// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "monitor_panel.h"

namespace zlpanel {
    void plotY(juce::Graphics &g, juce::Rectangle<float> bound,
               boost::circular_buffer<float> &y,
               size_t xNum, float yMin, float yMax,
               float thickness) {
        if (y.empty()) {
            return;
        }
        juce::Path path;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness, bound.getHeight() - thickness);
        path.startNewSubPath(getPointX(bound, static_cast<float>(xNum - y.size()), 0, static_cast<float>(xNum - 1)),
                             getPointY(bound, y[0], yMin, yMax));
        for (size_t i = 1; i < y.size(); i++) {
            path.lineTo(getPointX(bound, static_cast<float>(xNum - y.size() + i), 0, static_cast<float>(xNum - 1)),
                        getPointY(bound, y[i], yMin, yMax));
        }
        g.strokePath(path, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));
    }

    MonitorPanel::MonitorPanel(PluginProcessor &p) :
            image(juce::Image::ARGB, 100, 100, true) {
        processorRef = &p;
        meterIn = &p.getMeterIn();
        meterOut = &p.getMeterOut();
        rmsIn.set_capacity(callBackHz * timeInSeconds * 3);
        rmsOut.set_capacity(callBackHz * timeInSeconds * 3);
        rmsDiff.set_capacity(callBackHz * timeInSeconds * 3);
        startTimerHz(callBackHz);
    }

    MonitorPanel::~MonitorPanel() {
        stopTimer();
    }

    void MonitorPanel::paint(juce::Graphics &g) {
        // draw boundary
        auto bound = getLocalBounds().toFloat();
        bound = zlinterface::fillRoundedShadowRectangle(g, bound, 0.5f * fontSize, {.blurRadius=0.25f});
        bound = bound.withTrimmedLeft(
                fontSize * largePadding).withTrimmedBottom(
                fontSize * largePadding).withTrimmedRight(
                fontSize * largePadding).withTrimmedTop(
                fontSize * smallPadding);
        g.setColour(zlinterface::TextInactiveColor);
        auto thickness = fontSize * 0.1f;
        g.drawRect(bound, thickness);
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness,
                                            bound.getHeight() - thickness);
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
        area = area.withTrimmedLeft(deltaX * area.getWidth());
        auto oldImage = image.getClippedImage(area.toNearestInt());
        tempG.drawImageAt(oldImage, 0, 0);
        // draw the new part
        auto tempBound = image.getBounds().toFloat();
        tempBound = tempBound.withTrimmedLeft(
                juce::jmax((1 - deltaX) * tempBound.getWidth() - upScaling * fontSize * 0.15f,
                           0.f));
        tempG.setColour(zlinterface::TextHideColor);
        plotY(tempG, tempBound, rmsIn, rmsIn.size(), -60.f, 0.f, thickness * upScaling);
        tempG.setColour(zlinterface::TextColor);
        plotY(tempG, tempBound, rmsOut, rmsOut.size(), -60.f, 0.f, thickness * upScaling);
        tempG.setColour(juce::Colours::darkred);
        plotY(tempG, tempBound, rmsDiff, rmsDiff.size(), -60.f, 0.f, thickness * upScaling);
        while (rmsIn.size() > 1) {
            rmsIn.pop_front();
            rmsOut.pop_front();
            rmsDiff.pop_front();
        }
        // update image
        image = tempImage;
        image.duplicateIfShared();
        // draw image to panel
        g.setOpacity(1.0f);
        g.drawImageAt(image, bound.toNearestInt().getX(), bound.toNearestInt().getY());



//        auto bound = getLocalBounds().toFloat();
//        bound = zlinterface::fillRoundedShadowRectangle(g, bound, 0.5f * fontSize, {.blurRadius=0.25f});
//        bound = bound.withTrimmedLeft(
//                fontSize * largePadding).withTrimmedBottom(
//                fontSize * largePadding).withTrimmedRight(
//                fontSize * largePadding).withTrimmedTop(
//                fontSize * smallPadding);
//        g.setColour(zlinterface::TextInactiveColor);
//
//        auto thickness = fontSize * 0.1f;
//        g.drawRect(bound, thickness);
//
//        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness,
//                                            bound.getHeight() - thickness);
//
//        thickness = thickness * 0.75f;
//        g.setColour(zlinterface::TextHideColor);
//        plotY(g, bound, rmsIn, zlinterface::RefreshFreqHz * timeInSeconds, -60.f, 0.f, thickness);
//        g.setColour(zlinterface::TextColor);
//        plotY(g, bound, rmsOut, zlinterface::RefreshFreqHz * timeInSeconds, -60.f, 0.f, thickness);
//        g.setColour(juce::Colours::darkred);
//        plotY(g, bound, rmsDiff, zlinterface::RefreshFreqHz * timeInSeconds, -60.f, 0.f, thickness);

//        juce::Image image = juce::Image(juce::Image::PixelFormat::ARGB, timeInSeconds * callBackHz, 60, true);
//        for (size_t i = 0; i < rmsIn.size(); ++i) {
//            image.setPixelAt(callBackHz * timeInSeconds - static_cast<int>(rmsIn.size()) + static_cast<int>(i + 1),
//                             static_cast<int>(-rmsIn[i]), zlinterface::TextHideColor);
//        }
//        g.drawImage(image, bound);
    }

    void MonitorPanel::resized() {
//        image = image.rescaled(getWidth() * upScaling, getHeight() * upScaling);
    }

    void MonitorPanel::setFontSize(float fSize) {
        fontSize = fSize;
        auto bound = getLocalBounds().toFloat();
        bound = zlinterface::getRoundedShadowRectangleArea(bound, 0.5f * fontSize, {.blurRadius=0.25f});
        bound = bound.withTrimmedLeft(
                fontSize * largePadding).withTrimmedBottom(
                fontSize * largePadding).withTrimmedRight(
                fontSize * largePadding).withTrimmedTop(
                fontSize * smallPadding);
        auto thickness = fontSize * 0.1f;
        bound = bound.withSizeKeepingCentre(bound.getWidth() - thickness,
                                            bound.getHeight() - thickness);
        image = image.rescaled(bound.toNearestInt().getWidth() * upScaling,
                               bound.toNearestInt().getHeight() * upScaling);

    }

    void MonitorPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<bool>(newValue);
        if (parameterID == zlstate::showMonitor::ID) {
            isMonitorVisible.store(v);
        }
    }

    void MonitorPanel::timerCallback() {
        auto num = meterIn->appendHistory(rmsIn);
        meterOut->appendHistory(rmsOut, num);
        for (size_t i = rmsIn.size() - num; i < rmsIn.size(); ++i) {
            rmsDiff.push_back(rmsOut[i] - rmsIn[i]);
        }

//        auto inV = meterIn->getCurrentMeanPeak(), outV = meterOut->getCurrentMeanPeak();
//        rmsIn.push_back(inV);
//        rmsOut.push_back(outV);
//        rmsDiff.push_back(outV - inV);
//        repaint();
    }
} // zlpanel