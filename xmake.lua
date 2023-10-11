-- originally from https://github.com/xmake-io/xmake-repo/pull/1709

option("libtiff", {description = "Enable libtiff", default = false})
option("libpng", {description = "Enable libpng", default = false})
option("libjpeg", {description = "Enable libjpeg", default = false})
add_rules("mode.debug", "mode.release")
if has_config("libtiff") then
    add_requires("libtiff")
end
if has_config("libpng") then
    add_requires("libpng")
end
if has_config("libjpeg") then
    add_requires("libjpeg")
end
add_requires("freetype", "zlib", "libaesgm")
target("pdfhummus")
    set_kind("$(kind)")
    add_files("PDFWriter/*.cpp")
    add_headerfiles("(PDFWriter/*.h)")
    add_packages("freetype")
    add_packages("libtiff", "libpng", "libjpeg")
    add_packages("libaesgm", "zlib")
    if has_package("libtiff") then
        add_defines("_INCLUDE_TIFF_HEADER")
        add_cxflags("-Wno-deprecated-declarations")
    else
        add_defines("PDFHUMMUS_NO_TIFF=1")
    end
    if not has_package("libpng") then
        add_defines("PDFHUMMUS_NO_PNG=1")
    end
    if not has_package("libjpeg") then
        add_defines("PDFHUMMUS_NO_DCT=1")
    end
    -- port symbols for linker
    if is_plat("windows") and is_kind("shared") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end

