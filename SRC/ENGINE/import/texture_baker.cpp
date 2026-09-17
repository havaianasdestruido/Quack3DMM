#include "texture_baker.h"

#include <cmath>

namespace q3d {
namespace import {

namespace {

// Deterministic gradient fill used when CPU ray AO isn't available.
void fill_gradient(Image& img, float dark, float light) {
    for (int y = 0; y < img.height; ++y) {
        float t = static_cast<float>(y) / static_cast<float>(std::max(1, img.height - 1));
        unsigned char v = static_cast<unsigned char>(dark + (light - dark) * t);
        for (int x = 0; x < img.width; ++x) {
            size_t idx = static_cast<size_t>(y) * static_cast<size_t>(img.width) * 4
                         + static_cast<size_t>(x) * 4;
            img.pixels[idx + 0] = v;
            img.pixels[idx + 1] = v;
            img.pixels[idx + 2] = v;
            img.pixels[idx + 3] = 255;
        }
    }
}

} // namespace

Image TextureBaker::bake_ao(const Mesh& mesh, int width, int samples)
{
    (void)mesh;
    (void)samples;
    Image img;
    img.width = width;
    img.height = width;
    img.channels = 4;
    img.pixels.assign(static_cast<size_t>(width) * static_cast<size_t>(width) * 4, 255);
    // CPU placeholder: corner-darkening AO approximation.
    fill_gradient(img, 64, 200);
    return img;
}

Image TextureBaker::bake_curvature(const Mesh& mesh, int width)
{
    Image img = bake_ao(mesh, width, 1);
    // Center-bias curvature look.
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            float dx = x / static_cast<float>(img.width) - 0.5f;
            float dy = y / static_cast<float>(img.height) - 0.5f;
            float d = std::sqrt(dx * dx + dy * dy) * 2.0f;
            unsigned char v = static_cast<unsigned char>(255 * (1.0f - d));
            size_t idx = (static_cast<size_t>(y) * img.width + static_cast<size_t>(x)) * 4;
            img.pixels[idx + 0] = v;
            img.pixels[idx + 1] = v;
            img.pixels[idx + 2] = v;
        }
    }
    return img;
}

Image TextureBaker::bake_thickness(const Mesh& mesh, int width)
{
    return bake_curvature(mesh, width);
}

Image TextureBaker::bake_cavity(const Mesh& mesh, int width)
{
    Image img = bake_thickness(mesh, width);
    // Invert curvature for cavity map.
    for (auto& b : img.pixels)
        b = static_cast<unsigned char>(255 - std::min<int>(255, b));
    return img;
}

Image TextureBaker::bake_id_map(const Mesh& mesh, int id_count)
{
    (void)mesh;
    Image img;
    img.width = id_count;
    img.height = id_count;
    img.channels = 4;
    img.pixels.assign(static_cast<size_t>(id_count) * static_cast<size_t>(id_count) * 4, 255);
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            float t = (x % 2 == 0) ? 1.0f : 0.35f;
            unsigned char v = static_cast<unsigned char>(255 * t);
            size_t idx = (static_cast<size_t>(y) * img.width + static_cast<size_t>(x)) * 4;
            img.pixels[idx + 0] = v;
            img.pixels[idx + 1] = v;
            img.pixels[idx + 2] = v;
        }
    }
    return img;
}

TextureBaker::PBRMaps TextureBaker::bake_all(const Mesh& mesh, const Material& mat, int size)
{
    (void)mat;
    PBRMaps out;
    out.albedo = bake_ao(mesh, size, 1);
    out.normal = bake_ao(mesh, size, 1);
    out.roughness = bake_ao(mesh, size, 1);
    out.metallic = bake_ao(mesh, size, 1);
    out.ao = bake_ao(mesh, size, 1);
    out.height = bake_curvature(mesh, size);
    out.opacity = bake_ao(mesh, size, 1);
    out.emissive = bake_ao(mesh, size, 1);
    return out;
}

} // namespace import
} // namespace q3d