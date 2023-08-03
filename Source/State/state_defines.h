// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_STATE_DEFINES_H
#define ZLECOMP_STATE_DEFINES_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <BinaryData.h>

namespace state {
    class preset {
    public:
        enum {
            nothing,
            defaults,
            halfRMS,
            presetNUM
        };
        inline const static std::array xmls{BinaryData::nothing_xml, BinaryData::default_xml,
                                            BinaryData::half_rms_m_xml};
        inline const static std::array names{"Nothing", "Default", "Half RMS"};
    };

    // int
    inline auto static const versionHint = 1;

    template<class T>
    class IntParameters {
    public:
        static std::unique_ptr<juce::AudioParameterInt> get(bool automate = true) {
            auto attributes = juce::AudioParameterIntAttributes().withAutomatable(automate);
            return std::make_unique<juce::AudioParameterInt>(juce::ParameterID(T::ID, versionHint), T::name,
                                                             T::minV, T::maxV, T::defaultV, attributes);
        }
    };

    class programIdx : public IntParameters<programIdx> {
    public:
        auto static constexpr ID = "program_idx";
        auto static constexpr name = "Program Idx";
        inline auto static const minV = 0;
        inline auto static const maxV = preset::presetNUM - 1;
        auto static constexpr defaultV = 0;
    };

    inline juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(programIdx::get(false));
        return layout;
    }
}

#endif //ZLECOMP_STATE_DEFINES_H
