#pragma once

#include "image.h"
#include <vector>

namespace q3d {
namespace import {

using std::vector;

struct Mesh {};
struct Material {};

class TextureBaker {
public:
    Image bake_ao(const Mesh& mesh, int width = 1024, int samples = 64);
    Image bake_curvature(const Mesh& mesh, int width = 512);
    Image bake_thickness(const Mesh& mesh, int width = 512);
    Image bake_cavity(const Mesh& mesh, int width = 512);
    Image bake_id_map(const Mesh& mesh, int id_count);
    struct PBRMaps {
        Image albedo, normal, roughness, metallic, ao, height, opacity, emissive;
    };
    PBRMaps bake_all(const Mesh& mesh, const Material& mat, int size = 1024);
};

} // namespace import
} // namespace q3d