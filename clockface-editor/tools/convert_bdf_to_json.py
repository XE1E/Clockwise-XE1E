#!/usr/bin/env python3
"""
Converts BDF font files to Clockwise editor JSON format.

Output format matches pixel-fonts.js structure:
{
    "name": "fontName",
    "bitmaps": [byte array of packed bits],
    "glyphs": [[offset, width, height, xAdvance, xOffset, yOffset], ...],
    "first": 32,
    "last": 126,
    "yAdvance": height
}

Usage:
    python convert_bdf_to_json.py font.bdf --name fontName
    python convert_bdf_to_json.py font.bdf --name fontName --output ../fonts/
"""

import argparse
import json
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
        enc_match = re.search(r'ENCODING\s+(-?\d+)', block)
        if not enc_match:
            continue
        encoding = int(enc_match.group(1))

        # Skip non-printable or very high characters
        if encoding < 32 or encoding > 126:
            continue

        # Get device width (for advance)
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
        bits = []

        for line in bitmap_lines:
            line = line.strip()
            if line:
                try:
                    # Convert hex to integer
                    value = int(line, 16)
                    # Get bits for the row width
                    hex_digits = len(line)
                    total_bits = hex_digits * 4
                    binary = format(value, f'0{total_bits}b')
                    # Take only the width we need
                    row_bits = [int(b) for b in binary[:width]]
                    bits.extend(row_bits)
                except ValueError:
                    continue

        if bits:
            # Calculate y_offset relative to baseline (convert BDF convention)
            # BDF: y_offset is from bottom, we need from top
            gfx_y_offset = -(height + y_offset - font_y_offset - font_height)

            chars[encoding] = {
                'width': width,
                'height': height,
                'xAdvance': x_advance,
                'xOffset': x_offset,
                'yOffset': gfx_y_offset,
                'bits': bits
            }

    return {
        'fontWidth': font_width,
        'fontHeight': font_height,
        'chars': chars
    }


def convert_to_editor_format(font_data, font_name):
    """Convert parsed BDF data to editor JSON format."""
    chars = font_data['chars']

    if not chars:
        return None

    # Find first and last character codes
    codes = sorted(chars.keys())
    first_char = codes[0]
    last_char = codes[-1]

    bitmaps = []
    glyphs = []
    bit_offset = 0

    # Process each character from first to last (fill gaps with empty glyphs)
    for code in range(first_char, last_char + 1):
        if code in chars:
            char_data = chars[code]
            width = char_data['width']
            height = char_data['height']
            x_advance = char_data['xAdvance']
            x_offset = char_data['xOffset']
            y_offset = char_data['yOffset']
            bits = char_data['bits']

            # Calculate byte offset
            byte_offset = len(bitmaps)

            # Pack bits into bytes
            current_byte = 0
            bit_count = 0

            for bit in bits:
                current_byte = (current_byte << 1) | bit
                bit_count += 1

                if bit_count == 8:
                    bitmaps.append(current_byte)
                    current_byte = 0
                    bit_count = 0

            # Pad remaining bits if any
            if bit_count > 0:
                current_byte = current_byte << (8 - bit_count)
                bitmaps.append(current_byte)

            # Add glyph: [offset, width, height, xAdvance, xOffset, yOffset]
            glyphs.append([byte_offset, width, height, x_advance, x_offset, y_offset])
        else:
            # Empty glyph for missing character
            glyphs.append([len(bitmaps), 0, 0, font_data['fontWidth'], 0, 0])

    return {
        'name': font_name,
        'bitmaps': bitmaps,
        'glyphs': glyphs,
        'first': first_char,
        'last': last_char,
        'yAdvance': font_data['fontHeight']
    }


def convert_bdf_to_json(bdf_path, font_name, output_dir=None):
    """Convert BDF font to Clockwise editor JSON format."""
    font_data = parse_bdf(bdf_path)

    if not font_data['chars']:
        print(f"Error: No characters found in {bdf_path}")
        return None

    output = convert_to_editor_format(font_data, font_name)

    if not output:
        print(f"Error: Failed to convert {bdf_path}")
        return None

    # Write output
    output_path = output_dir or os.path.dirname(bdf_path)
    output_file = os.path.join(output_path, f'{font_name}.json')

    with open(output_file, 'w') as f:
        json.dump(output, f)

    print(f'Generated: {output_file}')
    print(f'  Characters: {output["first"]}-{output["last"]} ({output["last"] - output["first"] + 1} glyphs)')
    print(f'  Bitmaps: {len(output["bitmaps"])} bytes')
    print(f'  yAdvance: {output["yAdvance"]}')

    return output_file


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert BDF font to Clockwise editor JSON format')
    parser.add_argument('bdf_file', help='Input BDF font file')
    parser.add_argument('--name', required=True, help='Font name')
    parser.add_argument('--output', help='Output directory (default: same as input)')

    args = parser.parse_args()

    convert_bdf_to_json(args.bdf_file, args.name, args.output)
