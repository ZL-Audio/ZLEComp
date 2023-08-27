// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZL_GLOBAL_SETTING_PANEL_H
#define ZL_GLOBAL_SETTING_PANEL_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../DSP/dsp_definitions.h"
#include "../../GUI/interface_definitions.h"
#include "../../GUI/linear_slider_component.h"
#include "../../GUI/rotary_slider_component.h"
#include "../../GUI/combo_box_component.h"
#include "../panel_definitions.h"

namespace zlpanel {

    class GlobalSettingPanel : public juce::Component {
    public:
        explicit GlobalSettingPanel(juce::AudioProcessorValueTreeState &parameters, zlinterface::UIBase &base);

        ~GlobalSettingPanel() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        std::unique_ptr<zlinterface::RotarySliderComponent> gainSlider, mixSlider;
        std::array<std::unique_ptr<zlinterface::RotarySliderComponent>*, 2> rotarySliderList{&gainSlider, &mixSlider};

        std::unique_ptr<zlinterface::LinearSliderComponent> rmsSlider, lookaheadSlider, segmentSlider;
        std::array<std::unique_ptr<zlinterface::LinearSliderComponent>*, 3> linearSliderList{&rmsSlider, &lookaheadSlider, &segmentSlider};

        juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;

        std::unique_ptr<zlinterface::ComboboxComponent> oversampleBox;
        std::array<std::unique_ptr<zlinterface::ComboboxComponent>*, 1> boxList{&oversampleBox};

        juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> boxAttachments;

        zlinterface::UIBase *uiBase;
    };

} // zlpanel

#endif //ZL_GLOBAL_SETTING_PANEL_H
