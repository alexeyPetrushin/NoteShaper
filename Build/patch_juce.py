"""Keep explicit source paths during JUCE's helper bootstrap on Windows."""
import pathlib
import sys
import os
root = pathlib.Path(sys.argv[1])
compatibility = [
    ('modules/juce_audio_basics/buffers/juce_AudioSampleBuffer.h',
     'std::alignment_of_v<Type>', 'alignof(Type)'),
    ('modules/juce_audio_processors/processors/juce_AudioPluginInstance.h',
     'const short channelLayoutList[numLayouts][2]', 'const short (&channelLayoutList)[numLayouts][2]'),
]
for relative, old_value, new_value in compatibility:
    header = root / relative
    contents = header.read_text(encoding='utf-8')
    if old_value in contents:
        header.write_text(contents.replace(old_value, new_value), encoding='utf-8')
        print('Applied modern Clang compatibility: ' + relative)
file = root / 'extras/Build/juceaide/CMakeLists.txt'
text = file.read_text(encoding='utf-8')
old = 'execute_process(COMMAND "${CMAKE_COMMAND}"\n            "."'
new = 'execute_process(COMMAND "${CMAKE_COMMAND}"\n            "-S${JUCE_SOURCE_DIR}"'
if old in text:
    file.write_text(text.replace(old, new), encoding='utf-8')
    print('Applied JUCE Windows source-path workaround.')
else:
    print('JUCE source-path workaround already applied or not needed.')
if os.name == 'nt':
    import ctypes
    long_root = str(root.resolve()).replace('\\', '/')
    short_buffer = ctypes.create_unicode_buffer(32768)
    ctypes.windll.kernel32.GetShortPathNameW(str(root.resolve()), short_buffer, len(short_buffer))
    short_root = short_buffer.value.replace('\\', '/')
    support = root / 'extras/Build/CMake/JUCEModuleSupport.cmake'
    contents = support.read_text(encoding='utf-8')
    marker = 'macro(_juce_make_absolute path)'
    addition = '\n    string(REPLACE "' + long_root + '" "' + short_root + '" ${path} "${${path}}")'
    contents = contents.replace(addition, '')
    start = contents.index(marker)
    end = contents.index('endmacro()', start)
    contents = contents[:end] + addition + '\n' + contents[end:]
    support.write_text(contents, encoding='utf-8')
    print('Applied JUCE module short-path workaround.')
