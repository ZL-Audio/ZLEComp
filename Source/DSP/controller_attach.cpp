// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#include "controller_attach.h"

namespace controller {
    template<typename FloatType>
    ControllerAttach<FloatType>::ControllerAttach(juce::AudioProcessor &processor,
                                                  Controller<FloatType> &c,
                                                  juce::AudioProcessorValueTreeState &parameters) {
        m_processor = &processor;
        controller = &c;
        apvts = &parameters;
    }

    template<typename FloatType>
    ControllerAttach<FloatType>::~ControllerAttach() {
        for (auto &ID: IDs) {
            apvts->removeParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::initDefaultVs() {
        for (size_t i = 0; i < IDs.size(); ++i) {
            parameterChanged(IDs[i], defaultVs[i]);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::addListeners() {
        for (auto &ID: IDs) {
            apvts->addParameterListener(ID, this);
        }
    }

    template<typename FloatType>
    void ControllerAttach<FloatType>::parameterChanged(const juce::String &parameterID, float newValue) {
        auto v = static_cast<FloatType>(newValue);
        if (parameterID == ZLDsp::outGain::ID) {
            controller->setOutGain(v);
        } else if (parameterID == ZLDsp::mix::ID) {
            controller->setMixProportion(ZLDsp::mix::formatV(v));
        } else if (parameterID == ZLDsp::overSample::ID) {
            controller->setOversampleID(static_cast<size_t>(v));
        } else if (parameterID == ZLDsp::rms::ID) {
            controller->setRMSSize(ZLDsp::rms::formatV(v));
        } else if (parameterID == ZLDsp::lookahead::ID) {
            controller->setLookAhead(ZLDsp::lookahead::formatV(v));
            if (m_processor->getCurrentProgram() == ZLDsp::preset::halfRMS) {
                apvts->getParameter(ZLDsp::rms::ID)->beginChangeGesture();
                apvts->getParameter(ZLDsp::rms::ID)
                        ->setValueNotifyingHost(ZLDsp::rms::range.convertTo0to1(static_cast<float>(v * 2)));
                apvts->getParameter(ZLDsp::rms::ID)->endChangeGesture();
            }
        } else if (parameterID == ZLDsp::segment::ID) {
            controller->setSegment(ZLDsp::segment::formatV(v));
        } else if (parameterID == ZLDsp::audit::ID) {
            controller->setAudit(static_cast<bool>(v));
        } else if (parameterID == ZLDsp::external::ID) {
            controller->setExternal(static_cast<bool>(v));
        } else if (parameterID == ZLDsp::sideGain::ID) {
            controller->setSideGain(v);
        } else if (parameterID == ZLDsp::link::ID) {
            controller->setLink(v);
        }
    }

    template
    class ControllerAttach<float>;

    template
    class ControllerAttach<double>;
}