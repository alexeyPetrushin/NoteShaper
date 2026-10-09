"""Create a macOS distribution while preserving bundles, signatures and sources."""
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
    package = output / f'NoteShaper-{version}-macos'
    if package.exists():
        raise RuntimeError(f'Package already exists: {package}')
    artefacts = build / 'NoteShaper_artefacts/Release'
    for folder, name in [('VST3', 'NoteShaper.vst3'), ('Standalone', 'NoteShaper.app')]:
        original = artefacts / folder / name
        destination = package / folder / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        subprocess.check_call(['ditto', str(original), str(destination)])
        executable = destination / 'Contents/MacOS/NoteShaper'
        subprocess.check_call(['lipo', str(executable), '-verify_arch', 'x86_64', 'arm64'])
        subprocess.check_call(['codesign', '--force', '--sign', '-', str(destination)])
        subprocess.check_call(['codesign', '--verify', '--deep', '--strict', str(destination)])

    shutil.copy2(source / 'docs/MACOS.en.md', package / 'README.md')
    shutil.copy2(source / 'docs/MACOS.ru.md', package / 'README.ru.md')
    shutil.copy2(source / 'LICENSE.txt', package / 'LICENSE.txt')
    source_copy = package / 'SourceCode'
    source_copy.mkdir()
    for folder in ('Source', 'Tests', 'Build', 'ThirdParty', 'docs', 'assets', '.github'):
        shutil.copytree(source / folder, source_copy / folder, ignore=shutil.ignore_patterns('__pycache__', '*.zip'))
    for name in ('CMakeLists.txt', 'LICENSE.txt', 'README.md', 'README.ru.md', 'CHANGELOG.md'):
        shutil.copy2(source / name, source_copy / name)
    juce_archive = source_copy / 'ThirdParty/JUCE-7.0.12.zip'
    subprocess.check_call(['git', '-C', str(juce), 'archive', '--format=zip', '--prefix=JUCE-7.0.12/', '--output', str(juce_archive), 'HEAD'])
    info = {
        'product': 'NoteShaper', 'version': version,
        'source_commit': run('git', '-C', str(source), 'rev-parse', 'HEAD'),
        'juce_commit': run('git', '-C', str(juce), 'rev-parse', 'HEAD'),
        'juce_patches': ['Build/patch_juce_macos.py'],
        'architectures': ['x86_64', 'arm64'], 'minimum_macos': '11.0',
        'build_macos': platform.mac_ver()[0], 'xcode': run('xcodebuild', '-version'),
        'signing': 'ad-hoc', 'notarized': False,
        'validation': 'Universal bundle/signature checks; release upload requires native Intel and Apple Silicon DSP/VST3 host checks.'
    }
    (package / 'BUILD_INFO.json').write_text(json.dumps(info, indent=2) + '\n', encoding='utf-8')
    hashes = {p.relative_to(package).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
              for p in sorted(package.rglob('*')) if p.is_file()}
    (package / 'MANIFEST.sha256').write_text(''.join(f'{digest}  {name}\n' for name, digest in hashes.items()), encoding='utf-8')
    archive = output / f'NoteShaper-{version}-macos-universal.zip'
    subprocess.check_call(['ditto', '-c', '-k', '--sequesterRsrc', '--keepParent', str(package), str(archive)])
    checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
    (output / f'NoteShaper-{version}-macos-SHA256.txt').write_text(f'{checksum}  {archive.name}\n', encoding='utf-8')
    print(f'Created {archive.name}: universal x86_64/arm64, ad-hoc signature, SHA-256 {checksum}')


if __name__ == '__main__':
    main()
