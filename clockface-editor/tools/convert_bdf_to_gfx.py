#!/usr/bin/env python3
"""
Converts BDF font files to Adafruit GFX format (.h header file).

Usage:
    python convert_bdf_to_gfx.py font.bdf --name fontName
    python convert_bdf_to_gfx.py font.bdf --name fontName --output ../../firmware/clockfaces/cw-cf-0x07/fonts/
"""

import argparse
import os
import re


def parse_bdf(bdf_path):
    """Parse a BDF font file and return font data."""
    with open(bdf_path, 'r', encoding='latin-1') as f:
        content = f.read()

    # Get font bounding box
    bbox_match = re.search(r'FONTBOUNDINGBOX\s+(\d+)\s+(\d+)\s+(-?\d+)\s+(-?\d+)', content)
    if bbox_match:
        font_width = int(bbox_match.group(1))
        font_height = int(bbox_match.group(2))
        font_x_offset = int(bbox_match.group(3))
        font_y_offset = int(bbox_match.group(4))
    else:
        font_width, font_height = 8, 16
        font_x_offset, font_y_offset = 0, 0

    # Parse characters
    chars = {}
    char_blocks = re.split(r'STARTCHAR\s+', content)[1:]

    for block in char_blocks:
        # Get encoding (ASCII code)
        enc_match = re.search(r'ENCODING\s+(\d+)', block)
        if not enc_match:
            continue
        encoding = int(enc_match.group(1))

        # Skip non-printable or very high characters
        if encoding < 32 or encoding > 126:
            continue

        # Get device width (advance)
        dwidth_match = re.search(r'DWIDTH\s+(\d+)\s+(\d+)', block)
        if dwidth_match:
            x_advance = int(dwidth_match.group(1))
        else:
            x_advance = font_width

        # Get bounding box
        bbx_match = re.search(r'BBX\s+(\d+)\s+(\d+)\s+(-?\d+)\s+(-?\d+)', block)
        if bbx_match:
            width = int(bbx_match.group(1))
            height = int(bbx_match.group(2))
            x_offset = int(bbx_match.group(3))
            y_offset = int(bbx_match.group(4))
        else:
            width, height = font_width, font_height
            x_offset, y_offset = 0, 0

        # Get bitmap data
        bitmap_match = re.search(r'BITMAP\s*\n([\s\S]*?)\nENDCHAR', block)
        if not bitmap_match:
            continue

        bitmap_lines = bitmap_match.group(1).strip().split('\n')
        bitmap_rows = []
        for line in bitmap_lines:
            line = line.strip()
            if line:
                # Convert hex to binary
                try:
                    value = int(line, 16)
                    # Number of bits needed based on width
                    bits_needed = ((width + 7) // 8) * 8
                    binary = format(value, f'0{bits_needed}b')
                    row = [int(b) for b in binary[:width]]
                    bitmap_rows.append(row)
                except ValueError:
                    continue

        if bitmap_rows:
            chars[encoding] = {
                'width': width,
                'height': height,
                'x_offset': x_offset,
                'y_offset': y_offset,
                'x_advance': x_advance,
                'bitmap': bitmap_rows
            }

    return {
        'width': font_width,
        'height': font_height,
        'y_advance': font_height,
        'chars': chars
    }


def pack_bits(rows, width):
    """Pack row data into bytes, MSB first."""
    bits = []
    for row in rows:
        bits.extend(row)
        # Pad row to byte boundary
        padding = (8 - (width % 8)) % 8
        bits.extend([0] * padding)

    # Pack into bytes
    packed = []
    for i in range(0, len(bits), 8):
        byte = 0
        for j in range(8):
            if i + j < len(bits) and bits[i + j]:
                byte |= (1 << (7 - j))
        packed.append(byte)

    return packed


def convert_bdf_to_gfx(bdf_path, font_name, output_dir=None):
    """Convert BDF font to Adafruit GFX format."""

    font_data = parse_bdf(bdf_path)
    chars = font_data['chars']

    if not chars:
        print(f"Error: No characters found in {bdf_path}")
        return None

    # Get character range
    char_codes = sorted(chars.keys())
    first_char = min(char_codes)
    last_char = max(char_codes)

    # Build bitmap and glyph data
    bitmap_data = []
    glyph_data = []

    for code in range(first_char, last_char + 1):
        if code in chars:
            char_info = chars[code]
            width = char_info['width']
            height = char_info['height']
            bitmap = char_info['bitmap']

            bitmap_offset = len(bitmap_data)

            if width > 0 and height > 0 and bitmap:
                packed = pack_bits(bitmap, width)
                bitmap_data.extend(packed)
            else:
                width = 0
                height = 0

            # Calculate y_offset for GFX (from baseline, negative = above)
            # BDF y_offset is from bottom, GFX needs from top baseline
            gfx_y_offset = -(char_info['y_offset'] + height)

            glyph_data.append({
                'offset': bitmap_offset,
                'width': width,
                'height': height,
                'xAdvance': char_info['x_advance'],
                'xOffset': char_info['x_offset'],
                'yOffset': gfx_y_offset
            })
        else:
            # Missing character - placeholder
            glyph_data.append({
                'offset': len(bitmap_data),
                'width': 0,
                'height': 0,
                'xAdvance': font_data['width'] // 2,
                'xOffset': 0,
                'yOffset': 0
            })

    # Generate header file
    header = []
    header.append('#pragma once')
    header.append('#include <Adafruit_GFX.h>')
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
    header.append(f'  0x{first_char:02X}, 0x{last_char:02X}, {font_data["y_advance"]}')
    header.append('};')
    header.append('')

    # Write output
    output_path = output_dir or os.path.dirname(bdf_path)
    output_file = os.path.join(output_path, f'{font_name}.h')

    with open(output_file, 'w') as f:
        f.write('\n'.join(header))

    print(f'Generated: {output_file}')
    print(f'  Characters: {first_char}-{last_char} ({len([g for g in glyph_data if g["width"] > 0])} glyphs)')
    print(f'  Bitmap size: {len(bitmap_data)} bytes')

    return output_file


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert BDF font to Adafruit GFX format')
    parser.add_argument('bdf_file', help='Input BDF font file')
    parser.add_argument('--name', required=True, help='Font name for generated code')
    parser.add_argument('--output', help='Output directory (default: same as input)')

    args = parser.parse_args()

    convert_bdf_to_gfx(args.bdf_file, args.name, args.output)
