# Validation / Проверки

## English

Release 0.5.2 was built for Windows x64. The packaged VST3 was scanned and loaded in a JUCE host, and the native editor was rendered in twelve states at 720 × 552.

The UI check additionally verifies non-overlapping controls, direct numeric typing, twelve permanently visible note keys, full minor-chord transposition through every root, retained chord type, root/target indicators, read-only incoming MIDI display, direct preset buttons, English default, full RU/EN switching and saved language and exiting MIDI without losing the remembered chord. It covers target selection, scales, scale-to-custom continuity, three visible knobs, numeric values, presets, Natural Voice, MIDI waiting, parameter automation and state restoration. Native processor checks also cover sustain, independent MIDI channels, repeated notes, All Notes Off, All Sound Off, Reset Controllers and meter reset.

The 0.5.2 host check used a retained NoteFollow 0.3 binary to confirm the same VST3 identity and restoration of all 18 legacy parameters. The original 0.1 binary had passed this check for release 0.4; processor/DSP source is unchanged in 0.5. New scale/natural settings reset to off when absent from an old state.

DSP source is unchanged from 0.4. Its regression results are retained: 24 settled pitch cases at 44.1/48/96/192 kHz had maximum error below 0.22 cents on synthetic harmonic signals. The checks also cover exact delayed dry output at 0%/empty targets, partial pitch correction, chords/scales, noise/silence, block-size independence, retune onset/re-attack, deliberate melody changes and opposite-polarity stereo.

For a synthetic 6 Hz vibrato with 30-cent depth, hard tuning produced 0.24-cent depth; Natural Voice produced 26.31-cent depth with -0.78-cent mean centre error. For a synthetic boundary wobble, the hard target switched 25 times and the natural target did not switch.

The fixed reported delay is 3072 samples at 48 kHz, or 64 ms. These are synthetic/host checks, not a listening comparison against commercial plug-ins or a user study. Ableton Live and FL Studio themselves were not operated; compatibility with them is expected from the VST3 format, not established by a host-specific test. The macOS universal ZIP passed bundle/signature checks and the same DSP and native VST3 scan/load, stereo, state, editor-creation, MIDI-release and latency checks on macOS 15 Intel and Apple Silicon runners. The deployment target is macOS 11; older OS releases and physical audio devices were not tested. Mac signing is ad-hoc, without Developer ID or notarization. Full screen-reader testing and signed Windows distribution were not performed. The DSP workflow runs pure engine checks; the macOS workflow additionally builds and runs the native checks on both architectures.

## Русский

Сборка 0.5.2 проверена в VST3-хосте. Реальный нативный интерфейс отрисован в двенадцати состояниях при 720 × 552: выбор целей, гаммы, переход в свой набор, три крутилки, значения, пресеты, «Сохранить вибрато», MIDI, автоматизация и сохранение состояния.

Также проверены перенос целого минорного аккорда по всем 12 клавишам, сохранение типа, выделение основного тона и прочих целей, видимая клавиатура MIDI без ручного ввода, английский по умолчанию, полный перевод EN/RU и сохранение языка, отдельные кнопки пресетов, отсутствие перекрытия элементов, ввод числа с клавиатуры и выход из MIDI без потери прежнего аккорда. Проверены педаль sustain, независимые каналы, повторные ноты, отпускание, аварийное снятие нот и сброс показаний. Версия 0.5.2 загрузила прежние 18 параметров из сохранённого NoteFollow 0.3; исходная 0.1 прошла эту проверку для релиза 0.4, а код процессора в 0.5 не изменён. Отсутствующие новые параметры выключаются.

Движок версии 0.4 не изменён. На 24 синтетических гармонических сигналах при 44.1/48/96/192 кГц установившаяся ошибка была меньше 0,22 цента. Также проверены исходный сигнал при 0% и пустом наборе, частичная правка, гаммы/аккорды, шум/тишина, размеры блоков, начало фраз, смена мелодии и противоположная полярность стерео.

Синтетическое вибрато 6 Гц / 30 центов после жёсткой правки имело глубину 0,24 цента, после «Живого голоса» — 26,31 цента со средней ошибкой центра -0,78 цента. Пограничное колебание вызвало 25 смен целей в жёстком режиме и ни одной в естественном.

Задержка — 64 мс. Это проверки тестовых сигналов и хоста, не сравнение звучания с коммерческими плагинами и не исследование с пользователями. Ableton Live и FL Studio отдельно не проверялись: ожидаемая совместимость следует из формата Windows x64 VST3, а не из проверки в этих DAW. Один Mac universal ZIP прошёл проверки архитектур/подписи, DSP и нативной загрузки VST3, стерео, состояния, создания редактора, отпускания MIDI и задержки на Mac Intel и Apple Silicon с macOS 15. Целевая минимальная система — macOS 11; старые версии macOS и физические аудиоустройства не проверялись. Подпись Mac — ad-hoc, без Developer ID и нотарификации. Экранный диктор отдельно не проверялся. CI выполняет проверки движка, а Mac-сборка также запускает нативные проверки на обеих архитектурах.
