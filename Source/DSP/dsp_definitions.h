// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZLECOMP_DSP_DEFINITIONS_H
#define ZLECOMP_DSP_DEFINITIONS_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <BinaryData.h>

namespace zldsp {
    // float
    inline auto static const versionHint = 1;

    template<class T>
    class FloatParameters {
    public:
        static std::unique_ptr<juce::AudioParameterFloat> get(bool automate = true) {
            auto attributes = juce::AudioParameterFloatAttributes().withAutomatable(automate).withLabel(T::name);
            return std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(T::ID, versionHint), T::name, T::range,
                                                               T::defaultV, attributes);
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
        auto static constexpr defaultV = 1.f;
        inline auto static const range =
                juce::NormalisableRange<float>(1.f, 100.f, 0.01f, 0.2160127f);
    };

    class kneeW : public FloatParameters<kneeW> {
    public:
        auto static constexpr ID = "knee_width";
        auto static constexpr name = "Knee Width";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f, .5f);
        auto static constexpr defaultV = 0.f;

        inline static float formatV(float x) { return std::max(x * 60, 0.0625f); }

        inline static double formatV(double x) { return std::max(x * 60, 0.0625); }
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
                juce::NormalisableRange<float>(-16.f, 16.f, .01f, 0.5, true);
        auto static constexpr defaultV = 0.f;
    };

    class attack : public FloatParameters<attack> {
    public:
        auto static constexpr ID = "attack";
        auto static constexpr name = "Attack (ms)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 500.f, 0.01f, 0.30103f);
        auto static constexpr defaultV = 10.f;

        inline static float formatV(float x) { return x * .001f; }

        inline static double formatV(double x) { return x * .001; }
    };

    class release : public FloatParameters<release> {
    public:
        auto static constexpr ID = "release";
        auto static constexpr name = "Release (ms)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 5000.f, 0.01f, 0.30103f);
        auto static constexpr defaultV = 100.f;

        inline static float formatV(float x) { return x * .001f; }

        inline static double formatV(double x) { return x * .001; }
    };

    class smooth : public FloatParameters<smooth> {
    public:
        auto static constexpr ID = "smooth";
        auto static constexpr name = "Smooth";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 0.f;
    };

    class link : public FloatParameters<link> {
    public:
        auto static constexpr ID = "link";
        auto static constexpr name = "Stereo Link";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1.f, .01f);
        auto static constexpr defaultV = 1.f;

        inline static float formatV(float x) { return x * .5f; }

        inline static double formatV(double x) { return x * .5; }
    };

    class segment : public FloatParameters<segment> {
    public:
        auto static constexpr ID = "segment";
        auto static constexpr name = "Segment (ms)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 100.f, 1.f);
        auto static constexpr defaultV = 0.f;

        inline static float formatV(float x) { return x * .001f; }

        inline static double formatV(double x) { return x * .001; }
    };

    class outGain : public FloatParameters<outGain> {
    public:
        auto static constexpr ID = "out_gain";
        auto static constexpr name = "Out Gain (dB)";
        inline auto static const range =
                juce::NormalisableRange<float>(-16.f, 16.f, .01f, 0.5, true);
        auto static constexpr defaultV = 0.f;
    };

    class mix : public FloatParameters<mix> {
    public:
        auto static constexpr ID = "mix";
        auto static constexpr name = "Mix (%)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 100.f, .1f);
        auto static constexpr defaultV = 100.f;

        inline static float formatV(float x) { return x * .01f; }

        inline static double formatV(double x) { return x * .01; }
    };

    class rms : public FloatParameters<rms> {
    public:
        auto static constexpr ID = "rms";
        auto static constexpr name = "RMS (ms)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 1000.f, 1.f);
        auto static constexpr defaultV = 0.f;

        inline static float formatV(float x) { return x * .001f; }

        inline static double formatV(double x) { return x * .001; }
    };

    class lookahead : public FloatParameters<lookahead> {
    public:
        auto static constexpr ID = "lookahead";
        auto static constexpr name = "Lookahead (ms)";
        inline auto static const range =
                juce::NormalisableRange<float>(0.f, 250.f, 1.f);
        auto static constexpr defaultV = 0.f;

        inline static float formatV(float x) { return x * .001f; }

        inline static double formatV(double x) { return x * .001; }
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

    class byPass : public BoolParameters<byPass> {
    public:
        auto static constexpr ID = "byPass";
        auto static constexpr name = "By Pass";
        auto static constexpr defaultV = false;
    };

    // choice
    template<class T>
    class ChoiceParameters {
    public:
        static std::unique_ptr<juce::AudioParameterChoice> get(bool automate = true) {
            auto attributes = juce::AudioParameterChoiceAttributes().withAutomatable(automate).withLabel(T::name);
            return std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(T::ID, versionHint), T::name, T::choices, T::defaultI, attributes);
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

    class overSample : public ChoiceParameters<overSample> {
    public:
        auto static constexpr ID = "over_sample";
        auto static constexpr name = "Over Sampling";
        inline auto static const choices = juce::StringArray{"OFF", "x2", "x4", "x8"};
        int static constexpr defaultI = 0;
        enum {
            off, x2, x4, x8, overSampleNUM
        };
    };

    inline juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(threshold::get(), ratio::get(), kneeW::get(),
                   kneeD::get(), kneeS::get(), bound::get(),

                   sideGain::get(), attack::get(), release::get(),
                   smooth::get(), link::get(),
                   external::get(), audit::get(),
                   aStyle::get(), rStyle::get(),

                   outGain::get(), mix::get(), segment::get(),
                   rms::get(), lookahead::get(),
                   overSample::get(), byPass::get());
        return layout;
    }

} // namespace zldsp

#endif //ZLECOMP_DSP_DEFINITIONS_H
