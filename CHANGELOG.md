# Changelog / Изменения

## 0.5.2 — macOS distribution, 2026-10-09

- Added one universal VST3 package for Intel and Apple Silicon, targeting macOS 11+.
- Verified the same packaged plug-in and DSP natively on both architectures with GitHub macOS runners.
- Platform-labelled VST3 downloads, separate Windows/macOS installation guides, linked source and SHA-256. Ad-hoc signed; no Developer ID/notarization or DAW-specific Mac validation.
- Windows DSP, user interface and saved parameter identity are unchanged.

Добавлена универсальная Mac-сборка для Intel и Apple Silicon, macOS 11+. Один архив проверен на обеих архитектурах; есть отдельные инструкции Windows/macOS, ссылки на исходники и SHA-256. Подпись ad-hoc, без нотарификации. Windows-обработка и сохранённые параметры не изменены.

## 0.5.2 — 2026-10-08

- The mode dropdown is above the note keys, with the chord/scale type aligned on the right.
- Twelve note keys stay visible in every mode. Chord keys transpose the complete chord in one click while retaining its type.
- Note selects one target, Scale changes the tonic, Custom toggles notes, and MIDI shows incoming notes read-only.
- Root and allowed chord/scale notes have distinct indicators. Removed the separate root dropdown.
- Retained the three direct presets, three knobs, quiet language button and saved parameter identity.

Выбор режима находится над клавишами. Справа — тип аккорда или гаммы, ниже — постоянные клавиши C, C#, D и далее. В Chord нажатие переносит весь аккорд с сохранением типа. Основная нота выделена тёмным, другие допустимые ноты отмечены тонкой чертой. MIDI показывает поступающие ноты без ручного изменения.

## 0.5.1 — 2026-10-08

- One target-source dropdown includes Note, Chord, Scale, Custom and MIDI.
- Gentle, Tight and Rap are three directly visible preset buttons; manual changes clear selection.
- English by default, with a single language button (EN → RU → EN) in the header; language is saved with the project.
- Consistent Segoe UI typography, control heights and spacing; three rotary controls retained.
- Removed segmented-control containers, duplicate helper labels and permanent value-field backgrounds.
- Compact 720 × 552 editor; processing and saved parameter identity unchanged.
- DonationAlerts support links in both READMEs.

Единый порядок: способ выбора нот → ноты → коррекция. Один список объединяет способы выбора нот и MIDI; «Мягко», «Плотно» и «Рэп» остаются отдельными кнопками. Кнопка EN справа вверху включает русский и меняется на RU, английский включён по умолчанию. Сохранены три крутилки и ввод чисел; интерфейс стал ниже и использует одну шрифтовую семью и три размера.

## 0.5.0 — 2026-10-07

- Light, mostly monochrome interface with geometric controls and three visible knobs.
- Editable numeric fields, switches and clear segment selection; the 12-note grid appears only in Custom.
- Russian and English installation, user and build guides.
- Processing and saved parameter identity retained from 0.4.

Светлый минималистичный интерфейс, плоские ручки, крупные значения и инструкции RU/EN. Алгоритм обработки и параметры версии 0.4 сохранены.

## 0.4.0 — 2026-10-07

- Renamed NoteFollow to NoteShaper, preserving the VST3 identity and 18 legacy parameters.
- Added five scales and Natural Voice correction with steadier boundary transitions.
- Fixed retune onset/re-attack, MIDI sustain/channel handling and opposite-polarity stereo detection.
- Added correction readout and state/reset fixes.

Переименование, гаммы, «Живой голос», устойчивость цели и исправления начала фразы, MIDI и стерео.

## 0.3.0

Purple/black interface with three always-visible rotary controls. / Фиолетово-чёрный интерфейс с тремя крутилками.
