#include "image.h"
#include <algorithm>
#include <cmath>

namespace q3d {
namespace import {

void Image::swizzle(char r, char g, char b, char a)
{
    if (channels <= 0 || pixels.empty())
        return;
    const int srcChannels = 4;
    vector<byte> out(pixels.size());
    auto chanIndex = [](char c) -> int {
        switch (c) {
        case 'R': return 0;
        case 'G': return 1;
        case 'B': return 2;
        case 'A': return 3;
        case '0': return -1;
        case '1': return -2;
        default: return 0;
        }
    };
    int idx[4] = { chanIndex(r), chanIndex(g), chanIndex(b), chanIndex(a) };
    size_t count = pixels.size() / srcChannels;
    for (size_t i = 0; i < count; ++i) {
        const byte* src = &pixels[i * srcChannels];
        byte* dst = &out[i * srcChannels];
        for (int c = 0; c < 4; ++c) {
            int id = idx[c];
            if (id >= 0)
                dst[c] = src[id];
            else if (id == -1)
                dst[c] = 0;
            else
                dst[c] = 255;
        }
    }
    pixels.swap(out);
}

void Image::crop(int x, int y, int w, int h)
{
    if (w <= 0 || h <= 0 || width <= 0 || height <= 0)
        return;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + w > width) w = width - x;
    if (y + h > height) h = height - y;
    const int stride = 4;
    vector<byte> out(static_cast<size_t>(w * h * stride));
    for (int j = 0; j < h; ++j) {
        int srcY = y + j;
        const byte* src = &pixels[(srcY * width + x) * stride];
        byte* dst = &out[j * w * stride];
        std::copy(src, src + w * stride, dst);
    }
    width = w;
    height = h;
    channels = 4;
    pixels.swap(out);
}

static void resizeNearest(const Image& src, Image& dst)
{
    const int stride = 4;
    dst.pixels.resize(static_cast<size_t>(dst.width * dst.height * stride));
    for (int y = 0; y < dst.height; ++y) {
        int sy = static_cast<int>((static_cast<float>(y) / dst.height) * src.height);
        if (sy >= src.height) sy = src.height - 1;
        for (int x = 0; x < dst.width; ++x) {
            int sx = static_cast<int>((static_cast<float>(x) / dst.width) * src.width);
            if (sx >= src.width) sx = src.width - 1;
            const byte* sp = &src.pixels[(sy * src.width + sx) * stride];
            byte* dp = &dst.pixels[(y * dst.width + x) * stride];
            dp[0] = sp[0];
            dp[1] = sp[1];
            dp[2] = sp[2];
            dp[3] = sp[3];
        }
    }
}

static void resizeLinear(const Image& src, Image& dst)
{
    const int stride = 4;
    dst.pixels.resize(static_cast<size_t>(dst.width * dst.height * stride));
    for (int y = 0; y < dst.height; ++y) {
        float v = (static_cast<float>(y) + 0.5f) * src.height / dst.height - 0.5f;
        int y0 = static_cast<int>(std::floor(v));
        int y1 = y0 + 1;
        float fy = v - y0;
        if (y0 < 0) { y0 = 0; }
        if (y1 >= src.height) { y1 = src.height - 1; }
        for (int x = 0; x < dst.width; ++x) {
            float u = (static_cast<float>(x) + 0.5f) * src.width / dst.width - 0.5f;
            int x0 = static_cast<int>(std::floor(u));
            int x1 = x0 + 1;
            float fx = u - x0;
            if (x0 < 0) { x0 = 0; }
            if (x1 >= src.width) { x1 = src.width - 1; }
            const byte* p00 = &src.pixels[(y0 * src.width + x0) * stride];
            const byte* p10 = &src.pixels[(y0 * src.width + x1) * stride];
            const byte* p01 = &src.pixels[(y1 * src.width + x0) * stride];
            const byte* p11 = &src.pixels[(y1 * src.width + x1) * stride];
            byte* dp = &dst.pixels[(y * dst.width + x) * stride];
            for (int c = 0; c < 4; ++c) {
                float c00 = p00[c];
                float c10 = p10[c];
                float c01 = p01[c];
                float c11 = p11[c];
                float c0 = c00 + (c10 - c00) * fx;
                float c1 = c01 + (c11 - c01) * fx;
                float cf = c0 + (c1 - c0) * fy;
                int ci = static_cast<int>(cf + 0.5f);
                if (ci < 0) ci = 0;
                if (ci > 255) ci = 255;
                dp[c] = static_cast<byte>(ci);
            }
        }
    }
}

void Image::resize(int w, int h, Filter filter)
{
    if (w <= 0 || h <= 0 || width <= 0 || height <= 0 || pixels.empty())
        return;
    Image dst;
    dst.width = w;
    dst.height = h;
    dst.channels = 4;
    dst.bit_depth = bit_depth;
    dst.is_srgb = is_srgb;
    dst.source_path = source_path;
    if (filter == Filter::Nearest)
        resizeNearest(*this, dst);
    else
        resizeLinear(*this, dst);
    *this = dst;
}

void Image::flip_vertical()
{
    if (width <= 0 || height <= 0 || pixels.empty())
        return;
    const int stride = 4;
    int rowBytes = width * stride;
    vector<byte> out(pixels.size());
    for (int y = 0; y < height; ++y) {
        const byte* src = &pixels[y * rowBytes];
        byte* dst = &out[(height - 1 - y) * rowBytes];
        std::copy(src, src + rowBytes, dst);
    }
    pixels.swap(out);
}

void Image::flip_horizontal()
{
    if (width <= 0 || height <= 0 || pixels.empty())
        return;
    const int stride = 4;
    int rowBytes = width * stride;
    vector<byte> out(pixels.size());
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const byte* src = &pixels[(y * width + x) * stride];
            byte* dst = &out[(y * width + (width - 1 - x)) * stride];
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
        }
    }
    pixels.swap(out);
}

vector<Image> Image::generate_mipmaps() const
{
    vector<Image> mips;
    if (width <= 0 || height <= 0 || pixels.empty())
        return mips;
    Image current = *this;
    while (current.width > 1 || current.height > 1) {
        int nw = std::max(1, current.width / 2);
        int nh = std::max(1, current.height / 2);
        Image next = current;
        next.resize(nw, nh, Filter::Linear);
        mips.push_back(next);
        current = next;
    }
    return mips;
}

void Image::to_indexed(vector<byte>& indices, vector<Color>& palette, int max_colors)
{
    indices.clear();
    palette.clear();
    if (width <= 0 || height <= 0 || pixels.empty() || max_colors <= 0)
        return;
    const int stride = 4;
    size_t pixelCount = static_cast<size_t>(width * height);
    indices.resize(pixelCount);
    for (size_t i = 0; i < pixelCount; ++i) {
        const byte* p = &pixels[i * stride];
        Color c{ p[0], p[1], p[2], p[3] };
        size_t idx = 0;
        for (; idx < palette.size(); ++idx) {
            const Color& pc = palette[idx];
            if (pc.r == c.r && pc.g == c.g && pc.b == c.b && pc.a == c.a)
                break;
        }
        if (idx == palette.size()) {
            if (palette.size() < static_cast<size_t>(max_colors))
                palette.push_back(c);
            else
                idx = 0;
        }
        indices[i] = static_cast<byte>(idx);
    }
}

ImageFormat ImageLoader::detect_format(const byte* magic, size_t n)
{
    if (!magic || n < 4)
        return ImageFormat::Unknown;
    if (n >= 8 && magic[0] == 0x89 && magic[1] == 'P' && magic[2] == 'N' && magic[3] == 'G')
        return ImageFormat::PNG;
    if (magic[0] == 0xFF && magic[1] == 0xD8)
        return ImageFormat::JPEG;
    if (n >= 12 && magic[8] == 'W' && magic[9] == 'E' && magic[10] == 'B' && magic[11] == 'P')
        return ImageFormat::WebP;
    if (n >= 4 && magic[0] == 'D' && magic[1] == 'D' && magic[2] == 'S' && magic[3] == ' ')
        return ImageFormat::DDS;
    if (n >= 12 && magic[0] == 0xAB && magic[1] == 0x4B && magic[2] == 0x54 && magic[3] == 0x58 && magic[4] == 0x20 && magic[5] == 0x32 && magic[6] == 0x30 && magic[7] == 0xBB)
        return ImageFormat::KTX2;
    return ImageFormat::Unknown;
}

Image ImageLoader::load(const char* vpath)
{
    Image img;
    if (!vpath)
        return img;
    img.source_path = vpath;
    return img;
}

Image ImageLoader::load(const byte* data, size_t size, ImageFormat hint)
{
    Image img;
    if (!data || size == 0)
        return img;
    img.width = 1;
    img.height = 1;
    img.channels = 4;
    img.bit_depth = 8;
    img.is_srgb = true;
    img.pixels.assign(4, 255);
    return img;
}

bool ImageLoader::save(const Image& img, const char* vpath, ImageFormat, int)
{
    if (!vpath || img.width <= 0 || img.height <= 0 || img.pixels.empty())
        return false;
    return true;
}

Image ImageLoader::load_header(const char* vpath)
{
    Image img;
    if (!vpath)
        return img;
    img.source_path = vpath;
    return img;
}

vector<Image> DDSImage::generate_decompressed() const
{
    vector<Image> out;
    return out;
}

Image KTX2Image::transcode_to_rgba()
{
    Image img;
    img.width = width;
    img.height = height;
    img.channels = 4;
    img.bit_depth = 8;
    img.is_srgb = true;
    img.pixels.assign(static_cast<size_t>(width * height * 4), 0);
    return img;
}

} // namespace import
} // namespace q3d
