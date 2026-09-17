#include "import/image.h"
#include "import/texture.h"
#include "import/texture_baker.h"
#include <cassert>

int main()
{
    using namespace q3d::import;
    Image img;
    img.width = 4;
    img.height = 4;
    img.channels = 4;
    img.pixels.assign(4 * 4 * 4, 128);
    TextureImporter ti;
    Image n = ti.generate_normal_map(img, 1.0f);
    assert(n.width == 4 && n.height == 4);
    return 0;
}