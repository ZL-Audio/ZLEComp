//
// Created by Zishu Liu on 8/6/23.
//

#ifndef ZLECOMP_METER_H
#define ZLECOMP_METER_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"
#include <boost/circular_buffer.hpp>

namespace zlmeter {

    template<typename FloatType>
    class MeterSource {
    public:
        explicit MeterSource(juce::AudioProcessor &processor) {
            processorRef = &processor;
        }

        void reset() noexcept {}

        template<typename SampleType>
        SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType s) noexcept {
            return s;
        }

        template<typename ProcessContext>
        void process(const ProcessContext &context) noexcept {
            if (context.usesSeparateInputAndOutputBlocks())
                context.getOutputBlock().copyFrom(context.getInputBlock());
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
            historyRMS.push_back(std::accumulate(currentRMS.begin(), currentRMS.end(), FloatType(0)) /
                                 static_cast<FloatType>(currentRMS.size()));
        }

        void prepare(const juce::dsp::ProcessSpec &spec) {
            historyRMS.set_capacity(static_cast<size_t>(spec.sampleRate / spec.maximumBlockSize));
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

        size_t appendHistory(boost::circular_buffer<FloatType> &buffer, int &discardIndex, int discardNum = 1,
                             std::optional<size_t> popNum = std::nullopt) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            size_t num = 0;
            if (popNum == std::nullopt) {
                popNum = historyRMS.size();
            }
            while (!historyRMS.empty() && num < popNum) {
                if (discardIndex == 0) {
                    buffer.push_back(historyRMS.front());
                    num++;
                }
                historyRMS.pop_front();
                discardIndex = (discardIndex + 1) % discardNum;
            }
            return num;
        }

        inline FloatType getCurrentMeanRMS() {
            return std::accumulate(std::begin(currentRMS), std::end(currentRMS), FloatType(0)) /
                   static_cast<FloatType>(currentRMS.size());
        }

        inline FloatType getCurrentMeanPeak() {
            return std::accumulate(std::begin(currentPeak), std::end(currentPeak), FloatType(0)) /
                   static_cast<FloatType>(currentPeak.size());
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
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            for (size_t i = 0; i < bufferRMS.size(); ++i) {
                bufferRMS[i] = static_cast<FloatType>(-100);
                bufferPeak[i] = static_cast<FloatType>(-100);
            }
        }

        void resetPeakMax() {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            for (size_t i = 0; i < peakMax.size(); ++i) {
                peakMax[i] = static_cast<FloatType>(-100);
            }
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
        boost::circular_buffer<FloatType> historyRMS;
        juce::AudioProcessor *processorRef;
        float decayRate = 0.12f;
        bool dataFlag = false;

        template<typename T>
        T getRMSLevel(juce::dsp::AudioBlock<T> block, unsigned long channel, unsigned long startSample,
                      unsigned long numSamples) const noexcept {
            if (numSamples <= 0 || channel >= block.getNumChannels() || block.getNumSamples() == 0)
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
            if (numSamples <= 0 || channel >= block.getNumChannels() || block.getNumSamples() == 0)
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
