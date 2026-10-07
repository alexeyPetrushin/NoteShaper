# Design / Оформление

## Flow / Сценарий

The primary task is to choose the pitches a voice should follow, then adjust the correction. Version 0.5.1 follows one top-to-bottom flow in a fixed 720 × 552 editor:

1. **Targets:** the first dropdown selects Note, Chord, Scale, Custom or MIDI. Relevant note/type choices follow on the same row. Custom alone reveals the twelve note buttons below.
2. **Correction:** choose Gentle, Tight or Rap from one dropdown, or adjust the three knobs directly. A manual change displays “Вручную” (Manual). Presets retain target notes.
3. **Listen and adjust:** Natural Voice and the detected → target/cents readout share a quiet footer.

Основной сценарий: сначала выбрать ноты, затем настроить подтяжку. MIDI — один из способов выбора нот, поэтому находится в том же списке. Готовая настройка необязательна: ручки всегда доступны. В «Своём наборе» включаются отдельные ноты; переход из аккорда или гаммы сохраняет текущие цели.

## Consistency / Единообразие

One typeface, Segoe UI, is used throughout. The scale is 14 px for supporting information, 18 px for controls and section labels, and 20 px for the product name and numeric values. Bold is limited to the two section labels. All dropdowns and note/checkbox interaction areas are 44 px high. The outer margin is 32 px; the target row uses 16 px gaps. Surfaces use white, cool grey and graphite, with one border/radius treatment for dropdowns and note buttons.

The normal chord view has **8 visible controls instead of 14**: four dropdowns, three knobs and one checkbox. No control is wrapped in another button-like surface. Values appear as text below the knobs; hover/focus reveals that they can be edited. Tooltips provide detailed explanations without repeated labels in the main flow. Focus rings remain visible.

Одна семья шрифта, три размера, одинаковая высота списков и кнопок. Убраны контейнеры сегментов, дублирующие подписи и постоянные плашки под числами. В обычном режиме аккорда осталось 8 элементов управления вместо 14. Ни один блок целиком не выглядит нажимаемой кнопкой.

## First audit / Первый аудит

The 0.5.0 screen mixed large note/type fields, two segmented groups, a separate MIDI switch, small secondary captions and value pills. These introduced competing reading directions and many text sizes. The correction was to consolidate mutually exclusive target sources, consolidate optional starting presets, keep the three knobs, and standardise type/control geometry. Existing functions remain accessible without a submenu or modal.

Trade-off: selecting a source or preset now takes two pointer clicks (open and select), versus one for the old segments. This lowers persistent control count; keyboard selection remains available. Custom notes still take one click each. No additional confirmation or nested settings screen is introduced.

На первом проходе найдены конкурирующие группы управления и разные масштабы текста. Выбор источника и пресета теперь требует двух кликов вместо одного; это осознанный обмен на более спокойный экран. Вложенных меню и подтверждений нет, отдельные ноты включаются одним кликом.

## Second audit / Второй аудит

Eleven native states were rendered and inspected. The first renders showed that 16 px control text was too small beside the knobs; it was increased to 18 px and rechecked. Interaction checks passed for all modes, empty custom sets, presets/manual state, editable numbers, MIDI entry/exit, host automation and saved-state restoration. The editor keeps fixed section positions when modes change. This is an expert/code review, not a user study. See [validation](VALIDATION.md) for the completed build checks.
