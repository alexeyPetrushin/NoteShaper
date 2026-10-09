# NoteShaper 0.5.2 — user guide

[Русский](USER_GUIDE.ru.md) · [Home](../README.md)

## Compatibility

NoteShaper 0.5.2 release archives contain a **Windows x64 VST3** plug-in and, in the full package, a Windows standalone application. Use a 64-bit DAW that supports VST3.

| Platform and host | Status |
|---|---|
| Windows x64, Ableton Live with VST3 | Compatible format. Validation used a separate VST3 test host; Live itself has not been tested. |
| Windows x64, FL Studio with VST3 | Expected to work as a mixer effect. FL Studio supports VST3; NoteShaper has not yet been tested in FL Studio itself. |
| Windows x64, another VST3 DAW | Format compatibility is expected; the specific host still needs validation. |
| macOS, including Intel and Apple Silicon | No ready-made build is available. The Windows plug-in in these archives does not run on macOS. |

macOS requires a separate build of the C++/JUCE source for Intel/Apple Silicon and validation in the intended host. VST3 support in FL Studio or Ableton on Mac does not make the Windows binary compatible. See the [build guide](BUILD.en.md) and [Image-Line's explanation of separate Windows/macOS plug-ins](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm).

## Installation on Windows

1. Download the VST3 or full archive from [Releases](https://github.com/alexeyPetrushin/NoteShaper/releases/latest) and extract it.
2. Close your DAW and other applications using the plug-in.
3. Locate the **folder** `VST3/NoteShaper.vst3` in the extracted package. Copy that whole folder to one VST3 location scanned by your DAW. Keep `Contents`, `x86_64-win` and `Resources` intact; copying only the internal binary is not a complete installation.
4. Enable the corresponding VST3 source in your DAW and rescan plug-ins.
5. Insert NoteShaper as an **audio effect** on an isolated dry vocal track or mixer channel, before reverb and delay.

### Choosing a VST3 location

The destination depends on Windows and the host configuration. `C:\Program Files\Common Files\VST3` is a common default, not a fixed path for every computer. Paste **`%CommonProgramFiles%\VST3`** into File Explorer's address bar to use the system's actual shared location. Create the VST3 folder if it is missing; system locations may require administrator permission.

| Option | Destination for `NoteShaper.vst3` |
|---|---|
| Shared system folder | `%CommonProgramFiles%\VST3`. Preferred for multiple DAWs and FL Studio. |
| Current user only | `%LOCALAPPDATA%\Programs\Common\VST3`, if your host scans that user location. |
| Custom folder | A dedicated VST3 folder explicitly selected in your DAW. Use this only when the host supports custom VST3 paths. |

[Steinberg documents the standard locations](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Locations%2BFormat/Plugin%2BLocations.html); support for each location depends on the host. Install only the `NoteShaper.vst3` bundle in the scan folder. Keep source, documentation and the standalone app elsewhere.

### Ableton Live on Windows

In **Settings/Preferences → Plug-Ins**, enable **VST3 System Folders** for a system installation. For a custom installation, enable **VST3 Custom Folder** and select the containing folder. Avoid assigning the same path to both system/custom sources or to VST2. Use **Rescan** and find NoteShaper in the plug-in browser. Menu wording depends on Live's version and language. [Ableton's guide](https://help.ableton.com/hc/en-us/articles/209071729-Using-VST-plug-ins-on-Windows).

### FL Studio on Windows

Image-Line's VST3 instructions require standard locations: **`%CommonProgramFiles%\VST3`** or **`%ProgramFiles%\VST3`**. Adding an arbitrary VST2 search folder does not guarantee VST3 discovery. Open **Options → File settings → Manage plugins**, enable **Verify plugins** and run **Find installed plugins**. Load NoteShaper in a Mixer effect slot on the vocal channel. [Installing plug-ins](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm) · [Plugin Manager](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/envsettings_files.htm).

### Other DAWs

Check the host's manual for **VST3 x64** support, enabled scan locations and rescanning. A VST2 or sample-library folder is not necessarily a VST3 location.

### Upgrading

Close your DAW, back up the old bundle outside all scan locations, replace the complete `NoteShaper.vst3` folder and rescan. Check other enabled locations for duplicates. NoteFollow and NoteShaper share a VST3 identity for existing projects; install only one version at a time.

### If the plug-in is missing

Check Windows x64/VST3 compatibility, bundle contents, the selected location and the host's scan report. If the host marked an earlier copy as failed, re-verify it using its plug-in manager. When requesting help, include Windows, DAW and NoteShaper versions, the install path and the scanner's message.

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

The mode dropdown is at the top left, above the note keys. The chord/scale type dropdown is on the right of the same row. All twelve note keys remain visible in every mode. In Note, Chord and Scale, the dark key marks the selected note/root; thin marks indicate the other allowed chord/scale notes. In Custom, dark keys mark included notes. “Targets” (“Вести к”) lists all allowed pitch classes. In MIDI, the keys display incoming notes and are read-only. Below, “Correction” (“Коррекция”) contains three directly visible preset buttons and the three knobs; you can adjust the knobs without choosing a preset. For a vocal melody, a song scale is often more appropriate than a chord: a major triad has three notes, while a major scale has seven. NoteShaper corrects toward the nearest allowed note.

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

1. Create a MIDI track or MIDI source with a clip or keyboard input in your DAW.
2. Route its MIDI messages **to NoteShaper's MIDI input** on the vocal channel. Routing depends on the host; use its instructions for sending MIDI to audio effects.
3. Choose **MIDI** in the mode dropdown above the plug-in's keys. Return to Note, Chord, Scale or Custom for manual targets.

In Ableton Live, set the MIDI track's **MIDI To** destination to the audio track containing NoteShaper, then choose the plug-in. Other DAWs use different menus and routing. Manual target modes remain available if MIDI is not reaching the plug-in.

Incoming notes define allowed pitch classes, not exact vocal octaves. A MIDI chord supplies several possible targets for one voice. Sustain (CC64) holds released notes until pedal release, independently on each MIDI channel.

With no pressed or sustained notes, pitch is unchanged. Manual targets are disabled while MIDI targeting is on. All Sound Off clears the channel's held voices; Reset Controllers releases its sustain pedal state.

## Readouts and listening

The bottom-right readout shows detected pitch, target pitch and correction in cents. Use it to check the chosen target and how much correction is being applied. «Ждёт голос» means the detector is waiting for a stable voice. «MIDI: ждёт ноты» means MIDI is not supplying targets.

Compare processed and original vocals, listening to consonants, note endings and transitions. If the target is wrong, check the scale and tonic first. For overly obvious tuning, increase retune time, reduce strength or enable Preserve vibrato. An empty target set and 0% strength should keep the original pitch.

## Limitations

One voice, approximately 65–1000 Hz. Accompaniment, noise, rough/breathy vocals and large jumps can cause incorrect targets and artifacts. There is no automatic song-key detection.

The fixed 64 ms delay is reported to the DAW for playback compensation, but remains noticeable in live monitoring. This version has no formant/transpose controls, harmonization or graphical per-note editor.

In the full package, `Standalone/NoteShaper.exe` opens the interface without a DAW. It does not install the VST3 automatically.
