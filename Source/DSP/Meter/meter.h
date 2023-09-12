//
// Created by Zishu Liu on 8/6/23.
//

#ifndef ZLECOMP_METER_H
#define ZLECOMP_METER_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <boost/circular_buffer.hpp>

#include "../FixedBuffer/fixed_audio_buffer.h"

namespace zlmeter {

    template<typename FloatType>
    class MeterSource {
    public:
        auto static constexpr subBufferInSecond = 0.0135;

        explicit MeterSource(juce::AudioProcessor &processor) :
                subBuffer() {
            processorRef = &processor;
        }

        void reset() noexcept {
            resetBuffer();
            resetPeakMax();
        }

        template<typename SampleType>
        SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType s) noexcept {
            return s;
        }

        void process(const juce::dsp::AudioBlock<FloatType> block) noexcept {
            block.copyTo(m_buffer);
            juce::dsp::AudioBlock<FloatType> m_block(m_buffer);
            delayLine.process(juce::dsp::ProcessContextReplacing<FloatType>(m_block));
            subBuffer.pushBlock(m_block);
            while (subBuffer.isSubReady()) {
                subBuffer.popSubBuffer();
                const auto numSamples = static_cast<size_t>(subBuffer.subBuffer.getNumSamples());
                const auto numChannels = static_cast<size_t>(subBuffer.subBuffer.getNumChannels());
                for (size_t i = 0; i < numChannels; ++i) {
                    auto subBlock = juce::dsp::AudioBlock<FloatType>(subBuffer.subBuffer);
                    currentRMS[i] = juce::Decibels::gainToDecibels(getRMSLevel(subBlock, i, 0, numSamples));
                    currentPeak[i] = juce::Decibels::gainToDecibels(getPeakLevel(subBlock, i, 0, numSamples));
                    bufferRMS[i] = juce::jmax(bufferRMS[i], currentRMS[i]);
                    bufferPeak[i] = juce::jmax(bufferPeak[i], currentPeak[i]);
                    peakMax[i] = juce::jmax(currentPeak[i], peakMax[i]);
                }
                historyRMS.push_back(std::accumulate(currentRMS.begin(), currentRMS.end(), FloatType(0)) /
                                     static_cast<FloatType>(currentRMS.size()));
                historyPeak.push_back(*std::max_element(currentPeak.begin(), currentPeak.end()));
                subBuffer.pushSubBuffer();
            }
            subBuffer.popBlock(m_block, false);
        }

        void prepare(const juce::dsp::ProcessSpec &spec) {
            historyRMS.set_capacity(static_cast<size_t>(spec.sampleRate * subBufferInSecond * 10));
            historyPeak.set_capacity(static_cast<size_t>(spec.sampleRate * subBufferInSecond * 10));
            for (auto f: {&currentRMS, &currentPeak, &peakMax, &bufferRMS, &bufferPeak, &displayRMS, &displayPeak}) {
                (*f).resize(spec.numChannels);
            }
            resetPeakMax();
            resetBuffer();
            for (size_t i = 0; i < displayRMS.size(); ++i) {
                displayRMS[i] = static_cast<FloatType>(-100);
                displayPeak[i] = static_cast<FloatType>(-100);
            }

            m_buffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
            subBuffer.prepare(spec);
            subBuffer.setSubBufferSize(static_cast<int>(spec.sampleRate * subBufferInSecond));
            delayLine.prepare(spec);
            delayLine.setMaximumDelayInSamples(static_cast<int>(spec.sampleRate) * 2);
        }

        size_t appendHistoryRMS(boost::circular_buffer<FloatType> &buffer,
                             std::optional<size_t> popNum = std::nullopt) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            size_t num = 0;
            if (popNum == std::nullopt) {
                popNum = historyRMS.size();
            }
            while (!historyRMS.empty() && num < popNum) {
                buffer.push_back(historyRMS.front());
                num++;
                historyRMS.pop_front();
            }
            return num;
        }

        size_t appendHistoryPeak(boost::circular_buffer<FloatType> &buffer,
                                 std::optional<size_t> popNum = std::nullopt) {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            size_t num = 0;
            if (popNum == std::nullopt) {
                popNum = historyPeak.size();
            }
            while (!historyPeak.empty() && num < popNum) {
                buffer.push_back(historyPeak.front());
                num++;
                historyPeak.pop_front();
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

        void resetHistory() {
            const juce::GenericScopedLock<juce::CriticalSection> processLock(processorRef->getCallbackLock());
            historyRMS.clear();
            historyPeak.clear();
        }

        void setDecayRate(float x) {
            decayRate = x;
        }

        void setDelay(int numSamples) {
            delayLine.setDelay(static_cast<float>(numSamples));
            delayLine.reset();
        }

    private:
        std::vector<FloatType> peakMax;
        std::vector<FloatType> currentRMS, currentPeak;
        std::vector<FloatType> bufferRMS, bufferPeak;
        std::vector<FloatType> displayRMS, displayPeak;
        boost::circular_buffer<FloatType> historyRMS, historyPeak;
        juce::AudioProcessor *processorRef;
        float decayRate = 0.12f;
        bool useSubBuffer = false;
        fixedBuffer::FixedAudioBuffer<FloatType> subBuffer;
        juce::AudioBuffer<FloatType> m_buffer;
        juce::dsp::DelayLine<FloatType> delayLine;

        template<typename T>
        T getRMSLevel(juce::dsp::AudioBlock<T> block, size_t channel, size_t startSample,
                      size_t numSamples) const noexcept {
            if (numSamples <= 0 || channel >= block.getNumChannels() || block.getNumSamples() == 0)
                return FloatType(0);

            auto *data = block.getChannelPointer(channel) + startSample;
            double sum = 0.0;

            for (size_t i = 0; i < numSamples; ++i) {
                auto sample = static_cast<double>(data[i]);
                sum += sample * sample;
            }
            return static_cast<FloatType>(std::sqrt(sum / static_cast<double>(numSamples)));
        }

        template<typename T>
        T getPeakLevel(juce::dsp::AudioBlock<T> block, size_t channel, size_t startSample,
                       size_t numSamples) const noexcept {
            if (numSamples <= 0 || channel >= block.getNumChannels() || block.getNumSamples() == 0)
                return FloatType(0);

            auto *data = block.getChannelPointer(channel) + startSample;
            FloatType localPeak = -1000.0;

            for (size_t i = 0; i < numSamples; ++i) {
                if (data[i] > localPeak) {
                    localPeak = static_cast<FloatType>(data[i]);
                }
            }
            return localPeak;
        }
    };
}

#endif //ZLECOMP_METER_H
