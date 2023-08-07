//
// Created by Zishu Liu on 8/6/23.
//

#ifndef ZLECOMP_METER_H
#define ZLECOMP_METER_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"

namespace zlmeter {

    template<typename FloatType>
    class MeterSource {
    public:
        void reset() noexcept {}

        template<typename SampleType>
        SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType s) noexcept {
            return s;
        }

        template<typename ProcessContext>
        void process(const ProcessContext &context) noexcept {
            if (context.usesSeparateInputAndOutputBlocks())
                context.getOutputBlock().copyFrom(context.getInputBlock());
            if (lock) {
                return;
            }
            const auto numSamples = context.getInputBlock().getNumSamples();
            const auto numChannels = context.getInputBlock().getNumChannels();
            auto block = context.getInputBlock();
            for (size_t i = 0; i < numChannels; ++i) {
                currentRMS[i] = juce::Decibels::gainToDecibels(getRMSLevel(block, i, 0, numSamples));
                currentPeak[i] = juce::Decibels::gainToDecibels(getPeakLevel(block, i, 0, numSamples));
                bufferRMS[i] = juce::jmax(bufferRMS[i], currentRMS[i]);
                bufferPeak[i] = juce::jmax(bufferPeak[i], currentPeak[i]);
                peakMax[i] = juce::jmax(currentPeak[i], peakMax[i]);
            }
        }

        void prepare(const juce::dsp::ProcessSpec &spec) {
            for (auto f: {&currentRMS, &currentPeak, &peakMax, &bufferRMS, &bufferPeak, &displayRMS, &displayPeak}) {
                (*f).resize(spec.numChannels);
            }
            resetPeakMax();
            resetBuffer();
            for (size_t i = 0; i < displayRMS.size(); ++i) {
                displayRMS[i] = static_cast<FloatType>(-100);
                displayPeak[i] = static_cast<FloatType>(-100);
            }
        }

        inline FloatType getCurrentMeanRMS() {
            return std::accumulate(std::begin(currentRMS), std::end(currentRMS), FloatType(0)) /
                   static_cast<FloatType>(currentRMS.size());
        }

        inline FloatType getBufferMeanRMS() {
            return std::accumulate(std::begin(bufferRMS), std::end(bufferRMS), FloatType(0)) /
                   static_cast<FloatType>(bufferRMS.size());
        }

        inline std::vector<FloatType> getBufferRMS() {
            return bufferRMS;
        }

        inline std::vector<FloatType> getBufferPeak() {
            return bufferPeak;
        }

        std::vector<FloatType> getDisplayRMS() {
            for (size_t i = 0; i < displayRMS.size(); ++i) {
                displayRMS[i] = juce::jmax(displayRMS[i] - static_cast<FloatType>(decayRate), bufferRMS[i]);
            }
            return displayRMS;
        }

        std::vector<FloatType> getDisplayPeak() {
            for (size_t i = 0; i < displayPeak.size(); ++i) {
                displayPeak[i] = juce::jmax(displayPeak[i] - static_cast<FloatType>(decayRate), bufferPeak[i]);
            }
            return displayPeak;
        }

        std::vector<FloatType> getPeakMax() {
            return peakMax;
        }

        void resetBuffer() {
            lock = true;
            for (size_t i = 0; i < bufferRMS.size(); ++i) {
                bufferRMS[i] = static_cast<FloatType>(-100);
                bufferPeak[i] = static_cast<FloatType>(-100);
            }
            lock = false;
        }

        void resetPeakMax() {
            lock = true;
            for (size_t i = 0; i < peakMax.size(); ++i) {
                peakMax[i] = static_cast<FloatType>(-100);
            }
            lock = false;
        }

        void setDecayRate(float x) {
            decayRate = x;
        }

        bool getDataFlag() {
            return dataFlag;
        }

        void resetDataFlag() {
            dataFlag = false;
        }

    private:
        std::vector<FloatType> peakMax;
        std::vector<FloatType> currentRMS, currentPeak;
        std::vector<FloatType> bufferRMS, bufferPeak;
        std::vector<FloatType> displayRMS, displayPeak;
        std::atomic<bool> lock = false;
        float decayRate = 0.12f;
        bool dataFlag = false;

        template<typename T>
        T getRMSLevel(juce::dsp::AudioBlock<T> block, unsigned long channel, unsigned long startSample,
                      unsigned long numSamples) const noexcept {
            if (numSamples <= 0 || channel < 0 || channel >= block.getNumChannels() || block.getNumSamples() == 0)
                return FloatType(0);

            auto *data = block.getChannelPointer(channel) + startSample;
            double sum = 0.0;

            for (size_t i = 0; i < numSamples; ++i) {
                auto sample = data[i];
                sum += sample * sample;
            }
            return static_cast<FloatType> (std::sqrt(sum / static_cast<double>(numSamples)));
        }

        template<typename T>
        T getPeakLevel(juce::dsp::AudioBlock<T> block, unsigned long channel, unsigned long startSample,
                       unsigned long numSamples) const noexcept {
            if (numSamples <= 0 || channel < 0 || channel >= block.getNumChannels() || block.getNumSamples() == 0)
                return FloatType(0);

            auto *data = block.getChannelPointer(channel) + startSample;
            FloatType localPeak = -1000.0;

            for (size_t i = 0; i < numSamples; ++i) {
                if (data[i] > localPeak) {
                    localPeak = static_cast<FloatType> (data[i]);
                }
            }
            return localPeak;
        }
    };
}

#endif //ZLECOMP_METER_H
