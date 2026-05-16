#!/usr/bin/env python3
"""
Convert GFX font .h files to JSON format for the clockface editor.
Correctly handles bitmap packing and glyph yOffset values.
"""

import re
import json
import sys
import os

def parse_gfx_font(h_file_path):
    """Parse a GFX font header file and extract font data."""

    with open(h_file_path, 'r') as f:
        content = f.read()

    # Extract font name from filename
    basename = os.path.basename(h_file_path)
    font_name = basename.replace('.h', '').replace('font', '')

    # Extract bitmap array
    bitmap_match = re.search(r'const uint8_t \w+Bitmaps\[\]\s*PROGMEM\s*=\s*\{([^}]+)\}', content, re.DOTALL)
    if not bitmap_match:
        raise ValueError("Could not find bitmap array")

    bitmap_str = bitmap_match.group(1)
    # Parse hex values like 0x00, 0xFF
    bitmaps = []
    for match in re.finditer(r'0x([0-9A-Fa-f]{2})', bitmap_str):
        bitmaps.append(int(match.group(1), 16))

    # Extract glyph array - find everything between GFXglyph declaration and closing };
    glyph_match = re.search(r'const GFXglyph \w+Glyphs\[\]\s*PROGMEM\s*=\s*\{(.*?)\};', content, re.DOTALL)
    if not glyph_match:
        raise ValueError("Could not find glyph array")

    glyph_str = glyph_match.group(1)
    glyphs = []
    # Parse glyph entries like {     0,   4,   6,   4,    0,   -5 }
    for match in re.finditer(r'\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*([-]?\d+)\s*,\s*([-]?\d+)\s*\}', glyph_str):
        offset = int(match.group(1))
        width = int(match.group(2))
        height = int(match.group(3))
        xAdvance = int(match.group(4))
        xOffset = int(match.group(5))
        yOffset = int(match.group(6))
        glyphs.append([offset, width, height, xAdvance, xOffset, yOffset])

    # Extract font metadata (first, last, yAdvance)
    font_match = re.search(r'const GFXfont \w+\s*PROGMEM\s*=\s*\{[^,]+,\s*[^,]+,\s*0x([0-9A-Fa-f]+)\s*,\s*0x([0-9A-Fa-f]+)\s*,\s*(\d+)\s*\}', content)
    if not font_match:
        raise ValueError("Could not find font metadata")

    first = int(font_match.group(1), 16)
    last = int(font_match.group(2), 16)
    yAdvance = int(font_match.group(3))

    return {
        'name': font_name,
        'bitmaps': bitmaps,
        'glyphs': glyphs,
        'first': first,
        'last': last,
        'yAdvance': yAdvance
    }

def main():
    if len(sys.argv) < 2:
        print("Usage: python convert_gfx_font.py <font.h> [output.json]")
        print("\nConverts GFX font header files to JSON format for the editor.")
        sys.exit(1)

    h_file = sys.argv[1]

    if len(sys.argv) > 2:
        output_file = sys.argv[2]
    else:
        output_file = os.path.splitext(os.path.basename(h_file))[0].replace('font', '') + '.json'

    print(f"Converting {h_file}...")

    try:
        font_data = parse_gfx_font(h_file)

        with open(output_file, 'w') as f:
            json.dump(font_data, f, separators=(',', ':'))

        print(f"Saved to {output_file}")
        print(f"  Name: {font_data['name']}")
        print(f"  Glyphs: {len(font_data['glyphs'])} (0x{font_data['first']:02X}-0x{font_data['last']:02X})")
        print(f"  yAdvance: {font_data['yAdvance']}")
        print(f"  Bitmap size: {len(font_data['bitmaps'])} bytes")

        # Show sample glyph info
        if font_data['glyphs']:
            g = font_data['glyphs'][0]
            print(f"  First glyph: offset={g[0]}, size={g[1]}x{g[2]}, yOffset={g[5]}")

    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
