#!/usr/bin/env python3
"""
Compress WebUI.h HTML content with gzip and generate WebUI_gz.h
"""
import gzip
import re
import os

def extract_html_from_webui(filepath):
    """Extract HTML content from WebUI.h"""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Find the raw literal content between R"rawliteral( and )rawliteral"
    match = re.search(r'R"rawliteral\((.*?)\)rawliteral"', content, re.DOTALL)
    if match:
        return match.group(1)
    return None

def compress_and_generate_header(html_content, output_path):
    """Compress HTML and generate C header file"""
    # Compress with gzip
    compressed = gzip.compress(html_content.encode('utf-8'), compresslevel=9)

    # Generate C byte array
    hex_array = ', '.join(f'0x{b:02x}' for b in compressed)

    # Wrap at ~100 chars per line for readability
    lines = []
    current_line = []
    current_len = 0
    for byte_str in hex_array.split(', '):
        if current_len + len(byte_str) + 2 > 100:
            lines.append(', '.join(current_line))
            current_line = [byte_str]
            current_len = len(byte_str)
        else:
            current_line.append(byte_str)
            current_len += len(byte_str) + 2
    if current_line:
        lines.append(', '.join(current_line))

    formatted_array = ',\n  '.join(lines)

    header_content = f'''#pragma once

// Auto-generated gzipped WebUI HTML
// Original size: {len(html_content)} bytes
// Compressed size: {len(compressed)} bytes
// Compression ratio: {len(compressed) / len(html_content) * 100:.1f}%

const uint8_t WEB_UI_HTML_GZ[] PROGMEM = {{
  {formatted_array}
}};

const size_t WEB_UI_HTML_GZ_LEN = {len(compressed)};
'''

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(header_content)

    return len(html_content), len(compressed)

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    webui_path = os.path.join(script_dir, '..', 'lib', 'cw-commons', 'WebUI.h')
    output_path = os.path.join(script_dir, '..', 'lib', 'cw-commons', 'WebUI_gz.h')

    print(f"Reading: {webui_path}")
    html = extract_html_from_webui(webui_path)

    if not html:
        print("Error: Could not extract HTML from WebUI.h")
        return 1

    print(f"Compressing {len(html)} bytes...")
    original, compressed = compress_and_generate_header(html, output_path)

    print(f"Generated: {output_path}")
    print(f"Original: {original:,} bytes")
    print(f"Compressed: {compressed:,} bytes")
    print(f"Ratio: {compressed / original * 100:.1f}%")

    return 0

if __name__ == '__main__':
    exit(main())
