# STUDIO CMakeLists.txt
# Builds the 3D Movie Maker (3dmovie.exe) executable

cmake_minimum_required(VERSION 3.20)

# Legacy Studio app; skipped when legacy engine is off.
option(BUILD_LEGACY_ENGINE "Build the legacy 3D Movie Maker engine libraries" OFF)
if(BUILD_LEGACY_ENGINE)

# Studio source files (main executable sources)
set(STUDIO_SOURCES
    STUDIO.CPP
    STDIOSCB.CPP
    STDIOBRW.CPP
    TGOB.CPP
    TATR.CPP
    SPLOT.CPP
    SCNSORT.CPP
    BROWSER.CPP
    ESL.CPP
    POPUP.CPP
    MMINSTAL.CPP
    PORTF.CPP
    APE.CPP
    UTEST.CPP
    UTESTSCB.CPP
)

# Studio headers
set(STUDIO_HEADERS
    STDIOCRS.H
    MMINSTAL.H
    TOOLTIPS.H
)

# Resource files
set(STUDIO_RC_FILES
    UTEST.RC
    UTEST.RC2
)

# CHOMP tool is built by kauai/TOOLS (already added in kauai/CMakeLists.txt)
# Find the chomp executable in the build directory
find_program(CHOMP_EXECUTABLE chomp
    PATHS ${CMAKE_BINARY_DIR}/kauai/TOOLS
    DOC "Chunky file compiler (CHOMP)"
    NO_DEFAULT_PATH
)

# CHT files for 3dmovie.chk (from UTEST_CHT_FILES in Makefile)
set(UTEST_CHT_FILES
    UTESTGST.CHT
    UTEST.CHT
)

# CHT files for studio.chk (from STUDIO_CHT_FILES in Makefile)
set(STUDIO_CHT_FILES
    BROWSER.CHT
    CURSORS.CHT
    EASELS.CHT
    HELPBOOK.CHT
    HLPBKPER.CHT
    GADGET.CHT
    GADGPERS.CHT
    HELPRES.CHT
    HELPCTRL.CHT
    PROJMAIN.CHT
    POPUPS.CHT
    POPDN.CHT
    SECTOOLS.CHT
    STDIOSCB.CHT
    STDIOSND.CHT
    TBGUIDHP.CHT
    TBHTSCEN.CHT
    TBHTACTR.CHT
    TBHTSOUN.CHT
    TBHTWORD.CHT
    TBLOGOWZ.CHT
    TBBASICS.CHT
    TBPROJ1.CHT
    TBPROJ2.CHT
    TBPROJ3.CHT
    TBPROJ4.CHT
    TBPROJ5.CHT
    TBPROJ6.CHT
    STUDIO.CHT
)

# Custom command to compile .cht files to .chk using CHOMP
# 3dmovie.chk from utest.cht + utestgst.cht
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/3dmovie.chk
    COMMAND ${CHOMP_EXECUTABLE} /c
        ${CMAKE_CURRENT_SOURCE_DIR}/UTEST.CHT
        ${CMAKE_CURRENT_BINARY_DIR}/3dmovie.chk
    DEPENDS ${CHOMP_EXECUTABLE} ${UTEST_CHT_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling 3dmovie.chk from .cht files using CHOMP"
    VERBATIM
)

# Custom command to compile studio.chk from studio.cht (which includes all others)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/studio.chk
    COMMAND ${CHOMP_EXECUTABLE} /c
        ${CMAKE_CURRENT_SOURCE_DIR}/STUDIO.CHT
        ${CMAKE_CURRENT_BINARY_DIR}/studio.chk
    DEPENDS ${CHOMP_EXECUTABLE} ${STUDIO_CHT_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling studio.chk from .cht files using CHOMP"
    VERBATIM
)

# Resource compilation for Windows resources
if(WIN32)
    # Use windres or rc.exe for resource compilation
    find_program(WINDRES_EXECUTABLE windres)
    find_program(RC_EXECUTABLE rc)
    
    if(WINDRES_EXECUTABLE)
        set(RC_COMPILER ${WINDRES_EXECUTABLE})
        set(RC_FLAGS -O coff)
    elseif(RC_EXECUTABLE)
        set(RC_COMPILER ${RC_EXECUTABLE})
        set(RC_FLAGS /v)
    else()
        message(WARNING "No resource compiler found (windres or rc.exe). Resources will not be compiled.")
    endif()
    
    if(RC_COMPILER)
        # Compile UTEST.RC to .res file
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/utest.res
            COMMAND ${RC_COMPILER} ${RC_FLAGS}
                -I ${CMAKE_SOURCE_DIR}/INC
                -I ${CMAKE_SOURCE_DIR}/BREN/INC
                -I ${CMAKE_SOURCE_DIR}/KAUAI/SRC
                -I ${CMAKE_CURRENT_SOURCE_DIR}
                -D UNICODE -D _UNICODE
                -D WIN32 -D _WINDOWS
                -fo ${CMAKE_CURRENT_BINARY_DIR}/utest.res
                ${CMAKE_CURRENT_SOURCE_DIR}/UTEST.RC
            DEPENDS ${STUDIO_RC_FILES}
                ${CMAKE_SOURCE_DIR}/INC/UTESTRES.H
                ${CMAKE_SOURCE_DIR}/INC/VERSION.H
                ${CMAKE_SOURCE_DIR}/INC/SOCRES.H
                ${CMAKE_SOURCE_DIR}/INC/FRAMEDEF.H
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Compiling Windows resources (utest.res)"
            VERBATIM
        )
        
        set(STUDIO_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/utest.res)
    endif()
endif()

# Create the 3dmovie executable
add_executable(3dmovie
    ${STUDIO_SOURCES}
    ${STUDIO_HEADERS}
)

# Set output name and location
set_target_properties(3dmovie PROPERTIES
    OUTPUT_NAME "3dmovie"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(3dmovie PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/INC
    ${CMAKE_SOURCE_DIR}/BREN/INC
    ${CMAKE_SOURCE_DIR}/KAUAI/SRC
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated .chk files
)

# Link dependencies
target_link_libraries(3dmovie PRIVATE
    engine
    kauai
    bren
)

# Windows system libraries
if(WIN32)
    target_link_libraries(3dmovie PRIVATE
        user32.lib
        gdi32.lib
        winmm.lib
        advapi32.lib
        comdlg32.lib
        uuid.lib
        kernel32.lib
        vfw32.lib
        mpr.lib
    )
endif()

# Compile definitions
target_compile_definitions(3dmovie PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    WIN32
    WIN
    _WINDOWS
    STRICT
    UNICODE
    _UNICODE
)

# Compiler options for legacy code compatibility
target_compile_options(3dmovie PRIVATE
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

# Link resources if available
if(STUDIO_RESOURCES)
    target_sources(3dmovie PRIVATE ${STUDIO_RESOURCES})
endif()

# Dependencies on .chk files (ensure they're built before linking)
add_dependencies(3dmovie chomp)

# Custom target to ensure chunky files are built
add_custom_target(studio_chunks
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/3dmovie.chk ${CMAKE_CURRENT_BINARY_DIR}/studio.chk
    COMMENT "Building studio chunky files (.chk)"
)

add_dependencies(3dmovie studio_chunks)

endif() # BUILD_LEGACY_ENGINE

# Install target (optional)
# install(TARGETS 3dmovie RUNTIME DESTINATION bin)