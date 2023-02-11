

add_library(LibAesgm
#sources
aescrypt.c
aeskey.c
aes_modes.c
aes_ni.c
aestab.c


#headers
aescpp.h
aes.h
aes_ni.h
aesopt.h
aestab.h
aes_via_ace.h
brg_endian.h
brg_types.h

)

add_library(PDFHummus::LibAesgm ALIAS LibAesgm)
set(LIBAESGM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBAESGM_LDFLAGS LibAesgm PARENT_SCOPE)

install(TARGETS LibAesgm
    EXPORT PDFHummusTargets
    RUNTIME DESTINATION bin COMPONENT dependencies
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT dependencies
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
)