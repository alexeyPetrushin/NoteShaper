"""Render the public installation page after the Mac VST3 is verified."""
from pathlib import Path
import sys

version, checksum_file, destination = sys.argv[1:]
checksum = Path(checksum_file).read_text().split()[0]
template = (Path(__file__).parent / 'RELEASE_NOTES.md').read_text(encoding='utf-8')
Path(destination).write_text(template.replace('@VERSION@', version).replace('@MAC_SHA256@', checksum), encoding='utf-8')
