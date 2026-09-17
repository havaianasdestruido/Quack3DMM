#pragma once
#include "image.h"
#include <memory>
#include <vector>

namespace q3d {
namespace import {

using std::shared_ptr;
using std::vector;

enum class TextureFilter {
    Nearest,
    Linear,
    LinearMipLinear
};

enum class TextureWrap {
    Repeat,
    ClampToEdge
};

enum class CompressionMode {
    None,
    Auto,
    BC7,
    ASTC
};

struct TextureAsset {
    int width = 0;
    int height = 0;
    int layers = 1;
};

struct TextureOptions {
    bool generate_mipmaps = true;
    bool srgb_to_linear = false;
    bool linear_to_srgb = false;
    int max_size = 4096;
    bool premultiply_alpha = false;
    TextureFilter min_filter = TextureFilter::LinearMipLinear;
    TextureFilter mag_filter = TextureFilter::Linear;
    TextureWrap wrap_s = TextureWrap::Repeat;
    TextureWrap wrap_t = TextureWrap::Repeat;
    int aniso_level = 16;
};

class TextureImporter {
public:
    shared_ptr<TextureAsset> import(const char* vpath, const TextureOptions& opts = {});
    void set_compression_mode(CompressionMode mode);
    shared_ptr<KTX2Image> compress_bc7(const Image& img);
    Image generate_normal_map(const Image& img, float strength = 1.0f);
    Image generate_ao_map(const Image& img);
    Image height_to_normal(const Image& img);
    shared_ptr<TextureAsset> import_cubemap(
        const char* px, const char* nx,
        const char* py, const char* ny,
        const char* pz, const char* nz,
        const TextureOptions& opts = {});
    shared_ptr<TextureAsset> import_equirectangular(const char* vpath, const TextureOptions& opts = {});
    shared_ptr<TextureAsset> import_3d(const vector<const char*>& slices, const TextureOptions& opts = {});
    shared_ptr<TextureAsset> import_array(const vector<const char*>& paths, const TextureOptions& opts = {});

private:
    CompressionMode _mode = CompressionMode::None;
};

} // namespace import
} // namespace q3d