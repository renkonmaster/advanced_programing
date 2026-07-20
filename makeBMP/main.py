from pathlib import Path

OUTPUT_DIR = Path(__file__).resolve().parent

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)


def make_bmp(filename: str, pixels: list[list[tuple[int, int, int]]]) -> bytearray:
    """上から下のRGB配列から、24-bit bottom-up BMPを作る。"""
    height = len(pixels)
    width = len(pixels[0])

    if width <= 0 or height <= 0:
        raise ValueError("width and height must be positive")

    if any(len(row) != width for row in pixels):
        raise ValueError("all rows must have the same width")

    # 24-bit BMPでは1ピクセル3バイト。
    # 各行のサイズは4の倍数になるようにパディングされる。
    raw_row_size = width * 3
    row_size = (raw_row_size + 3) & ~3
    image_size = row_size * height
    file_size = 54 + image_size

    bmp = bytearray(file_size)

    # BITMAPFILEHEADER
    bmp[0:2] = b"BM"
    bmp[2:6] = file_size.to_bytes(4, "little")
    bmp[10:14] = (54).to_bytes(4, "little")

    # BITMAPINFOHEADER
    bmp[14:18] = (40).to_bytes(4, "little")
    bmp[18:22] = width.to_bytes(4, "little", signed=True)
    bmp[22:26] = height.to_bytes(4, "little", signed=True)
    bmp[26:28] = (1).to_bytes(2, "little")
    bmp[28:30] = (24).to_bytes(2, "little")
    bmp[30:34] = (0).to_bytes(4, "little")
    bmp[34:38] = image_size.to_bytes(4, "little")

    # BMPのピクセルデータは下の行から上の行へ格納する。
    # さらに、各ピクセルはRGBではなくBGR順。
    for file_y in range(height):
        source_y = height - 1 - file_y
        row_offset = 54 + file_y * row_size

        for x in range(width):
            r, g, b = pixels[source_y][x]
            pixel_offset = row_offset + x * 3
            bmp[pixel_offset:pixel_offset + 3] = bytes((b, g, r))

    (OUTPUT_DIR / filename).write_bytes(bmp)
    return bmp


def make_corner_image(width: int, height: int):
    """四隅だけに異なる色を設定した画像を作る。"""
    pixels = [
        [BLACK for _ in range(width)]
        for _ in range(height)
    ]

    # pixelsは上から下の順
    pixels[0][0] = RED
    pixels[0][width - 1] = GREEN
    pixels[height - 1][0] = BLUE
    pixels[height - 1][width - 1] = WHITE

    return pixels


def write_ex11_1_1_files():
    """Ex11-1-1のファイル形式判定用ファイルを作る。"""
    valid = make_bmp(
        "test_ok.bmp",
        [[RED]],
    )

    bad_signature = bytearray(valid)
    bad_signature[0:2] = b"ZZ"
    (OUTPUT_DIR / "test_bad_signature.bmp").write_bytes(bad_signature)

    bad_bit_count = bytearray(valid)
    bad_bit_count[28:30] = (32).to_bytes(2, "little")
    (OUTPUT_DIR / "test_bad_bit_count.bmp").write_bytes(bad_bit_count)

    bad_offset = bytearray(valid)
    bad_offset[10:14] = (55).to_bytes(4, "little")
    (OUTPUT_DIR / "test_bad_offset.bmp").write_bytes(bad_offset)

    bad_all = bytearray(valid)
    bad_all[0:2] = b"ZZ"
    bad_all[28:30] = (32).to_bytes(2, "little")
    bad_all[10:14] = (55).to_bytes(4, "little")
    (OUTPUT_DIR / "test_bad_all.bmp").write_bytes(bad_all)


def write_ex11_1_2_files():
    """Ex11-1-2の画像サイズ・四隅・パディング確認用。"""
    tests = [
        # width * 3 = 12、パディング0
        ("test_corners_4x3_padding0.bmp", 4, 3),

        # width * 3 = 15、パディング1
        ("test_corners_5x4_padding1.bmp", 5, 4),

        # width * 3 = 6、パディング2
        ("test_corners_2x3_padding2.bmp", 2, 3),

        # width * 3 = 9、パディング3
        ("test_corners_3x2_padding3.bmp", 3, 2),
    ]

    for filename, width, height in tests:
        make_bmp(filename, make_corner_image(width, height))

        padding = ((width * 3 + 3) & ~3) - width * 3

        print(
            f"{filename}: "
            f"{width}x{height}, padding={padding}"
        )


def main():
    write_ex11_1_1_files()
    write_ex11_1_2_files()

    print()
    print("Expected corner colors:")
    print("Upper-left  (R=255, G=0, B=0)")
    print("Upper-right (R=0, G=255, B=0)")
    print("Lower-left  (R=0, G=0, B=255)")
    print("Lower-right (R=255, G=255, B=255)")


if __name__ == "__main__":
    main()
