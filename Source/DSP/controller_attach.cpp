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

namespace zlcontroller {
    template<typename FloatType>
    ControllerAttach<FloatType>::ControllerAttach(juce::AudioProcessor &processor,
                                                  Controller<FloatType> &c,
                                                  juce::AudioProcessorValueTreeState &parameters,
                                                  juce::AudioProcessorValueTreeState &state_parameters) {
        m_processor = &processor;
        controller = &c;
        apvts = &parameters;
        states = &state_parameters;
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
        if (parameterID == zldsp::outGain::ID) {
            controller->setOutGain(v);
        } else if (parameterID == zldsp::mix::ID) {
            controller->setMixProportion(zldsp::mix::formatV(v));
        } else if (parameterID == zldsp::overSample::ID) {
            controller->setOversampleID(static_cast<size_t>(v));
        } else if (parameterID == zldsp::sStyle::ID) {
            controller->setStructureStyleID(static_cast<size_t>(v));
        } else if (parameterID == zldsp::rms::ID) {
            controller->setRMSSize(zldsp::rms::formatV(v));
        } else if (parameterID == zldsp::lookahead::ID) {
            controller->setLookAhead(zldsp::lookahead::formatV(v));
            if (static_cast<int>(*states->getRawParameterValue(zlstate::programIdx::ID)) == zlstate::preset::halfRMS) {
                apvts->getParameter(zldsp::rms::ID)->beginChangeGesture();
                apvts->getParameter(zldsp::rms::ID)
                        ->setValueNotifyingHost(zldsp::rms::range.convertTo0to1(static_cast<float>(v * 2)));
                apvts->getParameter(zldsp::rms::ID)->endChangeGesture();
            }
        } else if (parameterID == zldsp::segment::ID) {
            controller->setSegment(zldsp::segment::formatV(v));
        } else if (parameterID == zldsp::audit::ID) {
            controller->setAudit(static_cast<bool>(v));
        } else if (parameterID == zldsp::external::ID) {
            controller->setExternal(static_cast<bool>(v));
        } else if (parameterID == zldsp::byPass::ID) {
            controller->setByPass(static_cast<bool>(v));
        } else if (parameterID == zldsp::sideGain::ID) {
            controller->setSideGain(v);
        } else if (parameterID == zldsp::link::ID) {
            controller->setLink(zldsp::link::formatV(v));
        }
    }

    template
    class ControllerAttach<float>;

    template
    class ControllerAttach<double>;
}