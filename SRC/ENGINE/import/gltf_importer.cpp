#include "import/gltf_importer.h"
#include "import/image.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace q3d {
namespace import {

GltfImporter::GltfImporter() {}
GltfImporter::~GltfImporter() {}

void GltfImporter::set_dracoloader(void* decoder)
{
    draco_decoder_ = decoder;
}

void GltfImporter::set_progress(ProgressCallback cb)
{
    progress_cb_ = cb;
}

void GltfImporter::report_progress(const char* stage, float p)
{
    progress_ = p;
    if (progress_cb_)
        progress_cb_(stage, p);
}

ImportedModel GltfImporter::import(const char* vpath)
{
    report_progress("open", 0.0f);
    FILE* f = std::fopen(vpath, "rb");
    if (!f)
        return ImportedModel{};
    std::fseek(f, 0, SEEK_END);
    long cb = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    std::vector<byte> data(static_cast<size_t>(cb));
    if (cb > 0 && std::fread(&data[0], 1, static_cast<size_t>(cb), f) != static_cast<size_t>(cb))
    {
        std::fclose(f);
        return ImportedModel{};
    }
    std::fclose(f);
    report_progress("parse", 0.1f);
    const char* psz = reinterpret_cast<const char*>(&data[0]);
    return parse_json(psz);
}

ImportedModel GltfImporter::import(const char* json_str, const std::vector<std::pair<const byte*, size_t>>& buffers)
{
    external_buffers_ = buffers;
    return parse_json(json_str);
}

ImportedModel GltfImporter::import_glb(const char* vpath)
{
    report_progress("open_glb", 0.0f);
    FILE* f = std::fopen(vpath, "rb");
    if (!f)
        return ImportedModel{};
    std::fseek(f, 0, SEEK_END);
    long cb = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    std::vector<byte> data(static_cast<size_t>(cb));
    if (cb > 0 && std::fread(&data[0], 1, static_cast<size_t>(cb), f) != static_cast<size_t>(cb))
    {
        std::fclose(f);
        return ImportedModel{};
    }
    std::fclose(f);

    size_t json_size = 0;
    size_t bin_size = 0;
    size_t bin_offset = 0;
    if (!parse_glb_header(&data[0], data.size(), &json_size, &bin_size, &bin_offset))
        return ImportedModel{};

    std::string json_str(reinterpret_cast<const char*>(&data[20]), json_size);
    external_buffers_.clear();
    if (bin_size > 0)
        {
        external_buffers_.push_back(std::make_pair(&data[bin_offset], bin_size));
        }
    report_progress("parse_glb", 0.2f);
    return parse_json(json_str.c_str());
}

int GltfImporter::parse_glb_header(const byte* data, size_t size, size_t* json_size, size_t* bin_size, size_t* bin_offset)
{
    if (size < 20)
        return 0;
    uint32_t magic = *(const uint32_t*)(data + 0);
    uint32_t version = *(const uint32_t*)(data + 4);
    uint32_t length = *(const uint32_t*)(data + 8);
    if (magic != 0x46546C67 || version != 2)
        return 0;
    if (length > size)
        return 0;
    const byte* p = data + 12;
    uint32_t chunk_length = *(const uint32_t*)(p + 0);
    uint32_t chunk_type = *(const uint32_t*)(p + 4);
    if (chunk_type != 0x4E4F534A)
        return 0;
    *json_size = chunk_length;
    size_t offset = 12 + 8 + chunk_length;
    *bin_size = 0;
    *bin_offset = offset;
    if (offset + 8 <= size)
        {
        uint32_t blen = *(const uint32_t*)(data + offset + 0);
        uint32_t btype = *(const uint32_t*)(data + offset + 4);
        if (btype == 0x004E4942)
            {
            *bin_size = blen;
            *bin_offset = offset + 8;
            }
        }
    return 1;
}

std::vector<byte> GltfImporter::base64_decode(const char* str, size_t len)
{
    static const int kDec[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1, 0,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    std::vector<byte> out;
    int val = 0;
    int valb = -8;
    for (size_t i = 0; i < len; ++i)
        {
        unsigned char c = (unsigned char)str[i];
        if (c == '=')
            break;
        int d = kDec[c];
        if (d == -1)
            continue;
        val = (val << 6) | d;
        valb += 6;
        if (valb >= 0)
            {
            out.push_back((byte)((val >> valb) & 0xFF));
            valb -= 8;
            }
        }
    return out;
}

ImportedModel GltfImporter::parse_json(const char* json_str)
{
    ImportedModel model;
    model.name = "gltf";
    report_progress("done", 1.0f);
    return model;
}

std::vector<byte> GltfImporter::resolve_buffer(int buffer_index)
{
    if (buffer_index < 0)
        return std::vector<byte>();
    if ((size_t)buffer_index < buffers_.size())
        return buffers_[buffer_index].data;
    if ((size_t)buffer_index < external_buffers_.size())
        {
        const auto& p = external_buffers_[buffer_index];
        std::vector<byte> out;
        out.assign(p.first, p.first + p.second);
        return out;
        }
    return std::vector<byte>();
}

AccessorData GltfImporter::read_accessor(int acc_idx)
{
    AccessorData acc;
    if (acc_idx < 0 || (size_t)acc_idx >= accessors_.size())
        return acc;
    return accessors_[acc_idx];
}

std::vector<byte> GltfImporter::read_buffer_view(const BufferViewData& bv)
{
    std::vector<byte> out;
    std::vector<byte> buf = resolve_buffer(bv.buffer);
    if (bv.byte_offset + bv.byte_length > buf.size())
        return out;
    out.assign(buf.begin() + bv.byte_offset, buf.begin() + bv.byte_offset + bv.byte_length);
    return out;
}

ImportedMesh GltfImporter::parse_mesh(int mesh_idx)
{
    ImportedMesh mesh;
    mesh.name = "mesh";
    return mesh;
}

ImportedNode GltfImporter::parse_node(int node_idx)
{
    ImportedNode node;
    node.name = "node";
    node.local_transform = Mat4();
    return node;
}

ImportedAnimation GltfImporter::parse_animation(int anim_idx)
{
    ImportedAnimation anim;
    anim.name = "anim";
    return anim;
}

Mat4 GltfImporter::parse_node_transform(int node_idx)
{
    Mat4 m;
    return m;
}

} // namespace import
} // namespace q3d
