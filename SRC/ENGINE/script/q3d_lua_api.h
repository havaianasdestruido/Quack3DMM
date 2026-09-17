#pragma once

#include <memory>
#include <string>

extern "C" {
#include <lua.hpp>
}

namespace q3d {
namespace script {

void q3d_lua_api_expose(lua_State* L);

} // namespace script
} // namespace q3d
