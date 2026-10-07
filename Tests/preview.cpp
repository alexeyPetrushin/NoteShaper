#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>
#define NOMINMAX
#include <windows.h>
#include <chrono>

void expect(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
juce::Component& component(juce::AudioProcessorEditor& editor, const juce::String& id) {
    for (auto* c : editor.getChildren()) if (c->getComponentID() == id) return *c;
    throw std::runtime_error("missing control " + id.toStdString());
}
template<class T> T& control(juce::AudioProcessorEditor& editor, const juce::String& id) {
    auto* c = dynamic_cast<T*>(&component(editor, id)); expect(c != nullptr, "wrong control type"); return *c;
}
void settle() {
    const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(180);
    do {
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message); DispatchMessageW(&message);
        }
        Sleep(1);
    } while (std::chrono::steady_clock::now() < until);
}
void click(juce::AudioProcessorEditor& e, const juce::String& id) {
    control<juce::Button>(e, id).triggerClick(); settle();
}
void select(juce::AudioProcessorEditor& e, const juce::String& id, int choice) {
    auto& picker = control<juce::ComboBox>(e, id);
    expect(picker.isVisible() && picker.isEnabled(), "picker unavailable");
    picker.setSelectedId(choice, juce::sendNotificationSync); settle();
}
void selectRoot(juce::AudioProcessorEditor& e, int note) {
    auto& root = control<juce::ComboBox>(e, "root");
    expect(root.isVisible() && root.isEnabled(), "root picker unavailable");
    root.setSelectedId(note + 1, juce::sendNotificationSync); settle();
}
void render(juce::AudioProcessorEditor& editor, const juce::File& path) {
    // Settle audio and button feedback before taking a public state snapshot.
    juce::AudioBuffer<float> audio(2, 256); juce::MidiBuffer midi;
    for (int block = 0; block < 64; ++block) {
        for (int i = 0; i < 256; ++i) {
            const float value = 0.2f * std::sin(2 * 3.14159265358979323846 * 234 * (block * 256 + i) / 48000.0);
            audio.setSample(0, i, value); audio.setSample(1, i, value);
        }
        editor.getAudioProcessor()->processBlock(audio, midi);
    }
    settle();
    juce::Image screenshot(juce::Image::ARGB, editor.getWidth(), editor.getHeight(), true);
    juce::Graphics g(screenshot); editor.paintEntireComponent(g, true);
    expect(screenshot.getPixelAt(5, 5).getARGB() == 0xfff8f8fa, "editor did not paint");
    juce::FileOutputStream stream(path); expect(stream.openedOk(), "screenshot output unavailable");
    stream.setPosition(0); stream.truncate(); juce::PNGImageFormat format;
    expect(format.writeImageToStream(screenshot, stream), "screenshot could not be written");
}
void layout(juce::AudioProcessorEditor& e) {
    for (auto* c : e.getChildren()) if (c->isVisible()) {
        expect(e.getLocalBounds().contains(c->getBounds()), "visible control outside editor");
        if (dynamic_cast<juce::Button*>(c) || dynamic_cast<juce::Slider*>(c) || dynamic_cast<juce::ComboBox*>(c))
            expect(c->getHeight() >= 44, "interactive target too small");
    }
    std::vector<juce::Component*> interactive;
    for (auto* c : e.getChildren()) if (c->isVisible() &&
        (dynamic_cast<juce::Button*>(c) || dynamic_cast<juce::Slider*>(c) || dynamic_cast<juce::ComboBox*>(c))) interactive.push_back(c);
    for (size_t i = 0; i < interactive.size(); ++i) for (size_t j = i + 1; j < interactive.size(); ++j)
        expect(!interactive[i]->getBounds().intersects(interactive[j]->getBounds()), "interactive controls overlap");
}
int main(int argc, char** argv) {
    try {
        juce::ScopedJuceInitialiser_GUI gui;
        expect(argc == 2, "usage: preview output_directory");
        const auto out = juce::File::createFileWithoutCheckingPath(juce::String::fromUTF8(argv[1]));
        NoteShaperProcessor p; p.prepareToPlay(48000, 256);
        p.setParameter("root", 9); p.setParameter("chord", 2); p.applyPreset(2);
        juce::AudioBuffer<float> b(2, 256); juce::MidiBuffer m;
        for (int block = 0; block < 250; ++block) {
            for (int i = 0; i < 256; ++i) {
                const float v = 0.2f * std::sin(2 * 3.14159265358979323846 * 234 * (block * 256 + i) / 48000.0);
                b.setSample(0, i, v); b.setSample(1, i, v);
            }
            p.processBlock(b, m);
        }
        std::unique_ptr<juce::AudioProcessorEditor> e(p.createEditor()); e->setVisible(true); settle();
        expect(e->getWidth() == 720 && e->getHeight() == 552, "wrong compact size");
        layout(*e); render(*e, out.getChildFile("Preview.png"));
        expect(control<juce::ComboBox>(*e, "mode").getNumItems() == 5, "target choices incomplete");
        int visibleControls = 0;
        for (auto* c : e->getChildren()) if (c->isVisible() &&
            (dynamic_cast<juce::Button*>(c) || dynamic_cast<juce::Slider*>(c) || dynamic_cast<juce::ComboBox*>(c))) ++visibleControls;
        expect(visibleControls == 11, "chord mode has redundant controls");
        expect(component(*e, "root").isVisible() && !component(*e, "note0").isVisible(), "normal mode shows competing root controls");
        expect(control<juce::Button>(*e, "preset2").getToggleState(), "hard preset state wrong");
        expect(component(*e, "speed").isVisible() && component(*e, "tolerance").isVisible(), "three controls not visible");
        for (const auto& id : {"amount", "speed", "tolerance"})
            expect(control<juce::Slider>(*e, id).getSliderStyle() == juce::Slider::RotaryHorizontalVerticalDrag, "rotary control missing");
        expect(!control<juce::Button>(*e, "language").getToggleState(), "new instance does not default to English");
        expect(control<juce::Button>(*e, "language").getButtonText() == "EN", "language must be one EN button");
        expect(control<juce::ComboBox>(*e, "mode").getItemText(1) == "Chord", "English source labels missing");
        expect(control<juce::Button>(*e, "preset2").getButtonText() == "Rap", "English presets missing");
        const auto languageMask = notefollow::noteMask(p.settings());
        click(*e, "language");
        expect(control<juce::Button>(*e, "language").getButtonText() == "RU", "language button must switch to RU");
        expect(control<juce::ComboBox>(*e, "mode").getItemText(1) == juce::String::fromUTF8(u8"Аккорд"), "Russian source labels missing");
        expect(control<juce::Button>(*e, "preset2").getButtonText() == juce::String::fromUTF8(u8"Рэп"), "Russian presets missing");
        expect(control<juce::Slider>(*e, "speed").getTextValueSuffix().contains(juce::String::fromUTF8(u8"мс")), "Russian units missing");
        expect(notefollow::noteMask(p.settings()) == languageMask && p.settings().amount == 1.0f, "language changed audio settings");
        layout(*e); render(*e, out.getChildFile("Design/Russian.png"));
        juce::MemoryBlock languageState; p.getStateInformation(languageState);
        e.reset(); e.reset(p.createEditor()); e->setVisible(true); settle();
        expect(control<juce::Button>(*e, "language").getToggleState(), "language not retained after reopening");
        click(*e, "language");
        p.setStateInformation(languageState.getData(), static_cast<int>(languageState.getSize())); settle();
        expect(control<juce::Button>(*e, "language").getToggleState(), "saved language not reflected in open editor");
        click(*e, "language");
        expect(control<juce::ComboBox>(*e, "mode").getItemText(1) == "Chord", "return to English failed");
        expect(control<juce::Slider>(*e, "speed").getTextValueSuffix().contains("ms"), "English units missing");
        selectRoot(*e, 0);
        expect(p.settings().root == 0 && p.settings().chord == 2, "chord root selection edited note set");
        expect(notefollow::noteMask(p.settings()) == ((1u << 0) | (1u << 3) | (1u << 7)), "wrong C minor targets");
        select(*e, "mode", 1); selectRoot(*e, 9);
        expect(p.settings().chord == 0 && notefollow::noteMask(p.settings()) == (1u << 9), "single A picker selection failed");
        render(*e, out.getChildFile("Design/Single.png"));
        select(*e, "mode", 4);
        expect(!component(*e, "root").isVisible() && component(*e, "note0").isVisible(), "custom disclosure failed");
        expect(p.settings().chord == 10 && notefollow::noteMask(p.settings()) == (1u << 9), "custom mode lost current target");
        click(*e, "note0"); click(*e, "note9");
        expect(notefollow::noteMask(p.settings()) == 1u, "custom set edits failed");
        layout(*e); render(*e, out.getChildFile("Design/Custom.png"));
        click(*e, "note0");
        expect(notefollow::noteMask(p.settings()) == 0u, "empty custom set failed");
        expect(control<juce::Label>(*e, "noteCaption").getText().contains("original pitch"), "empty set not explained");
        render(*e, out.getChildFile("Design/Empty.png"));
        select(*e, "mode", 2);
        expect(p.settings().chord == 2, "previous chord not remembered");
        control<juce::ComboBox>(*e, "chord").setSelectedId(5, juce::sendNotificationSync); settle();
        expect(p.settings().chord == 4, "chord menu mapping failed");
        select(*e, "mode", 3);
        expect(p.settings().root == 9, "scale mode lost selected root");
        selectRoot(*e, 0);
        expect(p.settings().scale == 1 && notefollow::noteMask(p.settings()) == 0xab5, "major scale mode failed");
        control<juce::ComboBox>(*e, "scale").setSelectedId(2, juce::sendNotificationSync); selectRoot(*e, 9);
        expect(p.settings().root == 9 && p.settings().scale == 2 && notefollow::noteMask(p.settings()) == 0xab5, "A minor scale selection failed");
        layout(*e); render(*e, out.getChildFile("Design/Scale.png"));
        select(*e, "mode", 4);
        expect(p.settings().scale == 0 && notefollow::noteMask(p.settings()) == 0xab5, "scale-to-custom continuity failed");
        select(*e, "mode", 3);
        expect(p.settings().scale == 2, "previous scale not remembered");
        select(*e, "mode", 2);
        click(*e, "preset0");
        const auto targetBefore = notefollow::noteMask(p.settings());
        expect(std::abs(p.settings().amount - 0.55f) < 0.001f && p.settings().speedMs == 85, "soft preset failed");
        expect(p.settings().chord == 4 && control<juce::Button>(*e, "preset0").getToggleState(), "preset changed notes");
        expect(p.settings().natural && control<juce::Button>(*e, "natural").getToggleState(), "soft preset does not enable natural voice");
        render(*e, out.getChildFile("Design/Natural.png"));
        click(*e, "natural");
        expect(!p.settings().natural && !control<juce::Button>(*e, "preset0").getToggleState(), "natural toggle or preset highlight failed");
        control<juce::Slider>(*e, "amount").setValue(42, juce::sendNotificationSync); settle();
        expect(std::abs(p.settings().amount - 0.42f) < 0.001f, "strength did not reach parameter");
        expect(!control<juce::Button>(*e, "preset0").getToggleState(), "manual settings not reflected");
        expect(notefollow::noteMask(p.settings()) == targetBefore, "strength changed notes");
        control<juce::Slider>(*e, "amount").setValue(0, juce::sendNotificationSync); settle();
        expect(p.settings().amount == 0, "zero strength failed");
        expect(control<juce::Label>(*e, "status").getText().contains("Original pitch"), "zero strength not explained");
        render(*e, out.getChildFile("Design/Zero.png"));
        control<juce::Slider>(*e, "amount").setValue(42, juce::sendNotificationSync); settle();
        auto& speedSlider = control<juce::Slider>(*e, "speed");
        juce::Label* speedField = nullptr;
        for (auto* c : speedSlider.getChildren()) if (auto* l = dynamic_cast<juce::Label*>(c)) speedField = l;
        expect(speedField != nullptr, "numeric entry missing");
        speedField->showEditor();
        expect(speedField->getCurrentTextEditor() != nullptr, "numeric entry will not open");
        speedField->getCurrentTextEditor()->setText("32.5");
        speedField->hideEditor(false); settle();
        control<juce::Slider>(*e, "tolerance").setValue(6.5, juce::sendNotificationSync); settle();
        expect(p.settings().speedMs == 32.5f && p.settings().toleranceCents == 6.5f, "retune/freedom knobs did not reach parameters");
        layout(*e);
        render(*e, out.getChildFile("Design/Manual.png"));
        select(*e, "mode", 5);
        expect(p.parameters.getRawParameterValue("midi")->load() > 0.5f, "MIDI toggle did not reach parameter");
        b.clear(); p.processBlock(b, m); settle();
        expect(!component(*e, "root").isVisible() && component(*e, "mode").isEnabled() && control<juce::ComboBox>(*e, "mode").getSelectedId() == 5, "MIDI source selector is not usable");
        expect(control<juce::Label>(*e, "status").getText().contains("MIDI"), "MIDI source hidden in compact editor");
        layout(*e); render(*e, out.getChildFile("Design/MIDI.png"));
        m.addEvent(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100)), 0);
        p.processBlock(b, m); m.clear(); settle();
        expect(control<juce::Label>(*e, "summary").getText().contains("A"), "MIDI note readout missing");
        render(*e, out.getChildFile("Design/MIDI-active.png"));
        juce::MemoryBlock state; p.getStateInformation(state);
        e.reset(); p.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        e.reset(p.createEditor()); e->setVisible(true); settle();
        expect(e->getHeight() == 552 && control<juce::ComboBox>(*e, "mode").getSelectedId() == 5, "restored MIDI state not discoverable");
        select(*e, "mode", 2);
        expect(p.parameters.getRawParameterValue("midi")->load() < 0.5f && p.settings().chord == 4, "leaving MIDI lost manual target");
        p.setParameter("chord", 9); settle();
        expect(control<juce::ComboBox>(*e, "mode").getSelectedId() == 4, "legacy chromatic state not represented");
        render(*e, out.getChildFile("Design/Chromatic.png"));
        click(*e, "note0");
        expect(p.settings().chord == 10 && notefollow::noteMask(p.settings()) == 4094u, "legacy chromatic editing failed");
        p.setParameter("root", 7); p.setParameter("chord", 1); p.setParameter("amount", 27); settle();
        expect(control<juce::ComboBox>(*e, "root").getSelectedId() == 8, "host root automation not reflected");
        expect(control<juce::Slider>(*e, "amount").getValue() == 27, "host strength automation not reflected");
        p.setParameter("scale", 3); p.setParameter("natural", 1); settle();
        p.getStateInformation(state); p.setParameter("scale", 0); p.setParameter("natural", 0);
        p.setStateInformation(state.getData(), static_cast<int>(state.getSize())); settle();
        expect(p.settings().scale == 3 && p.settings().natural && component(*e, "scale").isVisible(), "new parameter state round-trip failed");
        // Channel-specific sustain, repeated notes and panic must obey MIDI semantics.
        p.reset(); p.setParameter("midi", 1);
        auto event = [&](juce::MidiMessage message) {
            b.clear(); m.clear(); m.addEvent(message, 128); p.processBlock(b, m); return p.activeNotes.load();
        };
        expect(event(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100))) == (1u << 9), "MIDI note-on missing");
        event(juce::MidiMessage::controllerEvent(1, 64, 127));
        expect(event(juce::MidiMessage::noteOff(1, 69)) == (1u << 9), "sustain failed to retain released note");
        event(juce::MidiMessage::noteOn(2, 60, static_cast<juce::uint8>(100)));
        expect(event(juce::MidiMessage::noteOff(2, 60)) == (1u << 9), "sustain leaked to another channel");
        expect(event(juce::MidiMessage::controllerEvent(1, 64, 0)) == 0, "pedal release left stuck notes");
        event(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100)));
        event(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100)));
        expect(event(juce::MidiMessage::noteOff(1, 69)) == (1u << 9), "first duplicate release cleared note");
        expect(event(juce::MidiMessage::noteOff(1, 69)) == 0, "duplicate release stuck note");
        event(juce::MidiMessage::controllerEvent(1, 64, 127));
        event(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100)));
        expect(event(juce::MidiMessage::allNotesOff(1)) == (1u << 9), "all-notes-off ignored sustain");
        expect(event(juce::MidiMessage::allSoundOff(1)) == 0, "all-sound-off failed to clear sustain");
        event(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100)));
        event(juce::MidiMessage::noteOff(1, 69));
        expect(event(juce::MidiMessage::controllerEvent(1, 121, 0)) == 0, "reset-controllers left sustained notes");
        p.reset(); expect(p.detected.load() < 0 && p.destination.load() < 0 && p.cents.load() == 0 && p.certainty.load() == 0, "reset left stale meters");
        std::cout << "PASS: direct note/chord selection, custom continuity, empty state, presets, strength, three visible rotary controls, MIDI visibility, legacy state, host automation\n";
        std::cout << "PASS: scales, natural voice, new parameter state, sustain/channel isolation/duplicate notes/panic, reset meters\n";
        std::cout << "PASS: real native editor renders 720 x 552; twelve state screenshots; English default, RU/EN switch and saved language, direct preset buttons, numeric typing and non-overlapping controls\n";
        return 0;
    } catch (const std::exception& x) { std::cerr << "FAIL: " << x.what() << '\n'; return 1; }
}
