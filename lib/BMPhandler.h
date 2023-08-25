#include <iostream>
#include <fstream>
#include <vector>

class BMPhandler {
private:
    const uint16_t kReserved = 0;
    const uint32_t kHeaderSize = 14;
    const uint32_t kInfoSize = 40;
    const uint16_t kPlanes = 1;
    const uint16_t kBitsPerPixel = 4;
    const uint32_t kZero = 0;
    const uint32_t kPixelDataOffset = kHeaderSize + kInfoSize + 5 * 4;

    const uint8_t kColorTableZero[4] = {0xFF, 0xFF, 0xFF, 0x00};
    const uint8_t kColorTableOne[4] = {0x00, 0x77, 0x22, 0x00};
    const uint8_t kColorTableTwo[4] = {0x77, 0x00, 0x66, 0x00};
    const uint8_t kColorTableThree[4] = {0x22, 0xCC, 0xFF, 0x00};
    const uint8_t kColorTableFour[4] = {0x00, 0x00, 0x00, 0x00};

    template <typename T>
    void write_little_endian(std::ofstream& os, T& value) {
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    void match_color(char& tmp, uint64_t value);


public:
    void fill_header_info(std::ofstream& os, uint32_t image_length, uint32_t image_width);
    void fill_pixels(std::ofstream& os, uint32_t image_length, uint32_t image_width, const std::vector<std::vector<uint64_t>>& grid);

};
