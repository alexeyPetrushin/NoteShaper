# NoteShaper 0.5.2 — user guide

[Русский](USER_GUIDE.ru.md) · [Home](../README.md)

## Installation

Requires Windows x64 and a VST3-compatible DAW. The build was checked in a test VST3 host; Ableton's interface was not operated during validation.

1. Download the VST3 or full archive from [Releases](https://github.com/alexeyPetrushin/NoteShaper/releases/latest) and extract it.
2. Close Ableton and any other application using the plug-in.
3. Copy the **entire folder** `VST3/NoteShaper.vst3` to `C:\Program Files\Common Files\VST3`. Windows may require administrator permission for this system folder. Keep the `Contents`, `x86_64-win` and `Resources` subfolders intact.
4. In Ableton's Plug-Ins settings, enable the VST3 system folder or select your custom VST3 folder. Click **Rescan**.
5. Find **NoteShaper** among VST3 effects. Insert it on an isolated, dry vocal track before reverb and delay.

### Upgrading

Back up the previous plug-in outside every folder scanned by the DAW. Install the new `NoteShaper.vst3` bundle and rescan. Do not keep NoteFollow and NoteShaper installed together: they share a VST3 identity so existing projects can find the update.

If the entire NoteFollow package was previously copied into the system folder, the old bundle may be inside `NoteFollow\VST3\NoteFollow.vst3`. Remove that old bundle from scanning. Copies in other enabled custom folders can also create a duplicate.

## Interface labels

New instances open in English. Click **EN** in the top-right corner to switch to Russian. The button changes to **RU**; click it again to return to English; the choice is retained when reopening the editor and saved with the project. The main labels are:

| Label | Meaning |
|---|---|
| Нота / Аккорд / Гамма / Свой набор | Note / Chord / Scale / Custom |
| Мягко / Плотно / Рэп | Gentle / Tight / Rap |
| Сила / Время / Допуск | Strength / Retune time / Pitch freedom |
| Сохранить вибрато | Preserve vibrato |
| MIDI | MIDI targets |

## Choosing targets

**Note:** click the desired C, C#, D or other note key. The nearest octave to the voice is chosen automatically.

**Chord:** choose Minor, Major or another type in the top-right dropdown. Click a note key to move the entire chord to that root; the chord type is retained. Chords constrain the allowed notes of one voice; they do not generate extra voices.

**Scale:** select the song's scale type on the right and its tonic with a note key. Available scales are major, natural minor, harmonic minor, major pentatonic and minor pentatonic.

**Custom:** click notes to include or exclude them. Switching to Custom retains the current chord or scale. An empty set keeps the original pitch.

The mode dropdown replaces the old “Targets” heading above the keyboard. The chord/scale type dropdown sits on the right, aligned with the correction presets below. All twelve note keys remain visible in every mode. In Note, Chord and Scale, the dark key marks the selected note/root; thin marks indicate the other allowed chord/scale notes. In Custom, dark keys mark included notes. “Targets” (“Вести к”) lists all allowed pitch classes. In MIDI, the keys display incoming notes and are read-only. Below, “Correction” (“Коррекция”) contains three directly visible preset buttons and the three knobs; you can adjust the knobs without choosing a preset. For a vocal melody, a song scale is often more appropriate than a chord: a major triad has three notes, while a major scale has seven. NoteShaper corrects toward the nearest allowed note.

## The three controls

**Strength:** 0% keeps the original pitch; 100% applies the full correction. Intermediate values apply a fraction of the pitch distance in cents. This is not a dry/wet audio blend.

**Retune time:** 0 ms produces sharp, stepped tuning; larger values make correction more gradual, including at the start of a phrase. This does not change the fixed 64 ms processing latency.

**Pitch freedom:** preserves small deviations within the chosen allowance. Beyond it, only the excess is corrected, so a nonzero allowance leaves a residual offset from the note centre. 100 cents equals one semitone.

Drag a rotary control vertically or click its numeric value to type a number.

## Starting points

| Preset | Strength | Retune time | Freedom | Preserve vibrato |
|---|---:|---:|---:|---|
| Gentle | 55% | 85 ms | 8 cents | On |
| Tight | 90% | 14 ms | 2 cents | Off |
| Rap | 100% | 0 ms | 0 | Off |

Click Gentle, Tight or Rap beside “Correction”. Presets retain your target notes. Changing any preset-controlled setting manually clears the preset selection. All three buttons remain directly visible.

For obvious hard tuning, select Rap. Use Note to deliberately constrain the voice to one pitch class, or the song's Scale for a stepped melody. For subtle correction, start with Gentle and adjust strength and time by ear.

**Preserve vibrato** corrects a smoothed pitch centre, retaining more rapid small movements such as vibrato. It also holds the target through brief fluctuations near a note boundary. Large transitions bypass the boundary-duration check. This is a simplified algorithm whose result depends on the voice and settings.

## MIDI

1. Create a separate MIDI track with a clip or keyboard input.
2. Route **MIDI To** to the audio track containing NoteShaper; choose the plug-in in the lower destination selector.
3. Choose **MIDI** in the mode dropdown above the keyboard. To return to manual targets, select Note, Chord, Scale or Custom in that same dropdown.

Incoming notes define allowed pitch classes, not exact vocal octaves. A MIDI chord supplies several possible targets for one voice. Sustain (CC64) holds released notes until pedal release, independently on each MIDI channel.

With no pressed or sustained notes, pitch is unchanged. Manual targets are disabled while MIDI targeting is on. All Sound Off clears the channel's held voices; Reset Controllers releases its sustain pedal state.

## Readouts and listening

The bottom-right readout shows detected pitch, target pitch and correction in cents. It is visually secondary to the target pickers and three rotary controls. «Ждёт голос» means the detector is waiting for a stable voice. «MIDI: ждёт ноты» means MIDI is not supplying targets.

Compare processed and original vocals, listening to consonants, note endings and transitions. If the target is wrong, check the scale and tonic first. For overly obvious tuning, increase retune time, reduce strength or enable Preserve vibrato. An empty target set and 0% strength should keep the original pitch.

## Limitations

One voice, approximately 65–1000 Hz. Accompaniment, noise, rough/breathy vocals and large jumps can cause incorrect targets and artifacts. There is no automatic song-key detection.

The fixed 64 ms delay is reported to the DAW for playback compensation, but remains noticeable in live monitoring. This version has no formant/transpose controls, harmonization or graphical per-note editor.

In the full package, `Standalone/NoteShaper.exe` opens the interface without a DAW. It does not install the VST3 automatically.
