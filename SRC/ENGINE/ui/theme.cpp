#include "theme.h"

#include <cstring>

namespace q3d {
namespace ui {

Color Color::hex(const char* hex) {
    if (!hex || std::strlen(hex) < 7 || hex[0] != '#') {
        return rgb(1.0f, 0.0f, 1.0f);
    }
    auto hex_to_float = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
        if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
        return 0;
    };
    int r = hex_to_float(hex[1]) * 16 + hex_to_float(hex[2]);
    int g = hex_to_float(hex[3]) * 16 + hex_to_float(hex[4]);
    int b = hex_to_float(hex[5]) * 16 + hex_to_float(hex[6]);
    return rgb(r / 255.0f, g / 255.0f, b / 255.0f);
}

} // namespace ui
} // namespace q3d
