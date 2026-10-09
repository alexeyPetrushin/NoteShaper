# NoteShaper на macOS

[English](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/MACOS.en.md) · [Гайд по использованию](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/USER_GUIDE.ru.md)

## Скачать и проверить совместимость

Используйте **`NoteShaper-0.5.2-macos-universal.zip`** из [Releases](https://github.com/alexeyPetrushin/NoteShaper/releases/latest), когда он доступен. Mac-сборка публикуется после нативных проверок DSP и загрузки VST3 на Intel и Apple Silicon. Windows-архивы содержат другую сборку.

Mac-комплект предназначен для **macOS 11 и новее**, **Intel x86_64 и Apple Silicon arm64**. Один универсальный VST3 и отдельное приложение работают на обеих архитектурах. AU и AAX в комплект не входят. Ableton Live и FL Studio на Mac поддерживают VST3, но сам NoteShaper в этих DAW отдельно не тестировался. Минимальная версия macOS — цель сборки; все старые версии системы не проверялись.

## Установка VST3

1. Скачайте и распакуйте Mac-архив. Закройте DAW.
2. В Finder выберите **Переход → Перейти к папке** и введите **`~/Library/Audio/Plug-Ins/VST3`** для текущего пользователя. Если папки `VST3` нет, создайте её. Для всех пользователей можно использовать **`/Library/Audio/Plug-Ins/VST3`**; этот каталог может потребовать права администратора.
3. Скопируйте туда целиком пакет **`VST3/NoteShaper.vst3`** из Mac-комплекта. Сохраните его внутреннюю структуру. Пакет из Windows-архива не подходит.
4. В Ableton Live включите системные папки VST3 в настройках Plug-Ins и пересканируйте плагины. В FL Studio откройте **Options → File settings → Manage plugins**, включите **Verify plugins** и запустите **Find installed plugins**.
5. Добавьте NoteShaper как аудиоэффект на дорожку или канал микшера с отдельным сухим вокалом, перед реверберацией и дилеем. В сканируемых каталогах оставьте одну копию.

[Расположения VST3 — Steinberg](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Locations%2BFormat/Plugin%2BLocations.html) · [Установка в FL Studio](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/basics_externalplugins.htm)

## Первый запуск и защита macOS

У сборки **ad-hoc подпись**, без сертификата Apple Developer ID и нотарификации. macOS может заблокировать скачанный плагин или приложение. Сверьте SHA-256 архива с файлом `macos-SHA256.txt`; скачивайте его из релиза этого проекта.

Если заблокировано отдельное приложение, после попытки запуска разрешите именно его через **Системные настройки → Конфиденциальность и безопасность → Всё равно открыть**, по [инструкции Apple](https://support.apple.com/en-us/102445).

Если доверенный VST3 не загружается в DAW из-за карантина, снимите атрибут только с установленного пакета NoteShaper и пересканируйте. Для пользовательской установки выше:

```sh
xattr -dr com.apple.quarantine "$HOME/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3"
```

При установке для всех пользователей укажите фактический путь `/Library/Audio/Plug-Ins/VST3/NoteShaper.vst3`; могут потребоваться права администратора. Это разрешение конкретного пакета, а не нотарификация. Защиту macOS оставьте включённой.

## Отдельное приложение и управление

`Standalone/NoteShaper.app` открывается без DAW и не устанавливает VST3. Перенесите его в «Программы» или выбранную папку приложений. Выберите устройства ввода и вывода звука; macOS может запросить доступ к микрофону. Используйте наушники, чтобы избежать обратной связи.

Над клавишами выберите **Note / Chord / Scale / Custom / MIDI**. В аккорде и гамме справа выбирается тип, а клавишами C, C#, D… B — основной тон. **Strength**, **Retune time**, **Pitch freedom** задают силу, скорость и допустимое отклонение. **Rap** даёт сильную ступенчатую коррекцию; **Gentle** — отправная настройка для мягкой правки. Кнопка **EN** справа вверху включает русский.

Плагин исправляет один голос и имеет фиксированную задержку **64 мс**; прежде всего он рассчитан на записанный вокал. Маршрутизация MIDI, пресеты и ограничения описаны в [полном гайде](https://github.com/alexeyPetrushin/NoteShaper/blob/main/docs/USER_GUIDE.ru.md).

## Данные сборки и помощь

`BUILD_INFO.json` содержит коммиты NoteShaper/JUCE, инструменты, архитектуры, минимальную систему и сведения о подписи. `MANIFEST.sha256` перечисляет хеши файлов комплекта. Исходники NoteShaper находятся в `SourceCode`, JUCE 7.0.12 — в `SourceCode/ThirdParty`.

При сообщении об ошибке укажите версию macOS, модель Intel/Apple Silicon, DAW и её версию, версию NoteShaper, путь установки и точное сообщение сканирования. Проверка загрузки в тестовом хосте не гарантирует работу в каждой DAW и с каждым аудиоустройством.
