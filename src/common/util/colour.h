#ifndef PONG_COLOUR_H
#define PONG_COLOUR_H
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct Colour
{
    uint8_t r, g, b, a;
};

inline Colour colour_from_rbga(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return {r, g, b, a};
}

inline Colour colour_from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return {r, g, b, 255};
}

inline Colour colour_from_hex(const char* hex)
{
    /**
     * @desc Takes a hex string in the format of "#RRGGBB"
    */
    assert(hex != nullptr);
    assert(hex[0] == '#');
    assert(hex[7] == '\0');

    uint64_t value = strtoul(hex + 1, nullptr, 16);

    return (Colour){
        .r = (uint8_t)((value >> 16) & 0xFF),
        .g = (uint8_t)((value >> 8) & 0xFF),
        .b = (uint8_t)(value & 0xFF),
        .a = 255
    };
}

inline Colour colour_from_hex_alpha(const char* hex)
{
    /**
     * @desc Takes a hex string in the format of "#RRGGBBAA"s
     */
    assert(hex != nullptr);
    assert(hex[0] == '#');
    assert(hex[9] == '\0');

    uint64_t value = strtoul(hex + 1, nullptr, 16);

    return (Colour){
        .r = (uint8_t)((value >> 24) & 0xFF),
        .g = (uint8_t)((value >> 16) & 0xFF),
        .b = (uint8_t)((value >> 8) & 0xFF),
        .a = (uint8_t)(value & 0xFF),
    };
}

#endif