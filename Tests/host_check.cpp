#include <juce_audio_utils/juce_audio_utils.h>
#include <iostream>
#include <cmath>
#include <stdexcept>

void expect(bool ok, const char* reason) { if (!ok) throw std::runtime_error(reason); }
void set(juce::AudioPluginInstance& instance, juce::String name, float value) {
    for (auto* param : instance.getParameters())
        if (param->getName(100) == name) { param->setValueNotifyingHost(value); return; }
    throw std::runtime_error("missing parameter " + name.toStdString());
}
int main(int argc, char** argv) {
    try {
        juce::ScopedJuceInitialiser_GUI gui;
        expect(argc >= 3, "usage: host_check plugin_path screenshot_path");
        juce::AudioPluginFormatManager formats; formats.addDefaultFormats();
        juce::KnownPluginList list; juce::OwnedArray<juce::PluginDescription> descriptions;
        for (auto* format : formats.getFormats())
            list.scanAndAddFile(juce::String::fromUTF8(argv[1]), true, descriptions, *format);
        expect(descriptions.size() == 1, "VST3 could not be scanned");
        expect(descriptions[0]->name == "NoteShaper", "product rename missing from VST3");
        std::cout << "Scanned: " << descriptions[0]->name << " / " << descriptions[0]->pluginFormatName << '\n';
        juce::String error;
        auto plugin = formats.createPluginInstance(*descriptions[0], 48000, 256, error);
        expect(plugin != nullptr, error.toRawUTF8());
        std::cout << "Parameter count: " << plugin->getParameters().size() << '\n';
        plugin->enableAllBuses(); plugin->prepareToPlay(48000, 256);
        expect(plugin->getLatencySamples() == 3072, "wrong reported latency");
        set(*plugin, "Root note", 9.0f / 11.0f); set(*plugin, "Target notes", 0);
        set(*plugin, "Retune time", 0); set(*plugin, "Follow strength", 1);
        juce::MemoryBlock state; plugin->getStateInformation(state); expect(state.getSize() > 100, "empty state");
        set(*plugin, "Follow strength", 0); plugin->setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        for (auto* p : plugin->getParameters())
            if (p->getName(100) == "Follow strength") expect(p->getValue() > 0.99f, "state restore failed");
        juce::AudioBuffer<float> audio(2, 256); juce::MidiBuffer midi;
        double energy = 0; int samples = 0;
        for (int block = 0; block < 400; ++block) {
            for (int i = 0; i < 256; ++i) {
                float value = 0.2f * std::sin(2 * 3.14159265358979323846 * 234.0 * (block * 256 + i) / 48000.0);
                audio.setSample(0, i, value); audio.setSample(1, i, value * 0.7f);
            }
            plugin->processBlock(audio, midi);
            for (int i = 0; i < 256; ++i) {
                float l = audio.getSample(0, i), r = audio.getSample(1, i);
                expect(std::isfinite(l) && std::isfinite(r), "invalid plugin output");
                expect(std::abs(r - l * 0.7f) < 0.00001f, "stereo image changed");
                if (block > 30) { energy += l * l; ++samples; }
            }
        }
        expect(energy / samples > 0.001, "plugin is silent");
        std::unique_ptr<juce::AudioProcessorEditor> editor(plugin->createEditor());
        expect(editor != nullptr, "editor failed");
        editor->setVisible(true);
        expect(editor->getWidth() == 720 && editor->getHeight() == 552, "editor has wrong dimensions");
        editor.reset();
        // MIDI note-off and no held-note state must return delayed dry audio.
        plugin->reset(); set(*plugin, "Follow MIDI notes", 1);
        std::vector<float> expected(48000, 0), rendered(48000, 0);
        int total = 0;
        for (int block = 0; total < 48000; ++block) {
            int n = std::min(256, 48000 - total); audio.setSize(2, n, false, false, true); midi.clear();
            if (block == 0) midi.addEvent(juce::MidiMessage::noteOn(1, 69, static_cast<juce::uint8>(100)), 0);
            if (block == 10) midi.addEvent(juce::MidiMessage::noteOff(1, 69), 128);
            for (int i = 0; i < n; ++i) {
                expected[total + i] = 0.2f * std::sin(2 * 3.14159265358979323846 * 234 * (total + i) / 48000.0);
                audio.setSample(0, i, expected[total + i]); audio.setSample(1, i, expected[total + i]);
            }
            plugin->processBlock(audio, midi);
            for (int i = 0; i < n; ++i) rendered[total + i] = audio.getSample(0, i);
            total += n;
        }
        for (int i = 10000; i < 48000; ++i) expect(std::abs(rendered[i] - expected[i - 3072]) < 1e-6f, "MIDI release fails to bypass");
        plugin->releaseResources();
        if (argc >= 4) {
            juce::KnownPluginList oldList; juce::OwnedArray<juce::PluginDescription> oldDescriptions;
            for (auto* format : formats.getFormats())
                oldList.scanAndAddFile(juce::String::fromUTF8(argv[3]), true, oldDescriptions, *format);
            expect(oldDescriptions.size() == 1, "old VST3 could not be scanned");
            expect(oldDescriptions[0]->uniqueId == descriptions[0]->uniqueId, "plugin identity changed");
            auto oldPlugin = formats.createPluginInstance(*oldDescriptions[0], 48000, 256, error);
            expect(oldPlugin != nullptr, "old VST3 could not be loaded");
            const auto& oldParams = oldPlugin->getParameters();
            const auto& newParams = plugin->getParameters();
            expect(newParams.size() == oldParams.size() + 2, "new parameters were not appended");
            // The remaining host parameters are JUCE-generated MIDI controllers, not stored NoteShaper settings.
            constexpr int noteFollowParameterCount = 18;
            expect(oldParams.size() >= noteFollowParameterCount, "missing NoteShaper parameters");
            for (int i = 0; i < noteFollowParameterCount; ++i) {
                expect(oldParams[i]->getName(100) == newParams[i]->getName(100), "parameter order changed");
                float value = static_cast<float>((i * 7 + 3) % 19) / 18.0f;
                const int choices = i == 0 ? 12 : i == 1 ? 11 : i >= 5 ? 2 : 0;
                if (choices > 0) value = std::round(value * (choices - 1)) / (choices - 1);
                oldParams[i]->setValueNotifyingHost(value);
            }
            oldPlugin->enableAllBuses(); oldPlugin->prepareToPlay(48000, 256);
            juce::AudioBuffer<float> oldAudio(2, 256); oldAudio.clear(); juce::MidiBuffer oldMidi;
            oldPlugin->processBlock(oldAudio, oldMidi); // Flush host automation into the old audio processor.
            juce::MemoryBlock oldState; oldPlugin->getStateInformation(oldState);
            // Canonicalise the old controller cache to the actual saved, stepped parameter values.
            oldPlugin->setStateInformation(oldState.getData(), static_cast<int>(oldState.getSize()));
            set(*plugin, "Target scale", 1); set(*plugin, "Natural voice", 1);
            audio.clear(); midi.clear(); plugin->processBlock(audio, midi);
            plugin->setStateInformation(oldState.getData(), static_cast<int>(oldState.getSize()));
            for (auto* p : newParams)
                if (p->getName(100) == "Target scale" || p->getName(100) == "Natural voice")
                    expect(p->getValue() == 0, "legacy state inherited new settings");
            for (int i = 0; i < noteFollowParameterCount; ++i) {
                if (std::abs(oldParams[i]->getValue() - newParams[i]->getValue()) >= 1e-5f)
                    std::cerr << "State mismatch " << i << " " << oldParams[i]->getName(100)
                              << ": old=" << oldParams[i]->getValue() << " new=" << newParams[i]->getValue() << '\n';
                expect(std::abs(oldParams[i]->getValue() - newParams[i]->getValue()) < 1e-5f, "0.1 state value changed");
            }
            oldPlugin->releaseResources();
            std::cout << "PASS: state from legacy " << oldDescriptions[0]->version
                      << " binary restores all 18 parameters in NoteShaper " << descriptions[0]->version
                      << ", same VST3 identity; new settings default off\n";
        }
        std::cout << "PASS: VST3 scan/load, stereo, state restore, native editor creation, MIDI note-off, latency\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; return 1; }
}

