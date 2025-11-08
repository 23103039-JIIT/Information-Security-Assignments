from PIL import Image

# Convert integer (0–255) to 8-bit binary string
def int_to_bin(value):
    return format(value, '08b')

# Convert 8-bit binary string back to integer
def bin_to_int(b):
    return int(b, 2)

def can_embed(cover_path, secret_path, bits_per_channel=2):
    cover = Image.open(cover_path).convert("RGB")
    secret = Image.open(secret_path).convert("RGB")

    cover_capacity = cover.width * cover.height * 3 * bits_per_channel
    secret_bits = secret.width * secret.height * 3 * 8

    print(f"Cover capacity: {cover_capacity} bits")
    print(f"Secret size: {secret_bits} bits")

    if secret_bits <= cover_capacity:
        print("Embedding is possible.")
        return True
    else:
        print("Embedding not possible: Secret image too large for cover image.")
        return False

def embed_image(cover_path, secret_path, output_path, bits_per_channel=2):

    if not can_embed(cover_path, secret_path, bits_per_channel):
        print("Operation stopped.")
        return

    cover = Image.open(cover_path).convert("RGB")
    secret = Image.open(secret_path).convert("RGB")

    cover_pixels = list(cover.getdata())
    secret_pixels = list(secret.getdata())

    # Flatten all secret pixel bits into one long bitstream
    secret_bits = ""
    for r, g, b in secret_pixels:
        secret_bits += int_to_bin(r)
        secret_bits += int_to_bin(g)
        secret_bits += int_to_bin(b)

    max_capacity = len(cover_pixels) * 3 * bits_per_channel

    new_pixels = []
    bit_index = 0

    for (r, g, b) in cover_pixels:
        if bit_index < len(secret_bits):
            r_bits = int_to_bin(r)
            g_bits = int_to_bin(g)
            b_bits = int_to_bin(b)

            # Replace LSBs with bits from secret stream
            r_bits = r_bits[:-bits_per_channel] + secret_bits[bit_index:bit_index+bits_per_channel]
            bit_index += bits_per_channel
            if bit_index < len(secret_bits):
                g_bits = g_bits[:-bits_per_channel] + secret_bits[bit_index:bit_index+bits_per_channel]
                bit_index += bits_per_channel
            if bit_index < len(secret_bits):
                b_bits = b_bits[:-bits_per_channel] + secret_bits[bit_index:bit_index+bits_per_channel]
                bit_index += bits_per_channel

            new_pixels.append((bin_to_int(r_bits), bin_to_int(g_bits), bin_to_int(b_bits)))
        else:
            new_pixels.append((r, g, b))

    stego = Image.new(cover.mode, cover.size)
    stego.putdata(new_pixels)
    stego.save(output_path)
    print(f"Secret image embedded successfully as {output_path}")

def extract_image(stego_path, secret_size, output_path, bits_per_channel=2):
    stego = Image.open(stego_path).convert("RGB")
    stego_pixels = list(stego.getdata())

    total_secret_bits = secret_size[0] * secret_size[1] * 3 * 8
    extracted_bits = ""

    for (r, g, b) in stego_pixels:
        extracted_bits += int_to_bin(r)[-bits_per_channel:]
        extracted_bits += int_to_bin(g)[-bits_per_channel:]
        extracted_bits += int_to_bin(b)[-bits_per_channel:]
        if len(extracted_bits) >= total_secret_bits:
            break

    # Reconstruct secret pixel values
    secret_pixels = []
    for i in range(0, len(extracted_bits), 24):
        if i + 24 <= len(extracted_bits):
            r = bin_to_int(extracted_bits[i:i+8])
            g = bin_to_int(extracted_bits[i+8:i+16])
            b = bin_to_int(extracted_bits[i+16:i+24])
            secret_pixels.append((r, g, b))

    secret = Image.new("RGB", secret_size)
    secret.putdata(secret_pixels)
    secret.save(output_path)
    print(f"Secret image extracted successfully as {output_path}")



if __name__ == "__main__":
    cover_image = "cover.png"
    secret_image = "secret.png"
    stego_image = "stego.png"
    recovered_image = "recovered_secret.png"

    embed_image(cover_image, secret_image, stego_image, bits_per_channel=2)
    extract_image(stego_image, secret_size=(400, 400), output_path=recovered_image, bits_per_channel=2)
