# SOFTMODDABLE.md -- The Absolute Best Plan to Make Quack3DMM a GMod-for-Animated-Videos

> **Mission**: Turn Quack3DMM into the **Garry's Mod of 3D animation** -- where every
> pixel, every actor, every animation, every UI panel, every line of game logic,
> every render pass, and every sound is hot-swappable user content. Mods are
> first-class citizens, not bolted-on afterthoughts.
>
> **Design philosophy**: Zero hardcoded anything. If the engine has it, a mod
> can replace it, extend it, remove it, or fork it. Modders never need to
> touch engine source to ship content -- but engine source itself is *also*
> patchable at runtime through overlay mods.

---

## Table of Contents

- [Pillar 0: Guiding Principles (The "GMod Mindset")](#pillar-0-guiding-principles)
- [Pillar 1: Virtual File System & Mod Loading](#pillar-1-virtual-file-system--mod-loading)
- [Pillar 2: Sandbox & Hardened Runtime](#pillar-2-sandbox--hardened-runtime)
- [Pillar 3: Scripting (Lua + Python + WASM)](#pillar-3-scripting)
- [Pillar 4: Content Authoring Pipelines](#pillar-4-content-authoring-pipelines)
- [Pillar 5: Actor / Character Modding](#pillar-5-actor--character-modding)
- [Pillar 6: Animation Modding](#pillar-6-animation-modding)
- [Pillar 7: Scene / Level Modding](#pillar-7-scene--level-modding)
- [Pillar 8: Rendering & Shader Modding](#pillar-8-rendering--shader-modding)
- [Pillar 9: Audio Modding](#pillar-9-audio-modding)
- [Pillar 10: UI / HUD Modding](#pillar-10-ui--hud-modding)
- [Pillar 11: Tools & Editor Modding](#pillar-11-tools--editor-modding)
- [Pillar 12: Event / Hook / Network API](#pillar-12-event--hook--network-api)
- [Pillar 13: Save Format Modding](#pillar-13-save-format-modding)
- [Pillar 14: Hot-Reload & Live Editing](#pillar-14-hot-reload--live-editing)
- [Pillar 15: Mod Manager, Workshop & Distribution](#pillar-15-mod-manager-workshop--distribution)
- [Pillar 16: Engine Code Patching (Overlay Mods)](#pillar-16-engine-code-patching-overlay-mods)
- [Pillar 17: Per-Mod Sandboxing & Permissions](#pillar-17-per-mod-sandboxing--permissions)
- [Pillar 18: Modding the Modder (Meta / Devkit)](#pillar-18-modding-the-modder-meta--devkit)
- [Pillar 19: Versioning, Migration & Long-Term Mod Health](#pillar-19-versioning-migration--long-term-mod-health)
- [Pillar 20: Test Suite for Mods (CI, Lint, Validation)](#pillar-20-test-suite-for-mods)

---

## Pillar 0: Guiding Principles (The "GMod Mindset")

These are non-negotiable. Every other pillar is judged against them.

- [ ] **0.1** Define the **Mod Contract**: any mod is a directory or `.q3m` archive with a `mod.toml` manifest. No other file format. No exceptions.
- [ ] **0.2** Define the **10-Year Mod Promise**: mods written today MUST load unmodified 10 years from now. Versioned APIs, additive-only evolution, deprecation cycles measured in years.
- [ ] **0.3** Define **Soft Everything**: no hardcoded actors, no hardcoded backgrounds, no hardcoded sounds, no hardcoded tools, no hardcoded UI panels, no hardcoded materials, no hardcoded shaders, no hardcoded file extensions in core code. All registered, all overridable.
- [ ] **0.4** Define **Layers, not Branches**: mods do not fork the engine. They stack as layers on top of a frozen base + each other. Conflicts resolved by load order, not by `git merge`.
- [ ] **0.5** Define **Hot by Default**: every asset, every script, every tool, every UI panel reloads without restart. Restarting to see your changes is a bug.
- [ ] **0.6** Define **Permissive Defaults, Strict Opt-Out**: mods can do anything unless they declare a sandbox. Power users keep the keys; casual modders get the walled garden.
- [ ] **0.7** Define **No Engine Source Edits Required**: a mod that requires recompiling the engine is a mod we failed. The only path that touches `.cpp` files is an explicit "overlay mod" (Pillar 16).
- [ ] **0.8** Define **Single Source of Truth for Content**: every model, every texture, every sound, every script lives at exactly one resolved path. No silent duplication.
- [ ] **0.9** Define **Determinism is Law**: given the same mod set + same inputs, two runs produce byte-identical movie output. Critical for shared movie files, version control, CI.
- [ ] **0.10** Define **Mod = Data + Behavior + UI**: a mod is not just a `.glb`. A mod is a complete experience: data assets, scripts that drive them, UI that exposes them, events that wire them. Bundle the whole package.

---

## Pillar 1: Virtual File System & Mod Loading

The VFS is the **foundation** of all modding. Every other pillar leans on it.

### 1.1 Virtual File System (VFS) Core
- [ ] **1.1.1** Build a **layered VFS** (`q3d::vfs`) backed by a stack of read-only layers + one write layer (sandboxed user dir).
- [ ] **1.1.2** Define a unified **path scheme**: `q3d://base/...`, `q3d://mod/<modid>/...`, `q3d://user/...`, `q3d://workshop/<id>/...`.
- [ ] **1.1.3** Implement **path resolution rules**: user > mod (in load order) > base. Make load order explicit and reorderable.
- [ ] **1.1.4** Support **transparent `.q3m` archives** as a single VFS layer. ZIP-based, indexed on first access, no extraction needed.
- [ ] **1.1.5** Implement **content-addressable storage** (CAS) for the base layer so duplicate assets across mods are deduped (sha256 keyed).
- [ ] **1.1.6** Add **VFS watcher**: inotify/FSEvents/ReadDirectoryChangesW integration; mods in `mods/` show up without restart.
- [ ] **1.1.7** Add **VFS introspection API**: mods can `vfs.list("textures/")`, `vfs.exists(...)`, `vfs.open(...)` from Lua/Python.
- [ ] **1.1.8** Add **VFS mount points**: mods can mount arbitrary folders (e.g., `~/Pictures/campaign/`) into the VFS at runtime.
- [ ] **1.1.9** Add **overlay writes**: mods can `vfs.write("user://save/myactor.actor", ...)` for saves without touching the VFS stack.
- [ ] **1.1.10** Add **VFS preloading controls**: mods declare `preload = ["textures/", "models/"]` so loading screens can show progress.

### 1.2 Mod Manifest (`mod.toml`)
- [ ] **1.2.1** Define the **canonical manifest schema** with versioning (`schema_version = 2`).
- [ ] **1.2.2** Manifest fields: `id`, `name`, `version`, `authors`, `description`, `tags`, `engine_version`, `dependencies`, `conflicts`, `load_after`, `load_before`, `sandbox`, `permissions`, `entry_points`, `assets`, `patches`, `tools`, `ui_panels`, `shaders`, `hooks`.
- [ ] **1.2.3** Add a **manifest linter** with friendly error messages (`"you wrote 'dependencys', did you mean 'dependencies'?"`).
- [ ] **1.2.4** Support **inheritance / extends**: a mod can extend another mod's content without copying files.
- [ ] **1.2.5** Support **mod variants** (e.g., HD/SD textures via `variant = "hd"` in the filename).
- [ ] **1.2.6** Support **mod flags**: `experimental`, `deprecated`, `server_only`, `client_only`, `editor_only`, `runtime_only`.
- [ ] **1.2.7** Support **content ratings** (`safe`, `mature`, `explicit`) with engine-level enforcement if user opts in.
- [ ] **1.2.8** Support **localization block** in manifest: `locales = { en = { name = "...", description = "..." }, ... }`.

### 1.3 `.q3m` Package Format
- [ ] **1.3.1** `.q3m` is a **ZIP archive** with a specific layout: `/mod.toml`, `/assets/...`, `/scripts/...`, `/shaders/...`, `/ui/...`, `/tools/...`, `/patches/...`.
- [ ] **1.3.2** Add **streaming reads**: large assets (videos, textures) read from ZIP without full extraction.
- [ ] **1.3.3** Support **deflate, zstd, lz4, lzma** compression (auto-selected per file type).
- [ ] **1.3.4** Support **delta patches**: a `.q3m` can be a delta over another mod's `.q3m` for tiny updates.
- [ ] **1.3.5** Add **`.q3m` digital signatures** (ed25519) for workshop-distributed mods.
- [ ] **1.3.6** Add **optional encryption** for paid/premium workshop content (decryption key fetched at install).
- [ ] **1.3.7** Provide a **command-line tool** `q3m` for `pack`, `unpack`, `inspect`, `sign`, `verify`, `diff`, `merge`.

### 1.4 Mod Discovery & Load Order
- [ ] **1.4.1** Scan directories: `~/.q3d/mods/`, `<install>/mods/`, `<install>/workshop/`, `<project>/.q3d/mods/`, CLI `--mod=path/to/mod`.
- [ ] **1.4.2** **Dependency resolution** with version ranges (`^1.2.0`, `~2.0.0`, `>=1.0.0 <2.0.0`).
- [ ] **1.4.3** **Conflict detection** between mods (same `id`, same asset overriding same path, declared `conflicts = [...]`).
- [ ] **1.4.4** **Topological sort** of mod graph for load order; UI lets user override.
- [ ] **1.4.5** **Mod profiles** (`profiles/horror.json`, `profiles/comedy.json`) -- save and load entire mod sets with one click.
- [ ] **1.4.6** **Quickswap** -- pin two mod profiles, press a hotkey to swap between them (for dev iteration).
- [ ] **1.4.7** **Mod lockfile** (`mods.lock`) -- pin exact versions for reproducible projects.
- [ ] **1.4.8** **Lazy load**: mods with `lazy = true` only load when first referenced; great for heavy optional content.

---

## Pillar 2: Sandbox & Hardened Runtime

- [ ] **2.1** Define a **sandboxed Lua VM per mod** (default for downloaded mods). Hard memory cap, CPU cap, instruction count cap.
- [ ] **2.2** Implement **capability-based permissions** (`can_read_vfs`, `can_write_vfs`, `can_network`, `can_filesystem`, `can_spawn_process`, `can_load_native`).
- [ ] **2.3** **Trust tiers**: `core` (engine-shipped, fully trusted), `workshop` (sandboxed by default), `local` (untrusted but user-installed), `dev` (full power, dev mode only).
- [ ] **2.4** **Process isolation** for high-risk mods: heavy mods can opt into a separate process with IPC; engine survives crashes.
- [ ] **2.5** **Crash containment**: a mod that crashes its sandbox reloads without taking down the editor.
- [ ] **2.6** **Audit log** of mod actions: every filesystem, network, and process call logged with mod id + timestamp.
- [ ] **2.7** **One-click mod disable** from the crash dialog (auto-prompts the user when a mod is suspected).
- [ ] **2.8** **Mod runtime versioning**: mods declare `runtime = "lua5.4"`, `runtime_version = ">=5.4.0 <6.0.0"`; engine provides a compatibility shim if the runtime differs.
- [ ] **2.9** **Memory budget** per mod (`memory_budget = "256MB"`); over-budget allocations throw.
- [ ] **2.10** **Rate limiting**: a runaway `tick` hook gets throttled, then disabled.

---

## Pillar 3: Scripting (Lua + Python + WASM)

### 3.1 Lua (Primary)
- [ ] **3.1.1** Embed **Lua 5.4** with a hard version promise. Lua is THE modding language.
- [ ] **3.1.2** Expose the entire engine API to Lua via a **generated binding** (from a `.qdef` schema, not hand-written).
- [ ] **3.1.3** Provide a **Lua standard library extension**: `q3d.io`, `q3d.math`, `q3d.scene`, `q3d.actor`, `q3d.asset`, `q3d.render`, `q3d.audio`, `q3d.ui`, `q3d.event`, `q3d.vfs`, `q3d.thread`.
- [ ] **3.1.4** **Hot-reload Lua files** on save; mid-tick reloads buffered, frame-boundary reloads immediate.
- [ ] **3.1.5** **Source maps**: errors point to original `.lua`, not compiled bytecode.
- [ ] **3.1.6** **Debug adapter** (DAP) for VSCode / Zed / Neovim: breakpoints, step, watch, eval.
- [ ] **3.1.7** **REPL inside the engine**: in-app Lua console with autocomplete, syntax highlighting, history.
- [ ] **3.1.8** **Coroutine-first** API: any long-running operation (`actor.move_to(...)`, `load_model(...)`) yields.
- [ ] **3.1.9** **Type annotations** via LuaLS (`---@class Actor`, `---@field ...`); bind them to the runtime for static analysis.
- [ ] **3.1.10** **WASM target for Lua scripts** (via w2c2 / wlua) so mods can run in the web build.

### 3.2 Python (Scientific / Power-User)
- [ ] **3.2.1** Embed **CPython 3.12+** as an optional second scripting language.
- [ ] **3.2.2** Provide the **same `q3d.*` Python module** generated from the same `.qdef` schema as Lua.
- [ ] **3.2.3** NumPy-aware bindings: `actor.vertices` returns a numpy array; `actor.set_positions(arr)` accepts one.
- [ ] **3.2.4** **Jupyter kernel** for the engine: drive the editor from a notebook, perfect for batch generation.
- [ ] **3.2.5** **Mods can be pure Python**; declare `runtime = "python"`.
- [ ] **3.2.6** Optional **Pyodide** for the web build.

### 3.3 WASM (Polyglot)
- [ ] **3.3.1** Load **arbitrary WASM modules** as mods (Rust, AssemblyScript, Go, Zig, etc.).
- [ ] **3.3.2** WIT-defined host bindings matching the Lua/Python surface.
- [ ] **3.3.3** WASI support: filesystem (sandboxed), clock, random.
- [ ] **3.3.4** WASM modules hot-reload like Lua.

### 3.4 Cross-Language Interop
- [ ] **3.4.1** Lua can call Python, Python can call Lua, both can call WASM.
- [ ] **3.4.2** Shared type system: an Actor object is the *same* Actor object across all three runtimes.
- [ ] **3.4.3** Shared GC roots: one engine-side object can be referenced by all three runtimes without double-free.

---

## Pillar 4: Content Authoring Pipelines

Every format below must be importable from a mod folder with **zero engine recompile**.

### 4.1 3D Models
- [ ] **4.1.1** **glTF 2.0 / GLB** (primary). Full spec: PBR, KHR_animation_pointer, KHR_mesh_quantization, EXT_mesh_gpu_instancing, KHR_draco_mesh_compression.
- [ ] **4.1.2** **FBX** with full animation curves, blend shapes, skinning.
- [ ] **4.1.3** **OBJ / PLY** (static meshes).
- [ ] **4.1.4** **VRM** (anime-style avatars) -- first-class for character modding.
- [ ] **4.1.5** **USD / USDA / USDC** (Pixar/Universal Scene Description).
- [ ] **4.1.6** **Collada DAE** (legacy interop).
- [ ] **4.1.7** **MD2/MD3/MD5** (id Tech engines -- retro modding nostalgia).
- [ ] **4.1.8** **Source Engine MDL** (GMod modding nostalgia -- proper GMod homage).
- [ ] **4.1.9** **Quake .bsp / .mdl** for level importing.
- [ ] **4.1.10** **SVG → 3D extrusion** for simple 2D-shape-into-3D assets.
- [ ] **4.1.11** **Procedural meshes** via Lua: `mesh:triangle(a, b, c)`, `mesh:quad(...)`, `mesh:subdivide(...)`.
- [ ] **4.1.12** **Runtime mesh generation**: `Actor.from_mesh(math.generate_torus(32, 16))` for code-driven content.
- [ ] **4.1.13** **LOD generation** automatic on import, manual override per mod.
- [ ] **4.1.14** **Mesh decimation** (QEM-based) for LOD0/1/2/3 generation.
- [ ] **4.1.15** **Texture baking** on import: generate ambient occlusion, normals, thickness maps.
- [ ] **4.1.16** **Format auto-detection** by magic bytes, not extension.
- [ ] **4.1.17** **Model preview panel** inside the asset browser with orbit camera.

### 4.2 Textures & Images
- [ ] **4.2.1** PNG, JPEG, WebP, AVIF, HEIC, TIFF, TGA, BMP, DDS, KTX2.
- [ ] **4.2.2** **Krita / Photoshop .psd / .kra** with layer preservation.
- [ ] **4.2.3** **SVG** for vector UI/decals.
- [ ] **4.2.4** **EXR / HDR** for high dynamic range.
- [ ] **4.2.5** **Texture arrays, cubemaps, 3D textures** for advanced shaders.
- [ ] **4.2.6** **Mip generation** automatic, sRGB vs linear auto-detected.
- [ ] **4.2.7** **GPU compressed texture support** (BC1-BC7, ETC2, ASTC).
- [ ] **4.2.8** **Texture variants per platform** (`texture.png` + `texture.dxt.ktx2` fallback).
- [ ] **4.2.9** **Texture atlasing** automatic on import for UI mods.
- [ ] **4.2.10** **Color space conversion** tool: linear/sRGB/HDR/spectral.

### 4.3 Fonts
- [ ] **4.3.1** TTF, OTF, WOFF, WOFF2, Variable fonts, Color emoji fonts (CBDT/SBIX/OpenType-SVG).
- [ ] **4.3.2** **Bitmap fonts** (BMFont, AngelCode) for retro modding.
- [ ] **4.3.3** **MSDF / MTSDF** for scalable signed-distance-field text.
- [ ] **4.3.4** **Procedural fonts**: modders define glyphs as Lua-callable functions for generative type.
- [ ] **4.3.5** **3D extruded text** from any font.
- [ ] **4.3.6** **Right-to-left + top-to-bottom scripts** for full unicode support.

### 4.4 Video
- [ ] **4.4.1** Any format FFmpeg supports (MP4, MOV, WebM, MKV, AVI).
- [ ] **4.4.2** **Hardware decoding** (NVDEC, VAAPI, VideoToolbox, DirectX Video Acceleration).
- [ ] **4.4.3** **Video as texture** for video backgrounds, character lipsync, TV props.
- [ ] **4.4.4** **Live video sources** (webcams, capture cards, NDI) for studio use.

### 4.5 Streaming / Networked Content
- [ ] **4.5.1** **HTTP/HTTPS streaming** for workshop assets on first reference.
- [ ] **4.5.2** **P2P asset sharing** via BitTorrent/WebTorrent for large mods.
- [ ] **4.5.3** **CDN caching** baked into the mod loader for popular mods.

---

## Pillar 5: Actor / Character Modding

Actors are the heart of GMod-style modding. Make them **infinitely malleable**.

### 5.1 Actor Definition Format
- [ ] **5.1.1** **`.actor` files** (YAML/JSON/TOML) describe an actor without any code: model, body parts, animations, sounds, default costume, properties, default scene.
- [ ] **5.1.2** **`.actor.lua` companion** for actors that need scripted behavior (no compile, hot-reloaded).
- [ ] **5.1.3** **Multiple model formats per actor** (`main = "char.glb"`, `head = "head.glb"`, `body = "body.glb"` for body-part swapping).
- [ ] **5.1.4** **Skeletal hierarchy declaration** in `.actor` (override any part of the imported rig).
- [ ] **5.1.5** **Default animations** mapping: declare which `.anim` file provides `walk`, `idle`, `talk`, `wave`, etc.
- [ ] **5.1.6** **Costume slots** in `.actor` (head, body, accessory1-9) for layer-based cosmetics.
- [ ] **5.1.7** **Actor traits** (`can_swim`, `can_fly`, `has_wings`, `cast_shadows`, `physics = "ragdoll"`).
- [ ] **5.1.8** **Voice / audio slots** with phoneme map for lip-sync.
- [ ] **5.1.9** **Personality metadata** for AI/LLM-driven behavior (text prompt per emotion).

### 5.2 Body-Part Swapping (Costume System)
- [ ] **5.2.1** **Slot-based**: `head`, `hair`, `hat`, `top`, `bottom`, `shoes`, `accessory1`-`accessory9`, `left_hand`, `right_hand`, `cape`, `wings`, `tail`, `aura`.
- [ ] **5.2.2** **Per-slot preview** in the browser -- hover, see a 360-degree rotating model wearing that slot.
- [ ] **5.2.3** **Mix-and-match across actors** -- wear another actor's hat if dimensions match (auto-fit, with optional manual override).
- [ ] **5.2.4** **Skin overrides** (texture-only) vs **mesh overrides** (full swap).
- [ ] **5.2.5** **Recolor support** -- swap palette indices in palette-mode, multiply color in true-color.
- [ ] **5.2.6** **Material variants** (matte, glossy, metallic) per costume piece.
- [ ] **5.2.7** **Hidden-slot system** for internal cosmetic pieces (body textures under clothing).

### 5.3 Custom Rigs & Skeletons
- [ ] **5.3.1** **Runtime skeleton remapping**: import a model with one rig, map it to a different skeleton, animations work.
- [ ] **5.3.2** **Bone-name aliases** (e.g., `mixamorig:Spine` → `spine_01`).
- [ ] **5.3.3** **Retargeting rules** in `.actor` (`retarget_from = "minecraft_steve"`).
- [ ] **5.3.4** **Humanoid IKRig** standard: mods targeting `q3d://rigs/humanoid` auto-get walking, pointing, IK.
- [ ] **5.3.5** **Quadruped IKRig** standard.
- [ ] **5.3.6** **Custom IKRig definition** by mod (`rigs/dog.rig.lua`).
- [ ] **5.3.7** **Bone merging** (combine two models' skeletons for hybrid characters).

### 5.4 Actor Behavior (Scripted)
- [ ] **5.4.1** **Lifecycle hooks**: `on_spawn(actor)`, `on_destroy(actor)`, `on_frame(actor, dt)`, `on_collision(actor, other)`.
- [ ] **5.4.2** **State machines** in `.actor.lua` with first-class `State` class.
- [ ] **5.4.3** **Behavior trees** (`BehaviorTree` class) for AI.
- [ ] **5.4.4** **Pathfinding**: A* on navmesh, navmesh auto-generated from scene geometry.
- [ ] **5.4.5** **Inverse kinematics**: limb solvers, look-at, FABRIK, CCD.
- [ ] **5.4.6** **Personality-driven dialogue** with LLM integration (off by default, opt-in per mod).
- [ ] **5.4.7** **Crowd simulation**: mods can spawn hundreds of simple AI actors.

---

## Pillar 6: Animation Modding

- [ ] **6.1** Define a **canonical `.anim` format**: keyframe-based, type-safe, language-agnostic.
- [ ] **6.2** Import from: glTF animations, FBX, BVH (motion capture), Maya `.anim`, Blender `.blend` (via blendfile lib).
- [ ] **6.3** **Procedural animations** via Lua: `animation.from_function(function(t) return transform end)`.
- [ ] **6.4** **Additive animation layers** (à la Source Engine).
- [ ] **6.5** **Animation blending tree** editor (per-mod): defines how `walk`+`aim`+`wound` combine.
- [ ] **6.6** **Animation retargeting** across actors with compatible rigs.
- [ ] **6.7** **Animation LOD** -- far away actors get fewer keyframes.
- [ ] **6.8** **Compression** of animations: lossy quantization (Bezier → spline → quaternion-log).
- [ ] **6.9** **Streaming animations**: long mocap files stream from disk, no full load.
- [ ] **6.10** **Animation event tracks**: embed `play_sound`, `spawn_particle`, `trigger_event` at specific frames.
- [ ] **6.11** **Animation hot-reload**: edit an anim in Blender, hit a hotkey, see the result instantly.
- [ ] **6.12** **Timeline editor** in-engine (already on roadmap 7.3) but every panel is a mod (see Pillar 11).
- [ ] **6.13** **Dope sheet + graph editor** as separate optional modded panels.
- [ ] **6.14** **Pose library**: store named poses (`angry_lean`, `surprised_step`) per actor.
- [ ] **6.15** **Inverse kinematics in editor** (Pillar 10.3 expanded): pose character with end-effectors, animation generated.
- [ ] **6.16** **Motion matching** for high-fidelity character animation.
- [ ] **6.17** **Physics-driven secondary motion**: cloth, hair, tails -- all scripted per actor.
- [ ] **6.18** **Animation marketplace API** within the engine (browse + buy workshop anims).

---

## Pillar 7: Scene / Level Modding

- [ ] **7.1** **Scene = directory of files**: `scene.toml` (config), `actors/`, `props/`, `lights/`, `triggers/`, `scripts/main.lua`, `navmesh/`.
- [ ] **7.2** **Prefab system**: any group of actors can be saved as a `.prefab` and re-instanced.
- [ ] **7.3** **Prefab variants**: instance with overrides (`prefab.gnome` with `red_hat = true`).
- [ ] **7.4** **Nested prefabs**: prefabs reference other prefabs; engine resolves at load.
- [ ] **7.5** **Streaming worlds**: huge scenes split into cells, streamed in/out based on camera proximity.
- [ ] **7.6** **Scene layers** (rendering, collision, navigation, gameplay -- each mod-togglable).
- [ ] **7.7** **Scene layers per mod** (e.g., mod adds a "rain" layer that doesn't affect the original).
- [ ] **7.8** **Triggers & volumes** as first-class scene objects: `BoxTrigger`, `SphereTrigger`, `TriggerOnce`, `TriggerOnEvent`.
- [ ] **7.9** **Scripted scene logic** in `scene.lua` with full engine access.
- [ ] **7.10** **Navmesh generation** runtime from walkable surfaces; mods can override.
- [ ] **7.11** **Lightmaps** optional and per-mod: bake once, ship with the mod.
- [ ] **7.12** **Scene LODs**: high-detail near camera, low-detail far.
- [ ] **7.13** **Occlusion culling** with PVS precomputed or runtime.
- [ ] **7.14** **Scene templates**: `office`, `park`, `space_station`, `fantasy_forest` shipped + user-created.
- [ ] **7.15** **Backgrounds as scenes**: pre-rendered 2.5D backgrounds can themselves be moddable scenes.
- [ ] **7.16** **Cutscene system**: cinematics with camera rails, shot lists, scripted events.
- [ ] **7.17** **Mirror/window portals** -- render-to-texture scenes for viewports within scenes.
- [ ] **7.18** **Weather/time-of-day** as a mod: override sky, sun, fog with custom logic.
- [ ] **7.19** **Scene physics** runtime toggle: static vs dynamic.
- [ ] **7.20** **Scene sharing format** `.qscene` -- a portable, self-contained mod-scene bundle.

---

## Pillar 8: Rendering & Shader Modding

- [ ] **8.1** **Shader system** built on **SPIR-V** (cross-compiled to GLSL/HLSL/MSL/WGSL) for write-once-run-anywhere.
- [ ] **8.2** **`.shader` files** (Lua-style syntax) compile to SPIR-V.
- [ ] **8.3** **Material format** `.mat` references shaders + textures + uniforms.
- [ ] **8.4** **Node-based material editor** as a mod (and the engine ships a working one as Pillar 11.x).
- [ ] **8.5** **Material templates** (`PBR`, `Toon`, `Cel`, `Fresnel`, `Glass`, `Skin`, `Hair`, `Eye`, `Cloth`, `Subsurface`, `Outline`, `Hologram`).
- [ ] **8.6** **Per-actor material override** via script or `mod.toml`.
- [ ] **8.7** **Per-mod render passes** -- a mod can insert a fullscreen pass between deferred stages.
- [ ] **8.8** **Compute shaders** exposed to Lua for procedural effects.
- [ ] **8.9** **Post-processing stack** as a chain of mods: each post-FX is a self-contained `.postfx` mod.
- [ ] **8.10** **Camera mods**: third-person, first-person, cinematic, security, picture-in-picture -- all hot-swappable.
- [ ] **8.11** **Custom render targets** mods can create + write to.
- [ ] **8.12** **Reflection probes** & **environment maps** as scene assets.
- [ ] **8.13** **Volumetric fog/lighting** as optional mod.
- [ ] **8.14** **Ray-traced reflections** as optional mod (RT-capable GPUs only).
- [ ] **8.15** **Path-traced mode** as opt-in (gated by hardware + user opt-in).
- [ ] **8.16** **Retro CRT/256-color mode** shipped as a mod (already in roadmap, but as a mod it gets this pillar's polish).
- [ ] **8.17** **Stereo rendering** (VR/AR) as a mod.
- [ ] **8.18** **Hot shader reload** with `RenderDoc`-style capture-on-error.
- [ ] **8.19** **Shader compile errors** show in-editor with original source location, not GLSL line 1234.
- [ ] **8.20** **Performance budget** per shader (instruction count, texture samplers); over-budget shaders warn.

---

## Pillar 9: Audio Modding

- [ ] **9.1** **Audio formats**: WAV, MP3, OGG, FLAC, Opus, AIFF, MIDI, tracker formats (.mod/.xm/.it/.s3m).
- [ ] **9.2** **`.sound` files** (TOML/JSON) wrap audio: path, volume, pitch, falloff, loop, category, max_instances.
- [ ] **9.3** **Sound banks**: collections of `.sound` files shareable as one mod asset.
- [ ] **9.4** **3D positional audio** with HRTF for headphones.
- [ ] **9.5** **Audio buses**: master, music, SFX, voice, ambient, UI -- mods can add new buses.
- [ ] **9.6** **Real-time DSP** per bus: reverb, EQ, compression, distortion, chorus -- chainable, scriptable.
- [ ] **9.7** **Sound events** (`event "actor_footstep"`) for SFX design -- mods rebind events to sounds.
- [ ] **9.8** **Adaptive music** -- `.mus` files define layers that duck/boost based on game state.
- [ ] **9.9** **Procedural audio** via Lua/WASM: oscillators, filters, envelopes, LFOs.
- [ ] **9.10** **Voice synthesis** mods: TTS engines pluggable (SAPI, eSpeak, ElevenLabs, custom neural).
- [ ] **9.11** **Lip sync** generation: extract phonemes from audio (forced alignment), drive mouth shapes.
- [ ] **9.12** **Audio recording** from microphone, with waveform editor mod.
- [ ] **9.13** **Sound replacer** mod: substitute one actor's footsteps for another's, or replace every voice line.
- [ ] **9.14** **Mixing board UI** as a mod (Pillar 11).
- [ ] **9.15** **Voice chat** integration (Discord/RTC) for collaborative movies.
- [ ] **9.16** **Sound occlusion** by scene geometry.
- [ ] **9.17** **Doppler effect** for moving actors.

---

## Pillar 10: UI / HUD Modding

- [ ] **10.1** **UI framework** exposed: layout, widgets, events, theming, animations, all Lua-scriptable.
- [ ] **10.2** **Widget library**: button, label, image, slider, list, tree, dropdown, text input, color picker, file picker, draggable, resizable, scrollable, modal, tooltip, tab, accordion.
- [ ] **10.3** **Layout systems**: flexbox, grid, absolute, anchored, dock, free.
- [ ] **10.4** **Theming**: every color, font, padding, animation is themeable; mods can ship `.theme` files.
- [ ] **10.5** **Style override per mod**: a mod can restyle only the elements it owns.
- [ ] **10.6** **Animations** on UI: fade, slide, scale, custom curves, spring physics.
- [ ] **10.7** **Localization**: every string tagged, mods ship translations.
- [ ] **10.8** **Accessibility**: screen reader, high contrast, large text, focus rings, keyboard nav.
- [ ] **10.9** **HUD mods**: the runtime HUD during playback is a UI mod; default shipped, but user-replaceable.
- [ ] **10.10** **Main menu moddable**: a mod can completely rebrand the home screen.
- [ ] **10.11** **In-game overlays** (e.g., tooltips, dialogues) are moddable.
- [ ] **10.12** **Easel panels** (3DMM's per-tool side panel) are now full moddable UI surfaces.
- [ ] **10.13** **Browser categories** are user-creatable with custom thumbnails + filters.
- [ ] **10.14** **Custom cursors** per tool, per scene, per mod.
- [ ] **10.15** **Multi-window**: drag a panel out to a second monitor, moddable.
- [ ] **10.16** **Touch / pen / gamepad** input mapping for every UI element.
- [ ] **10.17** **Markdown + rich text** rendering for in-app docs/help.
- [ ] **10.18** **Live UI reload**: edit a `.ui.lua` file, see the panel change without restart.
- [ ] **10.19** **WASM-rendered UI** for performance-critical custom widgets.

---

## Pillar 11: Tools & Editor Modding

This is the **GMod spawn-menu killer feature**: every tool, every gizmo, every editor panel is a mod.

- [ ] **11.1** **Tool registration API**: `engine.register_tool({ id = "spawn", label = "Spawn Actor", icon = "...", panel = "ui/spawn.lua", hotkey = "Q", activate = function() end, deactivate = function() end, on_frame = function() end, on_click = function(view, hit) end })`.
- [ ] **11.2** **Tool categories** (modeling, animation, scripting, debug) -- mods can add new categories.
- [ ] **11.3** **Spawnable mod**: place any actor from any mod in any scene.
- [ ] **11.4** **Manipulator gizmos** (move, rotate, scale) are pluggable; mods can add custom gizmos (e.g., bezier handles, spline gizmos).
- [ ] **11.5** **Viewport overlay UI** drawn from Lua (debug overlays, selection brackets, measurement tools).
- [ ] **11.6** **Editor modes** as mods: `Object`, `Sculpt`, `Paint`, `Animate`, `Script`, `Cinematic` -- all mod-shippable.
- [ ] **11.7** **Custom viewport rendering** per mode: sculpt mode shows wireframe + cavity shading, paint shows color picker, etc.
- [ ] **11.8** **Hotkey system** fully moddable: mods register, conflict-resolution UI shown to user.
- [ ] **11.9** **Command palette** (`Ctrl+Shift+P`): every tool, every function, every script, all searchable.
- [ ] **11.10** **Right-click context menus** fully moddable per object type.
- [ ] **11.11** **Tool palettes** (left/right/top/bottom) are dockable, savable, shareable per mod profile.
- [ ] **11.12** **Asset library panels** per mod: every mod contributes a "this is what I added" panel.
- [ ] **11.13** **Inspector panels** pluggable per object type: a modded actor gets a modded inspector.
- [ ] **11.14** **Tool output** as a stream mods can subscribe to (selection changed, file dropped, etc.).
- [ ] **11.15** **Tools as standalone executables**: a mod's tool can be invoked headless for batch processing.
- [ ] **11.16** **Tool licensing**: paid tools can be sold via workshop with platform-level DRM hooks.
- [ ] **11.17** **Tool analytics opt-in**: mod authors can see usage stats (always anonymous, always opt-in).
- [ ] **11.18** **Tutorial system**: each tool can ship its own interactive tutorial.

---

## Pillar 12: Event / Hook / Network API

- [ ] **12.1** **Central event bus**: every engine event is published; mods subscribe.
- [ ] **12.2** **500+ documented events** with stable IDs and types:
  - Scene: `scene_loaded`, `scene_saved`, `scene_frame_advanced`, `actor_added`, `actor_removed`, `actor_moved`, `actor_animation_changed`.
  - Editor: `tool_activated`, `tool_deactivated`, `selection_changed`, `property_edited`, `undo`, `redo`.
  - Runtime: `tick_pre`, `tick_post`, `render_pre`, `render_post`, `audio_mixed`, `physics_step`.
  - Asset: `asset_imported`, `asset_loaded`, `asset_unloaded`, `asset_missing`.
  - Mod: `mod_loaded`, `mod_unloaded`, `mod_error`, `mod_reloaded`.
- [ ] **12.3** **Priority-ordered hooks**: a mod can run before/after specific other mods' hooks.
- [ ] **12.4** **Hook cancellation**: `event.cancel()` short-circuits the default handler.
- [ ] **12.5** **Event replay**: record all events, replay them later (great for debugging and tutorials).
- [ ] **12.6** **Event filters**: subscribe to `actor_moved` only for a specific actor id.
- [ ] **12.7** **Event throttling** for high-frequency events (`actor_moved` while dragging is 60Hz; `actor_animation_changed` is event-driven).
- [ ] **12.8** **Network event bus**: events can be marked `replicated = true` for multiplayer movie editing.
- [ ] **12.9** **Cross-process events**: tools running in their own process can fire events back to the main editor.
- [ ] **12.10** **Custom event types**: mods can declare new event types for their own use.
- [ ] **12.11** **Event inspector UI**: live tail of events, filter, pause, inspect payloads.
- [ ] **12.12** **Typed events** via `q3d.event.define("MyEvent", { x = "number", y = "number" })` for static checking.

---

## Pillar 13: Save Format Modding

- [ ] **13.1** **`.3mm` format becomes fully extensible** with `extensions` block.
- [ ] **13.2** **Mods can register new chunk types** (`CTG_MOD_SOMETHING`) without engine changes.
- [ ] **13.3** **Schema registry**: every mod's custom chunk has a schema; engine validates on load.
- [ ] **13.4** **Forward compatibility**: a mod reading a future-version chunk treats it as opaque.
- [ ] **13.5** **Backward compatibility**: engine always loads vanilla `.3mm` files; mod chunks gracefully ignored if missing.
- [ ] **13.6** **Asset references in saves**: by VFS path + content hash; load order independent.
- [ ] **13.7** **Save migration framework**: modders write migrations to upgrade old saves to new format.
- [ ] **13.8** **Diff format**: `.3mmdiff` -- minimal diff between two movies, version-control friendly.
- [ ] **13.9** **Streaming save format**: large movies can save incrementally.
- [ ] **13.10** **Auto-save** with crash recovery.
- [ ] **13.11** **Mod-aware save dialog**: warns if a save depends on a missing mod.
- [ ] **13.12** **Bundle save**: `.3mm` can optionally embed all dependencies for a single-file project.
- [ ] **13.13** **Partial loads**: open a `.3mm`, see it warn about missing mods, open anyway in a "best effort" mode.
- [ ] **13.14** **YAML save format** for human-editable projects (optional, alongside binary).
- [ ] **13.15** **Git-friendly text format** for collaborative projects.

---

## Pillar 14: Hot-Reload & Live Editing

- [ ] **14.1** **File system watcher**: native, recursive, debounced.
- [ ] **14.2** **Granular reloads**:
  - Texture: re-upload GPU memory in place.
  - Model: re-import, re-create GPU buffers.
  - Script: stop, recompile, restart, preserve state where possible.
  - Shader: recompile SPIR-V, swap pipeline.
  - UI: tear down + rebuild, preserve user state.
- [ ] **14.3** **State preservation**: hot-reload of a script keeps the same Lua state if compatible; otherwise snapshots key vars and restores them.
- [ ] **14.4** **Editor keeps running during reload**: no flickering, no full repaint, just a small "1 file reloaded" toast.
- [ ] **14.5** **Reload scheduling**: heavy reloads deferred to next idle frame to keep editing fluid.
- [ ] **14.6** **Reload batching**: 5 files change in 100ms → 1 reload, not 5.
- [ ] **14.7** **Manual reload hotkey** for stubborn cases.
- [ ] **14.8** **Reload on focus**: alt-tab away and back → reload (catches external edits).
- [ ] **14.9** **Multi-tool live sync**: edit shader in editor A, see result in editor B without re-open.
- [ ] **14.10** **External tool integration**: detect file changes from VSCode, Blender, Photoshop, Audacity.
- [ ] **14.11** **Reload undo/redo**: if a reload breaks things, the previous state is one undo away.
- [ ] **14.12** **Reload performance metrics** to spot slow reloads.

---

## Pillar 15: Mod Manager, Workshop & Distribution

- [ ] **15.1** **In-engine Mod Manager** UI (or as a sibling app): browse, install, enable, disable, update, configure, uninstall.
- [ ] **15.2** **Workshop integration** (Steam Workshop as one provider; first-class platform).
- [ ] **15.3** **HTTP workshop** for non-Steam platforms: REST API for upload, download, search.
- [ ] **15.4** **Mod categories**: characters, scenes, animations, scripts, shaders, audio, UI, full-conversion, tools, utilities.
- [ ] **15.5** **Tags + search** with full-text + faceted filtering.
- [ ] **15.6** **Curated collections**: "Best horror mods", "Family-friendly", "Retro 90s", curated by community.
- [ ] **15.7** **Mod pages** with rich content: description, screenshots, video, changelog, comments, ratings, dependencies tree.
- [ ] **15.8** **Mod versioning & auto-update** with stable update channels (`stable`, `beta`, `nightly`).
- [ ] **15.9** **Mod authors** can mark `experimental` mods to opt into beta engine builds.
- [ ] **15.10** **Multi-author mods** with shared ownership.
- [ ] **15.11** **Mod awards / featured** workflow.
- [ ] **15.12** **Mod analytics dashboard** for authors (downloads, ratings, crash reports).
- [ ] **15.13** **Mod packager** (`q3m pack`) with smart dedup across mods.
- [ ] **15.14** **Mod offline cache** for air-gapped environments.
- [ ] **15.15** **Mod whitelist/blacklist** for parents/educators/companies.
- [ ] **15.16** **Mod voting** with anti-spam measures.
- [ ] **15.17** **Mod discoverability**: "users who installed X also installed Y".
- [ ] **15.18** **Mod install via URL** (e.g., `q3d://install/<id>`).
- [ ] **15.19** **Mod updates without restart** (background download, apply on next scene load).
- [ ] **15.20** **Mod conflict resolution UI**: see what's overridden, choose winner.

---

## Pillar 16: Engine Code Patching (Overlay Mods)

This is the **escape hatch** for when a mod truly needs to change engine internals -- but with explicit, opt-in, brutal honesty about consequences.

- [ ] **16.1** **Overlay mod directory**: `<install>/overlays/<name>/` with `.cpp`, `.h`, and `overlay.toml`.
- [ ] **16.2** **Patch format** inspired by `git apply`: target file, before-context, after-context; engine applies at startup.
- [ ] **16.3** **Or full source files**: overlay mod can replace whole files.
- [ ] **16.4** **Build system hook**: CMake rebuilds with overlays applied; cached when no overlay changes.
- [ ] **16.5** **Engine version pin**: overlay specifies `engine_version = "1.4.2"`; engine refuses to load mismatched overlays.
- [ ] **16.6** **Overlay mod manager UI**: enable/disable, see what each modifies, easy revert.
- [ ] **16.7** **Overlay conflict detection**: two overlays patching the same line warn loudly.
- [ ] **16.8** **Overlay testing** -- quick test build for "do my overlays still compile" before saving.
- [ ] **16.9** **"Stock engine" mode** -- one click to disable all overlays and verify the user's complaint isn't their own mod.
- [ ] **16.10** **Overlay changelog tracking**: when engine updates, engine shows which overlays may now be broken.
- [ ] **16.11** **Overlay preview** for dev branches: "if you upgrade engine, here's what would break".
- [ ] **16.12** **No silent overlays**: user always knows when an overlay is active (badge in title bar).
- [ ] **16.13** **Overlay SDK** -- upstream patches to make common moddable hooks more accessible (reduce overlay need over time).

---

## Pillar 17: Per-Mod Sandboxing & Permissions

- [ ] **17.1** **Permission tokens** a mod requests: `fs.read`, `fs.write.user`, `fs.write.engine`, `net.outbound`, `net.inbound`, `process.spawn`, `native.load`, `render.fullscreen`, `audio.capture`, `display.external`.
- [ ] **17.2** **Permission grants** via `mod.toml` declared; user must approve at install if higher than default tier.
- [ ] **17.3** **Per-feature toggles** in mod settings UI: "this mod wants to use the network, allow? (Yes / Yes for this session / No)".
- [ ] **17.4** **Capability keys** (per-mod encryption keys for sensitive operations like paid content decryption).
- [ ] **17.5** **Quota system**: each mod has storage/network/CPU quotas.
- [ ] **17.6** **Safe mode**: `q3d --safe` boots with NO mods except core.
- [ ] **17.7** **Single-mod boot**: `q3d --only=modid` for testing.
- [ ] **17.8** **Per-mod log channel**: each mod's logs separated; modders can subscribe to other mods' logs with permission.
- [ ] **17.9** **Memory isolation**: mod A cannot read mod B's memory; both can read engine's exposed state.
- [ ] **17.10** **OS-level isolation** (optional, opt-in): each mod in its own OS process.

---

## Pillar 18: Modding the Modder (Meta / Devkit)

- [ ] **18.1** **Mod Devkit** shipped as a mod + a separate IDE: code editor, scene preview, asset inspector, profiler, all in one.
- [ ] **18.2** **Project templates**: "Empty mod", "Character mod", "Scene mod", "Tool mod", "Shader mod", "UI mod", "Audio mod", "Full conversion mod".
- [ ] **18.3** **One-click new mod** with boilerplate.
- [ ] **18.4** **One-click build & test** (`F5` in devkit → launches Quack3DMM with the mod loaded).
- [ ] **18.5** **Built-in linter** for `mod.toml`, Lua, Python, scripts.
- [ ] **18.6** **Type checker** (LuaLS + mypy) integrated.
- [ ] **18.7** **Auto-formatter** for all languages.
- [ ] **18.8** **Debugger** with breakpoints, watch, step, eval, conditional breakpoints.
- [ ] **18.9** **Profiler**: per-function time, allocation tracking, draw call breakdown, GPU times.
- [ ] **18.10** **Memory inspector**: heap map, GC stats, leak detection.
- [ ] **18.11** **Asset inspector**: every mod's asset, with hot-reload buttons, dependency view.
- [ ] **18.12** **Dependency graph visualizer** for the whole mod load order.
- [ ] **18.13** **Performance budgets** ("mod must not exceed 5ms/frame") with CI enforcement.
- [ ] **18.14** **Live multiplayer test** ("invite a friend to your mod dev session").
- [ ] **18.15** **Auto-generated documentation** from `.qdef` schemas and `---@class` annotations.
- [ ] **18.16** **Mod changelog generator** from `git log` + manifest version bumps.
- [ ] **18.17** **Mod preview generator** (auto-screenshots from the test scene).
- [ ] **18.18** **Workshop uploader** with progress + draft + scheduled publish.
- [ ] **18.19** **A/B testing mod variants** in the devkit before publishing.
- [ ] **18.20** **AI assistant** (opt-in, opt-in) for mod authors: "create a character that looks like a robot but cute" → generates the assets, the manifest, the docs.

---

## Pillar 19: Versioning, Migration & Long-Term Mod Health

- [ ] **19.1** **API semver guarantees** with public/private split: private = can break, public = 5-year stability promise.
- [ ] **19.2** **Deprecation policy**: minimum 2 minor versions of deprecation warning before removal.
- [ ] **19.3** **Engine LTS releases** (Long Term Support) -- 3-year support, mods written for 1.x run on 1.x+5.
- [ ] **19.4** **Mod migration tool**: a 5-year-old mod opens in the new engine, gets auto-migrated, user notified.
- [ ] **19.5** **Migration reports**: "this mod uses 3 deprecated APIs, here's how to fix them".
- [ ] **19.6** **Engine compat shim layer**: mod authors' old code keeps working via shims; new code uses new APIs.
- [ ] **19.7** **API stability dashboard**: web page listing every API, its stability level, deprecation timeline.
- [ ] **19.8** **Mod "vintage" mode**: load a 2010 mod in 2030 engine with the original UI theme + API shims.
- [ ] **19.9** **Mod archival** when authors abandon: community forks become official, with author credit preserved.
- [ ] **19.10** **Open format forever**: the `.3mm` and `.q3m` specs are public, so even if the company dies, mods survive.

---

## Pillar 20: Test Suite for Mods (CI, Lint, Validation)

- [ ] **20.1** **Mod CI runner**: GitHub Action / generic CI command `q3m ci` runs lint + tests on every push.
- [ ] **20.2** **Mod unit testing framework** in Lua/Python (`q3d.test`): `assert_equals`, `assert_near`, mock engine APIs.
- [ ] **20.3** **Mod integration tests**: spawn a scene, run for N frames, assert no errors.
- [ ] **20.4** **Visual regression tests** for modded scenes.
- [ ] **20.5** **Performance regression tests** (mod must not slow down engine by more than X%).
- [ ] **20.6** **Memory regression tests** (mod must not leak).
- [ ] **20.7** **Compatibility tests** (mod works with mods A, B, C).
- [ ] **20.8** **Cross-platform tests** (Windows/macOS/Linux).
- [ ] **20.9** **Mod linter** checks: manifest validity, file references exist, scripts parse, shaders compile, no banned APIs, declared permissions match actual usage.
- [ ] **20.10** **Workshop auto-moderation**: new uploads run linter + smoke test; rejected mods show why.
- [ ] **20.11** **Mod badge system**: "Tested", "Verified", "Community Favorite", "Performance Verified", "Accessibility Verified".
- [ ] **20.12** **Mod crash reporting** via Sentry-like opt-in telemetry; mods that crash N times get auto-flagged for review.
- [ ] **20.13** **Mod A/B rollout**: publish to 10% of users first, watch crash rate, expand if clean.
- [ ] **20.14** **Mod rollback**: instantly unpublish bad mods from all users; engine warns users on next launch.
- [ ] **20.15** **Mod health dashboard** for engine team: aggregate health of all mods in the ecosystem.

---

## Cross-Cutting Concerns

These touch every pillar and are tracked separately to avoid duplication.

### X.1 Performance
- [ ] **X.1.1** **Mod performance budget**: aggregate mod overhead < 5ms/frame on a 2018 laptop.
- [ ] **X.1.2** **Mod profiler** built-in: see exactly which mod costs how much.
- [ ] **X.1.3** **Mod timeout enforcement**: a single mod can't hang a frame for > 50ms (configurable).
- [ ] **X.1.4** **Async mod APIs**: heavy ops (`load_model`, `compile_shader`) yield, never block the editor.

### X.2 Compatibility
- [ ] **X.2.1** **Original `.3MM` files load and play unchanged** in the modded engine -- the GMod promise to 3DMM fans.
- [ ] **X.2.2** **Original `.3CN`, `.CHK`, `.CHT` files** still work.
- [ ] **X.2.3** **Original SITOBREN pipeline** still works.
- [ ] **X.2.4** **Original scripts** still run; deprecated paths warn but don't fail.

### X.3 Documentation
- [ ] **X.3.1** **Modding wiki** at `wiki.q3d.dev` with: getting started, API reference, recipes, FAQ.
- [ ] **X.3.2** **Every API has at least one example** that compiles and runs.
- [ ] **X.3.3** **Every API has a stability badge** (Stable / Beta / Experimental / Deprecated).
- [ ] **X.3.4** **Changelog** with every API change, by version.
- [ ] **X.3.5** **Cookbook**: "How to add a custom character", "How to add a custom tool", "How to mod the menu", etc.
- [ ] **X.3.6** **Video tutorial series** for the most common mod types.
- [ ] **X.3.7** **API search in-engine**: `Ctrl+Shift+F` opens a docs panel bound to the current context.
- [ ] **X.3.8** **Example mods gallery** -- 50+ working examples downloadable from inside the engine.

### X.4 Community
- [ ] **X.4.1** **Official Discord** with channels per pillar: #modding-character, #modding-shader, #modding-tool, #modding-audio, #mod-help.
- [ ] **X.4.2** **Monthly mod jam**: theme announced, modders make mods, winners get featured.
- [ ] **X.4.3** **Modder of the month** spotlight.
- [ ] **X.4.4** **Office hours** with the engine team in Discord.
- [ ] **X.4.5** **Modder grants**: small stipends for high-quality educational mods.
- [ ] **X.4.6** **Roadmap voting**: community votes on which pillars to prioritize next.
- [ ] **X.4.7** **Public RFC process** for new modding APIs: discuss, iterate, freeze, ship.
- [ ] **X.4.8** **Translation drives** for the docs and the engine UI.

### X.5 Legal
- [ ] **X.5.1** **Mod license clarity**: mods default to "all rights reserved" but modders can pick MIT/CC/etc.
- [ ] **X.5.2** **Asset license tracking**: every asset in a mod has a license recorded; engine shows licenses on hover.
- [ ] **X.5.3** **Commercial mod path**: paid mods supported, with platform-level payment processing.
- [ ] **X.5.4** **DMCA process** for takedown requests.
- [ ] **X.5.5** **Trademark policy**: "Quack3DMM" name protected, mod namespaces free.
- [ ] **X.5.6** **Content moderation policy** for user-uploaded content.
- [ ] **X.5.7** **Modder code of conduct** enforced for workshop comments and reviews.

---

## Execution Order (The "Year 1" Plan)

To actually ship something resembling this, the dependency order matters.

**Quarter 1 (Foundation)**
- Pillar 1 (VFS) -- the foundation of everything
- Pillar 3.1 (Lua)
- Pillar 2 (Sandbox basics)
- Pillar 14 (Hot-reload basics)

**Quarter 2 (Content)**
- Pillar 4.1 (glTF import)
- Pillar 4.2 (texture import)
- Pillar 4.3 (font import)
- Pillar 4.5 (audio import)
- Pillar 5 (Actor modding basics)

**Quarter 3 (UX)**
- Pillar 6 (Animation)
- Pillar 7 (Scenes)
- Pillar 10 (UI)
- Pillar 11 (Tools)
- Pillar 15 (Mod Manager UI)

**Quarter 4 (Polish)**
- Pillar 8 (Shaders/render)
- Pillar 9 (Audio deep)
- Pillar 12 (Events)
- Pillar 13 (Saves)
- Pillar 18 (Devkit)
- Pillar 19 (Versioning)
- Pillar 20 (CI/tests)

**Year 2+**
- Pillar 3.2/3.3 (Python, WASM)
- Pillar 16 (Overlay mods)
- Pillar 17 (Advanced sandbox)
- All "stretch" sub-items

---

## The North Star

If the team can say **"yes, a complete stranger modded a new character, gave it a custom animation, placed it in a new scene with new props, gave it a custom voice, gave it a new tool to edit it, gave it a custom UI panel to configure it, did all of this without compiling the engine, did all of this on a cold install, did all of this in under 4 hours"** -- we won.

If a 12-year-old can do the above with a YouTube tutorial: we've changed animation forever.
