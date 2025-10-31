from PIL import Image

# Load an image
img = Image.open("sample.jpg")
img = img.convert("RGB")         # Ensure it's in RGB format

width, height = img.size

print(f"Image Size: {width} x {height}")

def flip_lsb(value):
    return value ^ 1

# Create a modified image
modified = Image.new("RGB", (width, height))

for y in range(height):
    for x in range(width):
        r, g, b = img.getpixel((x, y))
        r2, g2, b2 = flip_lsb(r), flip_lsb(g), flip_lsb(b)
        modified.putpixel((x, y), (r2, g2, b2))

modified.save("modified.jpg")
print("\nModified image saved as 'modified.jpg'")
modified.show()
