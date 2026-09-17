# AGENTS.md — Quack3DMM AI Agent Instructions

> This is the canonical instruction file. CLAUDE.md and AI.md point here.

---

## Project Overview

Microsoft 3D Movie Maker (1995) open-sourced under MIT. Being modernized into a Garry's Mod-style platform for creating animated videos via mods. The codebase contains the original ~160K LOC pre-C++98 C engine (BRender renderer, Kauai UI/scripting framework, Studio app) plus a modern C++17 modding layer of 14 standalone libraries.

**Key documents** (read when deep context is needed):
- `OVERVIEW.md` — architecture, 7-layer diagram, file format table
- `ROADMAP.md` — 10-phase modernization plan
- `SOFTMODDABLE.md` — 20-pillar modding blueprint (GMod-style)
- `README.md` — original Microsoft open-source README

---

## Repository Structure

```
├── CMakeLists.txt              ← Root build (CMake 3.20+, C++17, MSVC)
├── SRC/
│   ├── ENGINE/                 ← Modern modding layer (14 static libs)
│   │   ├── CMakeLists.txt      ← All engine lib targets
│   │   ├── vfs.{h,cpp}         ← Virtual filesystem (base dependency)
│   │   ├── q3m.{h,cpp}         ← .q3m package format
│   │   ├── mod_manager.{h,cpp} ← Top-level mod loading
│   │   ├── mod/                 ← mod_core (mod.h, mod_manifest.{h,cpp})
│   │   ├── actor/              ← Actor definition, registry, skeleton/IK
│   │   ├── audio/              ← Audio engine, DSP, sound manager
│   │   ├── event/              ← Typed event bus, hook system
│   │   ├── hotreload/          ← File watcher, reload manager, asset reloader
│   │   ├── import/             ← glTF, image, texture, texture baker, FBX stub
│   │   ├── render/             ← Shader compiler, materials, render passes
│   │   ├── sandbox/            ← Permission/trust system, Lua sandbox
│   │   ├── script/             ← Lua engine, script manager, q3d Lua API
│   │   ├── tools/              ← Tool framework, 10 built-in tools
│   │   └── ui/                 ← Theme, widgets, UI manager, Lua UI bindings
│   └── STUDIO/                 ← Legacy 3dmovie app (gated by BUILD_LEGACY_ENGINE)
├── BREN/                       ← BRender 3D renderer (Argonaut, 1995)
├── kauai/                      ← Kauai UI/scripting framework
├── INC/                        ← Legacy public headers (SOC, BODY, ACTOR, etc.)
├── third_party/luasrc/         ← Vendored Lua 5.4.7
├── TESTS/                      ← 12 modern C++ test files
├── cd2/, cd3/, cd9/, cd12/     ← Original CD images / duplicate source trees
├── SETUP/                      ← Installer source
├── TOOLS/                      ← Root-level content tools (TDFMAKE, etc.)
├── OBJ/, SRC/SHARED/           ← Legacy build artifacts, shared code
├── SOFTMODDABLE.md             ← 20-pillar modding blueprint
├── ROADMAP.md                  ← 10-phase modernization roadmap
├── OVERVIEW.md                 ← Architecture reference
└── .gitignore                  ← Standard VS + build/
```

---

## Build System

**Engine**: CMake 3.20+, C++17, MSVC x64

### Configure
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

### Build (all targets)
```bash
cmake --build build --config Release
```

### Build specific target
```bash
cmake --build build --config Release --target <target_name>
```

### Run tests
```bash
ctest --test-dir build -C Release --output-on-failure
```

### Legacy build (off by default)
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_LEGACY_ENGINE=ON
```

### Build output
- Static libs: `build/lib/Release/*.lib`
- Test executables: `build/bin/Release/*.exe`

---

## Architecture

### 14 Modern Static Libraries (always built)

| Library | Dependencies | Purpose |
|---------|-------------|---------|
| `vfs` | (none) | Virtual filesystem — base of all deps |
| `q3m` | vfs | .q3m package format (pack/unpack) |
| `mod_manager` | vfs | Mod discovery, load ordering |
| `mod_core` | vfs | Mod manifest parsing (TOML), ModEntry |
| `script` | vfs, lua | Lua VM management, q3d Lua API |
| `q3d_ui` | vfs, lua | Theme, widgets, panels, Lua UI bindings |
| `event` | vfs | Typed pub/sub event bus (34 built-in events) |
| `sandbox` | vfs, lua, mod_core | Permission system, trust tiers, Lua sandbox |
| `hotreload` | vfs | File watching, hot-reload orchestration |
| `tools` | vfs | Tool framework, 10 built-in tools, hotkeys |
| `render` | vfs | Shader compiler, materials, render passes |
| `audio` | vfs | Audio engine, DSP effects, sound manager |
| `import` | vfs | glTF importer, image/texture loading, texture baker |
| `actor` | vfs | Actor definitions, registry, skeleton/IK |

### Legacy Targets (BUILD_LEGACY_ENGINE=OFF by default)
- `engine` — 16 legacy .CPP files wrapping BRender + Kauai
- `3dmovie` — The actual 3D Movie Maker app (requires chomp tool, .cht→.chk compilation)

### Dependency Graph
```
vfs (foundation)
├── q3m, mod_manager, event, hotreload, tools, render, audio, import, actor
├── mod_core → sandbox (also needs vfs, lua)
├── script → q3d_ui (both need vfs + lua)
└── engine (legacy, needs bren + kauai)
```

---

## Namespace Convention

All modern code uses `q3d` with per-subsystem sub-namespaces:

```cpp
namespace q3d {
namespace vfs { }     // VFS
namespace mod { }     // mod_core, mod_manager
namespace actor { }   // Actor system
namespace render { }  // Shader, materials
namespace ui { }      // Theme, widgets
namespace tools { }   // Tool framework
namespace script { }  // Lua engine
namespace sandbox { } // Permissions, Lua sandbox
namespace import { }  // glTF, image, texture
namespace audio { }   // Sound, DSP
namespace event { }   // Event bus, hooks
namespace hotreload { } // File watching, reload
}
namespace q3m { }     // Package format (separate top-level)
```

---

## Coding Conventions

### C++ Standard
- **C++17** required (`-std=c++17`, `CMAKE_CXX_STANDARD 17`)
- No compiler extensions (`CMAKE_CXX_EXTENSIONS OFF`)
- MSVC: `/W3` with 17 specific warning suppressions (see root CMakeLists.txt)
- Use `<filesystem>`, `std::string`, `std::vector`, `std::unordered_map`

### Header Guards
- **Always use `#pragma once`** — no include guards

### Type Aliases (defined in headers)
```cpp
using StrPtr = const char*;   // used in vfs.h, mod_manager.h, etc.
using std::string;
using std::vector;
using std::shared_ptr;
using std::unordered_map;
```

### Patterns
- **Singletons**: Use `static T& instance()` (EventBus, ShaderCompiler, ToolManager, AudioManager, UIManager, SandboxManager, ActorRegistry, etc.)
- **Virtual interfaces**: `IStream`/`OStream`, `ITool`, `IRuntime`, `DSPNode`, `IKSolver`, `RenderPass`
- **Pimpl**: Used where ABI stability needed (e.g., `Sandbox::Impl`)
- **shared_ptr**: Ownership is shared; raw pointers for non-owning references
- **Event system**: Fire with `bus.fire<EventType>(payload)` — callbacks receive the struct: `const EventType&`
- **Lua integration**: `lua.hpp` (C++ wrapper), register via `luaL_Reg` tables, expose as `q3d.*` namespaces

### Naming
- **Classes/Structs**: PascalCase (`ShaderCompiler`, `ActorDefinition`, `PermissionSet`)
- **Functions/methods**: snake_case (`set_memory_budget`, `compile_file`)
- **Member variables**: snake_case with trailing underscore (`_layers`, `memory_limit_`, `current_tool_`) or plain snake_case depending on age
- **Enums**: PascalCase values (`TrustTier::Workshop`, `ShaderStage::Vertex`)
- **Files**: snake_case (`actor_def.cpp`, `shader_compiler.h`), except legacy files which are UPPERCASE
- **Namespaces**: lowercase (`q3d::render`, `q3d::sandbox::lua`)

### Files to NEVER modify
- `cd2/`, `cd3/`, `cd9/` — original CD images, read-only archives
- `cd12/` — duplicate source tree from CD backup
- `OBJ/` — legacy build artifacts
- `INC/` — legacy public headers (pre-C++98)
- `kauai/SRC/` — legacy Kauai framework source
- `BREN/` source files — legacy BRender renderer (only modify `BREN/CMakeLists.txt` if needed)
- `SRC/STUDIO/` — legacy Studio app source
- `SRC/SHARED/` — legacy shared code
- `SETUP/` — installer source

### Files it IS expected to modify
- `SRC/ENGINE/**` — the modern modding layer
- `TESTS/**` — test files
- `CMakeLists.txt` (root and subdirectory) — build configuration
- `third_party/luasrc/CMakeLists.txt` — Lua build config (rarely)
- Documentation: `SOFTMODDABLE.md`, `ROADMAP.md`, `OVERVIEW.md`, `AGENTS.md`

---

## Testing

### Test Files (in `TESTS/`)
| Test | Source File | Links To | What It Tests |
|------|------------|----------|---------------|
| test_vfs | test_vfs.cpp | vfs | Virtual filesystem mount, resolve, list |
| test_mods | test_manifest.cpp | mod_manager, mod_core, vfs | Manifest parsing, mod loading |
| test_event | test_event_bus.cpp | event | Event bus fire/subscribe/unsubscribe/record/replay |
| test_hotreload | test_hotreload.cpp | hotreload | Hot-reload system init |
| test_render | test_shader.cpp | render | Shader compilation, validation |
| test_audio | test_audio.cpp | audio | DSP nodes, procedural sound |
| test_import | test_image.cpp | import | Image load, header parse |
| test_texture_baker | test_texture_baker.cpp | import | Texture bake (AO, curvature, ID map) |
| test_actor | test_actor_def.cpp | actor | Actor definition save/load roundtrip |
| test_script | test_lua.cpp | script, lua, mod_core | Lua VM create, execute, GC |
| test_sandbox | test_sandbox.cpp | sandbox, lua | Permission system, sandbox budgets |
| test_ui | test_ui.cpp | q3d_ui | Widget hierarchy, theme, UIManager |

### Writing Tests
- Include headers with subdir prefix: `#include "mod/mod.h"`, `#include "import/image.h"`, `#include "event/event_bus.h"`
- Use `assert()` for assertions (no test framework)
- Each test has its own `main()`
- Tests run from source dir as working directory (set via CMake `WORKING_DIRECTORY`)

### Test commands
```bash
# Build all tests
cmake --build build --config Release

# Run all via CTest
ctest --test-dir build -C Release --output-on-failure

# Run single test directly
.\build\bin\Release\test_event.exe
```

---

## Key API Details

### Event Bus
```cpp
// Subscribe — callback receives the event struct
bus.on<SceneLoaded>("mymod", [](const SceneLoaded& e) { ... });

// Fire — event struct is the payload
bus.fire<SceneLoaded>({scene_path, frame_count});

// 34 built-in event types defined via Q3D_EVENT macro in event_bus.h
```

### Sandbox / Trust Tiers
```cpp
enum class TrustTier { Core = 0, Workshop = 1, Local = 2, Dev = 3 };
// Core: most restricted; Dev: least restricted

Sandbox sb("mymod", TrustTier::Workshop, grants);
sb.set_memory_budget(64 * 1024 * 1024);
sb.set_cpu_budget_ms(16);
sb.apply();
```

### VFS
```cpp
VFS vfs;
vfs.mount_layer("C:/mods/mymod", 100);
auto entry = vfs.resolve("models/character.glb");
auto stream = vfs.open_read("textures/diffuse.png");
```

### Lua Scripting
```cpp
LuaHost& host = LuaHost::instance();
auto vm = host.create_vm("mymod");
vm->do_string("q3d.actor.create('hero')");
vm->gc(0);  // force GC
```

---

## Known Technical Debt

1. **Stub implementations**: Many .cpp files are skeletal (e.g., `lua_ui.cpp` — all 35 Lua functions return 0; `gltf_to_engine.cpp` — conversion calls empty helpers; `assimp_fbx.cpp` — returns "assimp unavailable")
2. **No GPU**: All rendering/shader code is CPU-side only. No OpenGL/Vulkan/D3D integration.
3. **Windows-only**: `file_watcher.cpp` uses Win32 APIs (`FindFirstChangeNotification`). No cross-platform file watching.
4. **No linting/formatting**: No `.clang-format`, `.clang-tidy`, or `.editorconfig` configured.
5. **No CI/CD**: No GitHub Actions or similar.
6. **Duplicate source trees**: `cd12/` mirrors root `SRC/`, `cd3/3DMOVIE/` mirrors `cd9/`. Do not modify these.
7. **Legacy code**: Original pre-C++98 code in `INC/`, `SRC/STUDIO/`, `SRC/SHARED/`, `BREN/`, `kauai/` uses UPPERCASE filenames, `.CHT` script files, and NMake builds.

---

## Git Workflow

- **Branch**: `main`
- **Commit style**: Conventional Commits (`feat:`, `fix:`, etc.)
- **Do NOT commit**: `build/` directory (in .gitignore), `cd*` directories, legacy build artifacts
- **PR**: Create against `main`, include test results
- **3 commits total** in repo history

---

## Common Pitfalls

1. **Include paths**: Modern engine headers use subdir prefix (`mod/mod.h`, not `mod.h`). Tests and cross-library includes must match this pattern.
2. **MSVC warnings**: 17 warnings are globally suppressed. If you add new warnings, check the suppression list in root `CMakeLists.txt`.
3. **Lua 5.4.7**: Vendored at `third_party/luasrc/`. Use `extern "C" { #include "lua.h" }` in C++ or `#include "lua.hpp"`.
4. **Event callbacks**: Must match the event struct type exactly. `bus.on<T>(...)` callback signature is `void(const T&)`, not `void(const typename T::value_type&)`.
5. **Sandbox pimpl**: `Sandbox::Impl` is defined in `sandbox.cpp`. The dtor, move ctor, and move assign MUST be defined out-of-line in the .cpp (header declares them, .cpp defines them).
6. **Widget hierarchy**: `Widget::build()` and `Widget::measure()` are NOT pure virtual — they have default implementations in `widget.cpp`. Subclasses can override but don't have to.
7. **FILE_ACTION_REMOVED**: Win32 uses `FILE_ACTION_REMOVED`, not `FILE_ACTION_DELETED` (does not exist).
8. **M_PI**: Not guaranteed by MSVC. Use `#ifndef M_PI` fallback or `<cmath>` with `_USE_MATH_DEFINES`.

---

## Validation Checklist Before Finishing a Task

- [ ] Build succeeds: `cmake --build build --config Release` (no errors)
- [ ] All tests pass: `ctest --test-dir build -C Release` (12/12)
- [ ] No new warnings beyond existing suppressions
- [ ] Headers use `#pragma once`
- [ ] New code follows namespace/subdir convention (`q3d::subsystem`)
- [ ] New files added to appropriate CMakeLists.txt `target_sources()`
- [ ] No legacy SOC/Kauai/BRender dependencies leaked into modern engine code
- [ ] No secrets, keys, or credentials committed
- [ ] `build/` directory not staged for commit
