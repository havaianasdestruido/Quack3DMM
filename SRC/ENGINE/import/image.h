#pragma once
#include <vector>
#include <string>

namespace q3d {
namespace import {

using byte = unsigned char;
using std::string;
using std::vector;

struct Color {
    byte r, g, b, a;
};

enum class Filter {
    Nearest,
    Linear
};

struct Image {
    int width = 0;
    int height = 0;
    int channels = 0;
    int bit_depth = 8;
    bool is_srgb = true;
    vector<byte> pixels;
    string source_path;

    void swizzle(char r, char g, char b, char a);
    void crop(int x, int y, int w, int h);
    void resize(int w, int h, Filter filter = Filter::Linear);
    void flip_vertical();
    void flip_horizontal();
    vector<Image> generate_mipmaps() const;
    void to_indexed(vector<byte>& indices, vector<Color>& palette, int max_colors = 256);
};

enum class ImageFormat {
    PNG, JPEG, WebP, AVIF, HEIC, TIFF, TGA, BMP, DDS, KTX2, Unknown
};

class ImageLoader {
public:
    static ImageFormat detect_format(const byte* magic, size_t n);
    static Image load(const char* vpath);
    static Image load(const byte* data, size_t size, ImageFormat hint = ImageFormat::Unknown);
    static bool save(const Image& img, const char* vpath, ImageFormat fmt = ImageFormat::PNG, int quality = 90);
    static Image load_header(const char* vpath);
};

struct DDSImage {
    int width = 0;
    int height = 0;
    int mip_levels = 1;
    int format = 0;
    vector<byte> data;
    vector<Image> generate_decompressed() const;
};

struct KTX2Image {
    int width = 0;
    int height = 0;
    vector<byte> basis_data;
    bool is_transcoded = false;

    Image transcode_to_rgba();
};

} // namespace import
} // namespace q3d