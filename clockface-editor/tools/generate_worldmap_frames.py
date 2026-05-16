#!/usr/bin/env python3
"""
Generate 24 world map frames for hour-based sprite.
Each frame is shifted by ~5 pixels to represent the sun's position.
"""

import os
import sys
import json
import base64
from io import BytesIO

try:
    from PIL import Image
except ImportError:
    print("Installing Pillow...")
    os.system(f"{sys.executable} -m pip install Pillow")
    from PIL import Image

def generate_worldmap_frames(input_image_path, output_dir, output_width=64, output_height=32):
    """
    Generate 24 frames from a world map image.

    The map is assumed to be wider than the output (e.g., 120px wide).
    Each hour shifts the view by map_width/24 pixels.
    """
    # Load the image
    img = Image.open(input_image_path)

    # Convert to RGBA if needed
    if img.mode != 'RGBA':
        img = img.convert('RGBA')

    map_width, map_height = img.size
    print(f"Input image: {map_width}x{map_height}")

    # Calculate shift per hour (map wraps around)
    shift_per_hour = map_width / 24
    print(f"Shift per hour: {shift_per_hour:.2f} pixels")

    # Create a wider image for seamless wrapping (duplicate the map)
    wide_img = Image.new('RGBA', (map_width * 2, map_height))
    wide_img.paste(img, (0, 0))
    wide_img.paste(img, (map_width, 0))

    frames = []
    os.makedirs(output_dir, exist_ok=True)

    for hour in range(24):
        # Calculate the x offset for this hour
        # Hour 0 = UTC midnight, sun is at longitude 180 (anti-meridian)
        # We want the map to show the current "day" position
        x_offset = int(hour * shift_per_hour)

        # Crop the frame from the wide image
        left = x_offset
        top = 0
        right = left + output_width
        bottom = min(output_height, map_height)

        frame = wide_img.crop((left, top, right, bottom))

        # If the map is shorter than output_height, create a new image with black background
        if map_height < output_height:
            final_frame = Image.new('RGBA', (output_width, output_height), (0, 0, 0, 255))
            # Center vertically
            y_offset = (output_height - map_height) // 2
            final_frame.paste(frame, (0, y_offset))
            frame = final_frame

        # Save as PNG
        frame_path = os.path.join(output_dir, f"worldmap_frame_{hour:02d}.png")
        frame.save(frame_path, 'PNG')

        # Convert to base64 for JSON
        buffer = BytesIO()
        frame.save(buffer, format='PNG')
        b64 = base64.b64encode(buffer.getvalue()).decode('utf-8')
        frames.append({"image": b64})

        print(f"Frame {hour:02d}: offset={x_offset}px")

    # Create the sprite JSON structure
    sprite_data = {
        "frames": frames
    }

    # Save sprite JSON
    json_path = os.path.join(output_dir, "worldmap_sprite.json")
    with open(json_path, 'w') as f:
        json.dump(sprite_data, f, indent=2)

    print(f"\nGenerated {len(frames)} frames")
    print(f"Sprite JSON saved to: {json_path}")

    # Also create a complete clockface template
    # Sprite must be in "loop" for hour-based handling to work
    clockface = {
        "name": "World Clock",
        "version": 1,
        "author": "XE1E",
        "delay": 1000,
        "bgColor": 0,
        "sprites": [sprite_data["frames"]],
        "setup": [],
        "loop": [
            {
                "type": "sprite",
                "x": 0,
                "y": 0,
                "sprite": 0,
                "hourBased": True
            },
            {
                "type": "line",
                "x": 32,
                "y": 0,
                "x1": 32,
                "y1": output_height - 1,
                "color": 32768
            },
            {
                "type": "datetime",
                "x": 2,
                "y": output_height + 6,
                "content": "H:i",
                "font": "picopixel",
                "fgColor": 65535,
                "bgColor": 0
            }
        ]
    }

    clockface_path = os.path.join(output_dir, "world-clock.json")
    with open(clockface_path, 'w') as f:
        json.dump(clockface, f, indent=2)

    print(f"Clockface template saved to: {clockface_path}")

    return frames

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python generate_worldmap_frames.py <worldmap.png> [output_dir] [width] [height]")
        print("\nThis script generates 24 frames from a world map image,")
        print("suitable for the hour-based sprite feature.")
        print("\nDefaults: output_dir=worldmap_frames, width=64, height=32")
        sys.exit(1)

    input_path = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else "worldmap_frames"
    output_width = int(sys.argv[3]) if len(sys.argv) > 3 else 64
    output_height = int(sys.argv[4]) if len(sys.argv) > 4 else 32

    if not os.path.exists(input_path):
        print(f"Error: Input file not found: {input_path}")
        sys.exit(1)

    generate_worldmap_frames(input_path, output_dir, output_width, output_height)
