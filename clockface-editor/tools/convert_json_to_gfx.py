#!/usr/bin/env python3
"""
Converts Clockwise JSON font format to Adafruit GFX format (.h header file).

Usage:
    python convert_json_to_gfx.py led-display.json --name ledDisplay14x17
    python convert_json_to_gfx.py led-display.json --name ledDisplay14x17 --output ../../firmware/clockfaces/cw-cf-0x07/fonts/
"""

import json
import argparse
import os
from pathlib import Path


def pack_bits(rows):
    """Pack row data into bytes, MSB first."""
    bits = []
    for row in rows:
        bits.extend(row)

    # Pad to byte boundary
    while len(bits) % 8 != 0:
        bits.append(0)

    # Pack into bytes
    packed = []
    for i in range(0, len(bits), 8):
        byte = 0
        for j in range(8):
            if bits[i + j]:
                byte |= (1 << (7 - j))
        packed.append(byte)

    return packed


def convert_json_to_gfx(json_path, font_name, output_dir=None):
    """Convert JSON font to Adafruit GFX format."""

    with open(json_path, 'r') as f:
        font_data = json.load(f)

    font_width = font_data.get('width', 16)
    font_height = font_data.get('height', 17)
    chars = font_data.get('chars', {})

    # Build bitmap and glyph data
    bitmap_data = []
    glyph_data = []

    # Sort characters by ASCII code
    char_codes = sorted([int(c) for c in chars.keys()])
    first_char = min(char_codes)
    last_char = max(char_codes)

    # Process each character
    for code in range(first_char, last_char + 1):
        str_code = str(code)
        if str_code in chars:
            char_info = chars[str_code]
            rows = char_info.get('data', [])

            # Calculate actual bounds (trim empty rows/cols)
            min_x, max_x = font_width, 0
            min_y, max_y = font_height, 0

            for y, row in enumerate(rows):
                for x, pixel in enumerate(row):
                    if pixel:
                        min_x = min(min_x, x)
                        max_x = max(max_x, x)
                        min_y = min(min_y, y)
                        max_y = max(max_y, y)

            if max_x >= min_x and max_y >= min_y:
                # Has visible pixels
                width = max_x - min_x + 1
                height = max_y - min_y + 1

                # Extract trimmed bitmap
                trimmed = []
                for y in range(min_y, max_y + 1):
                    row = []
                    for x in range(min_x, max_x + 1):
                        row.append(rows[y][x] if y < len(rows) and x < len(rows[y]) else 0)
                    trimmed.append(row)

                bitmap_offset = len(bitmap_data)
                packed = pack_bits(trimmed)
                bitmap_data.extend(packed)

                # Glyph: bitmapOffset, width, height, xAdvance, xOffset, yOffset
                x_advance = width + 1  # Character width + spacing
                x_offset = min_x
                y_offset = min_y - font_height  # Negative offset from baseline

                glyph_data.append({
                    'offset': bitmap_offset,
                    'width': width,
                    'height': height,
                    'xAdvance': x_advance,
                    'xOffset': x_offset,
                    'yOffset': y_offset
                })
            else:
                # Empty character (like space)
                glyph_data.append({
                    'offset': len(bitmap_data),
                    'width': 0,
                    'height': 0,
                    'xAdvance': font_width // 3,  # Space width
                    'xOffset': 0,
                    'yOffset': 0
                })
        else:
            # Missing character - use placeholder
            glyph_data.append({
                'offset': len(bitmap_data),
                'width': 0,
                'height': 0,
                'xAdvance': font_width // 2,
                'xOffset': 0,
                'yOffset': 0
            })

    # Generate header file
    header = []
    header.append('#include <Arduino.h>')
    header.append('#include <gfxfont.h>')
    header.append('')

    # Bitmap array
    header.append(f'const uint8_t {font_name}Bitmaps[] PROGMEM = {{')
    for i in range(0, len(bitmap_data), 12):
        chunk = bitmap_data[i:i+12]
        hex_str = ', '.join(f'0x{b:02X}' for b in chunk)
        if i + 12 < len(bitmap_data):
            hex_str += ','
        header.append(f'  {hex_str}')
    header.append('};')
    header.append('')

    # Glyph array
    header.append(f'const GFXglyph {font_name}Glyphs[] PROGMEM = {{')
    for i, glyph in enumerate(glyph_data):
        char_code = first_char + i
        char_display = chr(char_code) if 32 <= char_code < 127 else f'0x{char_code:02X}'
        line = f"  {{ {glyph['offset']:5d}, {glyph['width']:3d}, {glyph['height']:3d}, {glyph['xAdvance']:3d}, {glyph['xOffset']:4d}, {glyph['yOffset']:4d} }}"
        if i < len(glyph_data) - 1:
            line += ','
        line += f"  // '{char_display}'"
        header.append(line)
    header.append('};')
    header.append('')

    # Font struct
    header.append(f'const GFXfont {font_name} PROGMEM = {{')
    header.append(f'  (uint8_t  *){font_name}Bitmaps,')
    header.append(f'  (GFXglyph *){font_name}Glyphs,')
    header.append(f'  {first_char}, {last_char}, {font_height}')
    header.append('};')
    header.append('')

    # Write output
    output_path = output_dir or os.path.dirname(json_path)
    output_file = os.path.join(output_path, f'{font_name}.h')

    with open(output_file, 'w') as f:
        f.write('\n'.join(header))

    print(f'Generated: {output_file}')
    print(f'  Characters: {first_char}-{last_char} ({len(glyph_data)} glyphs)')
    print(f'  Bitmap size: {len(bitmap_data)} bytes')

    return output_file


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert Clockwise JSON font to Adafruit GFX format')
    parser.add_argument('json_file', help='Input JSON font file')
    parser.add_argument('--name', required=True, help='Font name for generated code')
    parser.add_argument('--output', help='Output directory (default: same as input)')

    args = parser.parse_args()

    convert_json_to_gfx(args.json_file, args.name, args.output)
