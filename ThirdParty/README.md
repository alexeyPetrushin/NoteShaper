# JUCE

NoteShaper uses JUCE **7.0.12**, pinned to commit [`4f43011b96eb0636104cb3e433894cda98243626`](https://github.com/juce-framework/JUCE/tree/4f43011b96eb0636104cb3e433894cda98243626). JUCE is available under GPL or commercial licensing terms; this distribution uses GPL-3.0.

Download the [original JUCE source](https://github.com/juce-framework/JUCE/archive/4f43011b96eb0636104cb3e433894cda98243626.zip) with its upstream license notices. Corresponding NoteShaper source is in this repository. The installed bundle's `Contents/Resources/SOURCE.txt` records the build's exact source commit and source links.

The portable Windows build uses [Build/patch_juce.py](../Build/patch_juce.py). The Mac build uses [Build/patch_juce_macos.py](../Build/patch_juce_macos.py), which disables JUCE's unused native-window capture with macOS 15+ SDKs. Apply the platform's patch to the pinned JUCE source to reproduce the build; see [build instructions](../docs/BUILD.en.md).

NoteShaper использует JUCE **7.0.12** по условиям GPL-3.0. Оригинальные исходники библиотеки с лицензионными уведомлениями доступны по ссылке выше. Исходники NoteShaper находятся в этом репозитории; точный коммит сборки и ссылки записаны в `Contents/Resources/SOURCE.txt` внутри пакета.

Для портативной Windows-сборки применяется `Build/patch_juce.py`, для Mac — `Build/patch_juce_macos.py`. Mac-патч отключает только неиспользуемый захват нативного окна при SDK macOS 15+. Для воспроизведения примените нужный патч к закреплённым исходникам JUCE; см. [инструкцию сборки](../docs/BUILD.ru.md).
