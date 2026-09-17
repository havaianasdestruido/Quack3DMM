# KAUAI Tools CMakeLists.txt
# Builds Kauai tools as separate executables

cmake_minimum_required(VERSION 3.20)

# Common include directories for all tools
set(TOOLS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../SRC
)

# Common compile definitions for all tools
set(TOOLS_COMPILE_DEFINITIONS
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    WIN32
    WIN
    _WINDOWS
    STRICT
)

# Common compile options for legacy code
set(TOOLS_COMPILE_OPTIONS
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

# Common Windows libraries for kauai tools
set(KAUAI_WINDOWS_LIBS
    user32.lib
    gdi32.lib
    winmm.lib
    advapi32.lib
    comdlg32.lib
    uuid.lib
    kernel32.lib
    vfw32.lib
)

# Function to create a kauai tool executable
function(add_kauai_tool)
    set(options GUI CONSOLE)
    set(oneValueArgs NAME OUTPUT_NAME)
    set(multiValueArgs SOURCES RESOURCES HEADERS LINK_LIBS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_GUI)
        set(subsystem "WINDOWS")
    elseif(ARG_CONSOLE)
        set(subsystem "CONSOLE")
    else()
        message(FATAL_ERROR "add_kauai_tool requires either GUI or CONSOLE")
    endif()

    if(ARG_OUTPUT_NAME)
        set(output_name ${ARG_OUTPUT_NAME})
    else()
        set(output_name ${ARG_NAME})
    endif()

    add_executable(${ARG_NAME} ${ARG_SOURCES} ${ARG_RESOURCES} ${ARG_HEADERS})

    target_include_directories(${ARG_NAME} PRIVATE ${TOOLS_INCLUDE_DIRS})
    target_compile_definitions(${ARG_NAME} PRIVATE ${TOOLS_COMPILE_DEFINITIONS})
    target_compile_options(${ARG_NAME} PRIVATE ${TOOLS_COMPILE_OPTIONS})
    target_link_libraries(${ARG_NAME} PRIVATE kauai ${ARG_LINK_LIBS} ${KAUAI_WINDOWS_LIBS})

    if(WIN32)
        set_target_properties(${ARG_NAME} PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:${subsystem}"
            OUTPUT_NAME "${output_name}"
        )
    endif()

    set_target_properties(${ARG_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME "${output_name}"
    )
endfunction()

# ============================================================================
# CHOMP.EXE - Chunk viewer/editor (GUI)
# Sources: CHOMP.CPP, MSSIO.CPP, PIC.CPP, PICWIN.CPP (from kauai lib) + kauai lib
# Note: MSSIO.CPP, PIC.CPP, PICWIN.CPP are part of kauai library
# ============================================================================
add_kauai_tool(
    GUI
    NAME chomp
    OUTPUT_NAME chomp
    SOURCES
        CHOMP.CPP
    LINK_LIBS
)

# ============================================================================
# CHED.EXE - Chunk Editor (GUI)
# Sources: CHED.CPP, CHDOC.CPP, CHGRP.CPP, CHEX.CPP, CHMBMP.CPP, CHPIC.CPP, CHTXT.CPP, CHED.RC
# ============================================================================
add_kauai_tool(
    GUI
    NAME ched
    OUTPUT_NAME ched
    SOURCES
        CHED.CPP
        CHDOC.CPP
        CHGRP.CPP
        CHHEX.CPP
        CHMBMP.CPP
        CHPIC.CPP
        CHTXT.CPP
    RESOURCES
        CHED.RC
    HEADERS
        CHED.H
        CHDOC.H
        CHEDRES.H
    LINK_LIBS
        vfw32.lib
)

# ============================================================================
# CHELP.EXE - Chunk Help Editor (GUI)
# Sources: CHELP.CPP, CHTOP.CPP, CHELPEXP.CPP, CHELPDMP.CPP, CHELP.RC
# ============================================================================
add_kauai_tool(
    GUI
    NAME chelp
    OUTPUT_NAME chelp
    SOURCES
        CHELP.CPP
        CHTOP.CPP
        CHELPEXP.CPP
        CHELPDMP.CPP
    RESOURCES
        CHELP.RC
    HEADERS
        CHELP.H
        CHELPRES.H
        CHELPEXP.H
    LINK_LIBS
        vfw32.lib
)

# ============================================================================
# CHMERGE.EXE - Chunk Merger (Console)
# Sources: CHMERGE.CPP
# ============================================================================
add_kauai_tool(
    CONSOLE
    NAME chmerge
    OUTPUT_NAME chmerge
    SOURCES
        CHMERGE.CPP
)

# ============================================================================
# MKMBMP.EXE - Make Masked Bitmap (Console)
# Sources: MKMBMP.CPP
# ============================================================================
add_kauai_tool(
    CONSOLE
    NAME mkmbmp
    OUTPUT_NAME mkmbmp
    SOURCES
        MKMBMP.CPP
)

# ============================================================================
# KPACK.EXE - Kauai Packager (Console)
# Sources: KPACK.CPP
# ============================================================================
add_kauai_tool(
    CONSOLE
    NAME kpack
    OUTPUT_NAME kpack
    SOURCES
        KPACK.CPP
)

# ============================================================================
# CHELPDMP.EXE - CHelp Dumper (Console)
# Sources: CHELPDMP.CPP, CHELPEXP.CPP, MSSIO.CPP (from kauai lib)
# ============================================================================
add_kauai_tool(
    CONSOLE
    NAME chelpdmp
    OUTPUT_NAME chelpdmp
    SOURCES
        CHELPDMP.CPP
        CHELPEXP.CPP
)

# Set default target to build all tools
add_custom_target(kauai_tools ALL
    DEPENDS
        chomp
        ched
        chelp
        chmerge
        mkmbmp
        kpack
        chelpdmp
)