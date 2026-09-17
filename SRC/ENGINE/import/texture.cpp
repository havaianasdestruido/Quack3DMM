#include "texture.h"
#include "image.h"
#include <cmath>

namespace q3d {
namespace import {

shared_ptr<TextureAsset> TextureImporter::import(const char* vpath, const TextureOptions&)
{
    if (!vpath)
        return {};
    Image img = ImageLoader::load(vpath);
    if (img.width <= 0 || img.height <= 0 || img.pixels.empty())
        return {};
    auto tex = std::make_shared<TextureAsset>();
    tex->width = img.width;
    tex->height = img.height;
    tex->layers = 1;
    return tex;
}

void TextureImporter::set_compression_mode(CompressionMode mode)
{
    _mode = mode;
}

shared_ptr<KTX2Image> TextureImporter::compress_bc7(const Image& img)
{
    auto kt = std::make_shared<KTX2Image>();
    kt->width = img.width;
    kt->height = img.height;
    kt->basis_data.clear();
    kt->is_transcoded = false;
    return kt;
}

Image TextureImporter::generate_normal_map(const Image& img, float strength)
{
    Image out = img;
    out.channels = 4;
    if (out.width <= 0 || out.height <= 0 || out.pixels.empty())
        return out;
    const int stride = 4;
    for (int y = 0; y < out.height; ++y) {
        for (int x = 0; x < out.width; ++x) {
            int xm1 = x > 0 ? x - 1 : x;
            int xp1 = x + 1 < out.width ? x + 1 : x;
            int ym1 = y > 0 ? y - 1 : y;
            int yp1 = y + 1 < out.height ? y + 1 : y;
            const unsigned char* pL = &img.pixels[(y * img.width + xm1) * stride];
            const unsigned char* pR = &img.pixels[(y * img.width + xp1) * stride];
            const unsigned char* pU = &img.pixels[(ym1 * img.width + x) * stride];
            const unsigned char* pD = &img.pixels[(yp1 * img.width + x) * stride];
            float hL = pL[0] / 255.0f;
            float hR = pR[0] / 255.0f;
            float hU = pU[0] / 255.0f;
            float hD = pD[0] / 255.0f;
            float dx = (hL - hR) * strength;
            float dy = (hU - hD) * strength;
            float dz = 1.0f;
            float len = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (len == 0.0f)
                len = 1.0f;
            dx /= len;
            dy /= len;
            dz /= len;
            unsigned char* pOut = &out.pixels[(y * out.width + x) * stride];
            pOut[0] = static_cast<unsigned char>((dx * 0.5f + 0.5f) * 255.0f);
            pOut[1] = static_cast<unsigned char>((dy * 0.5f + 0.5f) * 255.0f);
            pOut[2] = static_cast<unsigned char>((dz * 0.5f + 0.5f) * 255.0f);
            pOut[3] = 255;
        }
    }
    return out;
}

Image TextureImporter::generate_ao_map(const Image& img)
{
    Image out = img;
    return out;
}

Image TextureImporter::height_to_normal(const Image& img)
{
    return generate_normal_map(img, 1.0f);
}

shared_ptr<TextureAsset> TextureImporter::import_cubemap(
    const char* px, const char* nx,
    const char* py, const char* ny,
    const char* pz, const char* nz,
    const TextureOptions& opts)
{
    (void)opts;
    if (!px || !nx || !py || !ny || !pz || !nz)
        return {};
    auto tex = std::make_shared<TextureAsset>();
    tex->layers = 6;
    return tex;
}

shared_ptr<TextureAsset> TextureImporter::import_equirectangular(const char* vpath, const TextureOptions& opts)
{
    (void)opts;
    return import(vpath, opts);
}

shared_ptr<TextureAsset> TextureImporter::import_3d(const vector<const char*>& slices, const TextureOptions& opts)
{
    (void)opts;
    if (slices.empty())
        return {};
    auto tex = std::make_shared<TextureAsset>();
    tex->layers = static_cast<int>(slices.size());
    return tex;
}

shared_ptr<TextureAsset> TextureImporter::import_array(const vector<const char*>& paths, const TextureOptions& opts)
{
    (void)opts;
    if (paths.empty())
        return {};
    auto tex = std::make_shared<TextureAsset>();
    tex->layers = static_cast<int>(paths.size());
    return tex;
}

} // namespace import
} // namespace q3d
