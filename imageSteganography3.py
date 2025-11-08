from PIL import Image

def char_to_code(ch):
    if ch == ' ':
        return 27
    elif 'a' <= ch <= 'z':
        return ord(ch) - ord('a') + 1           # 1–26
    elif 'A' <= ch <= 'Z':
        return ord(ch) - ord('A') + 28          # 28–53
    elif '0' <= ch <= '9':
        return ord(ch) - ord('0') + 54          # 54–63
    elif ch == '\0':
        return 0
    else:
        raise ValueError(f"Unsupported character: {ch}")

def code_to_char(code):
    if code == 0:
        return '\0'
    elif 1 <= code <= 26:
        return chr(code + ord('a') - 1)
    elif code == 27:
        return ' '
    elif 28 <= code <= 53:
        return chr(code - 28 + ord('A'))
    elif 54 <= code <= 63:
        return chr(code - 54 + ord('0'))
    else:
        return '?' 

def encode_message(message):
    codes = [char_to_code(ch) for ch in message]
    codes.append(0)  # Null terminator
    return codes

def decode_message(codes):
    message = ''
    for code in codes:
        if code == 0:
            break
        message += code_to_char(code)
    return message

def get_two_bit_pairs(code):
    binary = format(code, '06b')  # 6-bit binary string
    return [binary[i:i+2] for i in range(0, 6, 2)]

def bits_to_int(two_bits):
    return int(two_bits, 2)

def hide_message(cover_path, message, output_path):
    img = Image.open(cover_path).convert('RGB')
    pixels = img.load()
    width, height = img.size

    codes = encode_message(message)
    bit_pairs = []
    for code in codes:
        bit_pairs.extend(get_two_bit_pairs(code))

    max_capacity = width * height * 3  # 3 color channels per pixel
    if len(bit_pairs) > max_capacity:
        print("Message too long to fit in the cover image.")
        return

    pair_index = 0
    for y in range(height):
        for x in range(width):
            if pair_index >= len(bit_pairs):
                break
            r, g, b = pixels[x, y]
            if pair_index < len(bit_pairs):
                r = (r & 0b11111100) | bits_to_int(bit_pairs[pair_index])
                pair_index += 1
            if pair_index < len(bit_pairs):
                g = (g & 0b11111100) | bits_to_int(bit_pairs[pair_index])
                pair_index += 1
            if pair_index < len(bit_pairs):
                b = (b & 0b11111100) | bits_to_int(bit_pairs[pair_index])
                pair_index += 1
            pixels[x, y] = (r, g, b)
        if pair_index >= len(bit_pairs):
            break

    img.save(output_path)
    print("Message successfully hidden in:", output_path)

def extract_message(stego_path):
    img = Image.open(stego_path).convert('RGB')
    pixels = img.load()
    width, height = img.size

    bits = ''
    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y]
            bits += format(r & 0b11, '02b')
            bits += format(g & 0b11, '02b')
            bits += format(b & 0b11, '02b')

    codes = [int(bits[i:i+6], 2) for i in range(0, len(bits), 6)]
    message = decode_message(codes)
    return message

if __name__ == "__main__":
    cover_image = "cover_image.png"  
    stego_image = "stego.png"
    secret_message = "Hello World 123"

    hide_message(cover_image, secret_message, stego_image)
    recovered = extract_message(stego_image)
    print("Extracted message:", recovered)
