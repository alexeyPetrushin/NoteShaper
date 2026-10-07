# Build NoteShaper

[Русский](BUILD.ru.md) · [Home](../README.md)

## Dependencies

- C++17 compiler and CMake 3.22 or newer.
- JUCE **7.0.12** source. The full release includes `ThirdParty/JUCE-7.0.12.zip`; the GitHub source repository links to upstream instead of storing the archive.
- Windows x64 for the distributed VST3/standalone targets. The preview checker uses Windows APIs.

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
