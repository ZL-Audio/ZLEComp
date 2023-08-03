// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLINFLATOR_INTERFACE_DEFINES_H
#define ZLINFLATOR_INTERFACE_DEFINES_H

#include "juce_gui_basics/juce_gui_basics.h"

namespace interface {
    auto inline const WindowHeight = 280;
    auto inline const WindowWidth = 476;
    auto inline const WindowFixedAspectRatio = 1.7;
    auto inline const WindowMinHeight = 200;
    auto inline const WindowMinWidth = 340;
    auto inline const WindowMaxHeight = WindowMinHeight * 10;
    auto inline const WindowMaxWidth = WindowMinWidth * 10;


    auto inline const TextColor = juce::Colour(87, 96, 110);
    auto inline const TextInactiveColor = TextColor.withAlpha(0.5f);
    auto inline const TextHideColor = TextColor.withAlpha(0.25f);
    auto inline const BackgroundColor = juce::Colour(214, 223, 236);
    auto inline const BackgroundInactiveColor = BackgroundColor.withAlpha(0.8f);
    auto inline const BackgroundHideColor = BackgroundColor.withAlpha(0.5f);
    auto inline const DarkShadowColor = juce::Colour(168, 172, 178);
    auto inline const BrightShadowColor = juce::Colour(237, 246, 255);

    auto inline const FontTiny = 0.5f;
    auto inline const FontSmall = 0.75f;
    auto inline const FontNormal = 1.0f;
    auto inline const FontLarge = 1.25f;
    auto inline const FontHuge = 1.5f;
    auto inline const FontHuge2 = 3.0f;
    auto inline const FontHuge3 = 4.5f;

    auto inline const RefreshFreqHz = 60;

    inline juce::Rectangle<float> fillRoundedShadowRectangle(juce::Graphics &g,
                                                             juce::Rectangle<float> boxBounds,
                                                             float cornerSize,
                                                             bool curveTopLeft = true,
                                                             bool curveTopRight = true,
                                                             bool curveBottomLeft = true,
                                                             bool curveBottomRight = true,
                                                             bool fit = true,
                                                             bool drawBright = true, bool drawDark = true) {
        juce::Path path;
        auto radius = juce::jmax(juce::roundToInt(cornerSize * 0.75f), 1);
        if (fit) {
            boxBounds = boxBounds.withSizeKeepingCentre(
                    boxBounds.getWidth() - static_cast<float>(radius) - 1.42f * cornerSize,
                    boxBounds.getHeight() - static_cast<float>(radius) - 1.42f * cornerSize);
        }
        path.addRoundedRectangle(boxBounds.getX(), boxBounds.getY(),
                                 boxBounds.getWidth(), boxBounds.getHeight(),
                                 cornerSize, cornerSize,
                                 curveTopLeft, curveTopRight,
                                 curveBottomLeft, curveBottomRight);
        auto offset = static_cast<int>(cornerSize * 0.5f);
        if (drawBright) {
            juce::DropShadow brightShadow(BrightShadowColor, radius,
                                          {-offset, -offset});
            brightShadow.drawForPath(g, path);
        }
        if (drawDark) {
            juce::DropShadow darkShadow(DarkShadowColor, radius,
                                        {offset, offset});
            darkShadow.drawForPath(g, path);
        }
        g.setColour(BackgroundColor);
        g.fillPath(path);
        return boxBounds;
    }

    inline juce::Rectangle<float> fillRoundedInnerShadowRectangle(juce::Graphics &g,
                                                                  juce::Rectangle<float> boxBounds,
                                                                  float cornerSize,
                                                                  float blurRadius,
                                                                  bool curveTopLeft = true,
                                                                  bool curveTopRight = true,
                                                                  bool curveBottomLeft = true,
                                                                  bool curveBottomRight = true,
                                                                  bool flip = false) {
        juce::Path mask;
        mask.addRoundedRectangle(boxBounds.getX(), boxBounds.getY(),
                                 boxBounds.getWidth(), boxBounds.getHeight(),
                                 cornerSize, cornerSize,
                                 curveTopLeft, curveTopRight,
                                 curveBottomLeft, curveBottomRight);
        g.saveState();
        g.reduceClipRegion(mask);
        g.fillAll(BackgroundColor);
        auto offset = static_cast<int>(blurRadius * 1.5f);
        auto radius = juce::jmax(juce::roundToInt(blurRadius * 1.5f), 1);
        if (!flip) {
            juce::DropShadow darkShadow(DarkShadowColor.withAlpha(0.75f), radius,
                                        {-offset, -offset});
            darkShadow.drawForPath(g, mask);
            juce::DropShadow brightShadow(BrightShadowColor, radius,
                                          {offset, offset});
            brightShadow.drawForPath(g, mask);
        } else {
            juce::DropShadow brightShadow(DarkShadowColor, radius,
                                          {offset, offset});
            brightShadow.drawForPath(g, mask);
            juce::DropShadow darkShadow(BrightShadowColor.withAlpha(0.75f), radius,
                                        {-offset, -offset});
            darkShadow.drawForPath(g, mask);
        }
        boxBounds = boxBounds.withSizeKeepingCentre(
                boxBounds.getWidth() - 0.75f * static_cast<float>(radius),
                boxBounds.getHeight() - 0.75f * static_cast<float>(radius));
        juce::Path path;
        path.addRoundedRectangle(boxBounds.getX(), boxBounds.getY(),
                                 boxBounds.getWidth(), boxBounds.getHeight(),
                                 cornerSize, cornerSize,
                                 curveTopLeft, curveTopRight,
                                 curveBottomLeft, curveBottomRight);

        juce::DropShadow backShadow(BackgroundColor, radius,
                                    {0, 0});
        backShadow.drawForPath(g, path);
        g.restoreState();
        return boxBounds;
    }

    inline juce::Rectangle<float> drawShadowEllipse(juce::Graphics &g,
                                                    juce::Rectangle<float> boxBounds,
                                                    float cornerSize,
                                                    juce::Colour mainColour = BackgroundColor,
                                                    bool fit = true,
                                                    bool drawBright = true, bool drawDark = true) {
        juce::Path path;
        auto radius = juce::jmax(juce::roundToInt(cornerSize * 0.75f), 1);
        if (fit) {
            boxBounds = boxBounds.withSizeKeepingCentre(
                    boxBounds.getWidth() - static_cast<float>(radius) - 1.5f * cornerSize,
                    boxBounds.getHeight() - static_cast<float>(radius) - 1.5f * cornerSize);
        }
        path.addEllipse(boxBounds);
        auto offset = static_cast<int>(cornerSize * 0.5f);
        juce::Path mask;
        mask.addEllipse(boxBounds.withSizeKeepingCentre(boxBounds.getWidth() * 3, boxBounds.getHeight() * 3));
        mask.setUsingNonZeroWinding(false);
        mask.addEllipse(boxBounds);
        g.saveState();
        g.reduceClipRegion(mask);
        if (drawDark) {
            juce::DropShadow darkShadow(DarkShadowColor, radius,
                                        {offset, offset});
            darkShadow.drawForPath(g, path);
        }
        if (drawBright) {
            juce::DropShadow brightShadow(BrightShadowColor, radius,
                                          {-offset, -offset});
            brightShadow.drawForPath(g, path);
        }
        g.restoreState();
        g.setColour(mainColour);
        g.fillPath(path);
        return boxBounds;
    }

    inline juce::Rectangle<float> drawInnerShadowEllipse(juce::Graphics &g,
                                                         juce::Rectangle<float> boxBounds,
                                                         float cornerSize,
                                                         bool flip = false) {
        juce::Path mask;
        mask.addEllipse(boxBounds);
        g.saveState();
        g.reduceClipRegion(mask);
        g.fillAll(BackgroundColor);
        auto radius = juce::jmax(juce::roundToInt(cornerSize * 1.5f), 1);
        auto offset = static_cast<int>(cornerSize * 1.f);
        if (!flip) {
            juce::DropShadow darkShadow(DarkShadowColor.withAlpha(0.75f), radius,
                                        {-offset, -offset});
            darkShadow.drawForPath(g, mask);
            juce::DropShadow brightShadow(BrightShadowColor, radius,
                                          {offset, offset});
            brightShadow.drawForPath(g, mask);
        } else {
            juce::DropShadow brightShadow(DarkShadowColor, radius,
                                          {offset, offset});
            brightShadow.drawForPath(g, mask);
            juce::DropShadow darkShadow(BrightShadowColor.withAlpha(0.75f), radius,
                                        {-offset, -offset});
            darkShadow.drawForPath(g, mask);
        }
        boxBounds = boxBounds.withSizeKeepingCentre(
                boxBounds.getWidth() - 0.75f * static_cast<float>(radius),
                boxBounds.getHeight() - 0.75f * static_cast<float>(radius));
        juce::Path path;
        path.addEllipse(boxBounds);

        juce::DropShadow backShadow(BackgroundColor, radius,
                                    {0, 0});
        backShadow.drawForPath(g, path);
        g.restoreState();
        return boxBounds;
    }

    inline std::string formatFloat(float x, int precision) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << x;
        return stream.str();
    }
}

#endif //ZLINFLATOR_INTERFACE_DEFINES_H
