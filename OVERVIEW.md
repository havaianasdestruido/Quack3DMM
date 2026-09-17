# OVERVIEW.md -- Microsoft 3D Movie Maker (Quack3DMM)

> Open-sourced 1995 children's 3D animation tool by Microsoft. Codename "Socrates."
> Licensed under MIT. Built on the **Kauai** application framework and **BRender** 3D engine (Argonaut Software).

---

## 1. What Is This?

Microsoft 3D Movie Maker let children create animated 3D movies: place characters on backgrounds, assign actions (walk, talk, dance), record motion paths, add sounds and text boxes, then export to AVI. The product shipped on CD-ROM in 1995 with a "Building" (lobby navigation) UI and a "Studio" (movie editor) UI, hosted by the character McZee.

This repository contains the **full source code** (restored from Microsoft corporate archives), plus the Kauai engine, BRender 3D library, build tools, original CD content assets, and a modern CMake build system overlay.

---

## 2. Repository Structure

```
Quack3DMM/
|
|-- CMakeLists.txt          # Modern CMake 3.20+ build system (C++17)
|-- MAKEFILE                # Legacy NMake build (MSVC 2.0)
|-- MAKEFILE.KMK            # Kauai master build orchestrator
|-- MAKEFILE.RUL            # Shared compile rules (.cpp/.cht/.rc)
|-- setvars.bat              # Environment setup for NMake
|
|-- BREN/                   # BRender 3D rendering engine (Argonaut Software, 1993-95)
|   |-- INC/                # 48 header files (full 3D math, scene graph, materials)
|   |-- BWLD.CPP            # World manager (double-buffered Z-buffer renderer)
|   |-- TMAP.CPP            # Texture map wrapper
|   |-- ZBMP.CPP            # Z-buffer bitmap class
|   |-- MATERIAL.CPP        # Material definitions
|   |-- BRENFUN.CPP         # Demo/test scene (application glue)
|   +-- STD{ERR,FILE,MEM}.C # Platform abstraction (vtable-based plugins)
|
|-- kauai/                  # Kauai application framework (~148 source files)
|   |-- SRC/
|   |   |-- SCRCOM.{CPP,H}  # Script compiler (infix+postfix to bytecode)
|   |   |-- SCREXE.{CPP,H}  # Script interpreter (stack-based VM)
|   |   |-- SCRCOMG.{CPP,H} # Script compiler GOB extensions
|   |   |-- SCREXEG.{CPP,H} # Script interpreter GOB extensions
|   |   |-- LEX.{CPP,H}     # Lexer/tokenizer
|   |   |-- CMD.{CPP,H}     # Command dispatch system (MFC-like command maps)
|   |   |-- DOCB.{CPP,H}    # Document base class (document/view + undo)
|   |   |-- GOB.{CPP,H}     # Graphics Object tree (scene graph for UI)
|   |   |-- APPB.{CPP,H}    # Application base class (message loop)
|   |   |-- CHUNK.{CPP,H}   # Chunky file format (CFL/CRF/BLCK)
|   |   |-- CHCM.{CPP,H}    # Chunk compiler (text -> binary)
|   |   |-- CHSE.{CPP,H}    # Chunk emitter (binary -> text)
|   |   |-- CLOK.{CPP,H}    # Clock/alarm system (cooperative timers)
|   |   |-- GFX.{CPP,H}     # Cross-platform 2D graphics (GNV/GPT)
|   |   |-- GFXWIN.CPP      # Windows GDI implementation
|   |   |-- GFXMAC.CPP      # Mac QuickDraw implementation
|   |   |-- MBMP.{CPP,H}    # Masked bitmap sprites (RLE + x86 ASM)
|   |   |-- PIC.{CPP,H}     # Pictures (EMF/PICT wrappers)
|   |   |-- SNDM.{CPP,H}    # Sound manager (hierarchical device routing)
|   |   |-- SNDAM.{CPP,H}   # AudioMan WAV device
|   |   |-- MIDI.{CPP,H}    # MIDI stream parser
|   |   |-- MIDIDEV{,2}.CPP # MIDI playback (v1 midiOut, v2 midiStream)
|   |   |-- VIDEO.{CPP,H}   # AVI video playback (AVIFile + MCI)
|   |   |-- RTXT.{CPP,H}    # Rich text document + editing
|   |   |-- TEXT.{CPP,H}    # Edit controls (single/multi-line)
|   |   |-- CLIP.{CPP,H}    # Clipboard (internal + Win/Mac)
|   |   |-- SPELL.{CPP,H}   # Spell checker (DLL-loaded)
|   |   |-- KIDSPACE.{CPP,H}# GOK (interactive graphical objects)
|   |   |-- KIDWORLD.{CPP,H}# WOKS (world container for GOKs)
|   |   |-- KIDHELP.{CPP,H} # Help balloons (HBAL/TXHD/TXHG)
|   |   |-- *WIN.CPP        # Windows platform implementations (9 files)
|   |   +-- *MAC.CPP        # Mac platform implementations (9 files)
|   +-- TOOLS/              # Kauai dev tools (CHOMP, CHED, CHELP, MKMBMP, etc.)
|
|-- SRC/                    # 3D Movie Maker application code
|   |-- ENGINE/             # Core engine library (18 .CPP files)
|   |   |-- ACTOR.CPP       # Actor system (paths, events, costumes, rendering)
|   |   |-- BODY.CPP        # 3D body (BRender scene graph per actor)
|   |   |-- MODL.CPP        # Model loading (BRender meshes)
|   |   |-- MOVIE.CPP       # Movie document + view controller (~9000 LOC)
|   |   |-- SCENE.CPP       # Scene management (frame timeline, events)
|   |   |-- BKGD.CPP        # Background (pre-rendered RGB + Z compositing)
|   |   |-- TMPL.CPP        # Actor templates (skeleton, actions, costumes)
|   |   |-- TDT.CPP         # 3D text (procedural text animations)
|   |   |-- TDF.CPP         # 3D font loading
|   |   |-- MSND.CPP        # Movie sound playback
|   |   |-- MTRL.CPP        # Materials (palette-shaded, texture-mapped)
|   |   |-- TBOX.CPP        # Text box UI (speech bubbles)
|   |   |-- SREC.CPP        # Sound recording (WaveIn -> RIFF)
|   |   |-- TAGMAN.CPP      # Tag Manager (asset pipeline, CD/HD caching)
|   |   |-- TAGL.CPP        # Tag lists
|   |   |-- ACTREDIT.CPP    # Actor clipboard/undo operations
|   |   |-- ACTRSAVE.CPP    # Actor serialization
|   |   +-- ACTRSND.CPP     # Actor sound events
|   |
|   |-- STUDIO/             # Studio UI application (15 .CPP + 37 .CHT + resources)
|   |   |-- STUDIO.CPP      # Main studio class (STDIO), ~60 command handlers
|   |   |-- UTEST.CPP       # Application entry point (APP class)
|   |   |-- APE.CPP         # Actor Preview Entity (mini 3D viewport)
|   |   |-- ESL.CPP         # Easels (modal editing panels)
|   |   |-- BROWSER.CPP     # Browser display hierarchy (BRWD/BRWL/BRWT/BRWN)
|   |   |-- PORTF.CPP       # Portfolio (file open/save)
|   |   |-- POPUP.CPP       # Popup menus + font picker
|   |   |-- TGOB.CPP        # Text GOB widget
|   |   |-- SCNSORT.CPP     # Scene sorter (drag-and-drop reordering)
|   |   |-- SPLOT.CPP       # Splot machine (random content generator)
|   |   |-- TATR.CPP        # Theater (playback to AVI)
|   |   |-- MMINSTAL.CPP    # Driver/codec installation
|   |   |-- STDIOBRW.CPP    # Browser invocation bridge
|   |   |-- STDIOSCB.CPP    # Scrollbar widgets
|   |   |-- UTESTSCB.CPP    # Movie scrollbars (standalone mode)
|   |   |-- STUDIO.CHT      # Master UI definition (all .CHT included)
|   |   |-- BMP/            # 771 bitmap resources
|   |   |-- CUR/            # 58 cursor resources
|   |   +-- SOUND/          # 94 WAV files + 71 voice-over WAVs
|   |
|   |-- BUILDING/           # Building (lobby navigation) game module
|   |   |-- *.CHT           # 19 room scripts (lobby, theatre, street, etc.)
|   |   |-- *.SEQ           # 22 animation sequence files
|   |   |-- BITMAPS/        # Room background bitmaps
|   |   |-- PBM/            # Packed bitmap resources
|   |   +-- SOUND/          # Room sound effects
|   |
|   |-- HELP/               # Help system (38 .CHT topic scripts)
|   |-- HELPAUD/            # Audio help scripts
|   +-- SHARED/             # Shared resources (bio pages, cursors, maps)
|
|-- INC/                    # 52 application header files
|   |-- SOC.H              # Master include + all chunk type tags
|   |-- STUDIO.H           # Studio class + UI definitions
|   |-- STDIODEF.H         # All CID/KID constants
|   |-- KIDGS.CHH          # KidScript macro DSL
|   +-- (48 more headers)
|
|-- TOOLS/                  # Authoring tools
|   |-- SITOBREN.CPP       # SoftImage -> BRender converter
|   |-- TDFMAKE.CPP        # 3D Font maker (DAT -> TDF chunks)
|   +-- MKTMAP.CPP         # Bitmap -> texture map converter
|
|-- SETUP/                  # Microsoft Acme installer system
|   |-- 3DMOVIE.DDF        # Diamond CD layout definition
|   |-- CUSTDLL/            # Custom setup DLL (3DMSETUP.DLL)
|   |-- AUTORUN/            # CD auto-play application
|   +-- U/, Z/, D/          # Per-language setup files + STF tables
|
|-- cd9/                    # Runtime content assets
|   |-- *.AVI              # 26 video files (navigation transitions)
|   |-- *.3TH              # 7 thumbnail descriptors (browser palettes)
|   +-- *.3CN              # 6 content index files (asset registries)
|
|-- cd3/                    # Distribution CD layout
|   |-- 3DMOVIE/           # Full application + all content
|   |-- MMCAT/             # Microsoft Kids Catalog (VB3 app)
|   |-- PSS/               # Help files (3DMM.HLP)
|   +-- SAMPLES/           # 16 sample .3MM movie files
|
|-- cd12/                   # Full development CD (mirror of root + build output)
|-- cd2/                    # PowerPoint presentations (~60 files, organized by track)
|-- IMG/                    # README screenshots
|-- .notes/                 # Archive notes
+-- .vscode/                # VS Code workspace config
```

---

## 3. Architecture Layers

```
+------------------------------------------------------------------+
| Layer 5: STDIO (Studio UI)                                        |
| Toolbanks, browsers, easels, scrollbars, help book, gadget tutor |
+------------------------------------------------------------------+
| Layer 4: MVIE/MVU (Movie Document + 3D Viewport)                 |
| Scenes, actors, undo stack, playback, tool dispatch              |
+------------------------------------------------------------------+
| Layer 3: SCEN/ACTR/TBOX/BKGD (Content Objects)                   |
| Per-frame state, motion paths, events, rendering                 |
+------------------------------------------------------------------+
| Layer 2: Engine (TMPL/MODL/BODY/MTRL/TDT/TDF/MSND/TAGMAN)       |
| 3D models, templates, materials, fonts, sounds, asset pipeline   |
+------------------------------------------------------------------+
| Layer 1: Kauai Framework (GOB/GOK/WOKS/DOCB/CMD/CLOK/CHUNK)     |
| Scripting VM, chunky files, document/view, command dispatch, UI  |
+------------------------------------------------------------------+
| Layer 0: BRender 3D Engine (Argonaut Software)                    |
| Software Z-buffer renderer, scene graph, materials, math library |
+------------------------------------------------------------------+
| Layer -1: Platform Abstraction (GFX/GPT/FIL/HQ/DLG/MENU)        |
| Win32 GDI + Classic Mac Toolbox dual implementation              |
+------------------------------------------------------------------+
```

---

## 4. Key Subsystems

### 4.1 BRender 3D Engine
- **Rendering**: 8-bit indexed-color software Z-buffer rasterizer at 640x480
- **Math**: 16.16 signed fixed-point with assembly-optimized ops; float alternative
- **Scene graph**: Hierarchical `br_actor` tree with transform inheritance
- **Materials**: Phong shading with palette-ramp index-shade lookup tables
- **Platform plugin**: Vtable-based memory/file/diagnostics abstraction

### 4.2 Kauai Scripting Engine
- **Language**: Custom infix + postfix scripting (only 32-bit integers, no floats)
- **Compiler**: Single-pass (`SCCB`) emitting stack-based bytecode
- **VM**: Cooperative multitasking via `PAUSE` opcode; scripts suspend on idle
- **Extensions**: GOB-aware opcodes (create/destroy objects, play sounds, change states)
- **Content pipeline**: `.CHT` (C-preprocessed text) -> CHOMP compiler -> `.CHK` (binary chunks)

### 4.3 Chunky File Format
- **Addressing**: CTG (4-char type) + CNO (chunk number) + CHID (child relationship)
- **Structure**: Header -> Data Heap (optionally compressed) -> Sorted Index
- **Cache**: BACO (block-allocable cache objects) with reference counting
- **Sources**: Tag Manager tracks content across CD/HD with automatic caching

### 4.4 GOK/GOB UI System
- **GOB tree**: Parent/child/sibling hierarchy with coordinate mapping
- **GOK**: Interactive objects with state machine (up to 32 states), mouse tracking, animation, sound
- **GORP**: Pluggable visual representations (bitmap, tile, fill, video)
- **GOKD**: Data-driven descriptors with cursor maps (CUME) and position maps (LOP)
- **WOKS**: Container managing clocks, GOK creation, help balloons, script execution

### 4.5 Actor/Scene/Movie System
- **ACTR**: 3D character with motion path (APV vertices), events (AEV), costume, sound
- **SCEN**: Frame timeline with actors, text boxes, scene events, camera, background
- **MVIE**: Movie document (scenes list, roll call, palette, undo stack)
- **MVU**: View controller with ~50 editing tools (compose, record, rotate, scale, costume...)
- **Playback**: Timer-alarm driven, 6 fps, frame-advance with pre-rendering

### 4.6 Asset Pipeline
- **Authoring**: SoftImage `.hrc` -> SITOBREN tool -> `.cht` source -> CHOMP -> `.chk` binary
- **Runtime**: Tag Manager resolves TAGs {SID, CTG, CNO} across CD/HD sources
- **Content indexes**: `.3CN` files enumerate all available content per category
- **Thumbnails**: `.3TH` files provide browser palette images

### 4.7 Content & Resources
- **771 bitmaps** (BMP/), **58 cursors** (CUR/), **165 WAV files** (SOUND/), **26 AVI videos**
- **37+ CHT scripts** defining the entire Studio UI (buttons, tools, browsers, easels, help)
- **19 Building room scripts** + **22 animation sequences** for lobby navigation
- **38 help topic scripts** + **71 voice-over WAVs** for the help system

---

## 5. Build System

| Component | Legacy (NMake) | Modern (CMake) |
|-----------|---------------|----------------|
| BRender | `BREN/MAKEFILE` (pre-built lib) | `BREN/CMakeLists.txt` (compiles source) |
| Kauai | `kauai/MAKEFILE` + `.DEF` + `.KMK` | `kauai/CMakeLists.txt` (75 source files) |
| Engine | `SRC/ENGINE/MAKEFILE` | `SRC/ENGINE/CMakeLists.txt` |
| Studio | `SRC/STUDIO/MAKEFILE` | `SRC/STUDIO/CMakeLists.txt` |
| CHOMP | `kauai/TOOLS/MAKEFILE` | Referenced via `find_program` in CMake |
| Tools | `TOOLS/MAKEFILE` | Not ported to CMake |
| Setup | Acme95 + Diamond | Not ported (historical) |

**Content build**: `.CHT` files -> C preprocessor -> CHOMP -> `.CHK` binary chunks (integrated in CMake Studio target).

---

## 6. File Formats

| Extension | Name | Description |
|-----------|------|-------------|
| `.3MM` | 3D Movie Document | User's movie (chunky file: SCEN, MVIE, ACTR, TBOX, SND chunks) |
| `.CHK` | Chunky Resource File | Runtime binary container (typed chunks with CTG/CNO addressing) |
| `.CHT` | Chunky Text Source | UI/logic scripts (C-preprocessed, compiled by CHOMP) |
| `.3CN` | Content Index | Master registry of available content per category |
| `.3TH` | Thumbnail Descriptor | Browser palette images and metadata |
| `.AVI` | Video | Pre-rendered navigation transitions (640x480, 15fps) |
| `.HRC` | SoftImage Hierarchy | 3D model files from the authoring tool |
| `.S2B` | SoftImage-to-BRender | Conversion script for SITOBREN tool |
| `.DAT` | Model Data | 3D mesh vertex/face data for TDF font creation |
| `.TMAP` | Texture Map | BRender texture (palette-indexed pixel data) |
| `.ZBMP` | Z-Buffer Bitmap | 16-bit depth buffer data for background compositing |
| `.MBMP` | Masked Bitmap | RLE-encoded sprites with transparency |
| `.PIC` | Picture | Enhanced MetaFile (Win) / PICT (Mac) wrapper |
| `.CUR` | Cursor | Windows cursor resource |

---

## 7. Key Technical Characteristics

| Aspect | Detail |
|--------|--------|
| **Language** | Pre-C++98 C++ with macro-based RTTI, manual reference counting |
| **Rendering** | Software Z-buffer at 640x480, 8-bit indexed color, 16-bit depth |
| **Fixed-point** | 16.16 signed for all 3D math (with float alternative) |
| **Lighting** | Per-vertex Phong (ka/kd/ks) via palette-ramp index-shade tables |
| **Texturing** | Perspective-correct, environment mapping, screen-door transparency |
| **Scripting** | Custom stack-based VM with cooperative multitasking |
| **File I/O** | Chunky file format with typed chunks, optional LZ77 compression |
| **Platform** | Windows 3.1/95 primary; Mac OS port exists in Kauai (incomplete for 3DMM) |
| **Memory** | Handle-based (HQ) with lock-counting; purgable under pressure |
| **Performance** | Half-resolution rendering option; x86 inline ASM for sprite blitting |
| **Frame rate** | 6 fps (defined as `kfps` in SOCDEF.H) |
| **Resolution** | 640x480 fixed (palette-based) |
| **Cross-platform** | Dual Win/Mac implementations in Kauai; BRender is C with platform plugins |

---

## 8. Codebase Statistics

| Area | Files | Estimated LOC |
|------|-------|---------------|
| BRender engine | 11 source + 48 headers | ~8,000 |
| Kauai framework | ~148 source + headers | ~80,000 |
| Engine (SRC/ENGINE) | 18 source + 15 headers | ~35,000 |
| Studio (SRC/STUDIO) | 15 source + 37 CHT + resources | ~25,000 + resources |
| Building (SRC/BUILDING) | 19 CHT + 22 SEQ + resources | scripts + data |
| Help/Shared | 38+15 CHT + resources | scripts + data |
| INC headers | 52 headers | ~8,000 |
| Tools | 3 source files | ~4,000 |
| Build system | 10+ makefiles + CMake | ~1,500 |
| **Total** | **~400+ source/header files** | **~160,000+ LOC + extensive resources** |
