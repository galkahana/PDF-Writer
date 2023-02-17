add_library(FreeType
#sources
src/base/ftbbox.c
src/base/ftgxval.c
src/base/ftlcdfil.c
src/base/ftmm.c
src/base/ftotval.c
src/base/ftpatent.c
src/base/ftpfr.c
src/base/ftsynth.c
src/base/fttype1.c
src/base/ftwinfnt.c
src/base/ftxf86.c
#src/otvalid/otvalid.c
src/pcf/pcf.c
src/pfr/pfr.c
src/psaux/psaux.c
src/pshinter/pshinter.c
src/psnames/psmodule.c
#src/psnames/psnames.c
src/raster/raster.c
src/sfnt/sfnt.c
src/truetype/truetype.c
src/type1/type1.c
src/type42/type42.c
src/cid/type1cid.c
src/winfonts/winfnt.c
src/autofit/autofit.c
src/bdf/bdf.c
src/cff/cff.c
src/base/ftbase.c
src/base/ftbitmap.c
src/cache/ftcache.c
src/base/ftcid.c
src/base/ftdebug.c
src/base/ftfstype.c
src/base/ftgasp.c
src/base/ftglyph.c
src/gzip/ftgzip.c
src/base/ftinit.c
src/lzw/ftlzw.c
src/base/ftstroke.c
src/base/ftsystem.c
src/smooth/smooth.c

#headers
include/freetype/config/ftconfig.h
include/freetype/config/ftheader.h
include/freetype/config/ftmodule.h
include/freetype/config/ftoption.h
include/freetype/config/ftstdlib.h
include/ft2build.h
)
add_library(PDFHummus::FreeType ALIAS FreeType)

target_include_directories(FreeType
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/FreeType/include> 
)
target_compile_definitions(FreeType PUBLIC FT2_BUILD_LIBRARY=1)   

target_include_directories(FreeType
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freetype
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freetype/config
    ${CMAKE_CURRENT_SOURCE_DIR}/include/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freetype/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freetype/internal/services
)

install(TARGETS FreeType
    EXPORT PDFHummusTargets
    RUNTIME DESTINATION bin COMPONENT dependencies
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION include/FreeType
    FILES_MATCHING
    PATTERN "*.h"
)