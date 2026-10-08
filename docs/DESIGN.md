# Design / Оформление

## Flow / Сценарий

The primary action is to choose the pitches a voice should follow, then adjust correction. Version 0.5.2 keeps the 720 × 552 editor and a fixed keyboard:

1. The mode dropdown replaces the static Targets heading at the top left: Note, Chord, Scale, Custom or MIDI.
2. The relevant chord/scale type dropdown sits at the top right, aligned with the correction presets below. Minor, Major and other types are one level deep.
3. Twelve C–B note keys stay in the same place. Note selects one target; Chord transposes the complete chord and retains its type; Scale changes tonic; Custom toggles targets. MIDI displays incoming notes with manual selection disabled.
4. Gentle, Tight and Rap remain direct buttons beside Correction. Three knobs are always available; manual changes clear preset selection without changing notes.

Вместо Targets — список режимов слева. Справа — Minor/Major или тип гаммы, ниже — клавиши. В Chord одна клавиша переносит весь аккорд; повторное нажатие не выключает его. В Custom те же клавиши включают и выключают отдельные ноты. Дополнительного списка основной ноты нет.

## Selection / Выбор

A dark key is the selected root in Note, Chord and Scale. Other allowed chord/scale members have a thin neutral mark, avoiding several competing root selections. In Custom all included notes are dark. The summary lists the actual pitch classes; empty targets explain that original pitch is retained. MIDI uses the same stationary keys as a read-only incoming-note display.

Основной тон выделен тёмным. Тонкая черта обозначает остальные ноты аккорда/гаммы. В своём наборе выделены все включённые ноты. Пустой набор и ожидание MIDI имеют пояснения.

## Language / Язык

English is the default. One quiet EN button changes to RU after a click, and back to EN on the next click. Muted 14 px text has no resting border or fill; the 56 × 44 px click area, hover feedback and visible keyboard focus remain. Labels, menus, units, tooltips and accessible titles are translated. Language is saved as UI metadata with the project, without adding a host/audio parameter.

## Geometry / Геометрия

Segoe UI with 14 px support text, 18 px controls and 20 px name/values. Mode and type: y=80, height=44. Keys: y=140, height=52, twelve equal 51 px click targets, 4 px gaps. Outer margins are 32 px. Type dropdown and correction presets share x=384 and the same right edge. Sharps use a slightly darker neutral surface; root/selection treatment is consistent. Numeric values reveal editability on hover/focus.

The root changes in one click rather than opening a dropdown and choosing an item. Type changes need open + select. No nested controls, settings pages or confirmations are added. All keyboard buttons have the same context-dependent role within a mode.

## Two reviews / Две проверки

Pass 1 checked the primary action, click counts, root/type distinction and the MIDI exception. Pass 2 checked actual native renders and interactions: persistent key positions, all twelve minor-chord roots, repeat clicks, mode continuity, presets, language/state restore, automation, numeric entry and non-overlapping controls. This is an expert/code review, not a user study.
