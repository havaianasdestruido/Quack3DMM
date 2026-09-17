#include "import/assimp_fbx.h"

namespace q3d {
namespace import {

AssimpImporter::AssimpImporter() {}
AssimpImporter::~AssimpImporter() {}

bool AssimpImporter::available() const
{
    return false;
}

ImportedModel AssimpImporter::import(const char* vpath)
{
    ImportedModel model;
    model.name = "assimp unavailable";
    return model;
}

} // namespace import
} // namespace q3d
