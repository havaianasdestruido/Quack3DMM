# ENGINE (SOC Engine) CMakeLists.txt
# 3D Movie Maker Engine Library

cmake_minimum_required(VERSION 3.20)

# Modern modding layer (vfs / q3m / mod_manager)
add_library(vfs STATIC
    vfs.cpp
    vfs.h
)
set_target_properties(vfs PROPERTIES
    OUTPUT_NAME "vfs"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(vfs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(vfs PRIVATE _CRT_SECURE_NO_WARNINGS)

add_library(q3m STATIC
    q3m.cpp
    q3m.h
)
set_target_properties(q3m PROPERTIES
    OUTPUT_NAME "q3m"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(q3m PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(q3m PUBLIC vfs)
target_compile_definitions(q3m PRIVATE _CRT_SECURE_NO_WARNINGS)

add_library(mod_manager STATIC
    mod_manager.cpp
    mod_manager.h
)
set_target_properties(mod_manager PROPERTIES
    OUTPUT_NAME "mod_manager"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(mod_manager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(mod_manager PUBLIC vfs)
target_compile_definitions(mod_manager PRIVATE _CRT_SECURE_NO_WARNINGS)

add_library(q3d_ui STATIC
    ui/theme.cpp
    ui/widget.cpp
    ui/ui_manager.cpp
    ui/lua_ui.cpp
    ui/theme.h
    ui/widget.h
    ui/ui_manager.h
    ui/lua_ui.h
)
set_target_properties(q3d_ui PROPERTIES
    OUTPUT_NAME "q3d_ui"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(q3d_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(q3d_ui PUBLIC vfs lua)

add_library(script STATIC
    script/lua_engine.cpp
    script/lua_engine.inl
    script/q3d_lua_api.cpp
    script/script_manager.cpp
)
set_target_properties(script PROPERTIES
    OUTPUT_NAME "script"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(script PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(script PUBLIC vfs lua)

add_library(event STATIC
    event/event_bus.cpp
    event/hook_system.cpp
)
set_target_properties(event PROPERTIES
    OUTPUT_NAME "event"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(event PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(event PUBLIC vfs)

add_library(sandbox STATIC
    sandbox/sandbox.cpp
    sandbox/lua_sandbox.cpp
)
set_target_properties(sandbox PROPERTIES
    OUTPUT_NAME "sandbox"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(sandbox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(sandbox PUBLIC vfs lua mod_core)

add_library(hotreload STATIC
    hotreload/file_watcher.cpp
    hotreload/reload_manager.cpp
    hotreload/asset_reloader.cpp
)
set_target_properties(hotreload PROPERTIES
    OUTPUT_NAME "hotreload"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(hotreload PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(hotreload PUBLIC vfs)

add_library(tools STATIC
    tools/tool_system.cpp
    tools/tool_manager.cpp
    tools/builtin_tools.cpp
)
set_target_properties(tools PROPERTIES
    OUTPUT_NAME "tools"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(tools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(tools PUBLIC vfs)

add_library(render STATIC
    render/shader.cpp
    render/shader_compiler.cpp
    render/material.cpp
    render/render_pass_manager.cpp
)
set_target_properties(render PROPERTIES
    OUTPUT_NAME "render"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(render PUBLIC vfs)

add_library(audio STATIC
    audio/audio_engine.cpp
    audio/dsp.cpp
    audio/sound.cpp
    audio/sound_manager.cpp
)
set_target_properties(audio PROPERTIES
    OUTPUT_NAME "audio"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(audio PUBLIC vfs)

add_library(import STATIC
    import/gltf_importer.cpp
    import/gltf_to_engine.cpp
    import/assimp_fbx.cpp
    import/image.cpp
    import/texture.cpp
    import/texture_baker.cpp
)
set_target_properties(import PROPERTIES
    OUTPUT_NAME "import"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(import PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(import PUBLIC vfs)

add_library(actor STATIC
    actor/actor_def.cpp
    actor/actor_system.cpp
    actor/skeleton.cpp
)
set_target_properties(actor PROPERTIES
    OUTPUT_NAME "actor"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(actor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(actor PUBLIC vfs)

add_library(mod_core STATIC
    mod/mod.cpp
    mod/mod_manifest.cpp
)
set_target_properties(mod_core PROPERTIES
    OUTPUT_NAME "mod_core"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(mod_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(mod_core PUBLIC vfs)

# Audio modding system (Pillar 9)
set(AUDIO_SOURCES
    audio/audio_engine.cpp
    audio/dsp.cpp
    audio/sound.cpp
    audio/sound_manager.cpp
)

# Legacy SOC engine (pre-C++98 code, still being modernized). Off by default;
# the standalone modding libraries above are what the tests and softmod rely on.
option(BUILD_LEGACY_ENGINE "Build the legacy 3D Movie Maker engine libraries" OFF)
if(BUILD_LEGACY_ENGINE)

# Engine source files
set(ENGINE_SOURCES
    ACTOR.CPP
    ACTREDIT.CPP
    ACTRSAVE.CPP
    ACTRSND.CPP
    BKGD.CPP
    BODY.CPP
    MODL.CPP
    MOVIE.CPP
    MSND.CPP
    MTRL.CPP
    SCENE.CPP
    SREC.CPP
    TAGL.CPP
    TAGMAN.CPP
    TBOX.CPP
    TDF.CPP
    TDT.CPP
    TMPL.CPP
    mod/mod_manifest.cpp
    mod/mod.cpp
    hotreload/file_watcher.cpp
    hotreload/reload_manager.cpp
    hotreload/asset_reloader.cpp
    ${AUDIO_SOURCES}
)



set(ENGINE_HEADERS
    ../../INC/ACTOR.H
    ../../INC/APE.H
    ../../INC/BKGD.H
    ../../INC/BODY.H
    ../../INC/BROWSER.H
    ../../INC/ESL.H
    ../../INC/HELPBOOK.H
    ../../INC/HELPRES.H
    ../../INC/HELPTOPS.H
    ../../INC/KIDGS.CHH
    ../../INC/KIDGSDEF.H
    ../../INC/KIDSANIM.H
    ../../INC/MODL.H
    ../../INC/MOVIE.H
    ../../INC/MSND.H
    ../../INC/MTRL.H
    ../../INC/POPUP.H
    ../../INC/PORTF.H
    ../../INC/POS3.CHH
    ../../INC/PROJMAIN.H
    ../../INC/SCENE.H
    ../../INC/SCNSORT.H
    ../../INC/SHAREDEF.H
    ../../INC/SHARUTIL.CHH
    ../../INC/SITOBREN.H
    ../../INC/SOC.H
    ../../INC/SOCDEF.H
    ../../INC/SOCDEFN.H
    ../../INC/SOCRES.H
    ../../INC/SOCUTIL.H
    ../../INC/SPLOT.H
    ../../INC/SREC.H
    ../../INC/STDIODEF.H
    ../../INC/STDIOPOS.CHH
    ../../INC/STDIOSCB.H
    ../../INC/STUDIO.H
    ../../INC/TAGL.H
    ../../INC/TAGMAN.H
    ../../INC/TATR.H
     ../../INC/TBOX.H
     ../../INC/TDF.H
     ../../INC/TDFMAKE.H
     ../../INC/TDT.H
     ../../INC/TGOB.H
     ../../INC/TMPL.H
     ../../INC/UTEST.H
     ../../INC/UTESTRES.H
     ../../INC/VERSION.H
     ../../INC/IMAGE.H
     ../../INC/TEXTURE.H
     ../../INC/TEXTURE_BAKER.H
 )



# Create engine library
add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Target properties
set_target_properties(engine PROPERTIES
    OUTPUT_NAME "engine"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}/INC
    ${CMAKE_SOURCE_DIR}/BREN/INC
    ${CMAKE_SOURCE_DIR}/KAUAI/SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
)

# Dependencies
target_link_libraries(engine PRIVATE
    bren
    kauai
)

# Compile definitions
target_compile_definitions(engine PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    WIN32
    WIN
    _WINDOWS
    STRICT
    SOC_ENGINE
)

# Compiler options
target_compile_options(engine PRIVATE
    /W3
    /wd4996
    /wd4244
    /wd4267
    /wd4311
    /wd4312
    /wd4838
    /wd4245
    /wd4305
    /wd4018
    /wd4365
    /wd4100
    /wd4189
    /wd4706
    /wd4702
    /wd4127
    /wd4514
    /wd4710
    /wd4711
    /wd4505
    /wd4820
    /wd5045
)

# Windows libraries
if(WIN32)
    target_link_libraries(engine PRIVATE
        user32.lib
        gdi32.lib
        winmm.lib
        advapi32.lib
        comdlg32.lib
        uuid.lib
        kernel32.lib
        vfw32.lib
    )
endif()

endif() # BUILD_LEGACY_ENGINE
