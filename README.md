# NoteShaper

[Русский](README.ru.md) · [English guide](docs/USER_GUIDE.en.md) · [Русская инструкция](docs/USER_GUIDE.ru.md)

[Support the author](https://www.donationalerts.com/r/lesha_diabet_)

An open-source monophonic vocal pitch-correction effect for **Windows x64 / VST3**, with a standalone application. Choose a note, chord, scale or custom set, then dial in hard tuning or gentle correction with three always-visible rotary controls.

![NoteShaper interface](assets/preview.png)

## Download

Get the Windows build from [Releases](https://github.com/alexeyPetrushin/NoteShaper/releases/latest).

- **NoteShaper-0.5.2-vst3.zip**: ready-to-use VST3 bundle and guides.
- **NoteShaper-0.5.2-full.zip**: VST3, standalone application, source, tests and the JUCE source archive.

Extract the archive and close your DAW. Copy the entire `VST3/NoteShaper.vst3` folder to a VST3 location supported by your host, then rescan. The shared Windows location is `%CommonProgramFiles%\VST3`; the drive and custom-folder support depend on your setup and DAW. The [installation guide](docs/USER_GUIDE.en.md#installation-on-windows) covers system/user/custom paths, Ableton Live, FL Studio and updates.

## Compatibility

Ready-made downloads are **Windows x64**. Ableton Live, FL Studio and other 64-bit VST3 hosts are expected to load this format; NoteShaper has been checked in a JUCE VST3 host, not in those DAWs individually. [FL Studio supports VST3](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm).

**macOS: no release build yet.** The Windows VST3/EXE files cannot run on a Mac. A separate Mac build and host validation are required; see the [build guide](docs/BUILD.en.md). Source availability is not a claim of tested Mac support.

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

This distribution is licensed under [GPL-3.0](LICENSE.txt). JUCE has its own GPL/commercial licensing terms; upstream license notices are retained in the JUCE source archive included with the full release. See [third-party notes](ThirdParty/README.md).
