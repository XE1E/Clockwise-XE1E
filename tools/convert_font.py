#!/usr/bin/env python3
"""Convert mi-fuente JSON digit files to Adafruit GFX font format."""

import json
import os

def load_digit(path):
    """Load a digit JSON file and return its data."""
    with open(path, 'r') as f:
        data = json.load(f)
    char_code = list(data['chars'].keys())[0]
    char_data = data['chars'][char_code]['data']
    return {
        'code': int(char_code),
        'width': data['width'],
        'height': data['height'],
        'bitmap': char_data
    }

def bitmap_to_bytes(bitmap):
    """Convert 2D bitmap array to packed bytes (MSB first)."""
    bytes_list = []
    bit_buffer = 0
    bit_count = 0

    for row in bitmap:
        for pixel in row:
            bit_buffer = (bit_buffer << 1) | (1 if pixel else 0)
            bit_count += 1
            if bit_count == 8:
                bytes_list.append(bit_buffer)
                bit_buffer = 0
                bit_count = 0

    # Pad remaining bits
    if bit_count > 0:
        bit_buffer <<= (8 - bit_count)
        bytes_list.append(bit_buffer)

    return bytes_list

def generate_gfx_font(digits, font_name="NightDigits"):
    """Generate Adafruit GFX font header file content."""

    # Sort digits by char code
    digits = sorted(digits, key=lambda d: d['code'])

    # Calculate bitmap bytes for each digit
    all_bytes = []
    glyphs = []

    for digit in digits:
        offset = len(all_bytes)
        digit_bytes = bitmap_to_bytes(digit['bitmap'])
        all_bytes.extend(digit_bytes)

        glyphs.append({
            'offset': offset,
            'width': digit['width'],
            'height': digit['height'],
            'xAdvance': digit['width'] + 2,  # Add 2px spacing
            'xOffset': 0,
            'yOffset': -digit['height'] + 1  # Baseline adjustment
        })

    # Generate header file
    lines = []
    lines.append(f"// {font_name} - Custom night clock digits")
    lines.append("// Auto-generated from mi-fuente JSON files")
    lines.append("")
    lines.append("#pragma once")
    lines.append("#include <Adafruit_GFX.h>")
    lines.append("")

    # Bitmap array
    lines.append(f"const uint8_t {font_name}Bitmaps[] PROGMEM = {{")
    for i in range(0, len(all_bytes), 12):
        chunk = all_bytes[i:i+12]
        hex_vals = ", ".join(f"0x{b:02X}" for b in chunk)
        lines.append(f"  {hex_vals},")
    lines.append("};")
    lines.append("")

    # Glyph array
    lines.append(f"const GFXglyph {font_name}Glyphs[] PROGMEM = {{")
    lines.append("  // offset, width, height, xAdvance, xOffset, yOffset")
    for i, glyph in enumerate(glyphs):
        char_code = digits[i]['code']
        lines.append(f"  {{ {glyph['offset']:5}, {glyph['width']:3}, {glyph['height']:3}, "
                    f"{glyph['xAdvance']:3}, {glyph['xOffset']:3}, {glyph['yOffset']:4} }}, // '{chr(char_code)}'")
    lines.append("};")
    lines.append("")

    # Font structure
    first_char = digits[0]['code']
    last_char = digits[-1]['code']
    max_height = max(d['height'] for d in digits)

    lines.append(f"const GFXfont {font_name} PROGMEM = {{")
    lines.append(f"  (uint8_t *){font_name}Bitmaps,")
    lines.append(f"  (GFXglyph *){font_name}Glyphs,")
    lines.append(f"  0x{first_char:02X}, 0x{last_char:02X}, {max_height}  // first, last, yAdvance")
    lines.append("};")

    return "\n".join(lines)

def main():
    fonts_dir = os.path.join(os.path.dirname(__file__), "..", "clockface-editor", "fonts")

    # Load all digits
    digits = []
    for i in range(10):
        path = os.path.join(fonts_dir, f"mi-fuente-{i}.json")
        if os.path.exists(path):
            digit = load_digit(path)
            digits.append(digit)
            print(f"Loaded digit {i}: {digit['width']}x{digit['height']}")
        else:
            print(f"Warning: mi-fuente-{i}.json not found")

    if len(digits) != 10:
        print("Error: Need all 10 digits (0-9)")
        return

    # Generate font
    font_content = generate_gfx_font(digits)

    # Write to firmware
    output_path = os.path.join(os.path.dirname(__file__), "..", "firmware", "clockfaces", "cw-cf-0x07", "NightDigits.h")
    with open(output_path, 'w') as f:
        f.write(font_content)

    print(f"\nFont generated: {output_path}")

if __name__ == "__main__":
    main()
