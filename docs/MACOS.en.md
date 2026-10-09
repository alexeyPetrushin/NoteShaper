# NoteShaper on macOS

[Русский](MACOS.ru.md) · [Usage guide](USER_GUIDE.en.md)

## Download

**File for Mac:** [NoteShaper-0.5.2-mac-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v0.5.2/NoteShaper-0.5.2-mac-vst3.zip), for **macOS 11+, Intel x86_64 and Apple Silicon arm64**. One universal download covers both architectures. The ZIP contains the **`NoteShaper.vst3` bundle**.

**File for Windows:** [NoteShaper-0.5.2-win-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v0.5.2/NoteShaper-0.5.2-win-vst3.zip); see [Windows installation](USER_GUIDE.en.md#installation-on-windows).

## Install on macOS

1. Download and extract the **mac** ZIP. Close your DAW.
2. In Finder choose **Go → Go to Folder** and enter **`~/Library/Audio/Plug-Ins/VST3`** for the current user. Create the `VST3` folder if needed. For all users, use **`/Library/Audio/Plug-Ins/VST3`**; that location may require administrator permission.
3. Copy the entire **`NoteShaper.vst3` bundle** into that location. Keep its internal structure intact.
4. In Ableton Live's Plug-Ins preferences, enable **VST3 System Folders** and rescan. In FL Studio, open **Options → File settings → Manage plugins**, enable **Verify plugins**, and run **Find installed plugins**.
5. Insert NoteShaper as an audio effect on a clean vocal track or Mixer channel, before reverb/delay. Keep one installed copy across your DAW's scan locations.

[VST3 locations (Steinberg)](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Locations%2BFormat/Plugin%2BLocations.html) · [FL Studio installation](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm)

## If macOS blocks the plug-in

The bundle is **ad-hoc signed, without Apple Developer ID or notarization**. macOS can block a downloaded plug-in. Download from this project's release and compare the ZIP's SHA-256 with the value in the release page's **SHA-256 and license** section:

```sh
shasum -a 256 ~/Downloads/NoteShaper-0.5.2-mac-vst3.zip
```

For a trusted installed NoteShaper bundle that the DAW refuses because of quarantine, remove the quarantine attribute only from that bundle, then rescan. For the user installation above:

```sh
xattr -dr com.apple.quarantine "$HOME/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3"
```

For a shared installation, use the actual `/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3` path; administrator rights may be needed. Keep macOS security enabled. [Apple's security guidance](https://support.apple.com/en-us/102445).

## Controls and compatibility

Choose **Note / Chord / Scale / Custom / MIDI** above the note keys. In Chord and Scale, choose the type on the right and the root with C, C#, D… B. **Strength**, **Retune time** and **Pitch freedom** set the amount, speed and allowance of correction. **Rap** gives strong stepped tuning; **Gentle** is a starting point for subtle correction. **EN** at the top right switches to Russian.

The effect processes one voice with **64 ms** fixed latency, primarily for recorded vocals. See the [user guide](USER_GUIDE.en.md) for MIDI, presets and limitations.

The same universal ZIP passed architecture/signature, DSP and native VST3 host checks on Intel and Apple Silicon with macOS 15. macOS 11 is the deployment target; older OS releases were not individually tested. Ableton Live and FL Studio support VST3, but NoteShaper was not tested in either DAW individually. Logic Pro requires AU, which this download does not provide.

## Updates and help

Close your DAW, back up the previous bundle outside all scan folders, copy the complete new bundle and rescan. NoteFollow and NoteShaper share the VST3 identity for saved projects; keep one version installed.

When reporting an issue, include macOS version, Intel/Apple Silicon model, DAW/version, NoteShaper version, install path and the scanner's exact message. Source commits and JUCE licensing links are recorded in the bundle's `Contents/Resources/SOURCE.txt`; see [building from source](BUILD.en.md).
