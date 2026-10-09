# Сборка NoteShaper

[English](BUILD.en.md) · [Главная](../README.ru.md)

## Зависимости

- Компилятор C++17 и CMake 3.22 или новее.
- Исходники JUCE **7.0.12**. Полный релиз содержит `ThirdParty/JUCE-7.0.12.zip`; репозиторий ссылается на оригинальный выпуск библиотеки.
- Инструменты Windows x64 для Windows-сборки либо Xcode и Ninja на macOS для Mac-сборки. Генератор снимков интерфейса использует API Windows.

## Универсальная сборка macOS

Установите Xcode с инструментами командной строки, CMake 3.22+ и Ninja. Используйте оригинальные исходники JUCE 7.0.12. Из каталога исходников NoteShaper:

```sh
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git ../JUCE-7.0.12
python3 Build/patch_juce_macos.py ../JUCE-7.0.12
cmake -S . -B build-macos -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH="$PWD/../JUCE-7.0.12" '-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64' -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DNOTESHAPER_BUILD_HOST_CHECK=ON
cmake --build build-macos --target NoteShaper_VST3 NoteShaper_Standalone NoteShaperDSPTest NoteShaperHostCheck --parallel 3
./build-macos/NoteShaperDSPTest
./build-macos/NoteShaperHostCheck "$PWD/build-macos/NoteShaper_artefacts/Release/VST3/NoteShaper.vst3" unused
python3 Build/package_macos.py --build-dir build-macos --juce-dir ../JUCE-7.0.12 --output-dir dist
```

VST3 и приложение появляются в `build-macos/NoteShaper_artefacts/Release`. Упаковщик проверяет обе архитектуры, ставит ad-hoc подпись, добавляет исходники/JUCE и создаёт ZIP с SHA-256. Сертификат Developer ID и нотарификация этим не обеспечиваются. Патч JUCE для портативной Windows-сборки на Mac не применяйте.

Mac-патч JUCE отключает только неиспользуемую функцию захвата нативного окна при сборке с SDK macOS 15+, где Apple убрала старый API. NoteShaper её не вызывает; отрисовка компонентов и обработка звука не меняются. Оригинальные исходники JUCE и патч поставляются вместе.

[Mac-сборка в GitHub](../.github/workflows/macos.yml) использует Xcode 16.4, нативно проверяет один упакованный универсальный VST3 на Intel и Apple Silicon и добавляет архив в существующий релиз версии только после обеих успешных проверок. Уже опубликованный Mac-архив сохраняется; для замены нужна новая версия проекта. `NOTESHAPER_BUILD_HOST_CHECK` включает переносимую проверку хоста. Генератор снимков `NoteShaperPreview` предназначен только для Windows.

## Visual Studio

Установите инструменты Desktop development with C++ в Visual Studio. Распакуйте JUCE либо клонируйте закреплённую версию:

```powershell
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git C:/dev/JUCE-7.0.12
cmake -S . -B build -A x64 -DJUCE_PATH=C:/dev/JUCE-7.0.12
cmake --build build --config Release --target NoteShaper_All NoteShaperDSPTest
./build/Release/NoteShaperDSPTest.exe
```

Плагин и приложение появятся в `build/NoteShaper_artefacts/Release`. Это обычный путь для разработки; опубликованный бинарный файл собран портативными инструментами, описанными ниже.

## Портативный LLVM-MinGW

Для релиза использованы LLVM-MinGW 20261006 (Clang 23.1.3), CMake 4.4.4 и MinGW Makefiles. Пути к инструментам, исходникам и сборке должны содержать ASCII-символы; при кириллице в имени пользователя могут потребоваться короткие пути Windows. Добавьте `bin` компилятора в PATH.

Примените поставляемый патч к чистой JUCE 7.0.12. Он исправляет две конструкции для современного компилятора и пути при сборке вспомогательного инструмента:

```powershell
python Build/patch_juce.py C:/dev/JUCE-7.0.12
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH=C:/dev/JUCE-7.0.12 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_RC_COMPILER=windres
cmake --build build --target NoteShaper_All NoteShaperDSPTest -j 4
./build/NoteShaperDSPTest.exe
```

Библиотеки времени выполнения MinGW подключены статически. Системные DLL предоставляет Windows. Платный движок коррекции высоты и онлайн-сервис для работы не нужны.

## Дополнительные проверки

Включите проверку VST3 и генерацию снимков интерфейса при настройке сборки:

```powershell
cmake -S . -B build -DJUCE_PATH=C:/dev/JUCE-7.0.12 -DHOST_CHECK_SOURCE=C:/dev/NoteShaper/Tests/host_check.cpp
cmake --build build --config Release --target NoteShaperPreview NoteShaperHostCheck
```

Сохраняйте генератор и остальные параметры первоначальной настройки. Запускайте проверки с **абсолютными ASCII-путями**:

```powershell
./build/Release/NoteShaperPreview.exe C:/dev/NoteShaper
./build/Release/NoteShaperHostCheck.exe C:/dev/NoteShaper/build/NoteShaper_artefacts/Release/VST3/NoteShaper.vst3 unused
```

При MinGW программы находятся прямо в `build`. В папке для снимков должен быть каталог `Design`. Третий необязательный аргумент проверки хоста — путь к исходному NoteFollow VST3 для проверки старых настроек.

Движок можно проверить отдельно без JUCE:

```sh
g++ -std=c++17 -O2 -ISource Tests/dsp_test.cpp -o dsp-test
./dsp-test
```

Результаты и ограничения описаны в [проверках](VALIDATION.md). При распространении действуют GPL-3.0 и условия лицензирования JUCE.
