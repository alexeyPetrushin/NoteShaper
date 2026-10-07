# Design / Оформление

## Flow / Сценарий

The primary task is to choose the pitches a voice should follow, then adjust correction. Version 0.5.1 uses one top-to-bottom flow in a 720 × 552 editor:

1. **Targets:** the first dropdown selects Note, Chord, Scale, Custom or MIDI. Relevant note/type choices follow on the same row. Only Custom reveals twelve note buttons below.
2. **Correction:** Gentle, Tight and Rap are three directly visible buttons. No dropdown or group wrapper surrounds them. Choose one in a single click or adjust the three knobs immediately. Presets retain target notes; manual changes clear selection.
3. **Listen:** Preserve vibrato and the detected → target/cents readout sit in the footer.

Ноты сверху, коррекция ниже. MIDI находится среди способов выбора нот. «Мягко», «Плотно» и «Рэп» доступны одним нажатием, три крутилки доступны сразу. Вложенных экранов настроек и подтверждений нет.

## Language / Язык

New instances open in English. One quiet text button replaces the old subtitle at the top right. It shows EN in English and RU in Russian: 14 px muted text, no resting border or fill. Its 56 × 44 px click area is retained; hover gives subtle feedback and keyboard focus remains visible. One click translates labels, menus, units, tooltips and accessible control titles. The choice is retained on reopening and in the saved project through UI metadata; no host/audio parameter is added. Legacy states without this metadata open in English.

Английский по умолчанию, кнопка EN включает русский и меняется на RU; повторное нажатие возвращает английский. Выбор сохраняется в проекте; звуковые настройки и идентификаторы параметров не меняются.

## Consistency / Единообразие

One typeface, Segoe UI. Three sizes: 14 px supporting information, 18 px controls/section labels, 20 px product name/values. Bold is reserved for section labels. Main controls are 44 px high. Outer margin 32 px, field gaps 16 px, preset gaps 8 px. White, cool grey and graphite; one border/radius treatment for dropdowns and buttons. Editable values have no permanent background; hover/focus reveals the field.

The chord view exposes 11 controls including language, versus 14 before. No control is nested in another button-like container. A source change takes open + select; presets and custom notes each take one click.

Один шрифт, три размера, одинаковая высота и оформление управления. В режиме аккорда — 11 элементов вместе с языком вместо 14. Убраны контейнеры сегментов, повторные пояснения и постоянные плашки под числами.

## Two audit passes / Два прохода

The first pass identified competing segmented groups, a separate MIDI override, oversized fields and repeated labels. The final layout consolidates target selection and standardises control geometry while keeping all three presets directly visible.

The second pass inspected twelve native states in the final flow, including English/Russian, all target modes, empty sets, manual settings and MIDI. Initial control text was too small and increased to 18 px. Long labels fit, selection is visible, and the correction area remains stationary when modes change. Native checks verify language restoration, unchanged tuning settings during translation, direct presets, numeric entry, target continuity and non-overlapping controls.

Это экспертный аудит и проверки кода, не пользовательское исследование. See [validation](VALIDATION.md) for build checks and limits.
