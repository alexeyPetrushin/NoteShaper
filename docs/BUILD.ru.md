# Сборка NoteShaper

[English](BUILD.en.md) · [Главная](../README.ru.md)

## Зависимости

- Компилятор C++17 и CMake 3.22 или новее.
- Исходники JUCE **7.0.12**. Полный релиз содержит `ThirdParty/JUCE-7.0.12.zip`; репозиторий ссылается на оригинальный выпуск библиотеки.
- Windows x64 для готового VST3 и отдельного приложения. Проверка снимков интерфейса использует API Windows.

## Статус macOS

Готовые архивы предназначены только для Windows. Исходники C++/JUCE можно использовать как основу отдельной macOS-сборки VST3/standalone с инструментами Mac и нужной архитектурой Intel/Apple Silicon. Mac-сборка, её подпись и проверки в DAW в этот релиз не входят. Не включайте `HOST_CHECK_SOURCE` на macOS: используемые здесь нативные проверки интерфейса/хоста зависят от API Windows. Команды ниже описывают сборку Windows и не являются проверенной инструкцией для Mac.

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
