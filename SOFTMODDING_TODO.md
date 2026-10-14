# SOFTMODDING_TODO.md — Phased Plan for Full Softmodding Support

> **What this file is**: the single, ordered checklist of everything that must be
> built before Quack3DMM can honestly claim *full softmodding support* — a stranger
> installing the app, dropping a mod in a folder, and having models, textures,
> sounds, scripts, UI panels, tools and shaders all load and hot-reload without
> ever recompiling the engine.
>
> **How it relates to the other docs**:
>
> | Doc | Answers |
> |---|---|
> | [`SOFTMODDABLE.md`](SOFTMODDABLE.md) | *What* full softmodding looks like when it is done (20-pillar vision). |
> | [`ROADMAP.md`](ROADMAP.md) | *How the 1995 engine gets modernized* (10-phase engine work). |
> | [`OVERVIEW.md`](OVERVIEW.md) | *How the codebase is organized today*. |
> | **`SOFTMODDING_TODO.md` (this file)** | *What is still missing, in what order, with what exit criteria.* |

---

## Table of Contents

- [How to use this file](#how-to-use-this-file)
- [Definition of Done: what "full softmodding support" means](#definition-of-done-what-full-softmodding-support-means)
- [Where we stand today (audit snapshot)](#where-we-stand-today-audit-snapshot)
- [Cross-cutting rules for every subsystem](#cross-cutting-rules-for-every-subsystem)
- [Phase index](#phase-index)
- [Stage A — Load and run mods](#stage-a--load-and-run-mods)
  - [Phase 0 — Ground truth: a host process and a build you can trust](#phase-0--ground-truth-a-host-process-and-a-build-you-can-trust)
  - [Phase 1 — Virtual File System](#phase-1--virtual-file-system)
  - [Phase 2 — Mod packages (`.q3m`) and the `mod.toml` contract](#phase-2--mod-packages-q3m-and-the-modtoml-contract)
  - [Phase 3 — Mod discovery, dependencies, load order, lifecycle](#phase-3--mod-discovery-dependencies-load-order-lifecycle)
  - [Phase 4 — The Lua runtime and a real `q3d.*` API](#phase-4--the-lua-runtime-and-a-real-q3d-api)
  - [Phase 5 — Events and hooks fired from real code paths](#phase-5--events-and-hooks-fired-from-real-code-paths)
- [Stage B — Everything becomes content](#stage-b--everything-becomes-content)
  - [Phase 6 — Content import pipelines](#phase-6--content-import-pipelines)
  - [Phase 7 — Actors, costumes, skeletons and animation](#phase-7--actors-costumes-skeletons-and-animation)
  - [Phase 8 — Scenes, saves, undo/redo and determinism](#phase-8--scenes-saves-undoredo-and-determinism)
  - [Phase 9 — Rendering: GPU backend, materials, shaders, passes](#phase-9--rendering-gpu-backend-materials-shaders-passes)
  - [Phase 10 — Audio: device backend, DSP graph, sound banks](#phase-10--audio-device-backend-dsp-graph-sound-banks)
- [Stage C — Live, authorable, and safe](#stage-c--live-authorable-and-safe)
  - [Phase 11 — UI, themes and HUD](#phase-11--ui-themes-and-hud)
  - [Phase 12 — Tools and editor extensions](#phase-12--tools-and-editor-extensions)
  - [Phase 13 — Hot-reload and live editing](#phase-13--hot-reload-and-live-editing)
  - [Phase 14 — Sandbox enforcement and crash containment](#phase-14--sandbox-enforcement-and-crash-containment)
- [Stage D — Ship it](#stage-d--ship-it)
  - [Phase 15 — Bridge to the legacy 3D Movie Maker app](#phase-15--bridge-to-the-legacy-3d-movie-maker-app)
  - [Phase 16 — Devkit, CLI and example mods](#phase-16--devkit-cli-and-example-mods)
  - [Phase 17 — Distribution, versioning and mod health](#phase-17--distribution-versioning-and-mod-health)
- [Traceability matrix](#traceability-matrix)
- [Explicitly out of scope](#explicitly-out-of-scope)

---

## How to use this file

- `[ ]` open · `[~]` in progress · `[x]` done · `[-]` deliberately dropped (with a one-line reason).
- Every task carries a rough size: **S** ≈ ≤ 1 day, **M** ≈ 1 week, **L** ≈ 1 month (one engineer). These are
  ordering aids, not commitments.
- Phases are strictly ordered. Each has **Exit criteria** written so they can be checked off by
  running something, not by reading code.
- **Update rule**: when a task is merged, tick it here in the same PR. If a phase's exit criteria are met,
  move the phase's status in the [Phase index](#phase-index). Do not let this file drift from the code.
- A task is only "done" when it satisfies the [cross-cutting rules](#cross-cutting-rules-for-every-subsystem)
  for the subsystem it touches.

---

## Definition of Done: what "full softmodding support" means

The north star, written as things a stranger can verify on a clean machine with no compiler:

- [ ] **N1** Dropping a folder with a `mod.toml` into `mods/` is enough — the mod is discovered, ordered and loaded.
- [ ] **N2** A mod can ship assets (models, textures, sounds, fonts, backgrounds), scripts, UI panels, tools, materials and shaders — all of them load from inside the package.
- [ ] **N3** Nothing requires recompiling or relinking the engine; `.cpp` edits are never needed to ship a mod.
- [ ] **N4** Every mod-owned asset hot-reloads on save, without restarting the app (scripts, images, models, sounds, UI, shaders, themes, scenes).
- [ ] **N5** Untrusted (downloaded) mods run sandboxed: no disk, network, process or native load outside declared grants; budgets (memory/CPU/instructions) are enforced, not advisory.
- [ ] **N6** A misbehaving mod is contained: it can crash or hang its own runtime, the engine survives, and the user gets a one-click disable.
- [ ] **N7** Load order is deterministic and explicit (dependencies, conflicts, `load_after`/`load_before`), reproducible from a `mods.lock`.
- [ ] **N8** Same mod set + same inputs ⇒ byte-identical rendered movie output (determinism).
- [ ] **N9** Deleting the mod folder restores stock behaviour exactly — no residue in caches, saves or user config.
- [ ] **N10** The author path is real: `q3m` CLI (`init`, `validate`, `pack`, `run`), typed Lua API definitions, and example mods that work.
- [ ] **N11** It works in the actual 3D Movie Maker application (BRender viewport, Studio timeline, Theatre export) — not only in a headless test host.
- [ ] **N12** A mod written against the documented API keeps loading across engine upgrades (versioned schemas, deprecation cycles).

---

## Where we stand today (audit snapshot)

Audited at commit `f48ec0c`. Modern layer: **14 static libraries** in `SRC/ENGINE` (~8.5K LOC),
**12 CTest executables**, vendored Lua 5.4.7. Legacy engine (`BREN/`, `kauai/`, `SRC/STUDIO/`) is
**off by default** (`BUILD_LEGACY_ENGINE=OFF`) and untouched by the modern layer.

| Subsystem | Library | State | Evidence | Lands in |
|---|---|---|---|---|
| Host / runtime | — | **Missing** | Root `CMakeLists.txt` builds libs + tests only; nothing links them into a runnable app | Phase 0 |
| VFS | `vfs` | **Skeleton** | Directory layers only; `resolve`/`list` *skip* archive layers; `open_write` writes into `_layers.front()` (a read layer); `add_watcher` stores callbacks that are never invoked | Phase 1 |
| Packages (`.q3m`) | `q3m` | **Stub** | `PackageWriter::pack/unpack` are `fs::copy`; `Package::contains()` knows only `mod.toml`; `Package::open()` always returns `nullptr` | Phase 2 |
| Manifest | `mod_core` | **Good** | `ModManifest` covers the full pillar-1.2 schema; `ManifestParser::parse/lint/validate` exist | Phase 2 (wire-in) |
| Mod discovery | `mod_manager` | **Skeleton** | `scan()` reads folder *names* from `./mods`, never reads `mod.toml`; `resolve_load_order()` is a `sort` by `load_priority` — no dependencies, conflicts, topological order, profiles or lockfile | Phase 3 |
| Scripting | `script` | **Stub API** | All **32** `q3d.*` functions in `q3d_lua_api.cpp` return `nil`/no-op; `LuaVM::_load_file` uses `fopen` (bypasses VFS); `LuaHost::check_and_reload()` body is a no-op; `ScriptManager::init()` empty | Phase 4 |
| Sandboxing | `sandbox` | **Header-only in practice** | `LuaSandbox` has a real allocator + instruction hook, but is used *only by its unit test*; `apply_stdlib_whitelist`/`block_dangerous_globals` empty; `Sandbox::apply()` empty; `LuaHost::create_vm` uses the unsandboxed VM with `luaL_openlibs` | Phase 4 (unify), Phase 14 (enforce) |
| Events / hooks | `event` | **Works, unwired** | Bus (39 `Q3D_EVENT` types), record/replay and before/after/replace hooks are implemented — but `fire<...>` appears nowhere outside the library and `HookSystem::call_hook` is never called. Only `TESTS/test_event_bus.cpp` uses it | Phase 5 |
| Hot reload | `hotreload` | **Plumbing without effect** | `ReloadManager::process_reload()` is an empty dispatch point; every `*Reloader::reload()` is a no-op; `file_watcher.cpp` is Win32-only; script "state" snapshot only writes a marker string | Phase 13 |
| Import | `import` | **Partial** | `image.cpp` (298 LOC) and `gltf_importer.cpp` parse; `GltfToEngineConverter::convert()` calls four **empty** builders, so a parsed glTF never becomes an actor; `AssimpImporter::available()` returns `false` | Phase 6 |
| Actors | `actor` | **Skeleton** | `ActorDefinition` save/load + registry search exist; `instantiate_body` returns `nullptr`, `instantiate_actor` / `CostumeManager::apply_costume` do nothing, `validate_compatibility` always passes, `Retargeter::retarget` returns the input unchanged | Phase 7 |
| Render | `render` | **CPU mock** | `ShaderProgram` routes uniforms through `fake_upload_uniform`; no GPU backend; `RenderPassManager::setup/cleanup` empty; `Material::hot_reload_changed` empty | Phase 9 |
| Audio | `audio` | **DSP only** | `dsp.cpp` is the most complete subsystem; `AudioEngine::init/shutdown/update`, `Sound::seek/set_on_marker`, `SoundManager::load_bank/unload_bank/reload_bank/stop_recording` are all empty; no device backend | Phase 10 |
| UI | `q3d_ui` | **Stub** | All **36** functions in `lua_ui.cpp` `return 0;` and `ui_functions[]` is an empty table; `UIManager::load_theme`, `reload_*`, layout save/load, accessibility and `show_inspector` are empty | Phase 11 |
| Tools | `tools` | **Skeleton** | `ToolManager` (hotkeys, palette, context menus, command palette) is largely real; `ToolContext`, gizmo, overlay, undo are empty; of the 10 built-in tools only `SelectTool::on_mouse_down` has behaviour; `struct MvMovie/MvScene/MvActor` are forward-declared and **defined nowhere** | Phase 12 |
| Legacy integration | `engine` | **None** | Modern libs never call into `BREN`/`kauai`/`SRC/STUDIO`; the Studio app still only builds through the MSVC 2.0-era NMake path (`ROADMAP.md` Phase 0 is open) | Phase 15 |
| Tooling / CI | — | **Missing** | No `q3m` CLI, no mod template, no `.clang-format`/`.clang-tidy`, no CI workflow, no Linux/macOS support | Phase 0, Phase 16 |

**The short version**: the *shapes* are all there (headers, types, registries, tests) but the data path
from "a mod folder exists" to "a mod is running inside a renderer" is missing end to end. Phases 0–5
close that path; phases 6–14 make every subsystem moddable; phases 15–17 make it a product.

---

## Cross-cutting rules for every subsystem

Applies to any subsystem you touch. A feature that skips these is not "done" in this project.

- [ ] **R1 — VFS-only I/O.** No subsystem calls `fopen`/`std::filesystem` on asset paths directly; everything goes through `q3d::vfs` with a `q3d://` virtual path. (Today `LuaVM::_load_file` violates this.)
- [ ] **R2 — Registry, not switch.** Subsystems expose a registry/factory so mods can add implementations; no hardcoded `if (ext == ".glb")` chains in core code.
- [ ] **R3 — Events on every state change.** Load, unload, mutate, error ⇒ fire the matching `Q3D_EVENT` and offer a `HookSystem` point.
- [ ] **R4 — Lua-reachable.** Every public operation is exposed through `q3d.*` and typed for LuaLS.
- [ ] **R5 — Hot-reloadable.** The subsystem registers its assets with `hotreload::ReloadManager` and can be rebuilt in place.
- [ ] **R6 — Headless-testable.** Pure logic is reachable from a `TESTS/test_*.cpp` with `assert()`, no window, no GPU.
- [ ] **R7 — No legacy leakage.** New modern code must not include legacy `INC/`, `kauai/` or `BREN/` headers; bridges live in Phase 15's adapter layer.
- [ ] **R8 — Sandbox-aware.** Any I/O or allocation entry point checks the calling mod's `PermissionSet` and budget before acting.

---

## Phase index

| # | Phase | Goal | Gate (one line) | Size |
|---|---|---|---|---|
| 0 | [Ground truth](#phase-0--ground-truth-a-host-process-and-a-build-you-can-trust) | A host binary that boots every layer, plus CI | `q3d --headless --mod demo` boots, loads a mod, exits 0 | M |
| 1 | [Virtual File System](#phase-1--virtual-file-system) | Real layered VFS with archives, scheme, writes, watchers | A mod reads/writes assets through `q3d://` only | L |
| 2 | [Packages + manifest](#phase-2--mod-packages-q3m-and-the-modtoml-contract) | `.q3m` is a real archive; manifest contract frozen | `q3m pack` → mount → read, streaming, no extraction | M |
| 3 | [Mod lifecycle](#phase-3--mod-discovery-dependencies-load-order-lifecycle) | Discovery, deps, conflicts, order, profiles, lockfile | Load order is deterministic and reproducible | M |
| 4 | [Lua runtime](#phase-4--the-lua-runtime-and-a-real-q3d-api) | One sandboxed VM per mod; real `q3d.*` surface | A Lua mod moves an actor and reads an asset end to end | L |
| 5 | [Events & hooks](#phase-5--events-and-hooks-fired-from-real-code-paths) | The bus is actually used by the engine | 39 events have a documented emitter; mods can cancel hooks | M |
| 6 | [Import pipelines](#phase-6--content-import-pipelines) | glTF/image/font/audio → engine content, extensible | Drop a `.glb` → new actor template, scriptable | L |
| 7 | [Actors & animation](#phase-7--actors-costumes-skeletons-and-animation) | Actors are data; costumes, rigs, actions moddable | A mod ships a character with 3 actions and it plays | L |
| 8 | [Scenes & saves](#phase-8--scenes-saves-undoredo-and-determinism) | Scene format, migrations, deterministic output | Save → reload → export, byte-identical across runs | L |
| 9 | [Rendering](#phase-9--rendering-gpu-backend-materials-shaders-passes) | Real GPU path; materials/shaders/passes moddable | A mod-authored shader renders a custom pass | L |
| 10 | [Audio](#phase-10--audio-device-backend-dsp-graph-sound-banks) | Device backend + executed DSP graph | A mod ships a sound bank and a DSP chain that mixes | M |
| 11 | [UI](#phase-11--ui-themes-and-hud) | Widgets render; themes and panels are data | A Lua mod builds a working panel at runtime | L |
| 12 | [Tools](#phase-12--tools-and-editor-extensions) | Mods register and drive editor tools | A Lua-defined tool selects and moves an actor | M |
| 13 | [Hot reload](#phase-13--hot-reload-and-live-editing) | Watch → rebuild, everywhere, cross-platform | Editing a script swaps behaviour within one frame | M |
| 14 | [Sandbox hardening](#phase-14--sandbox-enforcement-and-crash-containment) | Enforcement, permissions, containment | A hostile mod is stopped by policy, not by luck | L |
| 15 | [Legacy bridge](#phase-15--bridge-to-the-legacy-3d-movie-maker-app) | Mods reach BRender, Studio, TAGMAN, CHT | Mods change the real app: props, actors, tools, UI | XL |
| 16 | [Devkit & CLI](#phase-16--devkit-cli-and-example-mods) | `q3m` toolchain, examples, docs, types | `q3m init && q3m run` yields a running mod | M |
| 17 | [Distribution & health](#phase-17--distribution-versioning-and-mod-health) | Mod manager, versioning, CI for mods | Mods keep loading across engine versions | L |

---

## Stage A — Load and run mods

### Phase 0 — Ground truth: a host process and a build you can trust

**Goal**: one executable that boots every modern library in order, so that every later phase has
something runnable to integrate against. Today there is no app at all — only test binaries.

**Why first**: Phases 1–5 all end in "the host does X". Without the host they cannot be verified.

#### 0.1 Host application
- [ ] **0.1.1** Add `q3d_host` target (CMake) producing `q3d` (console/headless first, window later). **S**
- [ ] **0.1.2** `main()` boot order: parse CLI → mount VFS layers → construct `ModManager` → `EventBus::instance()` → `ActorRegistry`/`SoundManager`/`ToolManager`/`UIManager` → `SandboxManager` → `ScriptManager` → load mods → enter tick loop → clean shutdown. **M**
- [ ] **0.1.3** CLI surface: `--headless`, `--mod <path>`, `--mods-dir <dir>`, `--profile <name>`, `--validate`, `--version`, `--tick <n>`, `--script <file>`, `--log-level <lvl>`, `--safe-mode`. **S**
- [ ] **0.1.4** Fixed-step deterministic tick loop (`--fixed-dt`) with `TickPre`/`TickPost` fired around it; `--tick N` runs N frames and exits 0 (CI-friendly). **M**
- [ ] **0.1.5** Structured logging service (`q3d::log`) with levels, mod-id tagging, and a redirectable sink; replace every `(void)x; // silently ignore` in the modding layer. **S**
- [ ] **0.1.6** Exit code contract: `0` ok, `1` mod error, `2` missing dependency, `3` manifest invalid, `4` sandbox violation. **S**
- [ ] **0.1.7** Wire `q3d_host` into `CMakeLists.txt` as an `add_test` smoke target (`q3d --headless --tick 1`). **S**

#### 0.2 Build, test and dev environment
- [ ] **0.2.1** Add `.clang-format`, `.clang-tidy`, `.editorconfig`; wire format check into the build. **S**
- [ ] **0.2.2** GitHub Actions: Windows (MSVC 2022) + Linux (GCC 14) + macOS (Clang 18), build matrix + `ctest`. **M**
- [ ] **0.2.3** Make the modern layer build on Linux/macOS: guard the Win32-only watcher (Phase 13) behind `IFileWatcher` with a polling fallback so CI is green everywhere. **M**
- [ ] **0.2.4** Sanitizer job (ASan/UBSan) for the modern layer. **S**
- [ ] **0.2.5** `compile_commands.json` + CMake presets (`dev`, `release`, `asan`) for IDE/LSP. **S**
- [ ] **0.2.6** Extend the existing 12 tests to cover the host boot path (`test_host_boot`). **S**

#### 0.3 Freeze the contracts (before writing features)
- [ ] **0.3.1** Write the **Mod Contract** doc: a mod is a directory or `.q3m` containing `mod.toml`; nothing else is legal. **S**
- [ ] **0.3.2** Freeze `mod.toml` `schema_version = 2` fields and the deprecation policy (additive-only, 2-year deprecation window). **S**
- [ ] **0.3.3** Freeze the `q3d://` path scheme and the `q3d.*` namespace layout; add `docs/API_STABILITY.md` with Stable/Beta/Experimental badges. **S**
- [ ] **0.3.4** Decide and document the renderer target for Phase 9 (D3D11-first vs. cross-platform GL/Vulkan) — this decision gates Phase 9 and Phase 15. **S**

**Exit criteria**
- [ ] `cmake -S . -B build && cmake --build build` succeeds on Windows, Linux and macOS.
- [ ] `ctest` is green (existing 12 + new smoke tests).
- [ ] `./q3d --headless --tick 1` boots every subsystem and exits `0`.
- [ ] CI runs on every push and posts test results.

**Pillars**: 0 (all), 20 (CI baseline). **Touches**: root `CMakeLists.txt`, new `SRC/HOST/`.

---

### Phase 1 — Virtual File System

**Goal**: a real layered VFS — directory *and* archive layers, a virtual path scheme, a dedicated
write layer, working watchers, and introspection — so that no other subsystem ever touches the disk directly.

**Why now**: every later phase loads assets "through the VFS". Today `vfs.cpp` handles directories only
and explicitly skips archive layers.

#### 1.1 Layer stack
- [ ] **1.1.1** Define the path scheme: `q3d://base/…`, `q3d://mod/<modid>/…`, `q3d://user/…`, `q3d://package/<modid>/…`. Normalize separators, reject `..` escapes, reject absolute paths. **M**
- [ ] **1.1.2** `Layer` becomes a real interface (`IDirLayer`, `IArchiveLayer`, `IWritableLayer`) with `resolve`, `exists`, `list`, `stat`, `open_read`. **M**
- [ ] **1.1.3** Deterministic ordering: explicit priority, then registration order; ties broken by mod id (never by directory iteration order). **S**
- [ ] **1.1.4** Separate **write layer** (`user://` only, sandboxed root) instead of writing into `_layers.front()`. **S**
- [ ] **1.1.5** `stat` support (size, mtime, content hash) — needed by the watcher, hot-reload and the lockfile. **S**
- [ ] **1.1.6** Thread safety: shared mutex around the layer vector; streams are independent of the lock. **S**
- [ ] **1.1.7** `list()` returns *merged* results across layers with per-layer provenance (which mod provided a path) — conflict UI depends on it. **S**

#### 1.2 Archive layers
- [ ] **1.2.1** Implement `IArchiveLayer` over a real ZIP reader (miniz/zipper or `libzip`) and wire it into `resolve`/`list` (currently `continue`d). **M**
- [ ] **1.2.2** Central directory index cached in memory; lazy per-entry inflate for large assets. **M**
- [ ] **1.2.3** Support compressed entries: deflate (mandatory), zstd (optional build flag). **S**
- [ ] **1.2.4** `.q3m` mounted as an archive layer transparently (Phase 2 builds the writer; VFS provides the reader). **M**

#### 1.3 Introspection, mounting and watching
- [ ] **1.3.1** `mount()` / `unmount()` at runtime, including user folders (`~/Pictures/campaign/`) and archive files. **S**
- [ ] **1.3.2** Wire `add_watcher` callbacks to a change queue fed by `hotreload` (Phase 13); watchers currently never fire. **M**
- [ ] **1.3.3** Preload/progress hints: `preload = ["textures/"]` from the manifest reports bytes for loading screens. **S**
- [ ] **1.3.4** Content-addressable store for deduped base assets (sha256 keyed), per pillar 1.1.5. **M**

#### 1.4 Enforce the rules
- [ ] **1.4.1** Route `LuaVM::_load_file` (currently `fopen`) through the VFS — first violation of rule **R1**. **S**
- [ ] **1.4.2** Grep-sweep `SRC/ENGINE` for direct filesystem access outside `vfs.cpp`; add a CI check that fails the build on new violations. **S**
- [ ] **1.4.3** Permission check hook: VFS consults the caller's `PermissionSet` (`FSRead`, `FSWriteUser`, …) — the seam Phase 14 tightens. **M**

**Exit criteria**
- [ ] A `.q3m` and a plain folder mounted at the same virtual path resolve identically from C++ and from Lua.
- [ ] `q3d.vfs.list("textures/")` returns merged, provenance-tagged entries.
- [ ] Writing only ever touches the `user://` layer; a test asserts an engine-dir write is refused.
- [ ] `TESTS/test_vfs.cpp` covers archives, scheme normalization, escape rejection and write isolation.

**Pillars**: 1.1. **Touches**: `SRC/ENGINE/vfs.{h,cpp}`, new `SRC/ENGINE/vfs/` (split per interface).

---

### Phase 2 — Mod packages (`.q3m`) and the `mod.toml` contract

**Goal**: `.q3m` becomes a real, streaming, signed archive format, and `mod.toml` becomes a validated,
versioned contract that the engine refuses to guess at.

#### 2.1 Real archive format
- [ ] **2.1.1** `PackageWriter::pack/unpack` implemented over the ZIP writer (replace the `fs::copy` placeholders). **M**
- [ ] **2.1.2** Canonical layout: `/mod.toml`, `/assets/`, `/scripts/`, `/ui/`, `/shaders/`, `/tools/`, `/patches/`, `/locale/`. Reject unexpected top-level entries with a helpful error. **S**
- [ ] **2.1.3** Streaming reads for large entries (video, textures) without full extraction. **M**
- [ ] **2.1.4** Compression selection per entry type; store-only for already-compressed media. **S**
- [ ] **2.1.5** Deterministic archive: sorted entries, fixed timestamps, so packing twice yields identical bytes. **S**
- [ ] **2.1.6** ed25519 signature + `verify` (signature stored in `META-INF/`); unsigned mods allowed only for local tiers. **M**
- [ ] **2.1.7** Delta packages: a `.q3m` patch applied over a base `.q3m`. **M** (stretch)
- [ ] **2.1.8** Optional encryption for premium content (key fetched at install). **M** (stretch)

#### 2.2 Manifest contract
- [ ] **2.2.1** Wire `ManifestParser::parse_file` into mod discovery — nothing reads manifests today. **S**
- [ ] **2.2.2** Make `lint()` produce the friendly suggestions it advertises (`"dependencys" → "dependencies"`), with line numbers. **M**
- [ ] **2.2.3** `validate()` returns blocking errors vs. warnings; host refuses to load on blocking errors with a precise message. **S**
- [ ] **2.2.4** `ManifestWriter::to_toml` round-trip test (`parse(write(parse(x))) == parse(x)`). **S**
- [ ] **2.2.5** Schema documentation generated from the parser, published with the CLI (Phase 16). **S**
- [ ] **2.2.6** Semver ranges: `^1.2.0`, `~2.0.0`, `>=1.0.0 <2.0.0` — verify `Version::satisfies` against a table of cases. **M**

#### 2.3 Package ergonomics
- [ ] **2.3.1** Install/uninstall into `mods/` with atomic writes and rollback. **S**
- [ ] **2.3.2** Cache invalidation on replace: VFS drops the archive layer index and re-indexes. **S**
- [ ] **2.3.3** Inspect: `q3m inspect` prints manifest, entry table, sizes, signature status, licence. **S**

**Exit criteria**
- [ ] `q3m pack ./mymod out.q3m` produces an archive that mounts as a VFS layer and serves every file.
- [ ] A signed archive fails verification with a clear error; a tampered file fails byte-for-byte.
- [ ] Packing the same folder twice produces identical archives.
- [ ] `TESTS/test_manifest.cpp` + new `TESTS/test_q3m.cpp` green.

**Pillars**: 1.2, 1.3. **Touches**: `SRC/ENGINE/q3m.{h,cpp}`, `SRC/ENGINE/mod/mod_manifest.{h,cpp}`, `TESTS/test_q3m.cpp` (new).

---

### Phase 3 — Mod discovery, dependencies, load order, lifecycle

**Goal**: a real mod manager — all discovery roots, manifest-driven metadata, dependency resolution,
conflict detection, deterministic load order, enable/disable at runtime, profiles, lockfile, lazy loading.

#### 3.1 Discovery
- [ ] **3.1.1** Scan every documented root: `~/.q3d/mods/`, `<install>/mods/`, `<install>/workshop/`, `<project>/.q3d/mods/`, CLI `--mod=…`. **M**
- [ ] **3.1.2** Discover both folder mods and `.q3m` packages; `ModInfo` populated from the manifest (not the folder name). **M**
- [ ] **3.1.3** Emit `ModLoaded` / `ModUnloaded` / `ModError` with mod id + reason; show errors in a UI panel later. **S**
- [ ] **3.1.4** `--validate` mode: load nothing, print a report for every discovered mod (CI/CLI use). **S**

#### 3.2 Resolution
- [ ] **3.2.1** Version-range dependency resolution; missing optional vs. required dependency messages. **M**
- [ ] **3.2.2** Conflict detection: duplicate ids, declared `conflicts`, and two mods claiming the same asset path (needs VFS provenance from 1.3.7). **M**
- [ ] **3.2.3** Topological sort over `dependencies` + `load_after`/`load_before`; cycle detection with a readable error. **M**
- [ ] **3.2.4** User override of load order, persisted and honoured next boot. **S**
- [ ] **3.2.5** Unload in exact reverse order; unloading must tear down scripts, tools, panels, sandboxes and VFS layers (rule **R3**). **M**

#### 3.3 Profiles and reproducibility
- [ ] **3.3.1** `profiles/*.json`: named mod sets, save/load, apply at boot. **M**
- [ ] **3.3.2** `mods.lock`: exact ids, versions and hashes; `--locked` boot fails loudly on drift. **M**
- [ ] **3.3.3** Quickswap between two pinned profiles via hotkey (dev iteration). **S**
- [ ] **3.3.4** `lazy = true` mods: not initialised until first referenced; counts as loaded for resolution. **M**

**Exit criteria**
- [ ] A fixture set of 5 mods with interdependencies loads in a stable, asserted order — and the same order on every run.
- [ ] Removing a dependency produces exit code `2` with the mod id and the missing requirement.
- [ ] `mods.lock` + `--locked` reproduces byte-identical VFS listings.
- [ ] Enable/disable at runtime leaves no residue (rule **N9**), verified by a test.

**Pillars**: 1.4. **Touches**: `SRC/ENGINE/mod_manager.{h,cpp}`, new `SRC/ENGINE/mod/loader.{h,cpp}`.

---

### Phase 4 — The Lua runtime and a real `q3d.*` API

**Goal**: one VM per mod, sandboxed by default, loading through the VFS, hot-reloadable, with a
`q3d.*` surface that is actually implemented instead of stubbed.

**Why pivotal**: Lua *is* the modding language. Until this is real, every other subsystem is
"visible to mods" only on paper.

#### 4.1 One runtime, sandboxed by default
- [ ] **4.1.1** Unify the two existing VMs: `q3d::script::LuaVM` (unsandboxed, `luaL_openlibs`) and `q3d::sandbox::lua::LuaSandbox` (budgets + hook) — one implementation, sandbox the default tier. **M**
- [ ] **4.1.2** Create one VM per mod id; map mod id → VM; tear down on unload. **S**
- [ ] **4.1.3** Remove stdlib by default; expose an allowlisted subset (`base`, `string`, `table`, `math`, plus `q3d.*`). Implement `apply_stdlib_whitelist` and `block_dangerous_globals` (both empty today). **M**
- [ ] **4.1.4** Custom allocator wired to the mod's memory budget; hard failure with a readable error. **S**
- [ ] **4.1.5** Instruction + CPU budget via `lua_sethook` (already present in `LuaSandbox::hook`); rate limit runaway `tick`. **M**
- [ ] **4.1.6** `os.execute`, `io.*`, `package.*`, `require`, `dofile`, `loadfile` unavailable unless granted. **S**

#### 4.2 Load path
- [ ] **4.2.1** Scripts load via the VFS (`mod://<id>/scripts/init.lua`), honouring the manifest `entry_points` order. **M**
- [ ] **4.2.2** Implement `ScriptManager::init()` (empty today): discover entry points, create VMs, run `init`, then per-phase hooks (`on_load`, `on_tick`, `on_unload`). **M**
- [ ] **4.2.3** Error reporting with traceback, mod id and the virtual source path; never "silently ignore" like `LuaHost::tick` does today. **S**
- [ ] **4.2.4** Coroutine-friendly long operations: `load_model`, `play_movie`, `import` yield instead of blocking (pillar 3.1.8). **M**
- [ ] **4.2.5** Hot-reload a VM in place, preserving state per pillar 3.1.4 (Phase 13 wires the watcher; the VM side is here). **M**

#### 4.3 Implement the API (all 32 current functions are stubs)
- [ ] **4.3.1** `q3d.log` — levels, mod tagging, in-game console sink. **S**
- [ ] **4.3.2** `q3d.vfs` — `read`, `write`, `exists`, `list`, `stat`, `mount` (permission-checked). **M**
- [ ] **4.3.3** `q3d.event` — `on`, `once`, `fire`, `cancel` with priority; bridge to `EventBus` + `HookSystem`. **M**
- [ ] **4.3.4** `q3d.actor` — `create`, `find`, `list`, `destroy`, transform + animation calls bound to `ActorRegistry` (ready after Phase 7). **L**
- [ ] **4.3.5** `q3d.scene` — `load`, `save`, `play`, `stop`, `set_frame`, `get_frame`, `set_speed`, timeline queries (Phase 8). **L**
- [ ] **4.3.6** `q3d.math` — `lerp`, `vec3`, `clamp` exist; add vectors, quaternions, matrices, easing curves, curves/keyframes. **M**
- [ ] **4.3.7** `q3d.asset` — `load`, `unload`, `info`, `hash`, `preload` (new module). **M**
- [ ] **4.3.8** `q3d.render` — materials, shaders, passes (Phase 9). **M**
- [ ] **4.3.9** `q3d.audio` — sounds, banks, DSP nodes (Phase 10). **M**
- [ ] **4.3.10** `q3d.ui` — panels and widgets (Phase 11). **M**
- [ ] **4.3.11** `q3d.tools` — register/drive editor tools (Phase 12). **M**
- [ ] **4.3.12** `q3d.thread` — coroutine scheduler, `after()`, `every()`. **M**
- [ ] **4.3.13** Every binding is generated from one schema (`.qdef`) so C++ and Lua never drift. **L**

#### 4.4 Author experience
- [ ] **4.4.1** LuaLS type definitions (`---@class Actor` etc.) generated from the same schema and shipped with the CLI. **M**
- [ ] **4.4.2** In-engine Lua console (REPL) with completion, history, and mod targeting (`> @mymod`). **M**
- [ ] **4.4.3** Debug adapter (DAP) for VS Code / Zed / Neovim: breakpoints, step, watch. **L** (stretch)
- [ ] **4.4.4** Sample mod + tutorial repo used by `TESTS/test_script.cpp`. **S**

**Exit criteria**
- [ ] `q3d --headless --mod fixtures/lua-mover` → the script moves an actor 100 units and the change is visible in the scene dump.
- [ ] Every one of the 32 previously-stubbed `q3d.*` functions either works or is removed from the table.
- [ ] A mod calling `os.execute` gets a permission error naming the missing grant.
- [ ] A mod exceeding its memory budget raises a catchable Lua error instead of taking the host down.

**Pillars**: 3.1, 2 (basics), 17 (basics). **Touches**: `SRC/ENGINE/script/**`, `SRC/ENGINE/sandbox/**`, new `.qdef` schema files.

---

### Phase 5 — Events and hooks fired from real code paths

**Goal**: the event bus stops being an island. Every subsystem fires its events from real code and
offers hook points mods can observe, extend, or cancel.

**Why here**: instrumentation installed early makes Phases 6–14 verifiable by mods and by tests.

#### 5.1 Emission
- [ ] **5.1.1** Map every one of the 39 `Q3D_EVENT` types to a concrete emitter (documented in a table: event → subsystem → call site). **M**
- [ ] **5.1.2** Fire asset events from VFS load/unload (real, not per-subsystem guesses). **M**
- [ ] **5.1.3** Fire mod lifecycle events from the mod manager (Phase 3). **S**
- [ ] **5.1.4** Fire frame events around the host tick loop (Phase 0). **S**
- [ ] **5.1.5** Fire tool/selection/property/undo events from the tool manager (Phase 12). **M**
- [ ] **5.1.6** Fire scene/frame/playback events from the scene system (Phase 8). **M**
- [ ] **5.1.7** Fire audio-mixed events from the mixer (Phase 10). **S**

#### 5.2 Hooks
- [ ] **5.2.1** Instrument `HookSystem::call_hook` (currently never called) at key extension points: cost calculation, transform resolution, texture selection, export settings, tool gating. **M**
- [ ] **5.2.2** Before/after/replace semantics documented per hook point, including cancellation rules. **S**
- [ ] **5.2.3** Mod-owned hooks are removed automatically on unload (`unregister_mod_hooks` must be called by the lifecycle). **S**
- [ ] **5.2.4** `list_hook_points()` exposed to Lua so mods can discover what they can hook. **S**

#### 5.3 Introspection and replay
- [ ] **5.3.1** Record/replay of an event session to JSON; `--replay <file>` reproduces a run (pillar 20 determinism aid). **M**
- [ ] **5.3.2** Event inspector panel (UI in Phase 11) showing live traffic per mod. **S**
- [ ] **5.3.3** Tests: subscription/unsubscription/priority/order, cancellation, record/replay equality. **S**

**Exit criteria**
- [ ] `grep -r "fire<" SRC/ENGINE` returns call sites in every subsystem (today: none).
- [ ] A Lua mod receives `SceneFrameAdvanced` during playback and can cancel a `ToolActivated` hook.
- [ ] `TESTS/test_event_bus.cpp` extended with a host-level integration test.

**Pillars**: 12. **Touches**: `SRC/ENGINE/event/**`, call sites in all subsystems.

---

## Stage B — Everything becomes content

### Phase 6 — Content import pipelines

**Goal**: any file a mod ships becomes engine content — today a parsed glTF goes nowhere because
`GltfToEngineConverter::convert()` calls four empty builders.

#### 6.1 Format registry (the moddable part)
- [ ] **6.1.1** `ImporterRegistry`: register importers by extension/priority; mods register their own. No hardcoded extension switches (rule **R2**). **M**
- [ ] **6.1.2** `ExporterRegistry` with the same shape. **S**
- [ ] **6.1.3** Import result is a resource handle + metadata + provenance (which mod, which vpath) — feeds N9 and the conflict UI. **S**

#### 6.2 Models
- [ ] **6.2.1** Finish `GltfToEngineConverter`: `build_body_from_meshes`, `build_actor`, `build_animations`, `apply_materials` (all empty today). **L**
- [ ] **6.2.2** Mesh hierarchy → body-part sets; bone-name → body-slot mapping with user-overridable aliases (the setter exists, the map does nothing). **M**
- [ ] **6.2.3** Skinned meshes: weights → per-frame morph/skin data, or a documented "static bake" fallback. **L**
- [ ] **6.2.4** Materials/textures: GLB-embedded textures land in the texture cache; PBR params mapped to `br_material` extension fields. **M**
- [ ] **6.2.5** Animations: clips → action library entries; retarget onto the engine skeleton (Phase 7). **M**
- [ ] **6.2.6** OBJ/MTL and STL importers (simple formats, good stretch goals). **M**
- [ ] **6.2.7** FBX via Assimp behind a build flag, or a documented "unsupported" diagnostic path instead of `available() == false` silence. **M** (stretch)

#### 6.3 Textures, fonts, audio assets
- [ ] **6.3.1** Images: keep the working `image.cpp` decoders, add JPEG/WebP behind flags; mipmap policy per asset class. **M**
- [ ] **6.3.2** Texture baker output wired to real material slots (AO/curvature/ID currently produced but unused). **M**
- [ ] **6.3.3** Font import → mod-owned 3D text (TDF-compatible) so a mod can ship its own typeface. **M**
- [ ] **6.3.4** Audio asset import → sound banks (Phase 10). **S**

#### 6.4 Pipeline UX
- [ ] **6.4.1** Import report: what was imported, what was skipped, with reasons (surfaces all the silent no-ops). **S**
- [ ] **6.4.2** Import progress events for the loading screen (`AssetLoaded`, `AssetMissing`). **S**
- [ ] **6.4.3** Deterministic import: same input ⇒ same output ids, for lockfiles and CI diffing. **M**

**Exit criteria**
- [ ] `fixtures/hello.glb` → a new actor template appears in the registry, visible from Lua, after `q3d --mod`.
- [ ] A mod registers a custom `.obj` importer override and wins by priority.
- [ ] Import failures produce a report entry, never a silent empty model.

**Pillars**: 4.1, 4.2, 4.3, 4.5. **Touches**: `SRC/ENGINE/import/**`, new `SRC/ENGINE/import/registry.{h,cpp}`.

---

### Phase 7 — Actors, costumes, skeletons and animation

**Goal**: actors become pure data a mod can define, costume, rig and animate; the registry does the work.

#### 7.1 Actor definitions
- [ ] **7.1.1** `ActorDefinition` format v2: body parts, slots, actions, costume sets, metadata, licence + provenance. **M**
- [ ] **7.1.2** `ActorRegistry::instantiate_actor` / `instantiate_body` implemented (both no-ops today) against the Phase 6 import output. **L**
- [ ] **7.1.3** Tags/metadata search powering the browser UI (`find(tag, name, author)` exists; needs real data). **S**

#### 7.2 Costumes and body parts
- [ ] **7.2.1** `CostumeManager::get_costumes_for_slot` / `apply_costume` implemented against real body handles. **M**
- [ ] **7.2.2** Compatibility validation with actionable errors (missing slot, size mismatch, material requirement). **M**
- [ ] **7.2.3** Mod-added body parts compose with stock ones; a mod can replace a stock part without patching the engine. **M**

#### 7.3 Skeletons, IK and retargeting
- [ ] **7.3.1** `Skeleton` load/validate; bind imported rigs to actor definitions. **M**
- [ ] **7.3.2** IK solver wired to tool + Lua use (`q3d.actor.ik_*`). **M**
- [ ] **7.3.3** `Retargeter::retarget` implemented — today it returns the source animation unchanged, so imported clips play on their own rig and nowhere else. **L**

#### 7.4 Animation
- [ ] **7.4.1** Action library: named, tagged, blendable actions; stock + mod actions in one namespace. **M**
- [ ] **7.4.2** Action authored in Lua: keyframe curves, easing, time warping. **M**
- [ ] **7.4.3** Path recording and playback preserved from the legacy engine, exposed to mods. **M**
- [ ] **7.4.4** Props: a mod-provided object that can be placed, moved, animated like an actor. **M**

**Exit criteria**
- [ ] A mod ships one character with three actions; the app lists it in the browser and plays it in a scene.
- [ ] A costume swap is a single Lua call and is undoable.
- [ ] Retargeting produces a visible result on a stock actor with no engine edits.

**Pillars**: 5, 6. **Touches**: `SRC/ENGINE/actor/**`, `SRC/ENGINE/import/gltf_to_engine.cpp`.

---

### Phase 8 — Scenes, saves, undo/redo and determinism

**Goal**: scenes, timelines and the `.3mm` save format become mod-extensible and versioned, with
deterministic output and undoable edits.

#### 8.1 Scene format
- [ ] **8.1.1** Scene v2 schema: actors, props, backgrounds, camera, timeline, events, mod-owned extension blocks. **L**
- [ ] **8.1.2** Extension blocks let a mod store its own data inside a scene without schema changes (pillar 13). **M**
- [ ] **8.1.3** Reader for the original `.3mm` (keep the 1995 movies working) + writer that upgrades them. **L**
- [ ] **8.1.4** Timeline: tracks, keys, easing, loops, per-actor constraints. **L**

#### 8.2 Saves
- [ ] **8.2.1** Save/load through the VFS (`user://` default), atomic write + `.bak`. **M**
- [ ] **8.2.2** Migration chain `v1 → v2` with tests for every historical shape. **M**
- [ ] **8.2.3** Schema migration hook so a mod can migrate *its own* extension block across versions. **M**
- [ ] **8.2.4** Save determinism: no timestamps/random ids in the file; `mods.lock` recorded in the header. **M**

#### 8.3 Editing and export
- [ ] **8.3.1** Undo/redo command stack shared by tools, scripts and UI; every scripted mutation is undoable. **M**
- [ ] **8.3.2** Playback engine: fixed timestep, deterministic evaluation, scrubbing. **L**
- [ ] **8.3.3** Movie export (Theatre) deterministic: identical frames for identical inputs, verified by hashing output. **L**
- [ ] **8.3.4** Autosave, crash recovery, and safe behaviour when a mod is missing at load time (graceful degradation, not a crash). **M**

**Exit criteria**
- [ ] Save → reload → export twice yields byte-identical output (test with hashes).
- [ ] A scene saved with a mod installed opens without that mod, degrading with a clear warning.
- [ ] A script-driven edit is undoable through the normal UI path.

**Pillars**: 7, 13, 0.9 (determinism). **Touches**: new `SRC/ENGINE/scene/**`, `SRC/ENGINE/save/**`.

---

### Phase 9 — Rendering: GPU backend, materials, shaders, passes

**Goal**: a real rendering path where materials, shaders, textures and render passes are all
mod-supplied. Today `ShaderProgram` writes uniforms into `fake_upload_uniform`.

#### 9.1 Backend
- [ ] **9.1.1** Backend interface (`IRenderBackend`) + first real implementation, per the Phase 0.3.4 decision. **L**
- [ ] **9.1.2** Real shader compile/link/validate with error reporting to the mod. **L**
- [ ] **9.1.3** Resource lifetime rules documented (mods must not leak GPU handles across unload). **S**
- [ ] **9.1.4** Keep a documented headless/null backend so CI can still run render-graph tests without a GPU. **M**

#### 9.2 Moddable rendering
- [ ] **9.2.1** `MaterialRegistry`: materials are data (JSON/TOML), resolvable and overridable by layer. **M**
- [ ] **9.2.2** Shader library: mods ship `.glsl`/`.hlsl`/`.wgsl`, compiled on load, hot-reloadable (Phase 13). **M**
- [ ] **9.2.3** `RenderPassManager`: real `setup`/`cleanup` (both empty), plus mod-registered passes with insertion points. **L**
- [ ] **9.2.4** Texture streaming + mip policy + mod-owned texture formats. **M**
- [ ] **9.2.5** Post-processing stack (bloom, SSAO, color grading) as standard passes mods can reorder or replace. **M**

#### 9.3 Compatibility with the legacy renderer
- [ ] **9.3.1** Decide the relationship to BRender: replace, wrap, or co-exist behind the backend interface (decide with the Phase 15 plan). **M** (decision task)
- [ ] **9.3.2** Material/shader compatibility layer so 1995 assets still render under the new backend. **L**

**Exit criteria**
- [ ] A mod ships a shader + material; an actor renders with it; editing the shader hot-reloads the material.
- [ ] CI's null backend renders a scene graph to an offscreen target and hashes the output (determinism).
- [ ] No uniform ever reaches `fake_upload_uniform` in a release build.

**Pillars**: 8. **Touches**: `SRC/ENGINE/render/**`, new `SRC/ENGINE/render/backend/**`.

---

### Phase 10 — Audio: device backend, DSP graph, sound banks

**Goal**: audio that actually plays, with a DSP graph mods can build, and sound banks they can ship.

#### 10.1 Backend
- [ ] **10.1.1** `AudioEngine::init/shutdown/update` implemented against a real device abstraction. **M**
- [ ] **10.1.2** Backends: WASAPI/WinMM first, then ALSA/PulseAudio and CoreAudio; null device for CI. **L**
- [ ] **10.1.3** Mixing thread with a deterministic offline mode for exports (feeds N8). **M**

#### 10.2 Moddable audio
- [ ] **10.2.1** Sound banks from mods: `SoundManager::load_bank/unload_bank/reload_bank` implemented (all empty). **M**
- [ ] **10.2.2** DSP graph execution: `dsp.cpp` nodes actually evaluated in the mix chain. **M**
- [ ] **10.2.3** `Sound::seek` and `set_on_marker` implemented (empty) so scripts can time dialogue to animation. **M**
- [ ] **10.2.4** Recording (`stop_recording` empty) for voiceover mods, permission-gated. **M**
- [ ] **10.2.5** Lip-sync / timing data a mod can author and bind to frames. **M**
- [ ] **10.2.6** Audio hooks so mods can filter or replace any sound at runtime. **M**

**Exit criteria**
- [ ] A mod ships a bank + a DSP chain; playback in a scene matches the chain.
- [ ] Offline render of a scene produces byte-identical audio across runs.
- [ ] `q3d --headless` mixes with the null device and exits cleanly.

**Pillars**: 9. **Touches**: `SRC/ENGINE/audio/**`.

---

## Stage C — Live, authorable, and safe

### Phase 11 — UI, themes and HUD

**Goal**: a working widget system with themes and panels that mods build from Lua — today all 36
`lua_ui.cpp` functions return `0` and `ui_functions[]` is empty.

#### 11.1 Widget system
- [ ] **11.1.1** `Widget::build/measure/destroy/animate` implemented (defaults are empty); layout pass for hbox/vbox/grid/dock. **L**
- [ ] **11.1.2** Input routing: focus, hover, click, drag, keyboard, text editing. **M**
- [ ] **11.1.3** Rendering backends for widgets (immediate-mode debug renderer first, then the real one). **M**

#### 11.2 Themes and accessibility
- [ ] **11.2.1** `UIManager::load_theme` from the VFS; theme inheritance and per-mod overrides. **M**
- [ ] **11.2.2** `reload_theme` / `reload_translations` / `reload_ui_scripts` implemented (empty) and wired to hot-reload. **M**
- [ ] **11.2.3** Locale loading from mod `locale/` dirs with fallback chain. **M**
- [ ] **11.2.4** Accessibility: high contrast, large text, screen reader hooks (all empty stubs today). **M**
- [ ] **11.2.5** Layout persistence: `save_layout/load_layout/reset_layout` with mod panel placement. **S**

#### 11.3 Lua UI API
- [ ] **11.3.1** Implement all 36 functions in `lua_ui.cpp` and populate `ui_functions[]` (currently an empty table — the table is never even registered). **L**
- [ ] **11.3.2** Panel lifecycle: `panel_new`, dock, show/hide, title, persistence. **M**
- [ ] **11.3.3** Widget set: button, label, image, slider, list, tree, text input, colour picker, file picker, dropdown, tabs. **L**
- [ ] **11.3.4** Widget callbacks with correct Lua upvalues and lifetime (mod unload must clean them). **M**
- [ ] **11.3.5** Mod menus: inject entries into existing menus; stock UI must survive a hostile mod panel. **M**
- [ ] **11.3.6** Decide the shell: extend the legacy Studio UI or ship the modern UI as the new app face (tie to Phase 15). **M** (decision task)

**Exit criteria**
- [ ] `fixtures/lua-panel` builds a working panel at runtime: buttons drive real engine calls.
- [ ] A theme swap changes the whole UI live, from a file save.
- [ ] Unloading the mod removes its panels, callbacks and menu entries — no dangling references.

**Pillars**: 10. **Touches**: `SRC/ENGINE/ui/**`.

---

### Phase 12 — Tools and editor extensions

**Goal**: mods can add editor tools that operate on real scene objects with real undo, gizmos and overlays.

#### 12.1 Real tool context
- [ ] **12.1.1** Define `MovieHandle`/`SceneHandle`/`ActorHandle` for real — `MvMovie`, `MvScene`, `MvActor` are forward-declared and defined nowhere today. **M**
- [ ] **12.1.2** Picking/hit-testing, ray casting, drag planes, snapping. **L**
- [ ] **12.1.3** Implement `ToolContext` (selection, hover, viewport, undo stack, scene queries) — the struct exists with empty methods. **M**
- [ ] **12.1.4** `apply_gizmo`, `render_overlay`, `on_frame`, `select_all`, `undo` implemented (all empty). **M**

#### 12.2 Mod tools
- [ ] **12.2.1** Register tools from Lua (`q3d.tools.register`) — Phase 4 API lands with a stub; this fills it. **M**
- [ ] **12.2.2** Tool categories, icons, cursors, tooltips for mod tools. **S**
- [ ] **12.2.3** Hotkeys with conflict detection and a rebindable palette (`save_palette/load_palette` are empty). **S**
- [ ] **12.2.4** Context menus and command palette entries contributed by mods. **M**
- [ ] **12.2.5** All 10 built-in tools completed (only `SelectTool` has behaviour today) so mods have working reference implementations. **L**
- [ ] **12.2.6** Undo/redo integration: every tool action, scripted or built-in, goes through the command stack (Phase 8.3.1). **M**

**Exit criteria**
- [ ] A Lua-defined tool selects an actor and moves it, with undo working through the UI.
- [ ] `unregister_all_for_mod` (empty today) leaves no tool, hotkey or menu entry behind.
- [ ] Two mods registering the same hotkey produce a conflict dialog, not silent failure.

**Pillars**: 11. **Touches**: `SRC/ENGINE/tools/**`.

---

### Phase 13 — Hot-reload and live editing

**Goal**: save a file, see it, everywhere — the pillar "restarting to see your changes is a bug".

#### 13.1 Watcher
- [ ] **13.1.1** Abstract `IFileWatcher`; keep Win32 `ReadDirectoryChangesW` and add inotify + FSEvents + a polling fallback (needed for CI from Phase 0.2.3). **L**
- [ ] **13.1.2** Debounce, ignore patterns, editor "atomic save" (write-temp-then-rename) handling. **M**
- [ ] **13.1.3** Watch roots derived from mounted VFS layers, so packages inside `.q3m` reload too. **M**

#### 13.2 Dispatch
- [ ] **13.2.1** Implement `ReloadManager::process_reload` — the dispatch point that is currently empty. **M**
- [ ] **13.2.2** Implement each `*Reloader::reload` (texture, model, shader, script, UI, audio) — all no-ops today. **L**
- [ ] **13.2.3** Extension-based classification (no `.png`/`.obj` string matching in core code — rule **R2**). **S**
- [ ] **13.2.4** Frame-budgeted reloads: one reload per frame by default, priority queue, metrics surfaced in the perf overlay. **M**

#### 13.3 State preservation
- [ ] **13.3.1** Script reload preserves mod state meaningfully — `snapshot_script_state` currently just sets `suspended_at = "snapshotted"`. **M**
- [ ] **13.3.2** Scene reload keeps playhead, selection and camera. **M**
- [ ] **13.3.3** Error resilience: a broken reload keeps the last-good asset and shows the error, never blanks the screen. **M**
- [ ] **13.3.4** Manual reload hotkey + `ReloadManager::metrics()` populated with real numbers. **S**

**Exit criteria**
- [ ] Editing a Lua script changes behaviour within one frame, with state preserved.
- [ ] Editing a texture updates the viewport without a restart, on Windows and Linux.
- [ ] A syntax error in a script leaves the previously working version running and shows a readable error.

**Pillars**: 14, 0.5. **Touches**: `SRC/ENGINE/hotreload/**`.

---

### Phase 14 — Sandbox enforcement and crash containment

**Goal**: turn the existing sandbox types from a description into an enforced boundary. This phase is
what makes "download a stranger's mod" acceptable.

#### 14.1 Enforcement
- [ ] **14.1.1** `Sandbox::apply()` implemented (empty) and invoked at VM creation; enforce it as the *only* VM path. **M**
- [ ] **14.1.2** Every I/O entry point consults the calling mod's `PermissionSet` — VFS, audio capture, process spawn, native load, network (stub the network APIs honestly rather than exposing them unwired). **L**
- [ ] **14.1.3** Enforce budgets for real: memory (allocator already exists — wire it to the mod budget), CPU ms, instruction count, rate limits per action. **M**
- [ ] **14.1.4** Manifest permissions → `PermissionSet` mapping; missing grants produce explicit errors naming the permission. **M**
- [ ] **14.1.5** Tier semantics implemented: `core` > `dev` > `local` > `workshop`, with per-tier default grants (`set_default_permissions` currently only stores them). **M**

#### 14.2 Containment
- [ ] **14.2.1** Crash handler wired end to end (`CrashHandler::on_crash` → UI dialog → one-click disable). **M**
- [ ] **14.2.2** Infinite-loop protection: watchdog + budget kill, then quarantine the mod. **M**
- [ ] **14.2.3** Safe mode (`enter_safe_mode` stores a flag and nothing else) — boot with all mods disabled but the app fully usable. **M**
- [ ] **14.2.4** `--single-mod <id>` for bisecting a bad mod. **S**
- [ ] **14.2.5** Out-of-process isolation as an opt-in for high-risk mods (pillar 2.4). **L** (stretch)

#### 14.3 Transparency
- [ ] **14.3.1** Audit log is real and complete (every privileged action recorded with mod id + timestamp). **M**
- [ ] **14.3.2** Permission prompt flow for locally installed mods requesting extra grants. **M**
- [ ] **14.3.3** In-game panel showing what each enabled mod may do (readable by a 12-year-old). **M**
- [ ] **14.3.4** Docs + the "what a mod can and cannot do" page in the CLI help. **S**

**Exit criteria**
- [ ] A test mod attempting `io.open("/etc/passwd")`, `os.execute`, and `require("socket")` is blocked, with the denial recorded in the audit log.
- [ ] A mod that loops forever is killed within its CPU budget; the host stays responsive and offers to disable it.
- [ ] `--safe-mode` boots a usable app with zero mods loaded.

**Pillars**: 2, 17. **Touches**: `SRC/ENGINE/sandbox/**`, VFS/audio/render permission seams.

---

## Stage D — Ship it

### Phase 15 — Bridge to the legacy 3D Movie Maker app

**Goal**: make softmodding real in the actual product — BRender viewport, Studio timeline, Kauai UI,
`.3CN`/`.3TH` content indexes, CHT/KidScript scenes and the Theatre export — not only in a headless host.

**Why this is XL**: the legacy engine is pre-C++98, Windows/GDI-bound and currently not even built by
default. This phase is scoped as adapters, not a rewrite.

#### 15.1 Build and modernize the legacy engine enough to embed
- [ ] **15.1.1** `BUILD_LEGACY_ENGINE=ON` compiles under MSVC 2022 (ROADMAP Phase 0 checklist). **L**
- [ ] **15.1.2** Port `TOOLS/` (sitobren, tdfmake, mktmap) and the CHOMP chunk compiler to CMake targets. **M**
- [ ] **15.1.3** Cross-platform shims for the platform layer (`STDERR/FILE/MEM`, `GFXWIN`/`GFXMAC`) — at least an offscreen path for CI. **L**
- [ ] **15.1.4** Keep the legacy build byte-compatible with the original `.chk`/`.3CN` content. **S**

#### 15.2 Adapters (the bridge)
- [ ] **15.2.1** VFS ↔ TAGMAN: asset lookups resolve through the VFS with mods in the layer stack. **L**
- [ ] **15.2.2** BRender renderer ↔ modern backend: mod materials/shaders reach the real renderer. **L**
- [ ] **15.2.3** Actor/scene model ↔ `ACTOR.CPP`/`SCENE.CPP`: a modern actor definition spawns a legacy actor. **L**
- [ ] **15.2.4** Kauai UI ↔ modern UI: decide the shell (Phase 11.3.6) and implement the chosen direction. **XL**
- [ ] **15.2.5** Audio: legacy SNDM/MIDI path fed by the modern sound manager. **M**

#### 15.3 Legacy content as moddable content
- [ ] **15.3.1** CHT/KidScript scenes loadable, hot-reloadable and mod-authorable (watch → recompile via CHOMP → invalidate). **L**
- [ ] **15.3.2** GOK/WOK behaviour hooks exposed to Lua as `q3d.gok`. **L**
- [ ] **15.3.3** Original content (actors, backgrounds, sounds, fonts, text boxes) registered as `core`-tier mods so mods can override them like any other asset. **M**
- [ ] **15.3.4** Preserve original movies: an unmodified 1995 `.3mm` opens and plays in the new shell. **M**

**Exit criteria**
- [ ] The app runs on a modern toolchain; a mod can add an actor, a prop, a sound and a tool to the real Studio UI.
- [ ] A mod replaces a stock background via the VFS layer stack, with no engine rebuild.
- [ ] Original sample movies still play.

**Pillars**: all (integration), ROADMAP Phases 0–3. **Touches**: `SRC/ENGINE/adapters/**` (new), legacy files only through adapters.

---

### Phase 16 — Devkit, CLI and example mods

**Goal**: the path from "I have an idea" to "a working mod" is short, typed, documented and testable.

#### 16.1 The `q3m` CLI
- [ ] **16.1.1** `q3m init <name>` — scaffold a mod (manifest, `scripts/init.lua`, `assets/`, tests). **M**
- [ ] **16.1.2** `q3m validate` — manifest schema + asset references + permissions + common mistakes. **M**
- [ ] **16.1.3** `q3m pack` / `unpack` / `inspect` / `sign` / `verify` / `diff` / `merge`. **M**
- [ ] **16.1.4** `q3m run` — launch the engine with the mod loaded and watch it (dev loop). **M**
- [ ] **16.1.5** `q3m new-tool` / `new-panel` / `new-actor` — templates for the common mod types. **M**
- [ ] **16.1.6** CI-friendly exit codes and `--json` output for every command. **S**

#### 16.2 Devkit
- [ ] **16.2.1** Headers + static libs for native extensions, with a stable C ABI and semver policy (ROADMAP 3.1). **L** (stretch)
- [ ] **16.2.2** LuaLS definitions + `lua-language-server` config template. **M**
- [ ] **16.2.3** Debug adapter (DAP) if not done in Phase 4. **L**
- [ ] **16.2.4** `q3d mod docs <symbol>` — API reference in the terminal. **S**

#### 16.3 Examples and docs
- [ ] **16.3.1** 8+ example mods, each tiny and focused: hello-world, custom actor, custom tool, custom panel, custom shader, custom sound bank, custom material, override stock asset. **L**
- [ ] **16.3.2** Each example is executed by a CI test (so examples cannot rot). **M**
- [ ] **16.3.3** Tutorial docs: "your first mod" in under 15 minutes; per-pillar cookbooks. **M**
- [ ] **16.3.4** Every API documented with at least one runnable example and a stability badge. **M**

**Exit criteria**
- [ ] A newcomer follows "your first mod" and gets a working panel without asking a question.
- [ ] `q3m validate` catches a typo'd dependency, a missing asset reference and an unrequested permission.
- [ ] CI runs all example mods on every push.

**Pillars**: 18, 20. **Touches**: new `TOOLS/q3m/`, `docs/`, `examples/`.

---

### Phase 17 — Distribution, versioning and mod health

**Goal**: mods outlive the engine that shipped them, and users can find, install and trust them.

#### 17.1 Mod manager UX
- [ ] **17.1.1** In-app browser: install, enable/disable, order, conflicts, updates, uninstall. **L**
- [ ] **17.1.2** Profile UI (Phase 3.3.1) surfaced, with quickswap. **M**
- [ ] **17.1.3** "Your mod set changed — reload?" flow with a preview of what will load. **S**

#### 17.2 Distribution
- [ ] **17.2.1** Local/folder workshop plus an optional remote index protocol (no platform lock-in required). **L**
- [ ] **17.2.2** Install from URL, verify signature, record provenance. **M**
- [ ] **17.2.3** Dependency auto-resolve with explicit user consent. **M**
- [ ] **17.2.4** Content rating and moderation hooks respected at install time (manifest `content_rating`). **S**

#### 17.3 Versioning and migration
- [ ] **17.3.1** The 10-year promise, written down and enforced by the schema process (additive-only, deprecation in years). **S**
- [ ] **17.3.2** Engine version ranges; mods declaring an incompatible range fail loudly with an explanation. **M**
- [ ] **17.3.3** Manifest + scene + actor schema migration paths, tested against archived versions. **M**
- [ ] **17.3.4** Changelog generated from conventional commits, per API area. **S**

#### 17.4 Mod health
- [ ] **17.4.1** `q3m test` — run a mod's own tests in CI against the current engine. **M**
- [ ] **17.4.2** Compatibility matrix: "this mod works on engine ≥ 1.4, tested up to 1.9". **M**
- [ ] **17.4.3** Breaking-change detection: run archived example mods against each release. **M**
- [ ] **17.4.4** Determinism test suite (N8) in CI for engine + examples. **M**

**Exit criteria**
- [ ] A mod written against API v1 still loads on the current engine, or the upgrade path is automatic.
- [ ] Installing a conflicting mod pair is prevented or explained before anything is written to disk.
- [ ] CI fails if any example mod breaks.

**Pillars**: 15, 19, 20. **Touches**: new `SRC/ENGINE/mod/manager_ui/**`, `docs/`, CI workflows.

---

## Traceability matrix

| Phase | SOFTMODDABLE.md pillars | ROADMAP.md phases |
|---|---|---|
| 0 Ground truth | 0, 20 | 0.2, 0.3 |
| 1 VFS | 1.1 | 3.3 |
| 2 Packages & manifest | 1.2, 1.3 | 3.3, 3.4 |
| 3 Mod lifecycle | 1.4 | 3.3 |
| 4 Lua runtime | 3.1, 2 (basics) | 3.2, 4 |
| 5 Events & hooks | 12 | — |
| 6 Import pipelines | 4.1–4.5 | 2 |
| 7 Actors & animation | 5, 6 | 2, 3.3 |
| 8 Scenes & saves | 7, 13 | 2, 3 |
| 9 Rendering | 8 | 5 |
| 10 Audio | 9 | 6 |
| 11 UI | 10 | 7 |
| 12 Tools | 11 | 3.1 |
| 13 Hot reload | 14 | 3.5 |
| 14 Sandbox | 2, 17 | 3.3 |
| 15 Legacy bridge | all (integration) | 0, 1, 3 |
| 16 Devkit & CLI | 18, 20 | 3.1, 3.4, 9 |
| 17 Distribution & health | 15, 19, 20 | 9 |

---

## Explicitly out of scope

- **Archive trees**: `cd2/`, `cd3/`, `cd9/`, `cd12/`, `OBJ/`, `INC/`, `kauai/SRC/`, `BREN/`,
  `SRC/STUDIO/`, `SRC/SHARED/`, `SETUP/` are historical/reference trees — they are read, never
  rewritten as part of softmodding work. Integration happens through Phase 15 adapters.
- **Overlay mods / engine binary patching** (pillar 16) and **Python + WASM runtimes** (pillar 3.2/3.3)
  are deliberately deferred until the Lua path is complete — see *Deferred* items inside phases.
- **Payments, encrypted premium content and platform-level commerce** are Phase 17 stretch items, not
  a blocker for softmodding.
- **Rewriting the 1995 engine** (ROADMAP Phases 1–2 modernization) is tracked in `ROADMAP.md`; only the
  parts Phase 15 needs are pulled forward here.

---

*Maintained alongside [`SOFTMODDABLE.md`](SOFTMODDABLE.md) (the destination),
[`ROADMAP.md`](ROADMAP.md) (engine modernization) and [`OVERVIEW.md`](OVERVIEW.md) (architecture).
Tick a box here in the same PR that lands the code.*