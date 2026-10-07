#pragma once
#include "PluginProcessor.h"

class NoteShaperSkin final : public juce::LookAndFeel_V4 {
public:
    NoteShaperSkin();
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override;
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    juce::Label* createSliderTextBox(juce::Slider&) override;
    juce::Font getLabelFont(juce::Label&) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
};

class NoteShaperEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit NoteShaperEditor(NoteShaperProcessor&);
    ~NoteShaperEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    void refresh();
    void chooseMode(int);
    void chooseNote(int);
    unsigned targetMask() const;
    juce::String noteText(float) const;
    NoteShaperProcessor& processor;
    NoteShaperSkin skin;
    juce::TooltipWindow tooltips{this, 650};
    juce::ComboBox root, chord, scale;
    std::array<juce::TextButton, 4> modes;
    std::array<juce::TextButton, 3> presets;
    std::array<juce::TextButton, 12> keys;
    juce::TextButton midi, natural;
    juce::Slider strength, speed, freedom;
    juce::Label status, summary, noteCaption;
    int rememberedChord = 1;
    int rememberedScale = 1;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<ButtonAttachment> midiAttachment, naturalAttachment;
    std::unique_ptr<SliderAttachment> strengthAttachment, speedAttachment, freedomAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteShaperEditor)
};
