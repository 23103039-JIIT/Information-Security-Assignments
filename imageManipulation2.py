from PIL import Image, ImageColor

def embedColorBits(image_path, color_name, output_path="output.png"):
    # Modify image such that - Lowest two bits of each pixel's RGB values are set to the highest two bits of the provided color.

    img = Image.open(image_path).convert("RGB")
    width, height = img.size

    color_rgb = ImageColor.getrgb(color_name)
    print(f"Provided color '{color_name}' → RGB = {color_rgb}")

    high2bits = [ (val >> 6) & 0b11 for val in color_rgb ]  # extract top 2 bits
    print(f"Highest two bits of color: R={high2bits[0]:02b}, G={high2bits[1]:02b}, B={high2bits[2]:02b}")

    new_img = Image.new("RGB", (width, height))

    for y in range(height):
        for x in range(width):
            r, g, b = img.getpixel((x, y))

            # Clear lowest two bits and set them to color's highest two bits
            r2 = (r & 0b11111100) | high2bits[0]
            g2 = (g & 0b11111100) | high2bits[1]
            b2 = (b & 0b11111100) | high2bits[2]

            new_img.putpixel((x, y), (r2, g2, b2))

    # Save output image
    new_img.save(output_path)
    print(f"New image saved as: {output_path}")
    return new_img

if __name__ == "__main__":
    result = embedColorBits("sample.jpg", "turquoise", "encoded_image.png")
    result.show()
