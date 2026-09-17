# ROADMAP.md -- Quack3DMM Modernization & Professional-Grade Additions

> A comprehensive roadmap for transforming Microsoft 3D Movie Maker from a
> historical reference into a fully functional, extensible, community-driven
> 3D animation and movie creation platform.

---

## Table of Contents

1. [Phase 0: Foundation & Build Fixes](#phase-0-foundation--build-fixes)
2. [Phase 1: Core Engine Modernization](#phase-1-core-engine-modernization)
3. [Phase 2: Content Import & Authoring Pipeline](#phase-2-content-import--authoring-pipeline)
4. [Phase 3: Modding & User-Side Modifications](#phase-3-modding--user-side-modifications)
5. [Phase 4: Scripting Language Extensions](#phase-4-scripting-language-extensions)
6. [Phase 5: Rendering & Visual Upgrades](#phase-5-rendering--visual-upgrades)
7. [Phase 6: Audio & Media Pipeline](#phase-6-audio--media-pipeline)
8. [Phase 7: UI/UX Modernization](#phase-7-uiux-modernization)
9. [Phase 8: Cross-Platform Support](#phase-8-cross-platform-support)
10. [Phase 9: Community & Distribution](#phase-9-community--distribution)
11. [Phase 10: Advanced Features](#phase-10-advanced-features)

---

## Phase 0: Foundation & Build Fixes

**Goal**: Get the project compiling and running on modern toolchains.

### 0.1 Compilation Fixes
- [ ] Audit and fix all pre-C++98 constructs for MSVC 2022, GCC 14+, Clang 18+
- [ ] Replace `GlobalAlloc`/`GlobalFree` in `STDMEM.C` with standard `malloc`/`free`
- [ ] Replace `MessageBox`-based error handling in `STDERR.C` with logging system
- [ ] Fix all implicit type conversions, signed/unsigned mismatches, and narrowing conversions
- [ ] Replace `#pragma warning(disable: ...)` with targeted `[[deprecated]]` or `[[nodiscard]]` attributes
- [ ] Add missing `#include <cstdint>` for fixed-width types throughout BRender
- [ ] Fix the `BR_CALLBACK` / `__cdecl` calling convention annotations for non-MSVC compilers
- [ ] Replace hand-rolled `HQ` handle system with `std::unique_ptr` / `std::vector` equivalents (or provide a compatibility shim)
- [ ] Modernize the macro-based RTTI (`RTCLASS_DEC`/`FIs()`/`FWouldBe()`) to use `dynamic_cast` or `constexpr` type IDs

### 0.2 Build System
- [ ] Complete CMake build for all components (currently BRender, Kauai, Engine, Studio)
- [ ] Port TOOLS (sitobren, tdfmake, mktmap) to CMake targets
- [ ] Add CI/CD pipeline (GitHub Actions) for Windows/Linux/macOS builds
- [ ] Add Conan/vcpkg integration for dependency management
- [ ] Create CMake presets for Debug/Release/ASan/UBSan configurations
- [ ] Fix CHOMP chunk compiler integration in CMake (currently finds via `find_program`)
- [ ] Generate compile_commands.json for IDE integration

### 0.3 Testing Infrastructure
- [ ] Create a Google Test / Catch2 test harness around the engine
- [ ] Port existing `UTEST.CPP` test cases to the modern framework
- [ ] Add unit tests for: chunky file read/write, script compilation, vector/matrix math
- [ ] Add integration tests for: actor creation, scene playback, movie save/load
- [ ] Add fuzz tests for: chunky file parser, script lexer, model loader

---

## Phase 1: Core Engine Modernization

**Goal**: Modernize the C++ codebase while preserving behavioral compatibility.

### 1.1 Memory Management
- [ ] Replace all `HQ` (handle-locked memory) with RAII wrappers (`std::unique_ptr`, `std::shared_ptr`)
- [ ] Implement a pool allocator for high-frequency allocations (vertices, faces, GOKs)
- [ ] Add memory-mapped file support for large chunky resources (backgrounds, models)
- [ ] Replace the manual reference counting in `BASE::AddRef()`/`Release()` with `std::shared_ptr` or intrusive ref counting
- [ ] Add a leak detector that replaces the `MARKMEM`/`MarkMem()` debug system

### 1.2 String & Data Handling
- [ ] Replace all fixed-size character arrays (`char rgch[kcchMax]`) with `std::string` / `std::string_view`
- [ ] Replace `GL` (growable list) and `PGL` (pointer growable list) with `std::vector`
- [ ] Replace `GG` (fixed growable) with `std::array` or `std::span`
- [ ] Replace `BSF`/`BSM` (byte stream) with `std::streambuf` or memory buffers
- [ ] Add `std::format` or `fmtlib` for all formatted string operations

### 1.3 Code Organization
- [ ] Move from flat namespace to nested namespaces (`quack3d::engine`, `quack3d::studio`, `quack3d::brender`)
- [ ] Remove all global variables (`vapp`, `vpcex`, `vpclip`, `vpsndm`) and replace with dependency injection or singleton wrappers
- [ ] Extract the "God class" `MVU` into smaller, focused tool classes (one per editing tool)
- [ ] Split `MOVIE.CPP` (~9000 LOC) into separate compilation units: MovieDocument, MovieView, MovieTools, MoviePlayback
- [ ] Split `SCENE.CPP` (~7500 LOC) into SceneTimeline, SceneActors, SceneEvents, SceneChop

### 1.4 Type Safety
- [ ] Replace all `void*` type-punning with proper types (especially in `ACTR::FRead`/`FWrite`)
- [ ] Replace C-style casts with `static_cast` / `reinterpret_cast` / `const_cast`
- [ ] Replace `#define` constants with `constexpr` values and `enum class` types
- [ ] Add strong typing for CTG (chunk type), CNO (chunk number), CID (command ID), CHID (chunk ID)
- [ ] Replace `long`-based color values with `br_colour` / `ACR` throughout

---

## Phase 2: Content Import & Authoring Pipeline

**Goal**: Allow users to import custom 3D models, textures, sounds, and animations.

### 2.1 3D Model Import
- [ ] **glTF 2.0 Importer** -- Import `.glb`/`.gltf` files as actor templates (TMPL)
  - Parse mesh hierarchy, materials, textures, and skeleton
  - Convert to BRender `br_model` format with proper face groups
  - Support skinned meshes (vertex bone weights -> per-frame morph targets)
  - Auto-generate body part sets from bone hierarchy naming conventions
- [ ] **FBX Importer** -- Import Autodesk `.fbx` files via the FBX SDK
  - Full animation support (keyframe -> per-frame model snapshots)
  - Material/texture extraction
- [ ] **OBJ/MTL Importer** -- Simple static mesh import for backgrounds and props
- [ ] **STL Importer** -- Import 3D print files as static models
- [ ] **Collada DAE Importer** -- Alternative interchange format
- [ ] Create an `IMPORT管线` class that converts external formats to chunky file chunks
- [ ] Support drag-and-drop import in the Studio UI (drag `.glb` file onto the viewport)

### 2.2 Texture & Material Import
- [ ] **PNG/JPEG/WebP Import** -- Replace BMP-only texture pipeline with modern formats
  - Use `stb_image` or `libpng` for decoding
  - Auto-convert to 8-bit indexed palette or upgrade to true-color rendering
- [ ] **DDS Import** -- DirectXTex for compressed texture formats (BC1-BC7)
- [ ] **SVG Import** -- Vector graphics for UI elements and decals
- [ ] **Normal Map Generation** -- Convert diffuse textures to normal maps for enhanced lighting
- [ ] **PBR Material Support** -- Extend `br_material` with metallic, roughness, AO parameters
- [ ] **Material Preset Library** -- Ship 50+ pre-made materials (wood, metal, glass, fabric, etc.)

### 2.3 Audio Import
- [ ] **MP3/OGG/WAV/AIFF Import** -- Support all common audio formats
  - Use `libsndfile` or `miniaudio` for decoding
  - Auto-resample to the engine's native sample rate
- [ ] **FLAC Import** -- Lossless audio for high-quality sound effects
- [ ] **MIDI Import** -- Import Standard MIDI Files (SMF) for background music
- [ ] **Text-to-Speech** -- Integrate a TTS engine (e.g., eSpeak, Microsoft SAPI) for generating voiceovers from text
- [ ] **Audio Recording** -- Modernize `SREC.CPP` with `miniaudio` or WASAPI for microphone recording

### 2.4 Animation Import
- [ ] **BVH Motion Capture Import** -- Import motion capture data as actor routes
  - Parse BVH hierarchy and channels
  - Map to `APV` (Actor Path Vertex) positions with frame interpolation
- [ ] **glTF Animation Import** -- Extract keyframe animations and convert to per-frame morphs
- [ ] **CSV/JSON Keyframe Import** -- Programmatic animation input for technical users
- [ ] **Audio-driven Animation** -- Generate lip-sync animations from audio waveforms (viseme extraction)

### 2.5 Background Import
- [ ] **Image-to-Background Pipeline** -- Import any image as a pre-rendered background
  - Auto-generate Z-buffer from depth estimation (MiDaS or similar)
  - Palette quantization to 256 colors (or upgrade to true-color rendering)
- [ ] **360-degree Panorama Import** -- Equirectangular images as immersive backgrounds
- [ ] **Video Background Import** -- Use video files as animated backgrounds
- [ ] **HDRI Lighting** -- Import HDRI environment maps for image-based lighting

---

## Phase 3: Modding & User-Side Modifications

**Goal**: Create a robust modding ecosystem that lets users extend the application without modifying core code.

### 3.1 Plugin Architecture (DLL/.so Extensions)
- [ ] **Define a Plugin API** -- Stable C ABI for third-party extensions
  ```
  // Example plugin interface
  struct Q3DPlugin {
      uint32_t version;
      const char* name;
      const char* author;
      int (*initialize)(Q3DHost* host);
      void (*shutdown)(void);
      // Registerable extension points:
      void (*register_importers)(Q3DImporterRegistry* reg);
      void (*register_renderers)(Q3DRendererRegistry* reg);
      void (*register_tools)(Q3DToolRegistry* reg);
      void (*register_effects)(Q3DEffectRegistry* reg);
  };
  ```
- [ ] **Plugin Manager** -- Discover, load, and manage plugins from a `plugins/` directory
  - Hot-reload support (unload/reload DLLs without restarting)
  - Dependency resolution between plugins
  - Version compatibility checking
- [ ] **Plugin SDK** -- Provide headers, libraries, and documentation for plugin development
  - Template project with CMake presets
  - Example plugins (importer, tool, effect)
  - API stability guarantees (semantic versioning)

### 3.2 Lua Scripting Integration
- [ ] **Embed Lua 5.4** as an alternative/complementary scripting language
  - Expose all engine APIs to Lua via bindings (`luaL_register`)
  - Lua scripts can define custom tools, effects, and behaviors
  - Hot-reload Lua scripts without application restart
- [ ] **Lua Content Scripts** -- Allow users to write Lua scripts for:
  - Custom actor behaviors (AI, pathfinding, interaction patterns)
  - Scene event triggers (conditional logic, timers, proximity checks)
  - Custom browser filters and content generators
  - Procedural animation (inverse kinematics, procedural walk cycles)
- [ ] **Python Scripting** -- Embed CPython 3.12+ for scientific/ML integrations
  - NumPy/SciPy for data analysis
  - TensorFlow/PyTorch for AI-assisted content generation
  - Pillow for image processing
- [ ] **Lua/Python Bridge** -- Shared scripting context allowing Lua and Python to interoperate

### 3.3 Asset Mod System
- [ ] **Mod Package Format** (`.q3m` files)
  - ZIP-based archive containing: models, textures, sounds, scripts, metadata
  - Manifest file (`mod.json`) with name, version, author, dependencies, load order
  - Asset override rules (user assets take precedence over built-in)
- [ ] **Mod Manager UI** -- Browse, install, enable/disable, and update mods
  - Conflict detection (two mods overriding the same asset)
  - Load order management
  - Mod profiles (different mod sets for different projects)
- [ ] **Content Mods**:
  - New actor templates (3D characters with animations)
  - New backgrounds and props
  - New sound effects and music tracks
  - New 3D fonts
  - New text box styles
  - New materials and textures
- [ ] **Behavior Mods**:
  - Custom KID scripts (GOK behaviors, state machines, transitions)
  - Custom help content
  - Custom project wizard tutorials
- [ ] **UI Mods**:
  - Custom cursors, buttons, toolbar layouts
  - Custom easel panels
  - Custom browser categories
  - Theme support (color schemes, font overrides)

### 3.4 Mod Distribution
- [ ] **Mod Store / Community Hub** -- Web-based platform for sharing mods
  - User accounts, ratings, reviews
  - Dependency tracking and auto-update
  - Version control and changelog management
- [ ] **Mod CLI Tools** -- Command-line utilities for mod development:
  - `q3m pack` -- Create a mod package from a directory
  - `q3m unpack` -- Extract a mod package
  - `q3m validate` -- Check a mod for errors and compatibility
  - `q3m install` -- Install a mod from file or URL
  - `q3m list` -- List installed mods

### 3.5 Hot-Reload Development
- [ ] **Live Script Editing** -- Edit `.CHT` scripts and see changes without restarting
  - File watcher monitors the content directories
  - On change: re-compile via CHOMP, invalidate cached chunks, rebuild affected GOBs
- [ ] **Live Model Preview** -- Drop a `.glb` file and see it rendered instantly
- [ ] **Live Texture Preview** -- Swap textures in real-time in the viewport
- [ ] **Undo/Redo for Content Changes** -- All modding operations are undoable

---

## Phase 4: Scripting Language Extensions

**Goal**: Modernize the Kauai scripting language while maintaining backward compatibility.

### 4.1 Language Improvements
- [ ] **Float support** -- Add 32-bit float as a native type alongside integers
  - Float variables: `float x = 3.14;`
  - Float arithmetic: `+`, `-`, `*`, `/`
  - Float comparisons: `<`, `>`, `<=`, `>=`
  - Math functions: `sin()`, `cos()`, `sqrt()`, `pow()`, `lerp()`
- [ ] **String improvements** -- First-class string type
  - String concatenation: `str = "hello" + " " + "world";`
  - String formatting: `str = format("Player %d has %d lives", id, lives);`
  - String methods: `length()`, `substr()`, `find()`, `replace()`, `trim()`
  - UTF-8 support throughout the string system
- [ ] **Struct/Class syntax** -- Named data structures
  ```
  struct ActorInfo {
      string name;
      float x, y, z;
      int state;
  };
  ActorInfo info = { "McZee", 100.0, 0.0, 50.0, 1 };
  ```
- [ ] **Function definitions** -- Named functions with parameters
  ```
  function moveToTarget(actor, targetX, targetY) {
      actor.x = lerp(actor.x, targetX, 0.1);
      actor.y = lerp(actor.y, targetY, 0.1);
  }
  ```
- [ ] **Event system** -- Named event handlers
  ```
  onCollision(actor1, actor2) {
      playSound("collision.wav");
      destroyGob(actor1);
  }
  ```
- [ ] **Arrays** -- Dynamic arrays with methods
  ```
  array actors = [];
  actors.push(createActor("mczee"));
  for (int i = 0; i < actors.length; i++) {
      actors[i].x = i * 100;
  }
  ```

### 4.2 New Built-in Commands
- [ ] **3D Math Commands**: `vec3()`, `mat4()`, `quat()`, `dot()`, `cross()`, `normalize()`, `slerp()`
- [ ] **Physics Commands**: `applyForce()`, `setVelocity()`, `checkCollision()`, `raycast()`
- [ ] **Animation Commands**: `tween()`, `keyframe()`, `interpolate()`, `easing()`
- [ ] **I/O Commands**: `loadFile()`, `saveFile()`, `httpGet()`, `jsonParse()`, `jsonStringify()`
- [ ] **Debug Commands**: `log()`, `drawDebugLine()`, `drawDebugSphere()`, `breakpoint()`

### 4.3 Script Debugging
- [ ] **Interactive Debugger** -- Step through script execution
  - Breakpoints, watch variables, call stack inspection
  - Variable watch panel in the UI
  - Console output window
- [ ] **Error Reporting** -- Enhanced error messages with:
  - File name and line number
  - Stack trace with function names
  - Suggested fixes for common errors
- [ ] **Profiler** -- Script execution time tracking
  - Per-function timing
  - Hot-spot identification
  - Memory allocation tracking

### 4.4 Script Compilation Improvements
- [ ] **Incremental Compilation** -- Only recompile changed functions
- [ ] **Bytecode Optimization** -- Constant folding, dead code elimination, inlining
- [ ] **Debug Symbols** -- Include source-level debug info in compiled scripts
- [ ] **Cross-file Linking** -- Allow scripts to call functions across files

---

## Phase 5: Rendering & Visual Upgrades

**Goal**: Modernize the rendering pipeline while optionally keeping the retro look.

### 5.1 Hardware-Accelerated Rendering
- [ ] **OpenGL 3.3+ Renderer** -- Replace software Z-buffer with GPU-accelerated rendering
  - Vertex/fragment shaders for per-pixel Phong lighting
  - Instanced rendering for multiple actors
  - Shadow mapping for depth perception
  - Post-processing pipeline (bloom, ambient occlusion, tone mapping)
- [ ] **Vulkan Renderer** -- Optional high-performance renderer for modern hardware
- [ ] **DirectX 11/12 Renderer** -- Windows-native alternative
- [ ] **WebGPU Renderer** -- Future web-based deployment
- [ ] **Software Renderer Fallback** -- Keep the original BRender software renderer as a compatibility option

### 5.2 Resolution & Color
- [ ] **Resolution Independence** -- Support 720p, 1080p, 1440p, 4K, 8K
  - The original 640x480 is a design constraint, not a requirement
  - Scale all UI elements proportionally
  - Adjust viewport and render target sizes dynamically
- [ ] **True-Color Rendering** -- Upgrade from 8-bit indexed to 24/32-bit color
  - Remove palette-ramp shading tables
  - Use standard RGB lighting equations
  - Support alpha blending (not just screen-door)
- [ ] **HDR Rendering** -- High dynamic range for modern displays
- [ ] **Anti-Aliasing** -- MSAA, FXAA, or TAA for smooth edges
- [ ] **V-Sync & Frame Rate** -- Unlock from fixed 6 fps to 30/60/120+ fps
  - Interpolate animation between keyframes for smooth playback
  - Variable frame rate with delta-time updates

### 5.3 Enhanced Materials & Textures
- [ ] **PBR Materials** -- Physically-based rendering (metallic-roughness workflow)
  - Albedo, normal, metallic, roughness, AO maps
  - Image-based lighting with environment maps
- [ ] **Shader System** -- User-definable GLSL/HLSL shaders
  - Shader editor UI with live preview
  - Shader library and presets (toon, cel, watercolor, sketch, pixel art)
- [ ] **Decal System** -- Project textures onto surfaces for stickers, labels, effects
- [ ] **Particle System** -- GPU-accelerated particles for fire, smoke, sparkles, rain, snow

### 5.4 Advanced Rendering Features
- [ ] **Real-time Shadows** -- Shadow maps for directional, point, and spot lights
- [ ] **Screen-space Reflections** -- Reflective surfaces (water, metal, glass)
- [ ] **Subsurface Scattering** -- Skin, wax, and translucent material rendering
- [ ] **Fog & Atmospheric Effects** -- Volumetric fog, god rays, depth-based haze
- [ ] **Post-Processing Stack** -- Bloom, motion blur, depth of field, color grading, vignette

### 5.5 Retro Mode
- [ ] **CRT Shader** -- Authentic 90s CRT monitor simulation (scanlines, curvature, phosphor glow)
- [ ] **256-Color Mode** -- Original palette-constrained rendering with dithering options
- [ ] **Scanline Filter** -- Optional scanline overlay
- [ ] **Resolution Lock** -- Option to run at exact 640x480 with integer scaling

---

## Phase 6: Audio & Media Pipeline

**Goal**: Modernize audio, video, and media handling.

### 6.1 Audio Engine
- [ ] **Replace AudioMan with miniaudio** -- Single-header audio library
  - Cross-platform (Windows, macOS, Linux, WASAPI, CoreAudio, ALSA/PulseAudio)
  - Built-in mixing, effects, and spatial audio
  - No COM dependency
- [ ] **Spatial Audio** -- 3D positioned sound effects
  - HRTF-based binaural rendering for headphones
  - Distance attenuation and doppler effect
- [ ] **Audio Effects** -- Real-time DSP chain per sound
  - Reverb, echo, chorus, flanger, compression, EQ
  - Speed/pitch shifting without quality loss
- [ ] **Audio Mixing Board** -- Visual mixer UI with per-track volume, pan, mute, solo
- [ ] **Music Sequencer** -- Timeline-based music arrangement
  - MIDI track editing with piano roll
  - Audio track editing with waveform view
  - Synchronization with scene timeline

### 6.2 Video Pipeline
- [ ] **FFmpeg Integration** -- Import/export any video format
  - H.264/H.265/VP9/AV1 encoding for modern exports
  - Hardware-accelerated encoding (NVENC, AMF, QSV)
- [ ] **Video Background Import** -- Use video files as animated scene backgrounds
- [ ] **Real-time Screen Recording** -- Record the viewport as video
- [ ] **Animated GIF Export** -- Export short clips as GIFs
- [ ] **WebM Export** -- Web-optimized video format for sharing
- [ ] **YouTube/Vimeo Upload** -- Direct export to video sharing platforms

### 6.3 Sound Design Tools
- [ ] **Waveform Editor** -- Visual audio editing (cut, copy, paste, fade, normalize)
- [ ] **Sound Effect Generator** -- Procedural sound synthesis (sine, square, noise, FM)
- [ ] **Audio Stems** -- Separate voice, music, and SFX tracks with independent export
- [ ] **Voice Changer** -- Real-time pitch/formant shifting for character voices
- [ ] **Lip-Sync Generator** -- Auto-generate mouth animations from audio

---

## Phase 7: UI/UX Modernization

**Goal**: Create a modern, intuitive interface while preserving the charming 90s aesthetic.

### 7.1 Immediate-Mode GUI
- [ ] **Dear ImGui Integration** -- Immediate-mode GUI for debug tools and property panels
  - Scene hierarchy inspector
  - Material property editor
  - Animation curve editor
  - Console/log window
- [ ] **Nuklear or Clay** -- Alternative lightweight GUI for embedded panels

### 7.2 Retained-Mode GUI (Main UI)
- [ ] **Resolution-Independent Layout** -- Modernize the pixel-perfect `POS3.CHH` system
  - Percentage-based positioning
  - Responsive layout containers (flexbox/grid)
  - DPI-aware scaling
- [ ] **Modern Window Management** -- Multi-document interface (MDI)
  - Detachable panels (properties, outliner, timeline)
  - Customizable workspace layouts
  - Remember panel positions across sessions
- [ ] **Drag-and-Drop** -- Throughout the entire application
  - Drag models onto the viewport to create actors
  - Drag textures onto actors to change materials
  - Drag sounds onto the timeline to add audio
  - Drag scenes to reorder them
- [ ] **Undo/Redo for Everything** -- Every UI operation is undoable
  - Visual undo history timeline
  - Branching undo tree (like Photoshop)

### 7.3 Timeline Editor
- [ ] **Professional Timeline** -- Multi-track timeline view for the movie
  - Actor tracks (one per character with per-frame keyframes)
  - Sound tracks (voice, music, SFX)
  - Camera tracks (position, rotation, FOV)
  - Effect tracks (transitions, particles, post-processing)
- [ ] **Dope Sheet View** -- Traditional animation keyframe editor
- [ ] **Graph Editor** -- Bezier curve editor for animation interpolation
- [ ] **Non-Linear Editor** -- Reorder, splice, and blend scenes like a video editor
- [ ] **Mark In/Out** -- Set playback/render ranges

### 7.4 Content Browser
- [ ] **Thumbnails with Preview** -- Hover-to-preview for all content
- [ ] **Search & Filter** -- Full-text search across all assets with tags, categories, ratings
- [ ] **Favorites & Recents** -- Quick access to frequently used content
- [ ] **Asset Metadata** -- Display creation date, author, polygon count, file size
- [ ] **Collection System** -- User-curated collections of assets

### 7.5 Property Inspector
- [ ] **3D Transform Gizmo** -- Visual move/rotate/scale handles in the viewport
  - Axis-constrained manipulation
  - Snap-to-grid and snap-to-angle
  - Multi-select with bounding box editing
- [ ] **Material Preview Sphere** -- Real-time material preview on a sphere/cube/plane
- [ ] **Animation Preview** -- Scrub through actor animations in the browser

---

## Phase 8: Cross-Platform Support

**Goal**: Run natively on all major desktop and mobile platforms.

### 8.1 Desktop Platforms
- [ ] **Windows** (primary) -- Windows 10/11 (64-bit)
  - Win32 API (current) or migrate to SDL3 for portability
- [ ] **macOS** -- Complete the existing Kauai Mac port
  - Native Cocoa window management (or SDL3)
  - Metal rendering backend
  - App Store distribution
- [ ] **Linux** -- Full native support
  - X11/Wayland via SDL3
  - Vulkan/OpenGL rendering
  - Flatpak/Snap/AppImage packaging
  - Package manager integration (AUR, PPA, etc.)

### 8.2 Mobile Platforms
- [ ] **iOS** -- iPad-first interface
  - Touch-optimized UI (gesture-based tools, virtual joystick)
  - Metal rendering
  - Apple Pencil support for drawing
  - App Store distribution
- [ ] **Android** -- Tablet and phone
  - Vulkan/OpenGL ES rendering
  - Touch UI with Material Design elements
  - Google Play distribution

### 8.3 Web Platform
- [ ] **WebAssembly (WASM)** -- Run in any modern browser
  - Emscripten compilation target
  - WebGL 2 / WebGPU rendering
  - Web Audio API for sound
  - IndexedDB for local storage
  - No installation required -- instant sharing via URL
- [ ] **PWA Support** -- Offline-capable progressive web app
- [ ] **Cloud Rendering** -- Server-side rendering for low-end devices

### 8.4 Console Platforms (Stretch Goals)
- [ ] **Nintendo Switch** -- Touch + Joy-Con controls
- [ ] **Steam Deck** -- Verified SteamOS support

---

## Phase 9: Community & Distribution

**Goal**: Build a thriving community around content creation and sharing.

### 9.1 Project File Format
- [ ] **Open Format Specification** -- Document the `.3MM` format completely
  - BNF grammar for chunky file structure
  - Complete chunk type reference
  - Migration guide from legacy format
- [ ] **Versioned Format** -- Forward-compatible versioning scheme
- [ ] **Compression Options** -- Multiple compression levels (none, LZ4, Zstd, LZMA)
- [ ] **Streaming Support** -- Lazy-loading for large movies (load scenes on demand)

### 9.2 Sharing & Collaboration
- [ ] **Cloud Save** -- Automatic sync across devices
- [ ] **Real-time Collaboration** -- Multiple users editing the same movie simultaneously
  - Operational Transform or CRDT-based conflict resolution
  - Voice/video chat integration
- [ ] **Version Control** -- Built-in project history (like Git for movies)
  - Branching and merging
  - Diff view (compare two versions of a movie)
- [ ] **Export Formats**:
  - MP4/MOV video (H.264/H.265)
  - Animated GIF
  - Image sequence (PNG/EXR)
  - FBX/glTF scene export
  - PDF storyboard
  - Standalone executable (self-playing movie with bundled player)

### 9.3 Community Features
- [ ] **Built-in Gallery** -- Browse and play movies created by the community
- [ ] **Rating & Commenting** -- Rate and discuss community creations
- [ ] **Tutorials & Templates** -- Curated learning paths and starter templates
- [ ] **Weekly Challenges** -- Theme-based creation contests
- [ ] **Asset Marketplace** -- Buy/sell/trade custom 3D models, textures, sounds
- [ ] **Creator Profiles** -- Portfolio of a user's creations

### 9.4 Documentation
- [ ] **User Manual** -- Comprehensive guide for all skill levels
  - Beginner: "Create your first movie in 5 minutes"
  - Intermediate: "Advanced animation techniques"
  - Expert: "Custom model pipeline with Blender"
- [ ] **API Reference** -- Auto-generated documentation for all public APIs
- [ ] **Modding Guide** -- Complete guide to creating plugins, mods, and scripts
  - "Your First Plugin" tutorial
  - "Importing a Blender Character" tutorial
  - "Creating a Custom Tool" tutorial
- [ ] **Video Tutorials** -- YouTube/video series for visual learners
- [ ] **Interactive Tutorials** -- In-app guided tutorials (modernizing the existing Gadget system)

---

## Phase 10: Advanced Features

**Goal**: Cutting-edge capabilities that push the boundaries of what a movie creation tool can do.

### 10.1 AI-Assisted Content Creation
- [ ] **AI Character Generation** -- Text-to-3D character generation
  - "A friendly robot with blue LEDs" -> full 3D model with textures
  - Integration with existing model generation services
- [ ] **AI Animation** -- Auto-generate walk cycles, idle animations, gestures
  - Audio-driven lip sync
  - Text-driven expression generation
- [ ] **AI Scene Composition** -- Suggest camera angles, lighting, and placement
- [ ] **AI Voice Generation** -- Text-to-speech with character-specific voices
- [ ] **AI Music Generation** -- Procedural background music matching scene mood
- [ ] **AI Script Assistance** -- Natural language -> script code generation

### 10.2 Physics Simulation
- [ ] **Rigid Body Physics** -- Bullet Physics or PhysX integration
  - Objects fall, bounce, and collide realistically
  - Ragdoll physics for characters
  - Breakable objects
- [ ] **Soft Body Physics** -- Cloth simulation, jelly, inflatable objects
- [ ] **Fluid Simulation** -- Water, lava, smoke effects
- [ ] **Particle Physics** -- Physics-driven particle effects

### 10.3 Advanced Animation
- [ ] **Inverse Kinematics** -- Pose characters with target positions
  - IK chains for arms, legs, spine
  - Foot planting on uneven surfaces
- [ ] **Motion Synthesis** -- Blend between multiple animations smoothly
- [ ] **Facial Animation** -- Blend shape system for facial expressions
  - Phoneme-based mouth shapes
  - Emotion presets (happy, sad, angry, surprised)
- [ ] **Procedural Animation** -- Rule-based animation (breathing, weight shift, eye tracking)
- [ ] **Motion Path Editor** -- Visual bezier curve editor for actor routes

### 10.4 Scene Management
- [ ] **Scene Templates** -- Pre-made scene layouts (office, park, space, underwater)
- [ ] **Scene Presets** -- Lighting presets (day, night, sunset, studio)
- [ ] **Scene Transitions** -- 50+ transition effects (fade, wipe, dissolve, zoom, slide, morph)
- [ ] **Scene Linking** -- Reference scenes from other movies (modular movie creation)

### 10.5 Collaboration & Social
- [ ] **Multiplayer Editing** -- Real-time collaborative movie creation
- [ ] **Live Streaming** -- Stream creation process to Twitch/YouTube
- [ ] **Comments & Annotations** -- Timestamped feedback on movies
- [ ] **Remix System** -- Fork and modify other users' movies (with attribution)

### 10.6 Export & Integration
- [ ] **Unity/Unreal Export** -- Export scenes as Unity packages or Unreal projects
- [ ] **Blender Add-on** -- Bi-directional sync with Blender
- [ ] **OBS Integration** -- Virtual camera output for streaming
- [ ] **VR Export** -- Export movies as 360-degree VR experiences (WebXR)
- [ ] **AR Export** -- Export characters as AR models (USDZ, GLB with AR extensions)

---

## Implementation Priority Matrix

### Tier 1: Essential (Months 1-6)
- Phase 0: Build fixes and CI/CD
- Phase 1.1-1.2: Memory management and string modernization
- Phase 2.1 (glTF import only)
- Phase 2.2 (PNG/JPEG import)
- Phase 2.3 (MP3/WAV import)
- Phase 3.1 (basic plugin API)
- Phase 5.1 (basic OpenGL renderer)
- Phase 5.2 (resolution independence)

### Tier 2: Important (Months 6-12)
- Phase 1.3-1.4: Code organization and type safety
- Phase 2.4 (animation import)
- Phase 2.5 (background import)
- Phase 3.2 (Lua scripting)
- Phase 3.3 (asset mod system)
- Phase 5.2 (true-color, unlocked frame rate)
- Phase 6.1 (miniaudio)
- Phase 6.2 (FFmpeg export)
- Phase 7.3 (timeline editor)

### Tier 3: Valuable (Months 12-18)
- Phase 3.4 (mod distribution)
- Phase 3.5 (hot-reload)
- Phase 4.1 (scripting language improvements)
- Phase 5.3 (PBR materials)
- Phase 5.4 (shadows, post-processing)
- Phase 7.1 (Dear ImGui)
- Phase 7.4 (content browser)
- Phase 8.1 (macOS, Linux)
- Phase 9.1 (open format spec)

### Tier 4: Aspirational (Months 18-24+)
- Phase 4.2-4.4 (advanced scripting)
- Phase 5.5 (retro mode)
- Phase 6.3 (sound design tools)
- Phase 8.2-8.3 (mobile, web)
- Phase 9.2-9.4 (community features)
- Phase 10 (AI, physics, advanced animation)

---

## Dependencies & Recommended Libraries

| Category | Library | License | Purpose |
|----------|---------|---------|---------|
| **3D Rendering** | bgfx | BSD-2 | Cross-platform rendering (OpenGL/Vulkan/DX/Metal) |
| **3D Math** | glm | MIT | GLM math library (vectors, matrices, quaternions) |
| **3D Import** | assimp | BSD-3 | Import 30+ 3D formats (glTF, FBX, OBJ, etc.) |
| **Image I/O** | stb_image | Public Domain | PNG, JPEG, BMP, TGA, HDR loading |
| **Image I/O** | libpng | libpng | PNG encoding/decoding |
| **Audio** | miniaudio | Public Domain | Cross-platform audio playback/recording |
| **Audio I/O** | libsndfile | LGPL-2.1 | Audio file format reading/writing |
| **Video** | FFmpeg | LGPL/GPL | Video encoding/decoding |
| **Compression** | zstd | BSD-3 | Fast compression for chunky files |
| **Compression** | lz4 | BSD-2 | Fast decompression for hot paths |
| **GUI** | Dear ImGui | MIT | Immediate-mode debug UI |
| **GUI** | Clay | BSD-2 | Retained-mode layout engine |
| **Scripting** | Lua 5.4 | MIT | Embedded scripting language |
| **Scripting** | Python 3.12 | PSF | Embedded scripting for ML/science |
| **Physics** | Bullet 3 | zlib | Rigid body, soft body, collision |
| **Networking** | enet | MIT | UDP networking for collaboration |
| **JSON** | nlohmann/json | MIT | JSON parsing for config/mods |
| **CLI** | CLI11 | BSD-2 | Command-line argument parsing |
| **Testing** | Google Test | BSD-3 | Unit testing framework |
| **Packaging** | vcpkg | MIT | C++ package management |

---

## Migration Strategy

The modernization should be **incremental and non-breaking**:

1. **Preserve the original** -- Keep all original source files intact and compilable
2. **Layer modernization** -- Add modern code alongside (not replacing) legacy code
3. **Adapter pattern** -- Create wrapper classes that bridge old and new APIs
4. **Feature flags** -- Use `#ifdef` / CMake options to enable/disable modern features
5. **Backward compatibility** -- Always be able to load and play original `.3MM` files
6. **Community first** -- Prioritize features that enable community content creation

The end goal is not to replace the original code but to **extend its reach** -- making it accessible to modern developers, content creators, and a new generation of users who want to create 3D animated stories.
