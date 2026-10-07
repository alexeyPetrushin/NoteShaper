#include "PluginProcessor.h"
#include "PluginEditor.h"

NoteShaperProcessor::NoteShaperProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "NoteFollowState", createParameters()) {
    const juce::StringArray ids{"root", "chord", "amount", "speed", "tolerance", "midi"};
    for (int i = 0; i < 6; ++i) cached[i] = parameters.getRawParameterValue(ids[i]);
    for (int i = 0; i < 12; ++i) cached[i + 6] = parameters.getRawParameterValue("note" + juce::String(i));
    cached[18] = parameters.getRawParameterValue("scale");
    cached[19] = parameters.getRawParameterValue("natural");
}

juce::AudioProcessorValueTreeState::ParameterLayout NoteShaperProcessor::createParameters() {
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"root", 1}, "Root note",
          juce::StringArray{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"chord", 1}, "Target notes",
          juce::StringArray{"Single note", "Major chord", "Minor chord", "Dominant 7", "Major 7",
          "Minor 7", "Sus2", "Sus4", "Power chord", "Chromatic", "Custom notes"}, 1));
    p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amount", 1}, "Follow strength",
          juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f}, 100.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
    p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"speed", 1}, "Retune time",
          juce::NormalisableRange<float>{0.0f, 250.0f, 0.1f, 0.45f}, 12.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"tolerance", 1}, "Pitch freedom",
          juce::NormalisableRange<float>{0.0f, 50.0f, 0.1f}, 0.0f, juce::AudioParameterFloatAttributes().withLabel("ct")));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"midi", 1}, "Follow MIDI notes", false));
    for (int i = 0; i < 12; ++i)
        p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"note" + juce::String(i), 1},
              "Custom " + juce::StringArray{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}[i],
              i == 0 || i == 4 || i == 7));
    // Append parameters: keep all original IDs, ranges and automation slots.
    p.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"scale", 2}, "Target scale",
          juce::StringArray{"Off", "Major", "Natural minor", "Harmonic minor", "Major pentatonic", "Minor pentatonic"}, 0));
    p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"natural", 2}, "Natural voice", false));
    return p;
}

void NoteShaperProcessor::prepareToPlay(double rate, int) { engine.prepare(rate); setLatencySamples(engine.latency()); reset(); }
void NoteShaperProcessor::clearMidi() { heldNotes.fill(0); sustainedNotes.fill(false); sustainDown.fill(false); }
void NoteShaperProcessor::reset() {
    engine.reset(); clearMidi(); detected.store(-1000); destination.store(-1000);
    cents.store(0); certainty.store(0); activeNotes.store(0);
}
bool NoteShaperProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
    return (l.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
        && l.getMainInputChannelSet() == l.getMainOutputChannelSet();
}
notefollow::Settings NoteShaperProcessor::settings() const {
    notefollow::Settings s;
    s.root = static_cast<int>(cached[0]->load());
    s.chord = static_cast<int>(cached[1]->load());
    s.amount = cached[2]->load() * 0.01f;
    s.speedMs = cached[3]->load();
    s.toleranceCents = cached[4]->load();
    s.scale = static_cast<int>(cached[18]->load());
    s.natural = cached[19]->load() > 0.5f;
    s.customMask = 0;
    for (int i = 0; i < 12; ++i)
        if (cached[i + 6]->load() > 0.5f) s.customMask |= 1u << i;
    return s;
}
unsigned NoteShaperProcessor::midiMask() const {
    unsigned mask = 0;
    for (int channel = 0; channel < 16; ++channel)
        for (int note = 0; note < 128; ++note)
            if (heldNotes[channel * 128 + note] || sustainedNotes[channel * 128 + note]) mask |= 1u << (note % 12);
    return mask;
}
void NoteShaperProcessor::handleMidi(const juce::MidiMessage& m) {
    int channel = juce::jlimit(0, 15, m.getChannel() - 1);
    const int offset = channel * 128;
    if (m.isController() && (m.getControllerNumber() == 64 || m.getControllerNumber() == 121)) {
        sustainDown[channel] = m.getControllerNumber() == 64 && m.getControllerValue() >= 64;
        if (!sustainDown[channel])
            std::fill(sustainedNotes.begin() + offset, sustainedNotes.begin() + offset + 128, false);
    } else if (m.isAllNotesOff() || m.isAllSoundOff()) {
        for (int note = offset; note < offset + 128; ++note) {
            sustainedNotes[note] = !m.isAllSoundOff() && sustainDown[channel]
                                  && (heldNotes[note] || sustainedNotes[note]);
            heldNotes[note] = 0;
        }
    } else if (m.isNoteOn()) {
        auto& count = heldNotes[channel * 128 + m.getNoteNumber()];
        sustainedNotes[channel * 128 + m.getNoteNumber()] = false;
        if (count < 255) ++count;
    } else if (m.isNoteOff()) {
        auto& count = heldNotes[channel * 128 + m.getNoteNumber()];
        if (count && --count == 0) sustainedNotes[channel * 128 + m.getNoteNumber()] = sustainDown[channel];
    }
}
void NoteShaperProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi, bool bypassed) {
    juce::ScopedNoDenormals noDenormals;
    if (clearMidiRequested.exchange(false)) clearMidi();
    auto s = settings();
    bool followMidi = cached[5]->load() > 0.5f;
    if (bypassed) s.amount = 0;
    auto render = [&](int first, int count) {
        if (count <= 0) return;
        if (followMidi) { s.scale = 0; s.chord = 10; s.customMask = midiMask(); }
        activeNotes.store(notefollow::noteMask(s));
        engine.process(buffer.getWritePointer(0) + first,
            buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) + first : nullptr, count, s);
    };
    int position = 0;
    for (const auto metadata : midi) {
        const int event = juce::jlimit(position, buffer.getNumSamples(), metadata.samplePosition);
        render(position, event - position); handleMidi(metadata.getMessage()); position = event;
    }
    render(position, buffer.getNumSamples() - position);
    if (followMidi) activeNotes.store(midiMask());
    detected.store(static_cast<float>(engine.pitch())); destination.store(static_cast<float>(engine.target()));
    cents.store(static_cast<float>(engine.correctionCents())); certainty.store(static_cast<float>(engine.pitchConfidence()));
    midi.clear();
}
void NoteShaperProcessor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer& m) { process(b, m, false); }
void NoteShaperProcessor::processBlockBypassed(juce::AudioBuffer<float>& b, juce::MidiBuffer& m) { process(b, m, true); }
void NoteShaperProcessor::setParameter(const juce::String& id, float value) {
    if (auto* p = parameters.getParameter(id)) {
        p->beginChangeGesture(); p->setValueNotifyingHost(p->convertTo0to1(value)); p->endChangeGesture();
    }
}
void NoteShaperProcessor::applyPreset(int preset) {
    constexpr float amount[] = {55, 90, 100};
    constexpr float speed[] = {85, 14, 0};
    constexpr float freedom[] = {8, 2, 0};
    int i = juce::jlimit(0, 2, preset);
    setParameter("amount", amount[i]); setParameter("speed", speed[i]); setParameter("tolerance", freedom[i]);
    setParameter("natural", i == 0 ? 1.0f : 0.0f);
}
juce::AudioProcessorEditor* NoteShaperProcessor::createEditor() { return new NoteShaperEditor(*this); }
void NoteShaperProcessor::getStateInformation(juce::MemoryBlock& dest) {
    if (auto xml = parameters.copyState().createXml()) copyXmlToBinary(*xml, dest);
}
void NoteShaperProcessor::setStateInformation(const void* data, int size) {
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(parameters.state.getType())) {
            auto state = juce::ValueTree::fromXml(*xml);
            for (const auto& id : {"scale", "natural"})
                if (!state.getChildWithProperty("id", id).isValid()) {
                    juce::ValueTree child("PARAM"); child.setProperty("id", id, nullptr);
                    child.setProperty("value", 0.0f, nullptr); state.appendChild(child, nullptr);
                }
            parameters.replaceState(state); clearMidiRequested.store(true);
        }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new NoteShaperProcessor(); }
