"""Package a universal Mac VST3 as one installable bundle."""
import argparse
import hashlib
import json
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys


def run(*args):
    return subprocess.check_output(args, text=True).strip()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-dir', required=True, type=Path)
    parser.add_argument('--juce-dir', required=True, type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    if sys.platform != 'darwin':
        parser.error('Packaging requires macOS tools (codesign, lipo, ditto).')
    source = Path(__file__).resolve().parent.parent
    version = re.search(r'project\(NoteShaper VERSION ([\d.]+)', (source / 'CMakeLists.txt').read_text()).group(1)
    build, juce, output = (p.resolve() for p in (args.build_dir, args.juce_dir, args.output_dir))
    output.mkdir(parents=True, exist_ok=True)
    package = output / 'NoteShaper.vst3'
    if package.exists():
        raise RuntimeError(f'Package already exists: {package}')
    artefacts = build / 'NoteShaper_artefacts/Release'
    for folder, name in [('VST3', 'NoteShaper.vst3')]:
        original = artefacts / folder / name
        destination = package
        destination.parent.mkdir(parents=True, exist_ok=True)
        subprocess.check_call(['ditto', str(original), str(destination)])
        resources = destination / 'Contents/Resources'
        resources.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source / 'LICENSE.txt', resources / 'LICENSE.txt')
        commit = run('git', '-C', str(source), 'rev-parse', 'HEAD')
        (resources / 'SOURCE.txt').write_text(
            f'NoteShaper {version} — macOS 11+, Intel x86_64 / Apple Silicon arm64\n'
            f'License: GPL-3.0\nSource: https://github.com/alexeyPetrushin/NoteShaper/tree/{commit}\n'
            'JUCE 7.0.12 source: https://github.com/juce-framework/JUCE/archive/4f43011b96eb0636104cb3e433894cda98243626.zip\n'
            'JUCE compatibility patch: Build/patch_juce_macos.py in the NoteShaper source.\n'
            'Signing: ad-hoc; no Developer ID or Apple notarization.\n', encoding='utf-8')
        executable = destination / 'Contents/MacOS/NoteShaper'
        subprocess.check_call(['lipo', str(executable), '-verify_arch', 'x86_64', 'arm64'])
        subprocess.check_call(['codesign', '--force', '--sign', '-', str(destination)])
        subprocess.check_call(['codesign', '--verify', '--deep', '--strict', str(destination)])

    archive = output / f'NoteShaper-{version}-mac-vst3.zip'
    subprocess.check_call(['ditto', '-c', '-k', '--sequesterRsrc', '--keepParent', str(package), str(archive)])
    checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
    (output / f'NoteShaper-{version}-mac-SHA256.txt').write_text(f'{checksum}  {archive.name}\n', encoding='utf-8')
    print(f'Created {archive.name}: universal x86_64/arm64, ad-hoc signature, SHA-256 {checksum}')


if __name__ == '__main__':
    main()
