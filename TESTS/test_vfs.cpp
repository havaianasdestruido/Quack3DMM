#include "vfs.h"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {
    q3d::vfs::VFS v;
    std::filesystem::path root = std::filesystem::current_path();
    v.mount_layer(root.string().c_str(), 0, false);

    const char* test_name = "CMakeLists.txt";
    if (!v.exists(test_name)) {
        std::cerr << "VFS cannot see CMakeLists.txt in current dir" << std::endl;
        return 1;
    }

    auto entry = v.resolve(test_name);
    if (!entry) {
        std::cerr << "VFS resolve failed" << std::endl;
        return 1;
    }

    auto s = v.open_read(test_name);
    if (!s) {
        std::cerr << "VFS open_read failed" << std::endl;
        return 1;
    }

    char buf[64] = {};
    std::size_t n = s->read(buf, sizeof(buf));
    if (n == 0) {
        std::cerr << "VFS read returned 0" << std::endl;
        return 1;
    }

    std::cout << "OK" << std::endl;
    return 0;
}
