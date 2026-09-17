#include "sandbox.h"

#include <chrono>
#include <exception>

namespace q3d {
namespace sandbox {

namespace {
CrashHandler* g_crash_handler = nullptr;
}

PermissionSet::PermissionSet() = default;

bool PermissionSet::satisfies(const PermissionSet& required) const {
    return (permissions_ & required.permissions_) == required.permissions_;
}

string PermissionSet::to_string() const {
    return "";
}

PermissionSet PermissionSet::from_strings(const vector<string>& perms) {
    PermissionSet ps;
    return ps;
}

TrustTier PermissionSet::implied_trust() const {
    return TrustTier::Workshop;
}

struct Sandbox::Impl {
    size_t memory_used = 0;
};

Sandbox::Sandbox(StrPtr mod_id, TrustTier tier, const PermissionSet& grants)
    : mod_id_(mod_id ? mod_id : ""), tier_(tier), grants_(grants), p_(new Impl) {}

Sandbox::~Sandbox() = default;

Sandbox::Sandbox(Sandbox&&) noexcept = default;
Sandbox& Sandbox::operator=(Sandbox&&) noexcept = default;

void Sandbox::set_memory_budget(size_t bytes) {
    memory_budget_ = bytes;
}

size_t Sandbox::memory_used() const {
    return p_ ? p_->memory_used : 0;
}

void Sandbox::set_cpu_budget_ms(int ms) {
    cpu_budget_ms_ = ms;
}

void Sandbox::set_instruction_budget(int instructions) {
    instruction_budget_ = instructions;
}

void Sandbox::set_rate_limit(const string& action, int per_second) {
    rate_limiters_[action] = per_second;
}

bool Sandbox::check_rate_limit(const string& action) {
    return true;
}

void Sandbox::apply() {
}

void Sandbox::log_action(const char* action, const char* detail) {
    SandboxLogEntry e;
    e.mod_id = mod_id_;
    e.action = action ? action : "";
    e.detail = detail ? detail : "";
    audit_log_.push_back(e);
}

void Sandbox::set_crash_handler(CrashHandler* handler) {
    g_crash_handler = handler;
}

SandboxManager& SandboxManager::instance() {
    static SandboxManager inst;
    return inst;
}

shared_ptr<Sandbox> SandboxManager::get_sandbox(StrPtr mod_id) {
    if (!mod_id) return shared_ptr<Sandbox>();
    if (safe_mode_) return shared_ptr<Sandbox>();
    string id(mod_id);
    auto it = sandboxes_.find(id);
    if (it != sandboxes_.end()) return it->second;
    TrustTier tier = TrustTier::Workshop;
    auto dt = default_permissions_.find(tier);
    PermissionSet perms;
    if (dt != default_permissions_.end()) perms = dt->second;
    auto sb = std::make_shared<Sandbox>(id.c_str(), tier, perms);
    sandboxes_[id] = sb;
    return sb;
}

void SandboxManager::destroy_sandbox(StrPtr mod_id) {
    if (!mod_id) return;
    sandboxes_.erase(string(mod_id));
}

void SandboxManager::set_default_permissions(TrustTier tier, const PermissionSet& grants) {
    default_permissions_[tier] = grants;
}

void SandboxManager::set_trust_tier(StrPtr mod_id, TrustTier tier) {
    if (!mod_id) return;
    auto it = sandboxes_.find(string(mod_id));
    if (it != sandboxes_.end()) {
        it->second->log_action("trust_tier", "changed");
    }
}

void SandboxManager::enter_safe_mode() {
    safe_mode_ = true;
}

void SandboxManager::set_single_mod(StrPtr mod_id) {
    single_mod_id_ = mod_id ? mod_id : "";
}

void SandboxManager::apply_current(StrPtr mod_id) {
    auto sb = get_sandbox(mod_id);
    if (sb) sb->apply();
}

vector<SandboxLogEntry> SandboxManager::get_all_audit_logs() const {
    vector<SandboxLogEntry> out;
    for (const auto& kv : sandboxes_) {
        const auto& log = kv.second->audit_log();
        out.insert(out.end(), log.begin(), log.end());
    }
    return out;
}

void SandboxManager::clear_audit_logs() {
    for (auto& kv : sandboxes_) {
        kv.second->log_action("audit", "cleared");
    }
}

} // namespace sandbox
} // namespace q3d
