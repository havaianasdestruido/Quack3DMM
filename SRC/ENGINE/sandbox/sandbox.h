#pragma once

#include <bitset>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "mod/mod_manifest.h"

namespace q3d {
namespace sandbox {

using std::string;
using std::vector;
using std::unordered_map;
using std::unique_ptr;
using std::shared_ptr;

using StrPtr = const char*;

struct SandboxLogEntry {
    string mod_id;
    string action;
    string detail;
};

struct CrashHandler {
    virtual ~CrashHandler() = default;
    virtual void on_crash(const string& mod_id, const string& reason) = 0;
};

// Permission tokens that a mod can request
enum class Permission {
    FSRead,          // Read from VFS
    FSWriteUser,     // Write to user directory
    FSWriteEngine,   // Write to engine directory
    FSWriteAny,      // Write anywhere (dangerous)
    NetOutbound,     // Make outbound network connections
    NetInbound,      // Accept inbound connections
    ProcessSpawn,    // Spawn subprocesses
    NativeLoad,      // Load native .dll/.so
    RenderFullscreen,// Use fullscreen rendering
    AudioCapture,    // Capture microphone
    DisplayExternal, // Use external displays
    Debugger,        // Attach debugger
};

// Trust tiers
enum class TrustTier {
    Core = 0,      // Engine-shipped, fully trusted
    Workshop = 1, // Sandbox by default
    Local = 2,    // User-installed, untrusted
    Dev = 3,      // Full power, dev mode only
};

class PermissionSet {
public:
    PermissionSet();
    
    bool has(Permission p) const { return permissions_.test(static_cast<size_t>(p)); }
    void grant(Permission p) { permissions_.set(static_cast<size_t>(p)); }
    void revoke(Permission p) { permissions_.reset(static_cast<size_t>(p)); }
    void revoke_all() { permissions_.reset(); }
    
    bool satisfies(const PermissionSet& required) const;
    
    string to_string() const;
    static PermissionSet from_strings(const vector<string>& perms);
    
    TrustTier implied_trust() const;
    
private:
    std::bitset<64> permissions_{};
};

class Sandbox {
public:
    Sandbox(StrPtr mod_id, TrustTier tier, const PermissionSet& grants);
    ~Sandbox();

    Sandbox(Sandbox&&) noexcept;
    Sandbox& operator=(Sandbox&&) noexcept;

    Sandbox(const Sandbox&) = delete;
    Sandbox& operator=(const Sandbox&) = delete;
    
    template<typename F>
    auto run(F&& fn) -> decltype(fn());
    
    void set_memory_budget(size_t bytes);
    size_t memory_used() const;
    size_t memory_budget() const { return memory_budget_; }
    
    void set_cpu_budget_ms(int ms);
    int cpu_budget_ms() const { return cpu_budget_ms_; }
    
    void set_instruction_budget(int instructions);
    
    void set_rate_limit(const string& action, int per_second);
    bool check_rate_limit(const string& action);
    
    void apply();
    
    void log_action(const char* action, const char* detail);
    const vector<SandboxLogEntry>& audit_log() const { return audit_log_; }
    
    static void set_crash_handler(CrashHandler* handler);
    
    const PermissionSet& grants() const { return grants_; }
    TrustTier tier() const { return tier_; }
    const string& mod_id() const { return mod_id_; }
    
private:
    string mod_id_{};
    TrustTier tier_{};
    PermissionSet grants_{};
    size_t memory_budget_ = 256 * 1024 * 1024;
    int cpu_budget_ms_ = 50;
    int instruction_budget_ = 1000000;
    unordered_map<string, int> rate_limiters_{};
    vector<SandboxLogEntry> audit_log_{};
    
    struct Impl;
    unique_ptr<Impl> p_{};
};

class SandboxManager {
public:
    static SandboxManager& instance();
    
    shared_ptr<Sandbox> get_sandbox(StrPtr mod_id);
    void destroy_sandbox(StrPtr mod_id);
    
    void set_default_permissions(TrustTier tier, const PermissionSet& grants);
    void set_trust_tier(StrPtr mod_id, TrustTier tier);
    
    void enter_safe_mode();
    bool in_safe_mode() const { return safe_mode_; }
    
    void set_single_mod(StrPtr mod_id);
    
    void apply_current(StrPtr mod_id);
    
    vector<SandboxLogEntry> get_all_audit_logs() const;
    void clear_audit_logs();
    
private:
    SandboxManager() = default;
    
    unordered_map<string, shared_ptr<Sandbox>> sandboxes_{};
    unordered_map<TrustTier, PermissionSet> default_permissions_{};
    bool safe_mode_ = false;
    string single_mod_id_{};
};

} // namespace sandbox
} // namespace q3d
