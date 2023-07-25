/*
==============================================================================
Copyright (C) 2023 - zsliu98
This file is part of ZLEComp

ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
==============================================================================
*/

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

    class segment : public FloatParameters<segment> {
    public:
        auto static constexpr ID = "segment";
        auto static constexpr name = "Segment (ms)";
        inline auto static const range =
                juce::NormalisableRange<float>(10.f, 200.f, 1.f);
        auto static constexpr defaultV = 40.f;
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

    // bool
    template<class T>
    class BoolParameters {
    public:
        static std::unique_ptr<juce::AudioParameterBool> get() {
            return std::make_unique<juce::AudioParameterBool>(T::ID, T::name,
                                                              T::defaultV);
        }
    };

    class ceil : public BoolParameters<ceil> {
    public:
        auto static constexpr ID = "ceil";
        auto static constexpr name = "Ceil";
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

    class mode : public ChoiceParameters<mode> {
    public:
        auto static constexpr ID = "mode";
        auto static constexpr name = "Mode";
        inline auto static const choices = juce::StringArray{"Effect", "Envelope"};
        int static constexpr defaultI = 0;
        enum {
            effect, envelope, modeNUM
        };
    };

    inline juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(segment::get(),
                   ceil::get(),
                   mode::get());
        return layout;
    }
} // namespace ZLDsp

#endif //ZLLMAKEUP_DSP_DEFINES_H
