// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLLMATCH_LOGOPANEL_H
#define ZLLMATCH_LOGOPANEL_H

#include <BinaryData.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace panel {
    class LogoPanel : public juce::Component {
    public:
        explicit LogoPanel();

        ~LogoPanel() override;

        void paint(juce::Graphics &g) override;

    private:
        const std::unique_ptr<juce::Drawable> brandDrawable, logoDrawable;
    };
}
#endif //ZLLMATCH_LOGOPANEL_H
