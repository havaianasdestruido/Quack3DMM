#include "import/image.h"
#include <cassert>

int main()
{
    using namespace q3d::import;
    Image img;
    img.width = 1;
    img.height = 1;
    img.channels = 4;
    img.pixels = { 255, 0, 0, 255 };
    auto mips = img.generate_mipmaps();
    assert(mips.empty());
    ImageFormat fmt = ImageLoader::detect_format(reinterpret_cast<const unsigned char*>("\x89PNG"), 4);
    assert(fmt == ImageFormat::PNG);
    return 0;
}