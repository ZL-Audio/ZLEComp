// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_METER_PANEL_H
#define ZLECOMP_METER_PANEL_H

#include "../../DSP/Meter/meter.h"
#include "../../GUI/meter_component.h"
#include "../../GUI/interface_definitions.h"

namespace zlpanel {

    class MeterPanel : public juce::Component {
    public:
        explicit MeterPanel(zlmeter::MeterSource<float> *input,
                            zlmeter::MeterSource<float> *output);

        ~MeterPanel() override;

        void paint(juce::Graphics &) override;

        void resized() override;

        void setFontSize(float size);

    private:
        zlinterface::MeterComponent inputMeter, outputMeter;
    };

} // zlpanel

#endif //ZLECOMP_METER_PANEL_H
