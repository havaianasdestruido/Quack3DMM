#include "sandbox/sandbox.h"
#include "sandbox/lua_sandbox.h"

#include <cassert>
#include <string>

int main() {
    // PermissionSet semantics
    q3d::sandbox::PermissionSet grants;
    grants.grant(q3d::sandbox::Permission::FSRead);

    q3d::sandbox::PermissionSet required;
    required.grant(q3d::sandbox::Permission::FSRead);
    assert(grants.satisfies(required));

    required.revoke_all();
    required.grant(q3d::sandbox::Permission::NetOutbound);
    assert(!grants.satisfies(required));

    assert(grants.implied_trust() == q3d::sandbox::TrustTier::Workshop);

    // Sandbox basics
    q3d::sandbox::Sandbox sb("test-mod", q3d::sandbox::TrustTier::Workshop, grants);
    sb.set_memory_budget(64 * 1024 * 1024);
    sb.set_cpu_budget_ms(16);
    sb.apply();
    assert(sb.memory_budget() == 64 * 1024 * 1024);
    assert(sb.cpu_budget_ms() == 16);
    assert(sb.grants().has(q3d::sandbox::Permission::FSRead));

    // Rate limiting
    sb.set_rate_limit("spawn", 2);
    assert(sb.check_rate_limit("spawn"));
    assert(sb.check_rate_limit("spawn"));
    assert(!sb.check_rate_limit("spawn"));

    // LuaSandbox
    q3d::sandbox::lua::LuaSandbox lsb("test-mod", &sb);
    lsb.set_memory_limit(16 * 1024 * 1024);
    lsb.set_instruction_limit(100000);
    lsb.init();
    assert(lsb.state());
    assert(lsb.run_string("return 42"));
    assert(lsb.memory_used() <= 16 * 1024 * 1024);

    // Whitlelists exist
    assert(!q3d::sandbox::lua::CoreAllowedStdlib.empty());
    assert(!q3d::sandbox::lua::WorkshopAllowedStdlib.empty());

    return 0;
}