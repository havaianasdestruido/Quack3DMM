#pragma once

#include <string>
#include <unordered_set>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "sandbox.h"

namespace q3d {
namespace sandbox {
namespace lua {

using std::string;
using std::unordered_set;

class LuaSandbox {
public:
    LuaSandbox(StrPtr mod_id, const Sandbox* parent_sandbox);
    ~LuaSandbox();
    
    lua_State* state() { return L_; }
    
    void init();
    
    void set_memory_limit(size_t bytes);
    
    void set_instruction_limit(int count);
    
    bool run_file(const char* path);
    bool run_string(const char* code, const char* chunkname = "=");
    
    static void* alloc(void* ud, void* ptr, size_t osize, size_t nsize);
    
    static void hook(lua_State* L, lua_Debug* ar);
    
    void check_limits();
    
    size_t memory_used() const;
    int instructions_executed() const { return instruction_count_; }
    
private:
    lua_State* L_ = nullptr;
    string mod_id_;
    const Sandbox* parent_sandbox_ = nullptr;
    size_t memory_limit_ = 64 * 1024 * 1024;
    int instruction_limit_ = 1000000;
    int instruction_count_ = 0;
    bool limits_enabled_ = true;
};

extern const unordered_set<string> CoreAllowedStdlib;
extern const unordered_set<string> WorkshopAllowedStdlib;
extern const unordered_set<string> LocalAllowedStdlib;

void apply_stdlib_whitelist(lua_State* L, TrustTier tier);

void block_dangerous_globals(lua_State* L);

} // namespace lua
} // namespace sandbox
} // namespace q3d
