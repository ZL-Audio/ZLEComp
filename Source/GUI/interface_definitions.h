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

namespace zlinterface {
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

    struct fillRoundedShadowRectangleArgs {
        float blurRadius = 0.5f;
        bool curveTopLeft = true, curveTopRight = true, curveBottomLeft = true, curveBottomRight = true;
        bool fit = true, flip = false;
        bool drawBright = true, drawDark = true, drawMain = true;
        juce::Colour mainColour = BackgroundColor;
    };

    inline juce::Rectangle<float> getRoundedShadowRectangleArea(juce::Rectangle<float> boxBounds,
                                                                float cornerSize,
                                                                const fillRoundedShadowRectangleArgs &args) {
        auto radius = juce::jmax(juce::roundToInt(cornerSize * args.blurRadius * 1.5f), 1);
        return boxBounds.withSizeKeepingCentre(
                boxBounds.getWidth() - static_cast<float>(radius) - 1.42f * cornerSize,
                boxBounds.getHeight() - static_cast<float>(radius) - 1.42f * cornerSize);
    }

    inline juce::Rectangle<float> fillRoundedShadowRectangle(juce::Graphics &g,
                                                             juce::Rectangle<float> boxBounds,
                                                             float cornerSize,
                                                             const fillRoundedShadowRectangleArgs &args) {
        juce::Path path;
        auto radius = juce::jmax(juce::roundToInt(cornerSize * args.blurRadius * 1.5f), 1);
        if (args.fit) {
            boxBounds = boxBounds.withSizeKeepingCentre(
                    boxBounds.getWidth() - static_cast<float>(radius) - 1.42f * cornerSize,
                    boxBounds.getHeight() - static_cast<float>(radius) - 1.42f * cornerSize);
        }
        path.addRoundedRectangle(boxBounds.getX(), boxBounds.getY(),
                                 boxBounds.getWidth(), boxBounds.getHeight(),
                                 cornerSize, cornerSize,
                                 args.curveTopLeft, args.curveTopRight,
                                 args.curveBottomLeft, args.curveBottomRight);
        auto offset = static_cast<int>(cornerSize * args.blurRadius);
        juce::Path mask(path);
        mask.setUsingNonZeroWinding(false);
        mask.addRectangle(boxBounds.withSizeKeepingCentre(boxBounds.getWidth() + cornerSize * 3,
                                                          boxBounds.getHeight() + cornerSize * 3));
        g.saveState();
        g.reduceClipRegion(mask);
        if (args.drawBright) {
            juce::DropShadow brightShadow(BrightShadowColor, radius,
                                          {-offset, -offset});
            brightShadow.drawForPath(g, path);
        }
        if (args.drawDark) {
            juce::DropShadow darkShadow(DarkShadowColor, radius,
                                        {offset, offset});
            darkShadow.drawForPath(g, path);
        }
        g.restoreState();
        if (args.drawMain) {
            g.setColour(args.mainColour);
            g.fillPath(path);
        }
        return boxBounds;
    }

    inline juce::Rectangle<float> fillRoundedInnerShadowRectangle(juce::Graphics &g,
                                                                  juce::Rectangle<float> boxBounds,
                                                                  float cornerSize,
                                                                  const fillRoundedShadowRectangleArgs &args) {
        juce::Path mask;
        mask.addRoundedRectangle(boxBounds.getX(), boxBounds.getY(),
                                 boxBounds.getWidth(), boxBounds.getHeight(),
                                 cornerSize, cornerSize,
                                 args.curveTopLeft, args.curveTopRight,
                                 args.curveBottomLeft, args.curveBottomRight);
        g.saveState();
        g.reduceClipRegion(mask);
        if (args.drawMain) {
            g.fillAll(args.mainColour);
        }
        auto offset = static_cast<int>(cornerSize * args.blurRadius);
        auto radius = juce::jmax(juce::roundToInt(cornerSize * args.blurRadius * 1.5f), 1);
        if (!args.flip) {
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
                                 args.curveTopLeft, args.curveTopRight,
                                 args.curveBottomLeft, args.curveBottomRight);

        juce::DropShadow backShadow(BackgroundColor, radius,
                                    {0, 0});
        backShadow.drawForPath(g, path);
        g.restoreState();
        return boxBounds;
    }

    struct fillShadowEllipseArgs {
        float blurRadius = 0.5f;
        bool fit = true, flip = false;
        bool drawBright = true, drawDark = true;
        juce::Colour mainColour = BackgroundColor;
    };

    inline juce::Rectangle<float> drawShadowEllipse(juce::Graphics &g,
                                                    juce::Rectangle<float> boxBounds,
                                                    float cornerSize,
                                                    const fillShadowEllipseArgs &args) {
        juce::Path path;
        auto radius = juce::jmax(juce::roundToInt(cornerSize * 0.75f), 1);
        if (args.fit) {
            boxBounds = boxBounds.withSizeKeepingCentre(
                    boxBounds.getWidth() - static_cast<float>(radius) - 1.5f * cornerSize,
                    boxBounds.getHeight() - static_cast<float>(radius) - 1.5f * cornerSize);
        }
        path.addEllipse(boxBounds);
        auto offset = static_cast<int>(cornerSize * args.blurRadius);
        juce::Path mask;
        mask.addEllipse(boxBounds.withSizeKeepingCentre(boxBounds.getWidth() * 3, boxBounds.getHeight() * 3));
        mask.setUsingNonZeroWinding(false);
        mask.addEllipse(boxBounds);
        g.saveState();
        g.reduceClipRegion(mask);
        if (args.drawDark) {
            juce::DropShadow darkShadow(DarkShadowColor, radius,
                                        {offset, offset});
            darkShadow.drawForPath(g, path);
        }
        if (args.drawBright) {
            juce::DropShadow brightShadow(BrightShadowColor, radius,
                                          {-offset, -offset});
            brightShadow.drawForPath(g, path);
        }
        g.restoreState();
        g.setColour(args.mainColour);
        g.fillPath(path);
        return boxBounds;
    }

    inline juce::Rectangle<float> drawInnerShadowEllipse(juce::Graphics &g,
                                                         juce::Rectangle<float> boxBounds,
                                                         float cornerSize,
                                                         const fillShadowEllipseArgs &args) {
        juce::Path mask;
        mask.addEllipse(boxBounds);
        g.saveState();
        g.reduceClipRegion(mask);
        g.fillAll(BackgroundColor);
        auto radius = juce::jmax(juce::roundToInt(cornerSize * 1.5f), 1);
        auto offset = static_cast<int>(cornerSize * args.blurRadius);
        if (!args.flip) {
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
        precision = std::max(0, precision);
        stream << std::fixed << std::setprecision(precision) << x;
        return stream.str();
    }

    inline std::string fixFormatFloat(float x, int length) {
        auto y = std::abs(x);
        if (y < 10) {
            return formatFloat(x, length - 1);
        } else if (y < 100) {
            return formatFloat(x, length - 2);
        } else if (y < 1000) {
            return formatFloat(x, length - 3);
        } else if (y < 10000) {
            return formatFloat(x, length - 4);
        } else {
            return formatFloat(x, length - 5);
        }
    }
}

#endif //ZLINFLATOR_INTERFACE_DEFINES_H
