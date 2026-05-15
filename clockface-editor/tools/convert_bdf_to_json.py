#!/usr/bin/env python3
"""
Converts BDF font files to Clockwise editor JSON format.

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
    else:
        font_width, font_height = 8, 16

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
                try:
                    value = int(line, 16)
                    bits_needed = ((width + 7) // 8) * 8
                    binary = format(value, f'0{bits_needed}b')
                    row = [int(b) for b in binary[:font_width]]
                    # Pad to font_width if needed
                    while len(row) < font_width:
                        row.append(0)
                    bitmap_rows.append(row)
                except ValueError:
                    continue

        # Pad height if needed
        while len(bitmap_rows) < font_height:
            bitmap_rows.append([0] * font_width)

        if bitmap_rows:
            chars[str(encoding)] = {
                'char': chr(encoding),
                'data': bitmap_rows
            }

    return {
        'width': font_width,
        'height': font_height,
        'chars': chars
    }


def convert_bdf_to_json(bdf_path, font_name, output_dir=None):
    """Convert BDF font to Clockwise editor JSON format."""

    font_data = parse_bdf(bdf_path)

    if not font_data['chars']:
        print(f"Error: No characters found in {bdf_path}")
        return None

    output = {
        'version': 1,
        'name': font_name,
        'width': font_data['width'],
        'height': font_data['height'],
        'chars': font_data['chars']
    }

    # Write output
    output_path = output_dir or os.path.dirname(bdf_path)
    output_file = os.path.join(output_path, f'{font_name}.json')

    with open(output_file, 'w') as f:
        json.dump(output, f)

    print(f'Generated: {output_file}')
    print(f'  Size: {font_data["width"]}x{font_data["height"]}')
    print(f'  Characters: {len(font_data["chars"])}')

    return output_file


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert BDF font to Clockwise editor JSON format')
    parser.add_argument('bdf_file', help='Input BDF font file')
    parser.add_argument('--name', required=True, help='Font name')
    parser.add_argument('--output', help='Output directory (default: same as input)')

    args = parser.parse_args()

    convert_bdf_to_json(args.bdf_file, args.name, args.output)
