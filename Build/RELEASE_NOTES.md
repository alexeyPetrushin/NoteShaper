## NoteShaper

**RU:** VST3-плагин коррекции высоты одного голоса. Выберите ноту, аккорд, гамму или свой набор нот и настройте силу, скорость и допуск коррекции. «Мягко» подходит как отправная настройка для аккуратной правки, «Рэп» — для резкого ступенчатого эффекта. Интерфейс EN/RU.

**EN:** A monophonic vocal pitch-correction VST3. Choose a note, chord, scale or custom note set and adjust strength, retune time and pitch freedom. Gentle is a starting point for subtle correction; Rap gives sharp stepped tuning. EN/RU interface.

## Windows

**Download:** [NoteShaper-@VERSION@-win-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v@VERSION@/NoteShaper-@VERSION@-win-vst3.zip) — **Windows x64**.

**RU — установка:**

1. Скачайте файл с **`win`** в имени и распакуйте ZIP.
2. Закройте DAW. Скопируйте **папку `NoteShaper.vst3` целиком** в каталог VST3, который сканирует ваша DAW. Общая папка Windows — **`%CommonProgramFiles%\VST3`**: вставьте этот путь в адресную строку Проводника. В Ableton можно использовать отдельную выбранную папку VST3; в FL Studio используйте стандартный каталог.
3. Пересканируйте плагины и добавьте NoteShaper как эффект на канал вокала.

**EN — installation:** Download the **`win`** ZIP, extract it and close your DAW. Copy the whole **`NoteShaper.vst3` folder** into a VST3 location scanned by your host, normally **`%CommonProgramFiles%\VST3`**. Rescan and insert NoteShaper as a vocal audio effect.

[Подробная установка Windows](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/USER_GUIDE.ru.md#установка-на-windows) · [Windows installation guide](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/USER_GUIDE.en.md#installation-on-windows)

## macOS

**Download:** [NoteShaper-@VERSION@-mac-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v@VERSION@/NoteShaper-@VERSION@-mac-vst3.zip) — **macOS 11+, Intel и Apple Silicon / Intel and Apple Silicon**. Один универсальный файл / One universal build.

**RU — установка:**

1. Скачайте файл с **`mac`** в имени и распакуйте ZIP.
2. Закройте DAW. В Finder откройте **Переход → Перейти к папке** и введите **`~/Library/Audio/Plug-Ins/VST3`**. Создайте папку VST3, если её нет. Для всех пользователей можно выбрать **`/Library/Audio/Plug-Ins/VST3`**.
3. Скопируйте туда **пакет `NoteShaper.vst3` целиком**, пересканируйте плагины и добавьте его на канал вокала.

**EN — installation:** Download the **`mac`** ZIP, extract it and close your DAW. Copy the whole **`NoteShaper.vst3` bundle** into **`~/Library/Audio/Plug-Ins/VST3`** for your user, or **`/Library/Audio/Plug-Ins/VST3`** for all users. Rescan and insert it as a vocal effect.

Mac-сборка имеет **ad-hoc подпись, без нотарификации Apple**. Возможная блокировка Gatekeeper и её разрешение описаны в гайде. / The Mac build is **ad-hoc signed, without Apple notarization**; see the guide if Gatekeeper blocks it.

[Подробная установка macOS](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/MACOS.ru.md) · [macOS installation guide](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/MACOS.en.md)

## Совместимость / Compatibility

Один чистый голос, 64-битная DAW с VST3, фиксированная задержка **64 мс**. Нативные проверки загрузки выполнены в тестовом VST3-хосте; Mac-пакет проверен на Intel и Apple Silicon с macOS 15. В Ableton Live и FL Studio отдельно не тестировался. / One clean voice, a 64-bit VST3 host, and **64 ms** fixed latency. Native test-host loading checks cover Windows and both Mac architectures; individual Ableton Live/FL Studio validation is pending.

<details>
<summary>SHA-256 и лицензия / SHA-256 and license</summary>

```text
47e3c16417c2ad3dc66193e62a5fd4f94c0521140b78e87457b057904cc536c6  NoteShaper-@VERSION@-win-vst3.zip
@MAC_SHA256@  NoteShaper-@VERSION@-mac-vst3.zip
```

GPL-3.0. [Исходники NoteShaper / NoteShaper source](https://github.com/alexeyPetrushin/NoteShaper) · [JUCE 7.0.12 source](https://github.com/juce-framework/JUCE/archive/4f43011b96eb0636104cb3e433894cda98243626.zip). Соответствующие коммиты и патчи указаны в `Contents/Resources/SOURCE.txt` внутри пакета. / Exact source commits and patches are listed in the bundle's `Contents/Resources/SOURCE.txt`.

</details>
