from PIL import Image

def embed_secret_image(cover_path, secret_path, output_path):
    cover = Image.open(cover_path).convert("RGB")
    secret = Image.open(secret_path).convert("RGB")

    secret = secret.resize(cover.size)
    
    cover_pixels = cover.load()
    secret_pixels = secret.load()

    width, height = cover.size

    for x in range(width):
        for y in range(height):
            r_cover, g_cover, b_cover = cover_pixels[x, y]
            r_secret, g_secret, b_secret = secret_pixels[x, y]

            # Take 2 MSBs of secret image and put into 2 LSBs of cover image
            r_new = (r_cover & 0b11111100) | (r_secret >> 6)
            g_new = (g_cover & 0b11111100) | (g_secret >> 6)
            b_new = (b_cover & 0b11111100) | (b_secret >> 6)

            cover_pixels[x, y] = (r_new, g_new, b_new)

    cover.save(output_path)
    print(f"Secret image embedded successfully into {output_path}")

def extract_secret_image(stego_path, output_path):
    stego = Image.open(stego_path).convert("RGB")
    stego_pixels = stego.load()

    width, height = stego.size
    secret = Image.new("RGB", (width, height))
    secret_pixels = secret.load()

    for x in range(width):
        for y in range(height):
            r_stego, g_stego, b_stego = stego_pixels[x, y]

            # Extract the 2 LSBs and shift to MSB position to reconstruct secret
            r_secret = (r_stego & 0b00000011) << 6
            g_secret = (g_stego & 0b00000011) << 6
            b_secret = (b_stego & 0b00000011) << 6

            secret_pixels[x, y] = (r_secret, g_secret, b_secret)

    secret.save(output_path)
    print(f"Secret image extracted successfully as {output_path}")


if __name__ == "__main__":
    embed_secret_image("cover_image.png", "secret_image.png", "stego_image.png")
    extract_secret_image("stego_image.png", "recovered_secret.png")
