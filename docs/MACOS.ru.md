# NoteShaper на macOS

[English](MACOS.en.md) · [Гайд по использованию](USER_GUIDE.ru.md)

## Скачать

**Файл для Mac:** [NoteShaper-0.5.2-mac-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v0.5.2/NoteShaper-0.5.2-mac-vst3.zip), для **macOS 11+, Intel x86_64 и Apple Silicon arm64**. Один универсальный архив подходит обеим архитектурам. Внутри ZIP находится **пакет `NoteShaper.vst3`**.

**Файл для Windows:** [NoteShaper-0.5.2-win-vst3.zip](https://github.com/alexeyPetrushin/NoteShaper/releases/download/v0.5.2/NoteShaper-0.5.2-win-vst3.zip); см. [установку на Windows](USER_GUIDE.ru.md#установка-на-windows).

## Установка на macOS

1. Скачайте и распакуйте архив с **mac** в имени. Закройте DAW.
2. В Finder выберите **Переход → Перейти к папке** и введите **`~/Library/Audio/Plug-Ins/VST3`** для текущего пользователя. Создайте папку `VST3`, если её нет. Для всех пользователей можно использовать **`/Library/Audio/Plug-Ins/VST3`**; могут потребоваться права администратора.
3. Скопируйте туда **пакет `NoteShaper.vst3` целиком**, сохранив его внутреннюю структуру.
4. В Ableton Live включите **VST3 System Folders** в настройках Plug-Ins и пересканируйте плагины. В FL Studio откройте **Options → File settings → Manage plugins**, включите **Verify plugins** и запустите **Find installed plugins**.
5. Добавьте NoteShaper как аудиоэффект на дорожку или канал микшера с отдельным сухим вокалом, до реверберации и дилея. В сканируемых каталогах оставьте одну копию.

[Расположения VST3 — Steinberg](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Locations%2BFormat/Plugin%2BLocations.html) · [Установка в FL Studio](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm)

## Если macOS блокирует плагин

У пакета **ad-hoc подпись, без Apple Developer ID и нотарификации**. macOS может заблокировать скачанный плагин. Скачивайте из релиза этого проекта и сверяйте SHA-256 ZIP с разделом **SHA-256 и лицензия** на странице релиза:

```sh
shasum -a 256 ~/Downloads/NoteShaper-0.5.2-mac-vst3.zip
```

Если доверенный установленный пакет NoteShaper не загружается в DAW из-за карантина, снимите атрибут только с этого пакета и пересканируйте. Для пользовательской установки выше:

```sh
xattr -dr com.apple.quarantine "$HOME/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3"
```

Для общей установки укажите фактический путь `/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3`; могут потребоваться права администратора. Защиту macOS оставьте включённой. [Руководство Apple](https://support.apple.com/en-us/102445).

## Управление и совместимость

Над клавишами выберите **Note / Chord / Scale / Custom / MIDI**. В аккорде и гамме справа выбирается тип, клавишами C, C#, D… B — основной тон. **Strength**, **Retune time** и **Pitch freedom** задают силу, скорость и допуск коррекции. **Rap** даёт сильный ступенчатый эффект, **Gentle** — отправная настройка для аккуратной правки. Кнопка **EN** справа вверху включает русский.

Плагин исправляет один голос и имеет фиксированную задержку **64 мс**; прежде всего он рассчитан на записанный вокал. MIDI, пресеты и ограничения описаны в [полном гайде](USER_GUIDE.ru.md).

Один universal ZIP прошёл проверки архитектур/подписи, DSP и нативной загрузки VST3 на Intel и Apple Silicon с macOS 15. macOS 11 — целевая минимальная система; старые версии отдельно не проверялись. Ableton Live и FL Studio поддерживают VST3, но сам NoteShaper в этих DAW отдельно не тестировался. Для Logic Pro нужен AU, которого в этом архиве нет.

## Обновление и помощь

Закройте DAW, сохраните прежний пакет вне каталогов сканирования, скопируйте новый пакет целиком и пересканируйте. NoteFollow и NoteShaper используют общий идентификатор VST3 для старых проектов; оставьте одну установленную версию.

При сообщении об ошибке укажите версию macOS, модель Intel/Apple Silicon, DAW и её версию, версию NoteShaper, путь установки и точное сообщение сканирования. Коммиты исходников и сведения о JUCE указаны в `Contents/Resources/SOURCE.txt` внутри пакета. См. [сборку из исходников](BUILD.ru.md).
