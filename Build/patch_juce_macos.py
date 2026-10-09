"""Let JUCE 7 build with macOS 15+ SDKs without its unused native-window capture.

NoteShaper does not use createSnapshotOfNativeWindow. Component painting and
editor rendering are unaffected. Older SDKs keep the original capture code.
The corresponding original JUCE source and this patch are shipped together.
"""
from pathlib import Path
import sys

path = Path(sys.argv[1]) / 'modules/juce_gui_basics/native/juce_Windowing_mac.mm'
source = path.read_text(encoding='utf-8')
marker = '// NoteShaper: native-window capture is unused; macOS 15 SDK removed its API.'
if marker not in source:
    signature = 'static Image createNSWindowSnapshot (NSWindow* nsWindow)\n{'
    start = source.index(signature) + len(signature)
    end = source.index('\nImage createSnapshotOfNativeWindow', start)
    body_end = source.rfind('\n}', start, end)
    assert body_end > start and 'CGWindowListCreateImage' in source[start:body_end]
    replacement = '''
   #if MAC_OS_X_VERSION_MAX_ALLOWED >= 150000
    // NoteShaper: native-window capture is unused; macOS 15 SDK removed its API.
    ignoreUnused (nsWindow);
    return {};
   #else'''
    source = source[:start] + replacement + source[start:body_end] + '\n   #endif' + source[body_end:]
    path.write_text(source, encoding='utf-8')
print('Applied JUCE 7 macOS SDK compatibility patch (native-window capture only).')
