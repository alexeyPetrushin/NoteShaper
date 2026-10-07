#include "PluginEditor.h"

namespace {
const juce::Colour background{0xfff8f8fa}, panel{0xffffffff}, line{0xffdadce2};
const juce::Colour text{0xff202127}, muted{0xff626570}, accent{0xff202127};
const juce::Colour controlLine{0xff808591}, rail{0xffedeef2};
constexpr float captionSize = 14, bodySize = 18, valueSize = 20;
constexpr float corner = 6;
juce::Font uiFont(float size, int style = juce::Font::plain) { return juce::Font("Segoe UI", size, style); }
const juce::StringArray names{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
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
    setDefaultSansSerifTypefaceName("Segoe UI");
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
    const bool toggle = id == "natural";
    if (id == "language") {
        if (hover || down) { g.setColour(rail); g.fillRoundedRectangle(bounds, corner); }
    } else if (toggle) {
        if (hover || down) { g.setColour(rail); g.fillRoundedRectangle(bounds, corner); }
        const float y = b.getHeight() * 0.5f;
        const juce::Rectangle<float> check(1, y - 10, 20, 20);
        g.setColour(selected ? accent : panel); g.fillRoundedRectangle(check, 4);
        g.setColour(selected ? accent : controlLine); g.drawRoundedRectangle(check, 4, 1);
        if (selected) {
            juce::Path mark; mark.startNewSubPath(6, y); mark.lineTo(10, y + 4); mark.lineTo(17, y - 4);
            g.setColour(panel); g.strokePath(mark, juce::PathStrokeType(1.8f));
        }
    } else {
        auto fill = selected ? accent : panel;
        if (hover || down) fill = selected ? fill.brighter(0.1f) : rail;
        g.setColour(fill.withMultipliedAlpha(b.isEnabled() ? 1.0f : 0.4f));
        g.fillRoundedRectangle(bounds, corner);
        g.setColour(selected ? accent : controlLine.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, corner, 0.8f);
    }
    if (b.hasKeyboardFocus(true)) {
        g.setColour(accent); g.drawRoundedRectangle(bounds.reduced(1), corner, 2);
    }
}
void NoteShaperSkin::drawButtonText(juce::Graphics& g, juce::TextButton& b, bool, bool) {
    const auto id = b.getComponentID();
    if (id == "language") {
        const bool ru = b.getToggleState();
        g.setFont(uiFont(captionSize, ru ? juce::Font::plain : juce::Font::bold));
        g.setColour(ru ? muted : text); g.drawText("EN", juce::Rectangle<int>(4, 0, 28, b.getHeight()), juce::Justification::centred);
        g.setFont(uiFont(captionSize)); g.setColour(muted);
        g.drawText("/", juce::Rectangle<int>(32, 0, 12, b.getHeight()), juce::Justification::centred);
        g.setFont(uiFont(captionSize, ru ? juce::Font::bold : juce::Font::plain));
        g.setColour(ru ? text : muted); g.drawText("RU", juce::Rectangle<int>(44, 0, 28, b.getHeight()), juce::Justification::centred);
        return;
    }
    const bool toggle = id == "natural";
    const bool selected = b.getToggleState();
    g.setFont(uiFont(bodySize));
    g.setColour((selected && !toggle ? panel : text).withAlpha(b.isEnabled() ? 1.0f : 0.45f));
    auto bounds = b.getLocalBounds().reduced(6);
    if (toggle) { bounds.setLeft(32); g.drawFittedText(b.getButtonText(), bounds, juce::Justification::left, 1); }
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
    return dynamic_cast<juce::Slider*>(l.getParentComponent()) ? uiFont(valueSize) : l.getFont();
}
void NoteShaperSkin::drawLabel(juce::Graphics& g, juce::Label& l) {
    if (dynamic_cast<juce::Slider*>(l.getParentComponent())) {
        if (!l.isBeingEdited()) {
            if (l.isMouseOverOrDragging()) {
                g.setColour(rail); g.fillRoundedRectangle(l.getLocalBounds().toFloat().reduced(1), corner);
            }
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
    g.setColour((box.isMouseOverOrDragging() ? rail : panel).withAlpha(box.isEnabled() ? 1.0f : 0.4f));
    g.fillRoundedRectangle(bounds, corner);
    g.setColour(box.hasKeyboardFocus(true) ? accent : controlLine.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, corner, box.hasKeyboardFocus(true) ? 2 : 0.8f);
    juce::Path arrow; arrow.startNewSubPath(w - 24.0f, h * 0.5f - 2);
    arrow.lineTo(w - 19.0f, h * 0.5f + 3); arrow.lineTo(w - 14.0f, h * 0.5f - 2);
    g.setColour(muted); g.strokePath(arrow, juce::PathStrokeType(1.6f));
}
juce::Font NoteShaperSkin::getComboBoxFont(juce::ComboBox&) { return uiFont(bodySize); }
juce::Font NoteShaperSkin::getPopupMenuFont() { return uiFont(bodySize); }
void NoteShaperSkin::positionComboBoxText(juce::ComboBox& box, juce::Label& l) {
    l.setBounds(12, 0, box.getWidth() - 44, box.getHeight());
    l.setBorderSize(juce::BorderSize<int>(0)); l.setFont(uiFont(bodySize));
}
void NoteShaperSkin::drawComboBoxTextWhenNothingSelected(juce::Graphics& g, juce::ComboBox& box, juce::Label& l) {
    g.setColour(text); g.setFont(uiFont(bodySize));
    g.drawText(box.getTextWhenNothingSelected(), l.getBounds(), juce::Justification::left);
}

NoteShaperEditor::NoteShaperEditor(NoteShaperProcessor& p) : AudioProcessorEditor(p), processor(p) {
    setLookAndFeel(&skin);
    russian = p.parameters.state.getProperty("editorLanguage", "en").toString() == "ru";
    setTitle(juce::String::fromUTF8(u8"NoteShaper — подтяжка вокала"));
    const juce::StringArray modeNames{tr(u8"Нота"), tr(u8"Аккорд"), tr(u8"Гамма"), tr(u8"Свой набор"), "MIDI"};
    for (int i = 0; i < modeNames.size(); ++i) mode.addItem(modeNames[i], i + 1);
    addAndMakeVisible(mode); identify(mode, "mode", tr(u8"Способ выбора нот"), 1);
    mode.setTooltip(tr(u8"Нота, аккорд, гамма, свой набор или ноты из MIDI. Сначала выбери способ, затем ноты справа."));
    mode.onChange = [this] { if (mode.getSelectedId() > 0) chooseMode(mode.getSelectedId() - 1); };
    for (int i = 0; i < names.size(); ++i) root.addItem(names[i], i + 1);
    addAndMakeVisible(root); identify(root, "root", tr(u8"Основная нота"), 2);
    root.setTooltip(tr(u8"Выберите основную ноту. Октава подбирается рядом с голосом."));
    root.onChange = [this] { if (root.getSelectedId() > 0) chooseNote(root.getSelectedId() - 1); };
    addAndMakeVisible(chord); identify(chord, "chord", juce::String::fromUTF8(u8"Тип аккорда"), 3);
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
    addAndMakeVisible(scale); identify(scale, "scale", tr(u8"Тип гаммы"), 3);
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
    for (int i = 0; i < 3; ++i) {
        addAndMakeVisible(presets[i]); identify(presets[i], "preset" + juce::String(i), "", i + 20);
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
    natural.setButtonText(tr(u8"Сохранить вибрато")); natural.setClickingTogglesState(true); addAndMakeVisible(natural);
    identify(natural, "natural", tr(u8"Сохранить движения голоса"), 26);
    natural.setTooltip(tr(u8"Подтягивать центр ноты, сохраняя больше вибрато. Короткие колебания около границы нот не меняют цель. «Мягко» включает этот режим, «Рэп» выключает."));
    naturalAttachment = std::make_unique<ButtonAttachment>(p.parameters, "natural", natural);
    natural.onClick = [this] { refresh(); };
    language.setButtonText("EN / RU"); language.setClickingTogglesState(true); addAndMakeVisible(language);
    identify(language, "language", "Interface language", 27);
    language.onClick = [this] {
        russian = language.getToggleState();
        processor.parameters.state.setProperty("editorLanguage", russian ? "ru" : "en", nullptr);
        updateLanguage(); refresh();
    };
    for (auto* l : {&status, &summary, &noteCaption}) {
        addAndMakeVisible(*l); l->setColour(juce::Label::textColourId, muted);
        l->setBorderSize(juce::BorderSize<int>(0)); l->setFont(uiFont(captionSize));
    }
    status.setComponentID("status"); summary.setComponentID("summary"); noteCaption.setComponentID("noteCaption");
    status.setJustificationType(juce::Justification::right);
    const int initialChord = static_cast<int>(p.parameters.getRawParameterValue("chord")->load());
    if (initialChord >= 1 && initialChord <= 8) rememberedChord = initialChord;
    if (p.settings().scale > 0) rememberedScale = p.settings().scale;
    setSize(720, 552);
    updateLanguage();
    refresh(); startTimerHz(24);
}
NoteShaperEditor::~NoteShaperEditor() { stopTimer(); setLookAndFeel(nullptr); }

juce::String NoteShaperEditor::translated(const char* en, const char* ru) const {
    return juce::String::fromUTF8(russian ? ru : en);
}
void NoteShaperEditor::updateLanguage() {
    const auto t = [this](const char* en, const char* ru) { return translated(en, ru); };
    setTitle(t("NoteShaper — vocal pitch correction", u8"NoteShaper — подтяжка вокала"));
    language.setToggleState(russian, juce::dontSendNotification);
    language.setTitle(t("Interface language: English", u8"Язык интерфейса: русский"));
    language.setTooltip(russian ? "Switch to English" : tr(u8"Переключить на русский"));
    const auto populate = [](juce::ComboBox& box, const juce::StringArray& items, int first) {
        const int selected = box.getSelectedId(); box.clear(juce::dontSendNotification);
        for (int i = 0; i < items.size(); ++i) box.addItem(items[i], first + i);
        box.setSelectedId(selected, juce::dontSendNotification);
    };
    populate(mode, {t("Note", u8"Нота"), t("Chord", u8"Аккорд"), t("Scale", u8"Гамма"), t("Custom", u8"Свой набор"), "MIDI"}, 1);
    populate(chord, {t("Major", u8"Мажор"), t("Minor", u8"Минор"), t("Dominant 7th", u8"Доминантсептаккорд"),
                    t("Major 7th", u8"Мажорный септаккорд"), t("Minor 7th", u8"Минорный септаккорд"), "Sus2", "Sus4", t("Fifth", u8"Квинта")}, 2);
    populate(scale, {t("Major", u8"Мажор"), t("Minor", u8"Минор"), t("Harmonic minor", u8"Гармонич. минор"),
                    t("Major pentatonic", u8"Мажорная пентатоника"), t("Minor pentatonic", u8"Минорная пентатоника")}, 1);
    mode.setTitle(t("Target source", u8"Способ выбора нот"));
    mode.setTooltip(t("Choose Note, Chord, Scale, Custom or MIDI, then the notes on the right.", u8"Выбери ноту, аккорд, гамму, свой набор или MIDI, затем ноты справа."));
    root.setTitle(t("Root or target note", u8"Основная или целевая нота"));
    root.setTooltip(t("Choose a note. The octave nearest to the voice is selected automatically.", u8"Выбери ноту. Октава подбирается рядом с голосом."));
    chord.setTitle(t("Chord type", u8"Тип аккорда"));
    chord.setTooltip(t("Allowed chord notes for one voice, without adding harmonies.", u8"Допустимые ноты аккорда для одного голоса, без создания дополнительных голосов."));
    scale.setTitle(t("Scale type", u8"Тип гаммы"));
    scale.setTooltip(t("Choose the song's scale and tonic.", u8"Выбери гамму и тонику песни."));
    const juce::StringArray presetNames{t("Gentle", u8"Мягко"), t("Tight", u8"Плотно"), t("Rap", u8"Рэп")};
    const juce::StringArray hints{
        t("55% strength, 85 ms, 8 cents, vibrato on. Keeps target notes.", u8"55% силы, 85 мс, 8 центов, вибрато включено. Целевые ноты сохраняются."),
        t("90% strength, 14 ms, 2 cents, vibrato off. Keeps target notes.", u8"90% силы, 14 мс, 2 цента, вибрато выключено. Целевые ноты сохраняются."),
        t("100% strength, 0 ms, 0 cents, vibrato off. Hard tuning to the selected notes.", u8"100% силы, 0 мс, без допуска, вибрато выключено. Жёсткая подтяжка к выбранным нотам.")};
    for (int i = 0; i < 3; ++i) { presets[i].setButtonText(presetNames[i]); presets[i].setTitle(presetNames[i]); presets[i].setTooltip(hints[i]); }
    strength.setTitle(t("Correction strength", u8"Сила подтяжки"));
    speed.setTitle(t("Retune time", u8"Время подтяжки"));
    freedom.setTitle(t("Pitch freedom in cents", u8"Допуск отклонения в центах"));
    strength.setTooltip(t("0% keeps the original pitch. 100% follows the target fully. Click the number to type a value.", u8"0% — исходная высота. 100% — полное следование цели. Нажми число для ввода значения."));
    speed.setTooltip(t("0 ms gives sharp transitions. Longer times make correction gradual.", u8"0 мс — резкие переходы. Больше времени — плавнее подтяжка."));
    freedom.setTooltip(t("Preserve small deviations within this allowance. 100 cents is one semitone.", u8"Сохранять небольшие отклонения в пределах допуска. 100 центов = полутон."));
    speed.setTextValueSuffix(t(" ms", u8" мс")); freedom.setTextValueSuffix(t(" ct", u8" ц"));
    for (auto* slider : {&strength, &speed, &freedom}) slider->updateText();
    natural.setButtonText(t("Preserve vibrato", u8"Сохранить вибрато"));
    natural.setTitle(t("Preserve vibrato", u8"Сохранить вибрато"));
    natural.setTooltip(t("Correct the pitch centre while retaining more vibrato. Gentle enables this; Rap disables it.", u8"Подтягивать центр ноты, сохраняя больше вибрато. «Мягко» включает этот режим, «Рэп» выключает."));
}

unsigned NoteShaperEditor::targetMask() const {
    return processor.parameters.getRawParameterValue("midi")->load() > 0.5f
        ? processor.activeNotes.load() : notefollow::noteMask(processor.settings());
}
void NoteShaperEditor::chooseMode(int selectedMode) {
    if (selectedMode == 4) { processor.setParameter("midi", 1); refresh(); return; }
    processor.setParameter("midi", 0);
    const int current = static_cast<int>(processor.parameters.getRawParameterValue("chord")->load());
    if (current >= 1 && current <= 8) rememberedChord = current;
    if (processor.settings().scale > 0) rememberedScale = processor.settings().scale;
    if (selectedMode == 3 && (current != 10 || processor.settings().scale > 0)) {
        const auto mask = notefollow::noteMask(processor.settings());
        for (int i = 0; i < 12; ++i)
            processor.setParameter("note" + juce::String(i), (mask & (1u << i)) ? 1.0f : 0.0f);
    }
    processor.setParameter("scale", selectedMode == 2 ? static_cast<float>(rememberedScale) : 0.0f);
    processor.setParameter("chord", selectedMode == 0 ? 0.0f : selectedMode == 1 ? static_cast<float>(rememberedChord) : 10.0f);
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
    const bool savedRussian = processor.parameters.state.getProperty("editorLanguage", "en").toString() == "ru";
    if (savedRussian != russian) { russian = savedRussian; updateLanguage(); }
    const auto s = processor.settings();
    const bool byMidi = processor.parameters.getRawParameterValue("midi")->load() > 0.5f;
    const int targetMode = s.scale > 0 ? 2 : s.chord == 0 ? 0 : s.chord < 9 ? 1 : 3;
    const auto mask = targetMask();
    mode.setSelectedId(byMidi ? 5 : targetMode + 1, juce::dontSendNotification);
    root.setVisible(targetMode != 3 && !byMidi); root.setEnabled(!byMidi);
    root.setSelectedId(s.root + 1, juce::dontSendNotification);
    chord.setVisible(targetMode == 1 && !byMidi); chord.setEnabled(!byMidi);
    scale.setVisible(targetMode == 2 && !byMidi); scale.setEnabled(!byMidi);
    if (targetMode == 1) {
        rememberedChord = s.chord; chord.setSelectedId(s.chord + 1, juce::dontSendNotification);
    }
    if (targetMode == 2) { rememberedScale = s.scale; scale.setSelectedId(s.scale, juce::dontSendNotification); }
    juce::StringArray allowed;
    for (int i = 0; i < 12; ++i) {
        const bool included = (mask & (1u << i)) != 0;
        keys[i].setToggleState(byMidi || targetMode == 3 ? included : i == s.root, juce::dontSendNotification);
        keys[i].setEnabled(!byMidi);
        keys[i].setVisible(targetMode == 3 && !byMidi);
        keys[i].setTooltip(translated("Include or exclude ", u8"Включить или исключить ") + names[i]);
        if (included) allowed.add(names[i]);
    }
    noteCaption.setVisible(byMidi || targetMode == 3 || targetMode == 0);
    noteCaption.setBounds(targetMode == 0 && !byMidi ? 344 : 224, 112, targetMode == 0 && !byMidi ? 344 : 464, 44);
    noteCaption.setText(byMidi ? translated("Route MIDI to this track", u8"Направь MIDI на эту дорожку") : targetMode == 3
                        ? (mask == 0 ? translated("No notes — original pitch", u8"Нет нот — исходная высота") : translated("Select notes below", u8"Выбери ноты ниже"))
                        : translated("Nearest vocal octave", u8"Октава подбирается к голосу"), juce::dontSendNotification);
    summary.setVisible(byMidi || targetMode != 3);
    summary.setText(mask == 0 ? translated("No notes selected — original pitch", u8"Нет выбранных нот — исходная высота")
                    : translated("Targets: ", u8"Вести к: ") + allowed.joinIntoString(juce::String::fromUTF8("  ·  ")), juce::dontSendNotification);
    summary.setColour(juce::Label::textColourId, mask == 0 ? text : muted);
    constexpr float amounts[] = {0.55f, 0.90f, 1.0f}, times[] = {85, 14, 0}, tolerances[] = {8, 2, 0};
    for (int i = 0; i < 3; ++i) presets[i].setToggleState(
        std::abs(s.amount - amounts[i]) < 0.0005f && std::abs(s.speedMs - times[i]) < 0.05f
        && std::abs(s.toleranceCents - tolerances[i]) < 0.05f && s.natural == (i == 0), juce::dontSendNotification);
    const float in = processor.detected.load();
    juce::String state;
    if (mask == 0) state = byMidi ? translated("MIDI: waiting for notes", u8"MIDI: ждёт ноты") : translated("No notes selected", u8"Нет выбранных нот");
    else if (s.amount < 0.001f) state = translated("Original pitch", u8"Исходная высота");
    else if (in < 0 || !std::isfinite(in)) state = byMidi ? translated("MIDI · waiting for voice", u8"MIDI · ждёт голос") : translated("Waiting for voice", u8"Ждёт голос");
    else state = (byMidi ? juce::String::fromUTF8("MIDI · ") : "") + noteText(in) + tr(u8"  →  ") + noteText(processor.destination.load())
                 + "   " + (processor.cents.load() >= 0 ? "+" : "") + juce::String(static_cast<int>(std::round(processor.cents.load()))) + translated(" ct", u8" ц");
    status.setText(state, juce::dontSendNotification);
    status.setColour(juce::Label::textColourId, muted);
    repaint();
}

void NoteShaperEditor::paint(juce::Graphics& g) {
    g.fillAll(background);
    label(g, "NoteShaper", {32, 24, 260, 24}, valueSize, text);
    g.setFont(uiFont(bodySize, juce::Font::bold)); g.setColour(text);
    g.drawText(translated("Targets", u8"Ноты"), juce::Rectangle<int>(32, 80, 280, 24), juce::Justification::left);
    g.drawText(translated("Correction", u8"Коррекция"), juce::Rectangle<int>(32, 248, 280, 24), juce::Justification::left);
    label(g, translated("Strength", u8"Сила"), {48, 296, 176, 24}, bodySize, text, juce::Justification::centred);
    label(g, translated("Retune", u8"Время"), {272, 296, 176, 24}, bodySize, text, juce::Justification::centred);
    label(g, translated("Freedom", u8"Допуск"), {496, 296, 176, 24}, bodySize, text, juce::Justification::centred);
    g.setColour(line);
    for (int y : {64, 224, 480}) g.drawHorizontalLine(y, 32, 688);
}
void NoteShaperEditor::resized() {
    status.setBounds(384, 498, 304, 24);
    mode.setBounds(32, 112, 176, 44);
    root.setBounds(224, 112, 104, 44);
    chord.setBounds(344, 112, 344, 44); scale.setBounds(344, 112, 344, 44);
    for (int i = 0; i < 12; ++i) keys[i].setBounds(32 + i * 55, 164, 51, 44);
    summary.setBounds(32, 176, 656, 24);
    natural.setBounds(32, 488, 280, 44);
    for (int i = 0; i < 3; ++i) presets[i].setBounds(384 + i * 104, 240, 96, 44);
    language.setBounds(612, 16, 76, 44);
    strength.setBounds(48, 320, 176, 144); speed.setBounds(272, 320, 176, 144); freedom.setBounds(496, 320, 176, 144);
}
