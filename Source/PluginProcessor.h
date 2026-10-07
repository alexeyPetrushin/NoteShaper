#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "TuneEngine.h"

class NoteShaperProcessor final : public juce::AudioProcessor {
public:
    NoteShaperProcessor();
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void reset() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "NoteShaper"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.064; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    notefollow::Settings settings() const;
    void setParameter(const juce::String& id, float value);
    void applyPreset(int preset);
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> detected{-1000}, destination{-1000}, cents{0}, certainty{0};
    std::atomic<unsigned> activeNotes{1};
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void process(juce::AudioBuffer<float>&, juce::MidiBuffer&, bool bypassed);
    void handleMidi(const juce::MidiMessage&);
    notefollow::TuneEngine engine;
    std::array<std::atomic<float>*, 20> cached{};
    std::array<unsigned char, 16 * 128> heldNotes{};
    std::array<bool, 16 * 128> sustainedNotes{};
    std::array<bool, 16> sustainDown{};
    std::atomic<bool> clearMidiRequested{false};
    unsigned midiMask() const;
    void clearMidi();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteShaperProcessor)
};
