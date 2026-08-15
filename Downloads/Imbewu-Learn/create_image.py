from PIL import Image, ImageDraw, ImageFont
import os

workspace = r'c:\Users\samke\Downloads\Imbewu-Learn'

# Create a beautiful image with warm, earthy tones
width, height = 1200, 800
img = Image.new('RGB', (width, height), color=(242, 220, 190))  # Warm cream background

draw = ImageDraw.Draw(img)

# Create a warm gradient effect
for y in range(height):
    ratio = y / height
    r = int(242 - (ratio * 40))
    g = int(220 - (ratio * 50))
    b = int(190 - (ratio * 60))
    draw.line([(0, y), (width, y)], fill=(r, g, b))

# Add warm orange/brown accent areas
draw.ellipse([100, 50, 400, 350], fill=(210, 140, 80))  # Left circle
draw.ellipse([800, 450, 1100, 750], fill=(220, 150, 100))  # Right circle

# Add border
draw.rectangle([30, 30, width-30, height-30], outline=(139, 69, 19), width=5)

# Add text
try:
    title_font = ImageFont.truetype('arial.ttf', 72)
    subtitle_font = ImageFont.truetype('arial.ttf', 36)
except:
    title_font = ImageFont.load_default()
    subtitle_font = ImageFont.load_default()

# Add centered text
title = "Learning Together"
draw.text((width//2 - 200, height//2 - 50), title, fill=(101, 67, 33), font=title_font)

subtitle = "Knowledge grows in community"
draw.text((width//2 - 150, height//2 + 50), subtitle, fill=(139, 100, 60), font=subtitle_font)

# Save with high quality
image_path = os.path.join(workspace, 'learning-together.jpg')
img.save(image_path, 'JPEG', quality=95)
print(f"Image created successfully: {image_path}")
print(f"Size: {width}x{height} pixels")
