#ifndef ZLLMATCH_CHANGINGVALUECOMPONENT_H
#define ZLLMATCH_CHANGINGVALUECOMPONENT_H

#include "../DSP/Controller.h"
#include "NameLookAndFeel.h"
#include "interface_defines.h"
#include "juce_gui_basics/juce_gui_basics.h"

class ChangingValueComponent : public juce::Component, private juce::Timer {
public:
    explicit ChangingValueComponent(Controller<float>* controller) {
        source = controller;
        startTimerHz(ZLInterface::RefreshFreqHz);
        label.setLookAndFeel(&nameLookAndFeel);
        addAndMakeVisible(label);
    }

    ~ChangingValueComponent() override {
        stopTimer();
        label.setLookAndFeel(nullptr);
    }

    void resized() override {
        label.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    }

    void timerCallback() override {
        label.setText(ZLInterface::formatFloat(source->getGain(), 2), juce::dontSendNotification);
    }

    void setFontSize(float size) {
        nameLookAndFeel.setFontSize(size * ZLInterface::FontHuge3);
    }

private:
    Controller<float>* source;
    juce::Label label;
    NameLookAndFeel nameLookAndFeel;
};

#endif //ZLLMATCH_CHANGINGVALUECOMPONENT_H
