#include "BMPhandler.h"

void BMPhandler::fill_header_info(std::ofstream& os, uint32_t image_length, uint32_t image_width) {
    uint32_t byte_length = ((image_length + (image_length & 1)) >> 1) + (4 - ((image_length + (image_length & 1)) >> 1) % 4) % 4;
    const uint32_t kFileSize = kPixelDataOffset + image_width * byte_length;
    // Write File header
    os << "BM";
    write_little_endian(os, kFileSize);
    write_little_endian(os, kReserved);
    write_little_endian(os, kReserved);
    write_little_endian(os, kPixelDataOffset);
    // Write Info header
    write_little_endian(os, kInfoSize);
    write_little_endian(os, image_length);
    write_little_endian(os, image_width);
    write_little_endian(os, kPlanes);
    write_little_endian(os, kBitsPerPixel);
    for (int i = 0; i < 6; ++i, write_little_endian(os, kZero));
    // Write RGBQuad (color table)
    write_little_endian(os, kColorTableZero);
    write_little_endian(os, kColorTableOne);
    write_little_endian(os, kColorTableTwo);
    write_little_endian(os, kColorTableThree);
    write_little_endian(os, kColorTableFour);
}

void BMPhandler::match_color(char& tmp, uint64_t value) {
    if (value > 4)
        tmp |= 4;
    else
        tmp |= static_cast<char>(value);
}

void BMPhandler::fill_pixels(std::ofstream& os, uint32_t image_length, uint32_t image_width, const std::vector<std::vector<uint64_t>>& grid) {
    uint32_t byte_length = ((image_length + (image_length & 1)) >> 1) + (4 - ((image_length + (image_length & 1)) >> 1) % 4) % 4;
    for (int64_t i = image_width - 1; i >= 0; --i) {
        for (int j = 0; j < byte_length * 2; j += 2) {
            char tmp = 0;
            if (j < image_length)
                match_color(tmp, grid[i][j]);
            else
                match_color(tmp, 0);
            tmp <<= 4;
            if (j + 1 < image_length)
                match_color(tmp, grid[i][j + 1]);
            else
                match_color(tmp, 0);
            os.write(&tmp, sizeof(tmp));
        }
    }
}

