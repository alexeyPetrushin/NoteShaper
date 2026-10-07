# Design / Оформление

The main task is to choose the pitches a voice should follow, then adjust the correction. Version 0.5 uses a compact 720 × 624 layout without large cards: mode and target pickers at the top, three rotary controls in the centre, secondary switches and readouts around the edges. The knobs stay visible in every mode.

White, cool light grey and graphite are the whole palette. Related choices use segmented controls; a white segment and bold label show selection. The root picker is large. The 12-note grid appears only when editing a custom set. MIDI and “Сохранить вибрато” (Natural Voice) have explicit switch states. Numeric values have small editable fields. Readouts sit at the bottom.

Apple-inspired grouping, selection and spacing are translated into a custom Windows JUCE interface. Windows system typography is used; Apple's fonts are not bundled. Opaque surfaces keep controls legible. [Apple layout](https://developer.apple.com/design/human-interface-guidelines/layout), [segmented controls](https://developer.apple.com/design/human-interface-guidelines/segmented-controls).

## Two audit passes / Два прохода

**First pass:** competing controls made the starting action unclear. The correction was to remove the always-visible 12-note grid, expose only the target choices relevant to the current mode, and keep the three knobs together. The user preferred a compact light design, so oversized cards and repeated section headings were removed.

**Second pass:** native renders were inspected in nine states, with automated checks for target selection, custom continuity, empty sets, presets, MIDI, automation and state restoration. A pressed-state problem that obscured selected segments was fixed. Selected segments remain white during interaction. No claim of a user usability study is made.

**Первый проход:** главный сценарий терялся среди 12 нот, режимов, пресетов и показаний. Сетка нот теперь нужна только для собственного набора; в остальных режимах показаны основная нота и нужный список. Три ручки сохранены. После выбора пользователем светлого компактного направления удалены большие карточки и повторные заголовки.

**Второй проход:** проверена нативная отрисовка девяти состояний и поведение управления. Исправлено затемнение выбранного сегмента при нажатии. Сила, время и допуск всегда доступны. Пресеты не меняют целевые ноты. Пустой набор явно объясняет, что высота остаётся исходной.

The editor has fixed dimensions and does not implement iOS Dynamic Type or native Apple materials. Main interaction regions are at least 44 px high. A note picker needs two clicks (open and select); custom notes need one click each. This trades a persistent 12-button grid for a clearer normal workflow.
