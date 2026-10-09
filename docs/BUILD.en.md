# Build NoteShaper

[Русский](BUILD.ru.md) · [Home](../README.md)

## Dependencies

- C++17 compiler and CMake 3.22 or newer.
- JUCE **7.0.12** source. Clone the pinned upstream tag `7.0.12` (commit `4f43011b96eb0636104cb3e433894cda98243626`) or download its source from JUCE.
- A Windows x64 toolchain for Windows builds, or Xcode and Ninja on macOS for Mac builds. The screenshot helper uses Windows APIs.

## macOS universal build

Install Xcode with command-line tools, CMake 3.22+ and Ninja. Obtain the unchanged JUCE 7.0.12 source as described above. From the NoteShaper source directory:

```sh
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git ../JUCE-7.0.12
python3 Build/patch_juce_macos.py ../JUCE-7.0.12
cmake -S . -B build-macos -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH="$PWD/../JUCE-7.0.12" '-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64' -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DNOTESHAPER_BUILD_HOST_CHECK=ON
cmake --build build-macos --target NoteShaper_VST3 NoteShaperDSPTest NoteShaperHostCheck --parallel 3
./build-macos/NoteShaperDSPTest
./build-macos/NoteShaperHostCheck "$PWD/build-macos/NoteShaper_artefacts/Release/VST3/NoteShaper.vst3" unused
python3 Build/package_macos.py --build-dir build-macos --juce-dir ../JUCE-7.0.12 --output-dir dist
```

The VST3 is under `build-macos/NoteShaper_artefacts/Release/VST3`. The packager verifies both architectures, applies an ad-hoc signature, embeds licensing/source links, and writes `NoteShaper-0.5.2-mac-vst3.zip` plus a SHA-256 file. The public ZIP contains only the VST3 bundle. A standalone developer app can be built separately with target `NoteShaper_Standalone`. This does not provide Developer ID signing or notarization. Do not apply the portable Windows JUCE patch for this Mac build.

The Mac JUCE patch disables only the unused native-window capture function when building with a macOS 15+ SDK, where Apple removed its old API. NoteShaper does not call it; component painting and audio processing are unaffected. The patch is in this repository; original JUCE source is available at the pinned upstream commit.

The [macOS workflow](../.github/workflows/macos.yml) builds on Xcode 16.4, tests the same packaged universal plug-in natively on Intel and Apple Silicon, and adds the Mac download to the existing version's release only after both checks pass. It preserves an already-published Mac archive; use a new project version for a replacement. `NOTESHAPER_BUILD_HOST_CHECK` enables the portable host checker. The screenshot helper `NoteShaperPreview` is Windows-only.

## Visual Studio

Install Visual Studio's Desktop development with C++ tools. Extract JUCE or clone its pinned release:

```powershell
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git C:/dev/JUCE-7.0.12
cmake -S . -B build -A x64 -DJUCE_PATH=C:/dev/JUCE-7.0.12
cmake --build build --config Release --target NoteShaper_All NoteShaperDSPTest
./build/Release/NoteShaperDSPTest.exe
```

The plug-in and standalone application appear under `build/NoteShaper_artefacts/Release`. This is the standard developer route; the shipped binary was built with the portable toolchain below.

## Portable LLVM-MinGW

The release used LLVM-MinGW 20261006 (Clang 23.1.3), CMake 4.4.4 and MinGW Makefiles. Keep tool, source and build paths ASCII; Windows short paths may be needed for non-ASCII account names. Add the toolchain's `bin` folder to PATH.

Apply the included patch to a clean JUCE 7.0.12 checkout. It updates two compatibility constructs and fixes source/module paths in the helper bootstrap:

```powershell
python Build/patch_juce.py C:/dev/JUCE-7.0.12
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH=C:/dev/JUCE-7.0.12 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_RC_COMPILER=windres
cmake --build build --target NoteShaper_All NoteShaperDSPTest -j 4
./build/NoteShaperDSPTest.exe
```

The MinGW runtime libraries are linked statically. Windows system DLLs are still supplied by Windows. No paid pitch-correction engine or online service is required at runtime.

## Additional checks

Enable the native editor renderer and VST3 host checker during configuration:

```powershell
cmake -S . -B build -DJUCE_PATH=C:/dev/JUCE-7.0.12 -DHOST_CHECK_SOURCE=C:/dev/NoteShaper/Tests/host_check.cpp
cmake --build build --config Release --target NoteShaperPreview NoteShaperHostCheck
```

Use the same generator/options as the initial configuration. Run the produced checkers with **ASCII absolute paths**:

```powershell
./build/Release/NoteShaperPreview.exe C:/dev/NoteShaper
./build/Release/NoteShaperHostCheck.exe C:/dev/NoteShaper/build/NoteShaper_artefacts/Release/VST3/NoteShaper.vst3 unused
```

For MinGW, the executables are directly under `build`. The preview output directory must contain `Design`. The host checker accepts an optional third argument pointing to an original NoteFollow VST3 for legacy-state validation.

The pure engine test can also run without JUCE:

```sh
g++ -std=c++17 -O2 -ISource Tests/dsp_test.cpp -o dsp-test
./dsp-test
```

See [validation](VALIDATION.md) for the checks and their limits. GPL-3.0 and JUCE's upstream licensing terms apply to redistribution.
