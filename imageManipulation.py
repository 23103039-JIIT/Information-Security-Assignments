from PIL import Image

# Load an image
img = Image.open("sample.jpg")
img = img.convert("RGB")         # Ensure it's in RGB format

# Get width and height
width, height = img.size

print(f"Image Size: {width} x {height}")
print("First few pixel RGB values:")

# Display RGB values of a few pixels
for y in range(5):               # Top 5 rows
    for x in range(5):           # Left 5 columns
        r, g, b = img.getpixel((x, y))
        print(f"Pixel ({x},{y}): R={r}, G={g}, B={b}")
    print()

# ----- Bit Manipulation -----
# Example: Flip the least significant bit (LSB) of each color channel
def flip_lsb(value):
    return value ^ 1  # XOR with 1 flips the last bit

# Create a modified image
modified = Image.new("RGB", (width, height))

for y in range(height):
    for x in range(width):
        r, g, b = img.getpixel((x, y))
        r2, g2, b2 = flip_lsb(r), flip_lsb(g), flip_lsb(b)
        modified.putpixel((x, y), (r2, g2, b2))

# Save and show results
modified.save("modified.jpg")
print("\nModified image saved as 'modified.jpg'")
modified.show()
