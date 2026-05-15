#!/usr/bin/env python3
"""
Converts old Clockwise JSON font format (chars/data) to new format (bitmaps/glyphs).

Old format:
{
    "name": "fontName",
    "width": 16,
    "height": 17,
    "chars": {
        "48": {"char": "0", "data": [[0,1,0,...], ...]}
    }
}

New format:
{
    "name": "fontName",
    "bitmaps": [byte array],
    "glyphs": [[offset, width, height, xAdvance, xOffset, yOffset], ...],
    "first": 32,
    "last": 126,
    "yAdvance": height
}
"""

import json
import sys
import os


def convert_old_to_new(input_path, output_path=None):
    with open(input_path, 'r') as f:
        old = json.load(f)

    font_name = old.get('name', 'unknown')
    font_width = old.get('width', 8)
    font_height = old.get('height', 16)
    chars = old.get('chars', {})

    if not chars:
        print(f"Error: No characters found in {input_path}")
        return None

    # Find first and last character codes
    codes = sorted([int(c) for c in chars.keys()])
    first_char = codes[0]
    last_char = codes[-1]

    bitmaps = []
    glyphs = []

    for code in range(first_char, last_char + 1):
        str_code = str(code)
        if str_code in chars:
            char_data = chars[str_code]
            data = char_data.get('data', [])

            if not data:
                glyphs.append([len(bitmaps), 0, 0, font_width, 0, 0])
                continue

            height = len(data)
            width = len(data[0]) if data else 0

            byte_offset = len(bitmaps)

            # Pack bits into bytes
            current_byte = 0
            bit_count = 0

            for row in data:
                for bit in row:
                    current_byte = (current_byte << 1) | (1 if bit else 0)
                    bit_count += 1

                    if bit_count == 8:
                        bitmaps.append(current_byte)
                        current_byte = 0
                        bit_count = 0

            # Pad remaining bits
            if bit_count > 0:
                current_byte = current_byte << (8 - bit_count)
                bitmaps.append(current_byte)

            # Glyph: [offset, width, height, xAdvance, xOffset, yOffset]
            glyphs.append([byte_offset, width, height, font_width, 0, -font_height + 1])
        else:
            glyphs.append([len(bitmaps), 0, 0, font_width, 0, 0])

    output = {
        'name': font_name,
        'bitmaps': bitmaps,
        'glyphs': glyphs,
        'first': first_char,
        'last': last_char,
        'yAdvance': font_height
    }

    output_file = output_path or input_path
    with open(output_file, 'w') as f:
        json.dump(output, f)

    print(f'Converted: {output_file}')
    print(f'  Characters: {first_char}-{last_char} ({last_char - first_char + 1} glyphs)')
    print(f'  Bitmaps: {len(bitmaps)} bytes')

    return output_file


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python convert_old_json.py input.json [output.json]")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) > 2 else input_path

    convert_old_to_new(input_path, output_path)
