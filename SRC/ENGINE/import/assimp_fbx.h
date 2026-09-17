#pragma once
#include "import/gltf_importer.h"

namespace q3d {
namespace import {

class AssimpImporter {
public:
    AssimpImporter();
    ~AssimpImporter();

    bool available() const;
    ImportedModel import(const char* vpath);
};

} // namespace import
} // namespace q3d
