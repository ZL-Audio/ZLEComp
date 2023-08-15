// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_STATE_DEFINITIONS_H
#define ZLECOMP_STATE_DEFINITIONS_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <BinaryData.h>

namespace zlstate {
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
            auto attributes = juce::AudioParameterIntAttributes().withAutomatable(automate).withLabel(T::name);
            return std::make_unique<juce::AudioParameterInt>(juce::ParameterID(T::ID, versionHint), T::name,
                                                             T::minV, T::maxV, T::defaultV, attributes);
        }

        inline static float convertTo01(int x) {
            return static_cast<float>(x - T::minV) / static_cast<float>(T::maxV - T::minV);
        }
    };

    template<class T>
    class FloatParameters {
    public:
        static std::unique_ptr<juce::AudioParameterFloat> get(bool automate = true) {
            auto attributes = juce::AudioParameterFloatAttributes().withAutomatable(automate).withLabel(T::name);
            return std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(T::ID, versionHint), T::name,
                                                             T::range, T::defaultV, attributes);
        }

        inline static float convertTo01(float x) {
            return T::range.convertTo0to1(x);
        }
    };

    class programIdx : public IntParameters<programIdx> {
    public:
        auto static constexpr ID = "program_idx";
        auto static constexpr name = "NA_Program Idx";
        inline static const int minV = 0;
        inline static const int maxV = preset::presetNUM - 1;
        inline static const int defaultV = 0;
    };

    class windowW : public FloatParameters<windowW> {
    public:
        auto static constexpr ID = "window_w";
        auto static constexpr name = "NA_Window W";
        inline static const int minV = 476;
        inline static const int maxV = 1428;
        inline static const int defaultV = 476;
        inline auto static const range =
                juce::NormalisableRange<float>(minV, maxV, 1.f);
    };

    class windowH : public FloatParameters<windowH> {
    public:
        auto static constexpr ID = "window_h";
        auto static constexpr name = "NA_Window H";
        inline static const int minV = 443;
        inline static const int maxV = 1329;
        inline static const int defaultV = 443;
        inline auto static const range =
                juce::NormalisableRange<float>(minV, maxV, 1.f);
    };

    // bool
    template<class T>
    class BoolParameters {
    public:
        static std::unique_ptr<juce::AudioParameterBool> get(bool automate = true) {
            auto attributes = juce::AudioParameterBoolAttributes().withAutomatable(automate).withLabel(T::name);
            return std::make_unique<juce::AudioParameterBool>(juce::ParameterID(T::ID, versionHint), T::name,
                                                              T::defaultV, attributes);
        }

        static std::unique_ptr<juce::AudioParameterBool> get(juce::String label, bool automate = true) {
            auto attributes = juce::AudioParameterBoolAttributes().withAutomatable(automate).withLabel(label);
            return std::make_unique<juce::AudioParameterBool>(juce::ParameterID(T::ID, versionHint), T::name,
                                                              T::defaultV, attributes);
        }
    };

    class showComputer : public BoolParameters<showComputer> {
    public:
        auto static constexpr ID = "show_computer";
        auto static constexpr name = "NA";
        auto static constexpr defaultV = true;
    };

    class showDetector : public BoolParameters<showDetector> {
    public:
        auto static constexpr ID = "show_detector";
        auto static constexpr name = "NA";
        auto static constexpr defaultV = false;
    };

//    class showMonitor : public BoolParameters<showMonitor> {
//    public:
//        auto static constexpr ID = "show_monitor";
//        auto static constexpr name = "NA";
//        auto static constexpr defaultV = true;
//    };

    // choice
    template<class T>
    class ChoiceParameters {
    public:
        static std::unique_ptr<juce::AudioParameterChoice> get(bool automate = true) {
            auto attributes = juce::AudioParameterChoiceAttributes().withAutomatable(automate).withLabel(T::name);
            return std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(T::ID, versionHint), T::name, T::choices, T::defaultI, attributes);
        }

        static std::unique_ptr<juce::AudioParameterChoice> get(juce::String label, bool automate = true) {
            auto attributes = juce::AudioParameterChoiceAttributes().withAutomatable(automate).withLabel(label);
            return std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(T::ID, versionHint), T::name, T::choices, T::defaultI, attributes);
        }
    };

    class monitorSetting : public ChoiceParameters<monitorSetting> {
    public:
        auto static constexpr ID = "monitor_setting";
        auto static constexpr name = "NA";
        inline auto static const choices = juce::StringArray{"OFF", "30Hz M", "30Hz L", "60Hz M", "60Hz L"};
        int static constexpr defaultI = 1;
        enum {
            off, hz30m, hz30l, hz60m, hz60l, settingNUM
        };
    };

    inline juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(programIdx::get(false),
                   windowW::get(false), windowH::get(false),
                   showComputer::get("Computer", false),
                   showDetector::get("Detector", false),
                   monitorSetting::get("Monitor", false));
        return layout;
    }

}

#endif //ZLECOMP_STATE_DEFINITIONS_H
