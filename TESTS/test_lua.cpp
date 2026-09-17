#include "script/lua_engine.h"
#include "script/script_manager.h"
#include <cassert>
#include <string>

int main() {
    // LuaHost evaluate
    auto& host = q3d::script::LuaHost::instance();

    // Create a VM via host's create_vm then evaluate
    auto vm = host.create_vm("test");
    assert(vm);

    std::string result = host.evaluate("return 2 + 3");
    assert(result == "5");

    // Direct VM execution
    assert(vm->do_string("x = 10; return x", "test"));
    assert(vm->do_string("x = 20", "test"));
    vm->gc();

    // ScriptManager runtime registration
    q3d::script::ScriptManager sm;
    sm.register_runtime("lua", nullptr);
    q3d::script::IRuntime* rt = sm.get_runtime("lua");
    assert(rt == nullptr); // registered with null runtime

    return 0;
}