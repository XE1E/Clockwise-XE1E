#!/usr/bin/env python3
"""
Convertidor de fuentes TTF a formato Clockwise

Uso:
    python convert_ttf.py "MiFuente.ttf" --size 12
    python convert_ttf.py "MiFuente.ttf" --size 12 --output ../fonts/
    python convert_ttf.py "MiFuente.ttf" --size 12 --js --name mifuente

Requisitos:
    pip install pillow fonttools
"""

import argparse
import json
import os
import sys

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("Error: Instala Pillow con: pip install pillow")
    sys.exit(1)


def convert_ttf_to_json(ttf_path, size, output_dir, name=None):
    """Convierte TTF a formato JSON para char-designer"""

    font = ImageFont.truetype(ttf_path, size)

    if not name:
        name = os.path.splitext(os.path.basename(ttf_path))[0].lower().replace(' ', '-')

    # Caracteres ASCII imprimibles
    chars = ''.join(chr(i) for i in range(32, 127))

    # Encontrar dimensiones maximas
    max_w, max_h = 0, 0
    for c in chars:
        bbox = font.getbbox(c)
        if bbox:
            w = bbox[2] - bbox[0]
            h = bbox[3] - bbox[1]
            max_w = max(max_w, w)
            max_h = max(max_h, h)

    print(f"Fuente: {name}")
    print(f"Tamaño: {max_w}x{max_h} pixels")

    # Crear estructura JSON
    output = {
        "version": 1,
        "name": name,
        "width": max_w,
        "height": max_h,
        "chars": {}
    }

    # Renderizar cada caracter
    for c in chars:
        img = Image.new('L', (max_w, max_h), 0)
        draw = ImageDraw.Draw(img)

        bbox = font.getbbox(c)
        if not bbox:
            continue

        char_w = bbox[2] - bbox[0]
        char_h = bbox[3] - bbox[1]
        x = (max_w - char_w) // 2 - bbox[0]
        y = (max_h - char_h) // 2 - bbox[1]

        draw.text((x, y), c, font=font, fill=255)

        grid = []
        for row in range(max_h):
            row_data = []
            for col in range(max_w):
                pixel = img.getpixel((col, row))
                row_data.append(1 if pixel > 127 else 0)
            grid.append(row_data)

        output["chars"][ord(c)] = {"char": c, "data": grid}

    # Guardar JSON
    json_path = os.path.join(output_dir, f"{name}.json")
    with open(json_path, 'w') as f:
        json.dump(output, f, indent=2)

    print(f"Guardado: {json_path}")
    print(f"Caracteres: {len(output['chars'])}")

    return json_path


def convert_ttf_to_js(ttf_path, size, name=None):
    """Convierte TTF a formato JS para pixel-fonts.js"""

    font = ImageFont.truetype(ttf_path, size)

    if not name:
        name = os.path.splitext(os.path.basename(ttf_path))[0]
        name = ''.join(c if c.isalnum() else '' for c in name)
        name = name[0].lower() + name[1:] if name else 'customFont'

    first_char = 32
    last_char = 126

    # Encontrar dimensiones maximas
    max_w, max_h = 0, 0
    for code in range(first_char, last_char + 1):
        c = chr(code)
        bbox = font.getbbox(c)
        if bbox:
            w = bbox[2] - bbox[0]
            h = bbox[3] - bbox[1]
            max_w = max(max_w, w)
            max_h = max(max_h, h)

    print(f"Fuente: {name}")
    print(f"Tamaño: {max_w}x{max_h} pixels")

    # Construir bitmaps y glyphs
    bitmaps = []
    glyphs = []

    for code in range(first_char, last_char + 1):
        c = chr(code)

        img = Image.new('L', (max_w * 2, max_h * 2), 0)
        draw = ImageDraw.Draw(img)
        draw.text((0, 0), c, font=font, fill=255)

        bbox = font.getbbox(c)
        if not bbox or bbox[2] - bbox[0] == 0:
            glyphs.append([len(bitmaps), 0, 0, max_w, 0, 0])
            continue

        # Encontrar limites de pixels
        min_x, min_y, max_x_pix, max_y_pix = max_w * 2, max_h * 2, 0, 0
        for y in range(max_h * 2):
            for x in range(max_w * 2):
                if img.getpixel((x, y)) > 127:
                    min_x = min(min_x, x)
                    min_y = min(min_y, y)
                    max_x_pix = max(max_x_pix, x)
                    max_y_pix = max(max_y_pix, y)

        if max_x_pix < min_x:
            glyphs.append([len(bitmaps), 0, 0, max_w, 0, 0])
            continue

        glyph_w = max_x_pix - min_x + 1
        glyph_h = max_y_pix - min_y + 1
        offset = len(bitmaps)

        # Empaquetar bits
        cur_byte = 0
        bit_count = 0
        for y in range(min_y, max_y_pix + 1):
            for x in range(min_x, max_x_pix + 1):
                pixel = 1 if img.getpixel((x, y)) > 127 else 0
                cur_byte = (cur_byte << 1) | pixel
                bit_count += 1
                if bit_count == 8:
                    bitmaps.append(cur_byte)
                    cur_byte = 0
                    bit_count = 0

        if bit_count > 0:
            bitmaps.append(cur_byte << (8 - bit_count))

        glyphs.append([offset, glyph_w, glyph_h, glyph_w + 1, min_x, min_y - max_h])

    # Generar codigo JS
    js = f'    {name}: {{\n'
    js += f'        bitmaps: [{",".join("0x" + format(b, "02X") for b in bitmaps)}],\n'
    js += f'        glyphs: [\n'
    for i, g in enumerate(glyphs):
        js += f'            [{",".join(str(x) for x in g)}]{"," if i < len(glyphs)-1 else ""}\n'
    js += f'        ],\n'
    js += f'        first: 0x{first_char:02X}, last: 0x{last_char:02X}, yAdvance: {max_h}\n'
    js += f'    }},'

    print(f"\nCodigo JS generado ({len(bitmaps)} bytes, {len(glyphs)} glyphs)")
    print("\nPegar en pixel-fonts.js antes de 'drawChar':\n")
    print(js)

    return js


def main():
    parser = argparse.ArgumentParser(description='Convertir fuente TTF a formato Clockwise')
    parser.add_argument('ttf', help='Archivo TTF a convertir')
    parser.add_argument('--size', type=int, default=12, help='Tamaño en pixels (default: 12)')
    parser.add_argument('--name', help='Nombre de la fuente (default: nombre del archivo)')
    parser.add_argument('--output', default='.', help='Carpeta de salida (default: actual)')
    parser.add_argument('--js', action='store_true', help='Generar codigo JS en lugar de JSON')

    args = parser.parse_args()

    if not os.path.exists(args.ttf):
        print(f"Error: No se encuentra {args.ttf}")
        sys.exit(1)

    print(f"Convirtiendo: {args.ttf}")
    print(f"Tamaño: {args.size}px\n")

    if args.js:
        convert_ttf_to_js(args.ttf, args.size, args.name)
    else:
        convert_ttf_to_json(args.ttf, args.size, args.output, args.name)


if __name__ == '__main__':
    main()
