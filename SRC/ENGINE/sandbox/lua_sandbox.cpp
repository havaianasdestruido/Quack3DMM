#include "lua_sandbox.h"

#include <new>
#include <unordered_map>

namespace q3d {
namespace sandbox {
namespace lua {

namespace {
struct AllocState {
    size_t used = 0;
    size_t limit = 64 * 1024 * 1024;
};

// Registry to let the static Lua hook find the owning sandbox for
// instruction-limit enforcement.
std::unordered_map<lua_State*, LuaSandbox*>& state_registry() {
    static std::unordered_map<lua_State*, LuaSandbox*> reg;
    return reg;
}
}

LuaSandbox::LuaSandbox(StrPtr mod_id, const Sandbox* parent_sandbox)
    : mod_id_(mod_id ? mod_id : ""), parent_sandbox_(parent_sandbox) {}

LuaSandbox::~LuaSandbox() {
    if (L_) {
        state_registry().erase(L_);
        lua_close(L_);
        L_ = nullptr;
    }
}

void LuaSandbox::init() {
    AllocState* as = new AllocState;
    as->limit = memory_limit_;
    L_ = lua_newstate(&LuaSandbox::alloc, as);
    if (!L_) {
        delete as;
        throw std::bad_alloc();
    }
    state_registry()[L_] = this;
    luaL_openlibs(L_);
    if (parent_sandbox_) {
        apply_stdlib_whitelist(L_, parent_sandbox_->tier());
    }
    block_dangerous_globals(L_);
    lua_sethook(L_, &LuaSandbox::hook, LUA_MASKCOUNT, 1000);
}

void LuaSandbox::set_memory_limit(size_t bytes) {
    memory_limit_ = bytes;
}

void LuaSandbox::set_instruction_limit(int count) {
    instruction_limit_ = count;
}

bool LuaSandbox::run_file(const char* path) {
    if (!L_) init();
    if (luaL_loadfile(L_, path) != LUA_OK) {
        return false;
    }
    if (lua_pcall(L_, 0, LUA_MULTRET, 0) != LUA_OK) {
        return false;
    }
    return true;
}

bool LuaSandbox::run_string(const char* code, const char* chunkname) {
    if (!L_) init();
    if (luaL_loadbuffer(L_, code, std::strlen(code), chunkname) != LUA_OK) {
        return false;
    }
    if (lua_pcall(L_, 0, LUA_MULTRET, 0) != LUA_OK) {
        return false;
    }
    return true;
}

void* LuaSandbox::alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
    AllocState* as = static_cast<AllocState*>(ud);
    if (!as) return nullptr;
    if (nsize == 0) {
        as->used -= osize;
        std::free(ptr);
        return nullptr;
    }
    if (ptr == nullptr) {
        if (as->used + nsize > as->limit) {
            return nullptr;
        }
        void* p = std::malloc(nsize);
        if (!p) return nullptr;
        as->used += nsize;
        return p;
    }
    if (as->used - osize + nsize > as->limit) {
        return nullptr;
    }
    void* p = std::realloc(ptr, nsize);
    if (!p) return nullptr;
    as->used = as->used - osize + nsize;
    return p;
}

void LuaSandbox::hook(lua_State* L, lua_Debug* ar) {
    (void)ar;
    auto it = state_registry().find(L);
    if (it == state_registry().end())
        return;
    LuaSandbox* sb = it->second;
    ++sb->instruction_count_;
    if (sb->limits_enabled_ && sb->instruction_count_ > sb->instruction_limit_) {
        luaL_error(L, "instruction limit exceeded");
    }
}

void LuaSandbox::check_limits() {
    if (!L_) return;
    void* ud = nullptr;
    lua_getallocf(L_, &ud);
    AllocState* as = static_cast<AllocState*>(ud);
    if (as && as->used > as->limit && limits_enabled_) {
        luaL_error(L_, "memory limit exceeded");
    }
}

size_t LuaSandbox::memory_used() const {
    if (!L_) return 0;
    void* ud = nullptr;
    lua_Alloc f = lua_getallocf(L_, &ud);
    AllocState* as = static_cast<AllocState*>(ud);
    return as ? as->used : 0;
}

const unordered_set<string> CoreAllowedStdlib = {};
const unordered_set<string> WorkshopAllowedStdlib = {};
const unordered_set<string> LocalAllowedStdlib = {};

void apply_stdlib_whitelist(lua_State* L, TrustTier tier) {
    (void)L;
    (void)tier;
}

void block_dangerous_globals(lua_State* L) {
    (void)L;
}

} // namespace lua
} // namespace sandbox
} // namespace q3d
