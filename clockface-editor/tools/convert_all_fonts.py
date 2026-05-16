#!/usr/bin/env python3
"""
Convert all GFX fonts from firmware to JSON format for the editor.
Maps firmware font names to editor font names.
"""

import os
import subprocess
import sys

# Mapping: firmware .h file -> editor JSON name
FONT_MAPPING = {
    # Standard fonts (fontXxY.h -> XxY.json)
    'font4x6.h': '4x6',
    'font5x7.h': '5x7',
    'font5x8.h': '5x8',
    'font6x9.h': '6x9',
    'font6x10.h': '6x10',
    'font6x12.h': '6x12',
    'font6x13.h': '6x13',
    'font6x13B.h': '6x13B',
    'font7x13.h': '7x13',
    'font7x13B.h': '7x13B',
    'font7x14.h': '7x14',
    'font7x14B.h': '7x14B',
    'font8x13.h': '8x13',
    'font8x13B.h': '8x13B',

    # Spleen fonts
    'spleen5x8.h': 'spleen-5x8',
    'spleen8x16.h': 'spleen-8x16',

    # Other fonts
    'creep.h': 'creep',
    'scientifica11.h': 'scientifica',
    'scientificaBold11.h': 'scientifica-bold',
    'haxorMedium10.h': 'haxor-10',
    'haxorMedium11.h': 'haxor-11',
    'haxorMedium12.h': 'haxor-12',
    'haxorMedium13.h': 'haxor-13',
    'clR6x12.h': 'clR6x12',
    'helvR12.h': 'helvetica',
    'knxt.h': 'knxt',
    'ledDisplay.h': 'led-display',

    # Skip these (internal use only, not in editor)
    # 'TomThumb.h': 'tomthumb',  # Already built into pixel-fonts.js as picopixel alternative
    # 'atari.h': 'square',  # Built-in
    # 'hour8pt7b.h': 'big',  # Built-in
    # 'minute7pt7b.h': 'medium',  # Built-in
    # 'FreeSansBold9pt7b.h': 'bold',  # Built-in
    # 'nocturno.h': 'nocturno',  # Already in pixel-fonts.js
}

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    firmware_fonts_dir = os.path.join(script_dir, '../../firmware/clockfaces/cw-cf-0x07/fonts')
    output_dir = os.path.join(script_dir, '../fonts')
    convert_script = os.path.join(script_dir, 'convert_gfx_font.py')

    converted = []
    failed = []

    for h_file, json_name in FONT_MAPPING.items():
        h_path = os.path.join(firmware_fonts_dir, h_file)
        json_path = os.path.join(output_dir, f'{json_name}.json')

        if not os.path.exists(h_path):
            print(f"[SKIP] {h_file} not found")
            continue

        print(f"Converting {h_file} -> {json_name}.json")

        result = subprocess.run(
            ['python', convert_script, h_path, json_path],
            capture_output=True,
            text=True
        )

        if result.returncode == 0:
            converted.append(json_name)
        else:
            failed.append((h_file, result.stderr))
            print(f"  [ERROR] {result.stderr}")

    print(f"\nConverted: {len(converted)} fonts")
    if failed:
        print(f"Failed: {len(failed)} fonts")
        for h_file, error in failed:
            print(f"  - {h_file}: {error}")

if __name__ == "__main__":
    main()
