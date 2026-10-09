# NoteShaper

[Русский](README.ru.md) · [English guide](docs/USER_GUIDE.en.md) · [Русская инструкция](docs/USER_GUIDE.ru.md)

[Support the author](https://www.donationalerts.com/r/lesha_diabet_)

An open-source monophonic vocal pitch-correction effect for **Windows x64 and macOS / VST3**. Choose a note, chord, scale or custom set, then dial in hard tuning or gentle correction with three always-visible rotary controls.

![NoteShaper interface](assets/preview.png)

## Download and install

### Windows

**File for Windows x64:** [NoteShaper-0.5.2-win-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v0.5.2/NoteShaper-0.5.2-win-vst3.zip).

1. Download the **win** archive and extract it. It contains the `NoteShaper.vst3` folder.
2. Close your DAW and copy that **whole folder** to a VST3 location scanned by your host. The shared Windows location is **`%CommonProgramFiles%\VST3`**; paste it into File Explorer's address bar. The drive and custom-folder support depend on your system and DAW.
3. Rescan plug-ins and insert NoteShaper as an audio effect on your vocal track.

The [Windows guide](docs/USER_GUIDE.en.md#installation-on-windows) covers other locations, Ableton Live, FL Studio and updates.

### macOS

**File for Mac, Intel and Apple Silicon, macOS 11+:** [NoteShaper-0.5.2-mac-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v0.5.2/NoteShaper-0.5.2-mac-vst3.zip).

1. Download the **mac** archive, extract it and close your DAW.
2. In Finder choose **Go → Go to Folder**, enter **`~/Library/Audio/Plug-Ins/VST3`**, and create the folder if needed. Copy the entire **`NoteShaper.vst3` bundle** there. For all users, use **`/Library/Audio/Plug-Ins/VST3`**.
3. Rescan plug-ins and insert NoteShaper on your vocal track. See the [Mac guide](docs/MACOS.en.md) if macOS blocks loading.

Both downloads contain only the VST3 bundle. Source and developer instructions are in this repository.

## Compatibility

Ready-made downloads include **Windows x64** and **macOS universal (Intel/Apple Silicon, macOS 11+)**. Ableton Live, FL Studio and other 64-bit VST3 hosts are expected to load this format; NoteShaper has been checked in a JUCE VST3 host, not in those DAWs individually. [FL Studio supports VST3](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm).

**macOS:** the same universal archive passed native DSP and JUCE VST3 host checks on both Intel and Apple Silicon. It is ad-hoc signed, without Developer ID or Apple notarization; see [Mac installation](docs/MACOS.en.md) for possible Gatekeeper blocking. Ableton Live and FL Studio have not been tested individually. [Build from source](docs/BUILD.en.md).

## Features

- Single-note, chord, scale, custom-note and incoming MIDI targets.
- Twelve always-visible note keys: move a chord or scale in one click, or toggle a custom set.
- Major, natural minor, harmonic minor and major/minor pentatonic scales.
- Three controls: **strength**, **retune time** and **pitch freedom**.
- Gentle, Tight and Rap presets; a Natural Voice option that retains more rapid pitch movement.
- MIDI sustain, independent channels and safe note release/reset handling.
- Mono or stereo processing; shared pitch marks preserve the channel relationship.
- Light, mostly monochrome interface with geometric controls.

The interface opens in English. Click **EN** in the top-right corner to switch to Russian; the button changes to **RU** and a second click returns to English; the choice is saved with the project.

## Scope and limitations

Use one clean voice without accompaniment. The detector covers approximately **65–1000 Hz**. The plug-in reports a fixed **64 ms latency**; retune time is a separate control. This build is primarily suited to recorded vocals, rather than live monitoring.

Rough, noisy or breathy vocals and large corrections can produce artifacts. It does not detect the song key automatically, create vocal harmonies, provide graphical note editing or offer formant/transpose controls. Synthetic checks do not establish equal sound quality to commercial pitch-correction engines.

## Guides and development

| | English | Русский |
|---|---|---|
| Installation and usage | [User guide](docs/USER_GUIDE.en.md) | [Инструкция](docs/USER_GUIDE.ru.md) |
| Build from source | [Build guide](docs/BUILD.en.md) | [Сборка](docs/BUILD.ru.md) |

[Validation](docs/VALIDATION.md) · [Design notes](docs/DESIGN.md) · [Changelog](CHANGELOG.md)

The implementation uses C++17 and JUCE 7.0.12. The original NoteFollow VST3 identity and first 18 parameter IDs/ranges are retained for saved-project compatibility. Install one version at a time.

## License

This distribution is licensed under [GPL-3.0](LICENSE.txt). JUCE has its own GPL/commercial licensing terms; JUCE source and upstream license notices are available from its pinned upstream release. See [third-party notes](ThirdParty/README.md).
