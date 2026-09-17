# Kauai UI Framework CMakeLists.txt
# Microsoft's internal UI framework used by 3D Movie Maker

cmake_minimum_required(VERSION 3.20)

# Kauai source files
set(KAUAI_SOURCES
    SRC/APPB.CPP
    SRC/APPBMAC.CPP
    SRC/APPBWIN.CPP
    SRC/BASE.CPP
    SRC/CHCM.CPP
    SRC/CHSE.CPP
    SRC/CHUNK.CPP
    SRC/CLIP.CPP
    SRC/CLOK.CPP
    SRC/CMD.CPP
    SRC/CODEC.CPP
    SRC/CODKAUAI.CPP
    SRC/CRF.CPP
    SRC/CTL.CPP
    SRC/CURSOR.CPP
    SRC/DLG.CPP
    SRC/DLGMAC.CPP
    SRC/DLGWIN.CPP
    SRC/DOCB.CPP
    SRC/FILE.CPP
    SRC/FILEMAC.CPP
    SRC/FILEWIN.CPP
    SRC/FNIMAC.CPP
    SRC/FNIWIN.CPP
    SRC/FT.CPP
    SRC/GFX.CPP
    SRC/GFXMAC.CPP
    SRC/GFXWIN.CPP
    SRC/GOB.CPP
    SRC/GOBMAC.CPP
    SRC/GOBWIN.CPP
    SRC/GROUPS.CPP
    SRC/GROUPS2.CPP
    SRC/KIDHELP.CPP
    SRC/KIDSPACE.CPP
    SRC/KIDWORLD.CPP
    SRC/LEX.CPP
    SRC/MBMP.CPP
    SRC/MBMPGUI.CPP
    SRC/MEMMAC.CPP
    SRC/MEMWIN.CPP
    SRC/MENUMAC.CPP
    SRC/MENUWIN.CPP
    SRC/MIDI.CPP
    SRC/MIDIDEV.CPP
    SRC/MIDIDEV2.CPP
    SRC/MSSIO.CPP
    SRC/PIC.CPP
    SRC/PICMAC.CPP
    SRC/PICWIN.CPP
    SRC/REGION.CPP
    SRC/RTXT.CPP
    SRC/RTXT2.CPP
    SRC/SCRCOM.CPP
    SRC/SCRCOMG.CPP
    SRC/SCREXE.CPP
    SRC/SCREXEG.CPP
    SRC/SNDAM.CPP
    SRC/SNDM.CPP
    SRC/SPELL.CPP
    SRC/STREAM.CPP
    SRC/TEST.CPP
    SRC/TEXT.CPP
    SRC/TEXTDOC.CPP
    SRC/UT.CPP
    SRC/UTIL.CPP
    SRC/UTILCOPY.CPP
    SRC/UTILERRO.CPP
    SRC/UTILGLOB.CPP
    SRC/UTILINT.CPP
    SRC/UTILMEM.CPP
    SRC/UTILRND.CPP
    SRC/UTILSTR.CPP
    SRC/VIDEO.CPP
)

set(KAUAI_HEADERS
    SRC/APPB.H
    SRC/AUDIOMAN.H
    SRC/BASE.H
    SRC/CHCM.H
    SRC/CHSE.H
    SRC/CHTRANS.H
    SRC/CHUNK.H
    SRC/CLIP.H
    SRC/CLOK.H
    SRC/CMD.H
    SRC/CODEC.H
    SRC/CODKPRI.H
    SRC/CRF.H
    SRC/CSAPI.H
    SRC/CTL.H
    SRC/CURSOR.H
    SRC/DEBUG.H
    SRC/DLG.H
    SRC/DOCB.H
    SRC/FILE.H
    SRC/FNI.H
    SRC/FRAME.H
    SRC/FRAMEDEF.H
    SRC/FRAMERES.H
    SRC/FTRES.H
    SRC/GFX.H
    SRC/GOB.H
    SRC/GROUPS.H
    SRC/KCD2_386.H
    SRC/KCDC_386.H
    SRC/KEYS.H
    SRC/KIDDEF.H
    SRC/KIDFRAME.H
    SRC/KIDHELP.H
    SRC/KIDSPACE.H
    SRC/KIDWORLD.H
    SRC/LEX.H
    SRC/MAC.H
    SRC/MBMP.H
    SRC/MDEV2PRI.H
    SRC/MENU.H
    SRC/MIDI.H
    SRC/MIDIDEV.H
    SRC/MIDIDEV2.H
    SRC/MSSIO.H
    SRC/PIC.H
    SRC/REGION.H
    SRC/RTXT.H
    SRC/SCRCOM.H
    SRC/SCRCOMG.H
    SRC/SCREXE.H
    SRC/SCREXEG.H
    SRC/SNDAM.H
    SRC/SNDM.H
    SRC/SPELL.H
    SRC/STREAM.H
    SRC/TEXT.H
    SRC/TEXTDOC.H
    SRC/UTIL.H
    SRC/UTILCOPY.H
    SRC/UTILERRO.H
    SRC/UTILGLOB.H
    SRC/UTILINT.H
    SRC/UTILMEM.H
    SRC/UTILRND.H
    SRC/UTILSTR.H
    SRC/VIDEO.H
)

# Create Kauai library
add_library(kauai STATIC ${KAUAI_SOURCES} ${KAUAI_HEADERS})

# Target properties
set_target_properties(kauai PROPERTIES
    OUTPUT_NAME "kauai"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(kauai PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SRC
)

# Compile definitions
target_compile_definitions(kauai PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    WIN32
    WIN
    _WINDOWS
    STRICT
)

# Compiler options for legacy code
target_compile_options(kauai PRIVATE
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
    /wd4189
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
    target_link_libraries(kauai PRIVATE
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

# Add tools subdirectory
add_subdirectory(TOOLS)