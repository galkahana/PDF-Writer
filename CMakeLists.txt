cmake_minimum_required(VERSION 3.19)

# Soname
# MAJOR is incremented when symbols are removed or changed in an incompatible way
# MINOR is incremented when new symbols are added
project(PDFHummus VERSION 4.7.0)


option(USE_BUNDLED  "Whether to use bundled libraries" TRUE)
option(USE_UNBUNDLED_FALLBACK_BUNDLED  "When USE_BUNDLED is false and a certain system library is not available should fallback on bundled lib")
option(PDFHUMMUS_NO_DCT  "Whether to drop support for DCT streams parsing (will not use LibJpeg)")
option(PDFHUMMUS_NO_TIFF "Whether to drop TIFF Images support (will not use LibTiff)" )
option(PDFHUMMUS_NO_PNG "Whether to drop PNG Images support (will not use LibPng)" )
option(PDFHUMMUS_NO_OPENSSL "Whether to drop OpenSSL Usage, and therefore PDF2.0 encryption support (will not allow encryption or decryption of PDF2.0 files)" )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT USE_BUNDLED)
    FIND_PACKAGE(PkgConfig)

    # zlib
    if(USE_UNBUNDLED_FALLBACK_BUNDLED)
        find_package (ZLIB)
        if(ZLIB_FOUND)
            set (USING_UNBUNDLED_ZLIB TRUE)
        else(ZLIB_FOUND)
            ADD_SUBDIRECTORY(Zlib)
        endif(ZLIB_FOUND)
    else(USE_UNBUNDLED_FALLBACK_BUNDLED)
        find_package (ZLIB REQUIRED)
        set (USING_UNBUNDLED_ZLIB TRUE)
    endif(USE_UNBUNDLED_FALLBACK_BUNDLED)
    if(USING_UNBUNDLED_ZLIB)
        set(PDFHUMMUS_DEPENDS_ZLIB "find_dependency(ZLIB)")
    endif()

    # freetype
    if(USE_UNBUNDLED_FALLBACK_BUNDLED)
        find_package (Freetype)
        if(Freetype_FOUND)
            set (USING_UNBUNDLED_Freetype TRUE)
        else(Freetype_FOUND)
            ADD_SUBDIRECTORY(FreeType)
        endif(Freetype_FOUND)
    else(USE_UNBUNDLED_FALLBACK_BUNDLED)
        find_package (Freetype REQUIRED)
        set (USING_UNBUNDLED_Freetype TRUE)
    endif(USE_UNBUNDLED_FALLBACK_BUNDLED)    
    if(USING_UNBUNDLED_Freetype)
        set(PDFHUMMUS_DEPENDS_FREETYPE "find_dependency(Freetype)")
    endif()


    # libaesgm
    if(USE_UNBUNDLED_FALLBACK_BUNDLED)
        find_package (Aesgm)
        if(aesgm_FOUND)
            set (USING_UNBUNDLED_aesgm TRUE)
        else(aesgm_FOUND)
            ADD_SUBDIRECTORY(LibAesgm)
        endif(aesgm_FOUND)
    else(USE_UNBUNDLED_FALLBACK_BUNDLED)
        find_package (Aesgm REQUIRED)
        set (USING_UNBUNDLED_aesgm TRUE)
    endif(USE_UNBUNDLED_FALLBACK_BUNDLED)    
    if(USING_UNBUNDLED_aesgm)
        install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindAesgm.cmake
            DESTINATION lib/cmake/PDFHummus/cmake
        )        
        set(PDFHUMMUS_APPEND_MODULE "list(APPEND CMAKE_MODULE_PATH \"${CMAKE_CURRENT_LIST_DIR}/cmake\")") 
        set(PDFHUMMUS_DEPENDS_AESGM "find_dependency(Aesgm)")
    endif()


    # libjpeg
    if(NOT PDFHUMMUS_NO_DCT)
        find_package (JPEG)
        if (JPEG_FOUND)
            set (USING_UNBUNDLED_JPEG TRUE)
        else(JPEG_FOUND)
            if(USE_UNBUNDLED_FALLBACK_BUNDLED)
                ADD_SUBDIRECTORY(LibJpeg)
            else(USE_UNBUNDLED_FALLBACK_BUNDLED)
                message(WARNING "JPEG (LibJpeg) not found, disabling DCT encoding/decoding support")
                set (PDFHUMMUS_NO_DCT TRUE)
            endif(USE_UNBUNDLED_FALLBACK_BUNDLED)
        endif(JPEG_FOUND)
    endif(NOT PDFHUMMUS_NO_DCT)
    if(USING_UNBUNDLED_JPEG)
        set(PDFHUMMUS_DEPENDS_JPEG "find_dependency(JPEG)")
    endif()


    # libtiff
    if(NOT PDFHUMMUS_NO_TIFF)
        find_package (TIFF)
        if (TIFF_FOUND)
            set (USING_UNBUNDLED_TIFF TRUE)
        else(TIFF_FOUND)
            if(USE_UNBUNDLED_FALLBACK_BUNDLED)
                ADD_SUBDIRECTORY(LibTiff)
            else(USE_UNBUNDLED_FALLBACK_BUNDLED)
                message(WARNING "TIFF (LibTiff) not found, disabling TIFF images support")
                set (PDFHUMMUS_NO_TIFF TRUE)
            endif(USE_UNBUNDLED_FALLBACK_BUNDLED)
        endif(TIFF_FOUND)
    endif(NOT PDFHUMMUS_NO_TIFF)
    if(USING_UNBUNDLED_TIFF)
        set(PDFHUMMUS_DEPENDS_TIFF "find_dependency(TIFF)")
    endif()

    # libpng
    if(NOT PDFHUMMUS_NO_PNG)
        find_package (PNG)
        if (PNG_FOUND)
            set (USING_UNBUNDLED_PNG TRUE)
        else(PNG_FOUND)
            if(USE_UNBUNDLED_FALLBACK_BUNDLED)
                ADD_SUBDIRECTORY(LibPng)
            else(USE_UNBUNDLED_FALLBACK_BUNDLED)
                message(WARNING "PNG (LibPng) not found, disabling PNG images support")
                set (PDFHUMMUS_NO_PNG TRUE)
            endif(USE_UNBUNDLED_FALLBACK_BUNDLED)
        endif(PNG_FOUND)
    endif(NOT PDFHUMMUS_NO_PNG)
    if(USING_UNBUNDLED_PNG)
        set(PDFHUMMUS_DEPENDS_PNG "find_dependency(PNG)")
    endif()    

    # OpenSSL
    if(NOT PDFHUMMUS_NO_OPENSSL)
        find_package (OpenSSL)
        if (OPENSSL_FOUND)
            set (USING_UNBUNDLED_OPENSSL TRUE)
        else(OPENSSL_FOUND)
            if(USE_UNBUNDLED_FALLBACK_BUNDLED)
                # no bundled open ssl option, so turn off open ssl anyways
                message(WARNING "OpenSSL not found and there's no bundled option, disabling PDF 2.0 encyrption/decryption")
                set (PDFHUMMUS_NO_OPENSSL TRUE)
            else(USE_UNBUNDLED_FALLBACK_BUNDLED)
                message(WARNING "OpenSSL not found, disabling PDF 2.0 encyrption/decryption")
               set (PDFHUMMUS_NO_OPENSSL TRUE)
            endif(USE_UNBUNDLED_FALLBACK_BUNDLED)
        endif(OPENSSL_FOUND)
    endif(NOT PDFHUMMUS_NO_OPENSSL)
    if(USING_UNBUNDLED_PNG)
        set(PDFHUMMUS_DEPENDS_OPENSSL "find_dependency(PNG)")
    endif()    

else(NOT USE_BUNDLED)
    # zlib
    ADD_SUBDIRECTORY(Zlib)

    # freetype
    ADD_SUBDIRECTORY(FreeType)

    # libaesgm
    ADD_SUBDIRECTORY(LibAesgm)

    # libjpeg
    if(NOT PDFHUMMUS_NO_DCT)
        ADD_SUBDIRECTORY(LibJpeg)
    endif(NOT PDFHUMMUS_NO_DCT)

    # libtiff
    if(NOT PDFHUMMUS_NO_TIFF)
        ADD_SUBDIRECTORY(LibTiff)
    endif(NOT PDFHUMMUS_NO_TIFF)

    # libpng
    if(NOT PDFHUMMUS_NO_PNG)
        ADD_SUBDIRECTORY(LibPng)
    endif(NOT PDFHUMMUS_NO_PNG)

    if(NOT PDFHUMMUS_NO_OPENSSL)
        # OpenSSL is never bundled
        find_package (OpenSSL)
        if (OPENSSL_FOUND)
            set (USING_UNBUNDLED_OPENSSL TRUE)       
        else(OPENSSL_FOUND)
            message(WARNING "OpenSSL not found and there's no bundled option, disabling PDF 2.0 encyrption/decryption")
            set (PDFHUMMUS_NO_OPENSSL TRUE)
        endif(OPENSSL_FOUND)
        if(USING_UNBUNDLED_OPENSSL)
            set(PDFHUMMUS_DEPENDS_OPENSSL "find_dependency(OpenSSL)")
        endif(USING_UNBUNDLED_OPENSSL)
    endif(NOT PDFHUMMUS_NO_OPENSSL)

endif(NOT USE_BUNDLED)


ADD_SUBDIRECTORY(PDFWriter)


if(PROJECT_IS_TOP_LEVEL AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/PDFWriterTesting)
    # avoid installing the testing lib altogether when included in another project.
    # it's annoying when in parent all, and more annoying to then get the tests added
    # to the parent project ctest.

    # option specific for testing
    option(BUILD_FUZZING_HARNESS "Build the fuzzing harness, requires LLVM's clang")

    enable_testing()
    ADD_SUBDIRECTORY(PDFWriterTesting)
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PDFHummus_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PDFHummus_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")
include(CPack)

install(EXPORT PDFHummusTargets
  FILE PDFHummusTargets.cmake
  DESTINATION lib/cmake/PDFHummus
  NAMESPACE PDFHummus::
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/PDFHummus"
  )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfigVersion.cmake"
    VERSION "${PDFHummus_VERSION_MAJOR}.${PDFHummus_VERSION_MINOR}"
    COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/PDFHummusConfigVersion.cmake
  DESTINATION lib/cmake/PDFHummus
  )

export(EXPORT PDFHummusTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PDFHummusTargets.cmake"
  NAMESPACE PDFHummus::
)