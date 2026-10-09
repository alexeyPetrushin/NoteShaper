# NoteShaper on macOS

[Русский](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/MACOS.ru.md) · [Usage guide](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/USER_GUIDE.en.md)

## Download and compatibility

Use **`NoteShaper-0.5.2-macos-universal.zip`** from [Releases](https://github.com/alexeyPetrushin/NoteShaper/releases/latest) when available. The macOS workflow uploads it only after native DSP and VST3 host checks pass on Intel and Apple Silicon. The Windows ZIP files are a different build.

The Mac package targets **macOS 11 or later**, **Intel x86_64 and Apple Silicon arm64**, with a universal VST3 and standalone application. It does not include AU or AAX. Ableton Live and FL Studio on Mac support VST3, but NoteShaper has not been tested in those DAWs individually. The minimum OS is a build target, not a claim that every older macOS release was tested.

## Install the VST3

1. Download and extract the Mac ZIP. Close your DAW.
2. In Finder, choose **Go → Go to Folder** and enter **`~/Library/Audio/Plug-Ins/VST3`** for the current user. Create the `VST3` folder if needed. Alternatively, use **`/Library/Audio/Plug-Ins/VST3`** for all users; that location may require administrator permission.
3. Copy the entire **`VST3/NoteShaper.vst3`** bundle from the package into that location. Keep the bundle intact. The Windows bundle cannot replace it.
4. Enable VST3 system folders in Ableton Live's Plug-Ins preferences and rescan. In FL Studio, open **Options → File settings → Manage plugins**, enable **Verify plugins**, and run **Find installed plugins**.
5. Insert NoteShaper in an audio-effect slot on a clean vocal track or Mixer channel, before reverb/delay. Keep a single installed copy across the folders your DAW scans.

[VST3 locations (Steinberg)](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Locations%2BFormat/Plugin%2BLocations.html) · [FL Studio installation](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm)

## First use and macOS security

This build is **ad-hoc signed**, without an Apple Developer ID certificate or notarization. A downloaded plug-in or app can be blocked by macOS. Check the archive against its `macos-SHA256.txt` file and obtain it from this project's release.

If macOS blocks the standalone app, follow [Apple's instructions](https://support.apple.com/en-us/102445) to allow that specific app through **System Settings → Privacy & Security → Open Anyway** after attempting to open it.

For a trusted VST3 bundle that the DAW refuses because of quarantine, remove the quarantine attribute only from this installed plug-in, then rescan. For the user installation above:

```sh
xattr -dr com.apple.quarantine "$HOME/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3"
```

If installed for all users, use the actual `/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3` path instead; administrator rights may be needed. This is a per-bundle workaround, not notarization. Keep macOS security enabled.

## Standalone and controls

`Standalone/NoteShaper.app` opens without a DAW and does not install the plug-in. Move it to Applications or your preferred application folder. Select the audio input/output device; macOS may ask for microphone access. Use headphones to avoid feedback.

Choose **Note / Chord / Scale / Custom / MIDI** above the note keys. In Chord and Scale, choose the type on the right and the root with C, C#, D… B. **Strength**, **Retune time**, and **Pitch freedom** set the amount, speed and allowed deviation of correction. **Rap** is the strongest stepped setting; **Gentle** is a starting point for subtle correction. **EN** in the top-right corner switches to Russian.

The effect corrects one voice, with a fixed **64 ms latency**; it is primarily intended for recorded vocals. See the [full user guide](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/USER_GUIDE.en.md) for MIDI routing, presets and limitations.

## Build details and support

`BUILD_INFO.json` records the source/JUCE commits, toolchain, architecture, minimum OS and signing status. `MANIFEST.sha256` lists packaged-file hashes. The archive includes the corresponding NoteShaper source under `SourceCode` and JUCE 7.0.12 under `SourceCode/ThirdParty`.

When reporting a problem, include the macOS version, Intel/Apple Silicon model, DAW/version, NoteShaper version, install path and the scanner's exact message. A successful test-host load does not establish compatibility with every DAW or audio device.
