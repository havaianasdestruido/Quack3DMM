# BREN (BRender) CMakeLists.txt
# BRender 3D Graphics Engine from Argonaut Software

cmake_minimum_required(VERSION 3.20)

# BREN source files
set(BREN_SOURCES
    BRENFUN.CPP
    BWLD.CPP
    MATERIAL.CPP
    TMAP.CPP
    ZBMP.CPP
)

set(BREN_HEADERS
    INC/ACTOR.H
    INC/ANGLES.H
    INC/ANIMATE.H
    INC/BLOCKOPS.H
    INC/BRASSERT.H
    INC/BRDIAG.H
    INC/BREN.H
    INC/BRENDER.H
    INC/BRERR.H
    INC/BRFILE.H
    INC/BRFONT.H
    INC/BRHTON.H
    INC/BRLIMITS.H
    INC/BRMEM.H
    INC/BWLD.H
    INC/CAMERA.H
    INC/COLOUR.H
    INC/COMPILER.H
    INC/CUSTSUPT.H
    INC/FIXED.H
    INC/FMT.H
    INC/FMTPROTO.H
    INC/FWPEXTRA.H
    INC/FWPROTO.H
    INC/G386IFIX.H
    INC/HTON.H
    INC/LIGHT.H
    INC/MATERIAL.H
    INC/MATRIX.H
    INC/MODEL.H
    INC/MSCIFIX.H
    INC/PIXELMAP.H
    INC/POOL.H
    INC/POSITION.H
    INC/QHTON.H
    INC/QUANTIZE.H
    INC/QUAT.H
    INC/SCALAR.H
    INC/SCALE.H
    INC/TMAP.H
    INC/TRANSFRM.H
    INC/TRAVERSE.H
    INC/VECFNS.H
    INC/VECTOR.H
    INC/WTCIFIX.H
    INC/ZBMP.H
    INC/ZBPROTO.H
)

# Create BREN library
add_library(bren STATIC ${BREN_SOURCES} ${BREN_HEADERS})

# Target properties
set_target_properties(bren PROPERTIES
    OUTPUT_NAME "bren"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(bren PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/INC
)

# Compile definitions for BREN
target_compile_definitions(bren PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    WIN32
    WIN
    _WINDOWS
    BR_RENDERER
)

# Compiler options for legacy BRender code
target_compile_options(bren PRIVATE
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
    target_link_libraries(bren PRIVATE
        user32.lib
        gdi32.lib
        winmm.lib
        kernel32.lib
    )
endif()

# Install target
# install(TARGETS bren ARCHIVE DESTINATION lib)
# install(DIRECTORY INC/ DESTINATION include/bren FILES_MATCHING PATTERN "*.h")