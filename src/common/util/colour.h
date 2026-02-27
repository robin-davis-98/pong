#ifndef PET_COLOUR_H
#define PET_COLOUR_H
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct Colour
{
    float r, g, b, a;
};

inline Colour colour_from_rbga(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}

inline Colour colour_from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return colour_from_rbga(r, g, b, 255);
}
inline Colour colour_from_hex(const char* hex)
{
    assert(hex != nullptr && hex[0] == '#' && strlen(hex) == 7);

    uint32_t value = (uint32_t)strtoul(hex + 1, nullptr, 16);

    return colour_from_rbga(
        (value >> 16) & 0xFF,
        (value >> 8) & 0xFF,
        (value & 0xFF),
        255
    );
}

#endif