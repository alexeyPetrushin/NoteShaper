#include "PluginEditor.h"

namespace {
const juce::Colour background{0xfff8f8fa}, panel{0xffffffff}, line{0xffdadce2};
const juce::Colour text{0xff202127}, muted{0xff626570}, accent{0xff202127};
const juce::Colour controlLine{0xff808591}, rail{0xffedeef2};
juce::Font uiFont(float size, int style = juce::Font::plain) { return juce::Font("Segoe UI", size, style); }
const juce::StringArray names{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const juce::StringArray chords{juce::String::fromUTF8(u8"Мажор"), juce::String::fromUTF8(u8"Минор"), juce::String::fromUTF8(u8"Доминантсептаккорд"), juce::String::fromUTF8(u8"Мажорный септаккорд"),
                             juce::String::fromUTF8(u8"Минорный септаккорд"), "Sus2", "Sus4", juce::String::fromUTF8(u8"Квинта")};
void label(juce::Graphics& g, const juce::String& value, juce::Rectangle<int> bounds,
           float size = 14, juce::Colour colour = muted, int alignment = juce::Justification::left) {
    g.setColour(colour); g.setFont(uiFont(size)); g.drawText(value, bounds, alignment);
}
void identify(juce::Component& c, const juce::String& id, const juce::String& title, int order) {
    c.setComponentID(id); c.setTitle(title); c.setExplicitFocusOrder(order);
    c.setWantsKeyboardFocus(true);
}
juce::String tr(const char* value) { return juce::String::fromUTF8(value); }
}

NoteShaperSkin::NoteShaperSkin() {
    setColour(juce::ComboBox::backgroundColourId, panel);
    setColour(juce::ComboBox::textColourId, text);
    setColour(juce::ComboBox::outlineColourId, controlLine);
    setColour(juce::PopupMenu::backgroundColourId, panel);
    setColour(juce::PopupMenu::textColourId, text);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, accent);
    setColour(juce::PopupMenu::highlightedTextColourId, background);
    setColour(juce::Slider::textBoxTextColourId, text);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::highlightColourId, accent.withAlpha(0.3f));
    setColour(juce::TextEditor::focusedOutlineColourId, accent);
    setColour(juce::TextEditor::backgroundColourId, panel);
    setColour(juce::TextEditor::textColourId, text);
    setColour(juce::CaretComponent::caretColourId, text);
    setColour(juce::TooltipWindow::backgroundColourId, panel);
    setColour(juce::TooltipWindow::textColourId, text);
    setColour(juce::TooltipWindow::outlineColourId, line);
    setColour(juce::TextButton::textColourOffId, text);
    setColour(juce::TextButton::textColourOnId, text);
}

void NoteShaperSkin::drawButtonBackground(juce::Graphics& g, juce::Button& b,
                                         const juce::Colour&, bool hover, bool down) {
    const bool selected = b.getToggleState();
    auto bounds = b.getLocalBounds().toFloat().reduced(0.7f);
    const auto id = b.getComponentID();
    const bool segment = id.startsWith("mode") || id.startsWith("preset");
    const bool toggle = id == "midi" || id == "natural";
    if (toggle) {
        if (hover || down) { g.setColour(rail); g.fillRoundedRectangle(bounds, 11); }
        const float y = b.getHeight() * 0.5f;
        const float x = b.getWidth() - 50.0f;
        g.setColour(selected ? accent : controlLine); g.fillRoundedRectangle(x, y - 13, 46, 26, 13);
        g.setColour(panel); g.fillEllipse(x + (selected ? 23 : 3), y - 10, 20, 20);
    } else {
        auto fill = segment ? selected ? panel : juce::Colours::transparentBlack : selected ? accent : rail;
        if (hover || down) fill = segment && selected ? panel : selected ? fill.darker(0.1f) : rail.darker(0.015f);
        g.setColour(fill.withMultipliedAlpha(b.isEnabled() ? 1.0f : 0.4f));
        g.fillRoundedRectangle(bounds, segment ? 11 : 10);
        if (segment && selected) {
            g.setColour(line); g.drawRoundedRectangle(bounds, 11, 0.8f);
        }
        if (!segment && selected) {
            g.setColour(panel); g.fillEllipse(b.getWidth() * 0.5f - 2, b.getHeight() - 10.0f, 4, 4);
        }
    }
    if (b.hasKeyboardFocus(true)) {
        g.setColour(accent); g.drawRoundedRectangle(bounds.reduced(1), toggle ? 11 : 10, 2);
    }
}
void NoteShaperSkin::drawButtonText(juce::Graphics& g, juce::TextButton& b, bool, bool) {
    const auto id = b.getComponentID();
    const bool toggle = id == "midi" || id == "natural";
    const bool segment = id.startsWith("mode") || id.startsWith("preset");
    const bool selected = b.getToggleState();
    g.setFont(uiFont(id == "natural" ? 18 : 17, selected && !toggle ? juce::Font::bold : juce::Font::plain));
    g.setColour((selected && !segment && !toggle ? panel : text).withAlpha(b.isEnabled() ? 1.0f : 0.45f));
    auto bounds = b.getLocalBounds().reduced(6);
    if (toggle) { bounds.removeFromRight(54); g.drawFittedText(b.getButtonText(), bounds, juce::Justification::left, 1); }
    else g.drawFittedText(b.getButtonText(), bounds, juce::Justification::centred, 1);
}
void NoteShaperSkin::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                     float value, float start, float end, juce::Slider& s) {
    const float cx = x + w * 0.5f, cy = y + h * 0.5f;
    const float r = juce::jmin(w, h) * 0.5f - 9;
    const float angle = start + value * (end - start);
    juce::Path track; track.addCentredArc(cx, cy, r, r, 0, start, end, true);
    const juce::PathStrokeType stroke(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(line); g.strokePath(track, stroke);
    if (value > 0) {
        juce::Path active; active.addCentredArc(cx, cy, r, r, 0, start, angle, true);
        g.setColour(text); g.strokePath(active, stroke);
    }
    const float body = r * 0.76f;
    g.setColour(background);
    g.fillEllipse(cx - body, cy - body, body * 2, body * 2);
    juce::Path pointer;
    pointer.startNewSubPath(cx + std::sin(angle) * r * 0.30f, cy - std::cos(angle) * r * 0.30f);
    pointer.lineTo(cx + std::sin(angle) * r * 0.64f, cy - std::cos(angle) * r * 0.64f);
    g.setColour(text); g.strokePath(pointer, juce::PathStrokeType(3, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    if (s.hasKeyboardFocus(true)) {
        g.setColour(accent); g.drawEllipse(cx - r - 7, cy - r - 7, r * 2 + 14, r * 2 + 14, 1.2f);
    }
}
juce::Label* NoteShaperSkin::createSliderTextBox(juce::Slider& slider) {
    auto* box = juce::LookAndFeel_V4::createSliderTextBox(slider);
    box->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    return box;
}
juce::Font NoteShaperSkin::getLabelFont(juce::Label& l) {
    return dynamic_cast<juce::Slider*>(l.getParentComponent()) ? uiFont(26) : l.getFont();
}
void NoteShaperSkin::drawLabel(juce::Graphics& g, juce::Label& l) {
    if (dynamic_cast<juce::Slider*>(l.getParentComponent())) {
        if (!l.isBeingEdited()) {
            g.setColour(rail); g.fillRoundedRectangle(l.getLocalBounds().toFloat().reduced(1), 8);
            g.setColour(text); g.setFont(getLabelFont(l));
            g.drawFittedText(l.getText(), l.getLocalBounds().reduced(3, 1), juce::Justification::centred, 1);
        }
        if (l.hasKeyboardFocus(true)) {
            g.setColour(accent); g.drawRoundedRectangle(l.getLocalBounds().toFloat().reduced(1), 6, 1);
        }
    } else juce::LookAndFeel_V4::drawLabel(g, l);
}
void NoteShaperSkin::drawComboBox(juce::Graphics& g, int w, int h, bool,
                                 int, int, int, int, juce::ComboBox& box) {
    auto bounds = juce::Rectangle<float>(0.7f, 0.7f, w - 1.4f, h - 1.4f);
    g.setColour(panel.withAlpha(box.isEnabled() ? 1.0f : 0.4f)); g.fillRoundedRectangle(bounds, 10);
    g.setColour(box.hasKeyboardFocus(true) ? accent : controlLine.withAlpha(0.6f)); g.drawRoundedRectangle(bounds, 10, box.hasKeyboardFocus(true) ? 2 : 0.8f);
    juce::Path arrow; arrow.startNewSubPath(w - 24.0f, h * 0.5f - 2);
    arrow.lineTo(w - 19.0f, h * 0.5f + 3); arrow.lineTo(w - 14.0f, h * 0.5f - 2);
    g.setColour(muted); g.strokePath(arrow, juce::PathStrokeType(1.6f));
}
juce::Font NoteShaperSkin::getComboBoxFont(juce::ComboBox& box) { return uiFont(box.getComponentID() == "root" ? 40 : 25); }

NoteShaperEditor::NoteShaperEditor(NoteShaperProcessor& p) : AudioProcessorEditor(p), processor(p) {
    setLookAndFeel(&skin);
    setTitle(juce::String::fromUTF8(u8"NoteShaper — подтяжка вокала"));
    const juce::StringArray modeNames{tr(u8"Нота"), tr(u8"Аккорд"), tr(u8"Гамма"), tr(u8"Свой набор")};
    for (int i = 0; i < 4; ++i) {
        modes[i].setButtonText(modeNames[i]); addAndMakeVisible(modes[i]);
        identify(modes[i], "mode" + juce::String(i), modeNames[i], i + 1);
        modes[i].onClick = [this, i] { chooseMode(i); };
    }
    modes[0].setTooltip(juce::String::fromUTF8(u8"Следовать одной ноте. Октава выбирается рядом с голосом."));
    modes[1].setTooltip(juce::String::fromUTF8(u8"Следовать ближайшей ноте выбранного аккорда."));
    modes[2].setTooltip(tr(u8"Подтягивать голос к нотам гаммы. Выбери лад и основную ноту."));
    modes[3].setTooltip(tr(u8"Включить свой набор нот. Текущий набор сохраняется при переходе сюда."));
    for (int i = 0; i < names.size(); ++i) root.addItem(names[i], i + 1);
    addAndMakeVisible(root); identify(root, "root", tr(u8"Основная нота"), 5);
    root.setTooltip(tr(u8"Выберите основную ноту. Октава подбирается рядом с голосом."));
    root.onChange = [this] { if (root.getSelectedId() > 0) chooseNote(root.getSelectedId() - 1); };
    for (int i = 0; i < chords.size(); ++i) chord.addItem(chords[i], i + 2);
    addAndMakeVisible(chord); identify(chord, "chord", juce::String::fromUTF8(u8"Тип аккорда"), 5);
    chord.setTooltip(juce::String::fromUTF8(u8"Аккорд задаёт допустимые ноты для одного голоса."));
    chord.onChange = [this] {
        const int choice = chord.getSelectedId() - 1;
        if (choice >= 1 && choice <= 8) {
            rememberedChord = choice; processor.setParameter("chord", static_cast<float>(choice)); refresh();
        }
    };
    const juce::StringArray scaleNames{tr(u8"Мажор"), tr(u8"Минор"), tr(u8"Гармонич. минор"),
                                      tr(u8"Мажорная пентатоника"), tr(u8"Минорная пентатоника")};
    for (int i = 0; i < scaleNames.size(); ++i) scale.addItem(scaleNames[i], i + 1);
    addAndMakeVisible(scale); identify(scale, "scale", tr(u8"Тип гаммы"), 5);
    scale.setTooltip(tr(u8"Гамма задаёт допустимые ноты всей мелодии, а не только ноты аккорда."));
    scale.onChange = [this] {
        if (scale.getSelectedId() > 0) {
            rememberedScale = scale.getSelectedId(); processor.setParameter("scale", static_cast<float>(rememberedScale)); refresh();
        }
    };
    for (int i = 0; i < 12; ++i) {
        keys[i].setButtonText(names[i]); addAndMakeVisible(keys[i]);
        identify(keys[i], "note" + juce::String(i), names[i], i + 7);
        keys[i].onClick = [this, i] { chooseNote(i); };
    }
    const juce::StringArray presetNames{juce::String::fromUTF8(u8"Мягко"), juce::String::fromUTF8(u8"Плотно"), juce::String::fromUTF8(u8"Рэп")};
    const juce::StringArray presetHints{
        juce::String::fromUTF8(u8"55% · 85 мс · допуск 8 центов · живой голос. Выбранные ноты сохраняются."),
        juce::String::fromUTF8(u8"90% · 14 мс · допуск 2 цента. Выбранные ноты сохраняются."),
        juce::String::fromUTF8(u8"100% · 0 мс · без допуска. Резкое выравнивание по выбранным нотам.")};
    for (int i = 0; i < 3; ++i) {
        presets[i].setButtonText(presetNames[i]); addAndMakeVisible(presets[i]);
        identify(presets[i], "preset" + juce::String(i), presetNames[i], i + 20);
        presets[i].setTooltip(presetHints[i]);
        presets[i].onClick = [this, i] { processor.applyPreset(i); refresh(); };
    }
    for (auto* slider : {&strength, &speed, &freedom}) {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 116, 32);
        slider->setScrollWheelEnabled(false);
        slider->setNumDecimalPlacesToDisplay(1);
        addAndMakeVisible(*slider);
    }
    strength.setTextValueSuffix(" %"); speed.setTextValueSuffix(juce::String::fromUTF8(u8" мс")); freedom.setTextValueSuffix(juce::String::fromUTF8(u8" ц"));
    identify(strength, "amount", juce::String::fromUTF8(u8"Сила подтяжки"), 23);
    identify(speed, "speed", juce::String::fromUTF8(u8"Время подтяжки"), 24);
    identify(freedom, "tolerance", juce::String::fromUTF8(u8"Допуск отклонения в центах"), 25);
    strength.setTooltip(juce::String::fromUTF8(u8"0% — исходная высота. 100% — полное следование выбранным нотам. Число можно ввести вручную."));
    speed.setTooltip(juce::String::fromUTF8(u8"0 мс — резкие переходы. Чем больше время, тем плавнее подтяжка."));
    freedom.setTooltip(juce::String::fromUTF8(u8"Небольшие отклонения в пределах допуска сохраняются. 100 центов = полутон."));
    strengthAttachment = std::make_unique<SliderAttachment>(p.parameters, "amount", strength);
    speedAttachment = std::make_unique<SliderAttachment>(p.parameters, "speed", speed);
    freedomAttachment = std::make_unique<SliderAttachment>(p.parameters, "tolerance", freedom);
    for (auto* slider : {&strength, &speed, &freedom}) {
        slider->textFromValueFunction = [slider](double value) {
            return juce::String(value, std::abs(value - std::round(value)) < 0.001 ? 0 : 1);
        };
        slider->updateText();
    }
    strength.onValueChange = [this] { refresh(); };
    speed.onValueChange = [this] { refresh(); };
    freedom.onValueChange = [this] { refresh(); };
    midi.setButtonText("MIDI"); midi.setClickingTogglesState(true); addAndMakeVisible(midi);
    identify(midi, "midi", juce::String::fromUTF8(u8"Ноты из MIDI"), 6);
    midi.setTooltip(juce::String::fromUTF8(u8"Использовать MIDI-ноты вместо выбранного набора. Педаль sustain удерживает ноты. Без нот — исходная высота."));
    midiAttachment = std::make_unique<ButtonAttachment>(p.parameters, "midi", midi);
    midi.onClick = [this] { refresh(); };
    natural.setButtonText(tr(u8"Сохранить вибрато")); natural.setClickingTogglesState(true); addAndMakeVisible(natural);
    identify(natural, "natural", tr(u8"Сохранить движения голоса"), 19);
    natural.setTooltip(tr(u8"Подтягивать центр ноты, сохраняя больше вибрато. Короткие колебания около границы нот не меняют цель. «Мягко» включает этот режим, «Рэп» выключает."));
    naturalAttachment = std::make_unique<ButtonAttachment>(p.parameters, "natural", natural);
    natural.onClick = [this] { refresh(); };
    for (auto* l : {&status, &summary, &noteCaption}) {
        addAndMakeVisible(*l); l->setColour(juce::Label::textColourId, muted);
        l->setBorderSize(juce::BorderSize<int>(0)); l->setFont(uiFont(16));
    }
    status.setComponentID("status"); summary.setComponentID("summary"); noteCaption.setComponentID("noteCaption");
    status.setJustificationType(juce::Justification::right); status.setFont(uiFont(15));
    const int initialChord = static_cast<int>(p.parameters.getRawParameterValue("chord")->load());
    if (initialChord >= 1 && initialChord <= 8) rememberedChord = initialChord;
    if (p.settings().scale > 0) rememberedScale = p.settings().scale;
    setSize(720, 624);
    refresh(); startTimerHz(24);
}
NoteShaperEditor::~NoteShaperEditor() { stopTimer(); setLookAndFeel(nullptr); }

unsigned NoteShaperEditor::targetMask() const {
    return processor.parameters.getRawParameterValue("midi")->load() > 0.5f
        ? processor.activeNotes.load() : notefollow::noteMask(processor.settings());
}
void NoteShaperEditor::chooseMode(int mode) {
    if (processor.parameters.getRawParameterValue("midi")->load() > 0.5f) return;
    const int current = static_cast<int>(processor.parameters.getRawParameterValue("chord")->load());
    if (current >= 1 && current <= 8) rememberedChord = current;
    if (processor.settings().scale > 0) rememberedScale = processor.settings().scale;
    if (mode == 3 && (current != 10 || processor.settings().scale > 0)) {
        const auto mask = notefollow::noteMask(processor.settings());
        for (int i = 0; i < 12; ++i)
            processor.setParameter("note" + juce::String(i), (mask & (1u << i)) ? 1.0f : 0.0f);
    }
    processor.setParameter("scale", mode == 2 ? static_cast<float>(rememberedScale) : 0.0f);
    processor.setParameter("chord", mode == 0 ? 0.0f : mode == 1 ? static_cast<float>(rememberedChord) : 10.0f);
    refresh();
}
void NoteShaperEditor::chooseNote(int note) {
    if (processor.parameters.getRawParameterValue("midi")->load() > 0.5f) return;
    if (processor.settings().scale == 0 && processor.parameters.getRawParameterValue("chord")->load() >= 9.0f) {
        const auto mask = notefollow::noteMask(processor.settings());
        for (int i = 0; i < 12; ++i)
            processor.setParameter("note" + juce::String(i), (mask & (1u << i)) ? 1.0f : 0.0f);
        processor.setParameter("note" + juce::String(note), (mask & (1u << note)) ? 0.0f : 1.0f);
        processor.setParameter("chord", 10.0f);
    } else processor.setParameter("root", static_cast<float>(note));
    refresh();
}
juce::String NoteShaperEditor::noteText(float value) const {
    if (value < 0 || !std::isfinite(value)) return juce::String::fromUTF8("—");
    const int n = static_cast<int>(std::round(value));
    return names[((n % 12) + 12) % 12] + juce::String(n / 12 - 1);
}
void NoteShaperEditor::timerCallback() { refresh(); }
void NoteShaperEditor::refresh() {
    const auto s = processor.settings();
    const bool byMidi = processor.parameters.getRawParameterValue("midi")->load() > 0.5f;
    const int mode = s.scale > 0 ? 2 : s.chord == 0 ? 0 : s.chord < 9 ? 1 : 3;
    const auto mask = targetMask();
    for (int i = 0; i < 4; ++i) {
        modes[i].setToggleState(i == mode, juce::dontSendNotification); modes[i].setEnabled(!byMidi);
    }
    root.setVisible(mode != 3 && !byMidi); root.setEnabled(!byMidi);
    root.setSelectedId(s.root + 1, juce::dontSendNotification);
    chord.setVisible(mode == 1 && !byMidi); chord.setEnabled(!byMidi);
    scale.setVisible(mode == 2 && !byMidi); scale.setEnabled(!byMidi);
    if (mode == 1) {
        rememberedChord = s.chord; chord.setSelectedId(s.chord + 1, juce::dontSendNotification);
    }
    if (mode == 2) { rememberedScale = s.scale; scale.setSelectedId(s.scale, juce::dontSendNotification); }
    juce::StringArray allowed;
    for (int i = 0; i < 12; ++i) {
        const bool included = (mask & (1u << i)) != 0;
        keys[i].setToggleState(byMidi || mode == 3 ? included : i == s.root, juce::dontSendNotification);
        keys[i].setEnabled(!byMidi);
        keys[i].setVisible(mode == 3 && !byMidi);
        keys[i].setTooltip(mode == 3 ? juce::String(juce::String::fromUTF8(u8"Включить или исключить ")) + names[i]
                                    : juce::String(juce::String::fromUTF8(u8"Выбрать основную ноту ")) + names[i]);
        if (included) allowed.add(names[i]);
    }
    noteCaption.setText(byMidi ? tr(u8"Направьте MIDI на эту дорожку") : mode == 1 ? tr(u8"Основа")
                        : mode == 2 ? tr(u8"Тоника") : mode == 3 ? tr(u8"Включите нужные ноты") : tr(u8"Нота"), juce::dontSendNotification);
    noteCaption.setBounds(32, 137, mode == 3 || byMidi ? 656 : 120, 18);
    summary.setText(mask == 0 ? juce::String(juce::String::fromUTF8(u8"Нет выбранных нот — исходная высота"))
                    : juce::String::fromUTF8(u8"Вести к: ") + allowed.joinIntoString(juce::String::fromUTF8("  ·  ")), juce::dontSendNotification);
    summary.setColour(juce::Label::textColourId, mask == 0 ? text : muted);
    constexpr float amounts[] = {0.55f, 0.90f, 1.0f}, times[] = {85, 14, 0}, tolerances[] = {8, 2, 0};
    for (int i = 0; i < 3; ++i) presets[i].setToggleState(
        std::abs(s.amount - amounts[i]) < 0.0005f && std::abs(s.speedMs - times[i]) < 0.05f
        && std::abs(s.toleranceCents - tolerances[i]) < 0.05f && s.natural == (i == 0), juce::dontSendNotification);
    const float in = processor.detected.load();
    juce::String state;
    if (mask == 0) state = byMidi ? juce::String(juce::String::fromUTF8(u8"MIDI: ждёт ноты")) : juce::String(juce::String::fromUTF8(u8"Нет выбранных нот"));
    else if (s.amount < 0.001f) state = juce::String(juce::String::fromUTF8(u8"Исходная высота"));
    else if (in < 0 || !std::isfinite(in)) state = byMidi ? juce::String(juce::String::fromUTF8(u8"MIDI · ждёт голос")) : juce::String(juce::String::fromUTF8(u8"Ждёт голос"));
    else state = (byMidi ? juce::String::fromUTF8("MIDI · ") : "") + noteText(in) + tr(u8"  →  ") + noteText(processor.destination.load())
                 + "   " + (processor.cents.load() >= 0 ? "+" : "") + juce::String(static_cast<int>(std::round(processor.cents.load()))) + tr(u8" ц");
    status.setText(state, juce::dontSendNotification);
    status.setColour(juce::Label::textColourId, muted);
    repaint();
}

void NoteShaperEditor::paint(juce::Graphics& g) {
    g.fillAll(background);
    label(g, "NoteShaper", {32, 20, 260, 32}, 26, text);
    label(g, tr(u8"Подтяжка вокала"), {420, 24, 268, 24}, 16, muted, juce::Justification::right);
    g.setColour(rail); g.fillRoundedRectangle(30, 76, 414, 48, 12);
    g.fillRoundedRectangle(378, 286, 312, 48, 12);
    const auto s = processor.settings();
    const bool byMidi = processor.parameters.getRawParameterValue("midi")->load() > 0.5f;
    if (byMidi) label(g, targetMask() == 0 ? tr(u8"Жду MIDI-ноты") : tr(u8"Ноты задаёт MIDI"), {32, 162, 656, 50}, 30, text);
    else if (s.scale > 0 || (s.chord >= 1 && s.chord <= 8))
        label(g, s.scale > 0 ? tr(u8"Лад") : tr(u8"Тип аккорда"), {164, 137, 524, 18}, 16);
    else if (s.chord == 0) label(g, tr(u8"Одна нота в ближайшей октаве"), {180, 173, 508, 30}, 18);
    g.setColour(line); g.drawHorizontalLine(264, 32, 688);
    label(g, tr(u8"Характер"), {32, 293, 230, 32}, 22, text);
    label(g, tr(u8"Сила"), {48, 350, 176, 25}, 20, text, juce::Justification::centred);
    label(g, tr(u8"Время"), {272, 350, 176, 25}, 20, text, juce::Justification::centred);
    label(g, tr(u8"Допуск"), {496, 350, 176, 25}, 20, text, juce::Justification::centred);
    label(g, tr(u8"Насколько исправлять"), {48, 375, 176, 20}, 14, muted, juce::Justification::centred);
    label(g, tr(u8"Резко или плавно"), {272, 375, 176, 20}, 14, muted, juce::Justification::centred);
    label(g, tr(u8"Оставить отклонения"), {496, 375, 176, 20}, 14, muted, juce::Justification::centred);
    g.setColour(line); g.drawHorizontalLine(555, 32, 688);
}
void NoteShaperEditor::resized() {
    status.setBounds(390, 579, 298, 24);
    for (int i = 0; i < 4; ++i) modes[i].setBounds(32 + i * 102, 78, 102, 44);
    root.setBounds(32, 160, 120, 56);
    chord.setBounds(164, 160, 524, 56); scale.setBounds(164, 160, 524, 56);
    midi.setBounds(554, 78, 134, 44);
    noteCaption.setBounds(32, 137, 656, 18);
    for (int i = 0; i < 12; ++i) keys[i].setBounds(32 + i * 55, 160, 51, 56);
    summary.setBounds(32, 231, 656, 22);
    natural.setBounds(32, 566, 274, 44);
    for (int i = 0; i < 3; ++i) presets[i].setBounds(380 + i * 102, 288, 102, 44);
    strength.setBounds(48, 399, 176, 140); speed.setBounds(272, 399, 176, 140); freedom.setBounds(496, 399, 176, 140);
}
