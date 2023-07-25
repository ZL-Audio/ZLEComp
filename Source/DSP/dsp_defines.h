// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLLMAKEUP_DSP_DEFINES_H
#define ZLLMAKEUP_DSP_DEFINES_H

#include <juce_audio_processors/juce_audio_processors.h>

namespace ZLDsp {
    // float
    template<class T>
    class FloatParameters {
    public:
        static std::unique_ptr<juce::AudioParameterFloat> get() {
            return std::make_unique<juce::AudioParameterFloat>(T::ID, T::name, T::range,
                                                               T::defaultV);
        }
    };

    class threshold : public FloatParameters<threshold> {
    public:
        auto static constexpr ID = "threshold";
        auto static constexpr name = "Threshold (dB)";
        inline auto static const range =
                juce::NormalisableRange<float>(-60.f, 0.f, .1f);
        auto static constexpr defaultV = -18.f;
    };

    class ratio : public FloatParameters<ratio> {
    public:
        auto static constexpr ID = "ratio";
        auto static constexpr name = "Ratio";
        auto static constexpr defaultV = 4.f;
        inline static const juce::NormalisableRange<float>::ValueRemapFunction from01 = [](float start, float end, float x) {
            return std::pow(2, x * 10) / 8;
        };
        inline static const juce::NormalisableRange<float>::ValueRemapFunction to01 = [](float start, float end, float x) {
            return std::log2(x * 8) / 10;
        };
        inline static const juce::NormalisableRange<float>::ValueRemapFunction snap = [](float start, float end, float x) {
            if (x < 100) {
                return std::floor(x * 100) / 100;
            } else {
                return std::floor(x * 10) / 10;
            }
        };
        inline auto static const range =
                juce::NormalisableRange<float>(1.f, 100.f, from01, to01, snap);
    };

    class kneeW : public FloatParameters<kneeW> {
    public:
        auto static constexpr ID = "knee_width";
        auto static constexpr name = "Knee Width";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 0.f;
        inline static float formatV(float x) {return std::max(x * 60, 0.0625f);}
        inline static double formatV(double x) {return std::max(x * 60, 0.0625);}
    };

    class kneeD : public FloatParameters<kneeD> {
    public:
        auto static constexpr ID = "knee_depth";
        auto static constexpr name = "Knee Depth";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 0.5f;
    };

    class kneeS : public FloatParameters<kneeS> {
    public:
        auto static constexpr ID = "knee_slope";
        auto static constexpr name = "Knee Slope";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 0.5f;
    };

    class bound : public FloatParameters<bound> {
    public:
        auto static constexpr ID = "bound";
        auto static constexpr name = "Bound (dB)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 60.f, .1f);
        auto static constexpr defaultV = 60.f;
    };

    class sideGain : public FloatParameters<sideGain> {
    public:
        auto static constexpr ID = "side_gain";
        auto static constexpr name = "Side Gain (dB)";
        inline auto static const range =
                juce::NormalisableRange<float>(-9.f, 9.f, .1f);
        auto static constexpr defaultV = 0.f;
    };

    class attack : public FloatParameters<attack> {
    public:
        auto static constexpr ID = "attack";
        auto static constexpr name = "Attack (ms)";
        inline static const juce::NormalisableRange<float>::ValueRemapFunction from01 = [](float start, float end, float x) {
            return (std::pow(2, 8 * x) - 1) * 2;
        };
        inline static const juce::NormalisableRange<float>::ValueRemapFunction to01 = [](float start, float end, float x) {
            return std::log2(x / 2 + 1) / 8;
        };
        inline static const juce::NormalisableRange<float>::ValueRemapFunction snap = [](float start, float end, float x) {
            if (x < 100) {
                return std::floor(x * 100) / 100;
            } else {
                return std::floor(x * 10) / 10;
            }
        };
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 510.f, from01, to01, snap);
        auto static constexpr defaultV = 10.f;
    };

    class release : public FloatParameters<release> {
    public:
        auto static constexpr ID = "release";
        auto static constexpr name = "Release (ms)";
        inline static const juce::NormalisableRange<float>::ValueRemapFunction from01 = [](float start, float end, float x) {
            return (std::pow(2, 8 * x) - 1) * 20;
        };
        inline static const juce::NormalisableRange<float>::ValueRemapFunction to01 = [](float start, float end, float x) {
            return std::log2(x / 20 + 1) / 8;
        };
        inline static const juce::NormalisableRange<float>::ValueRemapFunction snap = [](float start, float end, float x) {
            if (x < 100) {
                return std::floor(x * 100) / 100;
            } else if (x < 1000) {
                return std::floor(x * 10) / 10;
            } else {
                return std::floor(x);
            }
        };
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 5100.f, from01, to01, snap);
        auto static constexpr defaultV = 100.f;
    };

    class smooth : public FloatParameters<smooth> {
    public:
        auto static constexpr ID = "smooth";
        auto static constexpr name = "Smooth";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 0.0f;
    };

    class link : public FloatParameters<link> {
    public:
        auto static constexpr ID = "link";
        auto static constexpr name = "Stereo Link";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 1.0f;
    };

    // bool
    template<class T>
    class BoolParameters {
    public:
        static std::unique_ptr<juce::AudioParameterBool> get() {
            return std::make_unique<juce::AudioParameterBool>(T::ID, T::name,
                                                              T::defaultV);
        }
    };

    class external : public BoolParameters<external> {
    public:
        auto static constexpr ID = "external";
        auto static constexpr name = "External";
        auto static constexpr defaultV = false;
    };

    class audit : public BoolParameters<audit> {
    public:
        auto static constexpr ID = "audit";
        auto static constexpr name = "Audit";
        auto static constexpr defaultV = false;
    };

    // choice
    template<class T>
    class ChoiceParameters {
    public:
        static std::unique_ptr<juce::AudioParameterChoice> get() {
            return std::make_unique<juce::AudioParameterChoice>(
                    T::ID, T::name, T::choices, T::defaultI);
        }
    };

    class aStyle : public ChoiceParameters<aStyle> {
    public:
        auto static constexpr ID = "a_style";
        auto static constexpr name = "Style";
        inline auto static const choices = juce::StringArray{"Classic", "1", "2", "3", "4"};
        int static constexpr defaultI = 0;
        enum {
            classic, style1, style2, style3, style4, styleNUM
        };
    };

    class rStyle : public ChoiceParameters<rStyle> {
    public:
        auto static constexpr ID = "r_style";
        auto static constexpr name = "Style";
        inline auto static const choices = juce::StringArray{"Classic", "1", "2", "3", "4"};
        int static constexpr defaultI = 0;
        enum {
            classic, style1, style2, style3, style4, styleNUM
        };
    };

    inline juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(threshold::get(), ratio::get(),
                   external::get());
        return layout;
    }
} // namespace ZLDsp

#endif //ZLLMAKEUP_DSP_DEFINES_H
