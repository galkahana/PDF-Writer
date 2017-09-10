{
    'targets': [
        {
            'target_name': 'libpng',
            'type': 'static_library',

            'include_dirs': [
                '<(module_root_dir)/src/deps/ZLib',
            ],            
            'sources': [
                'png.c',
                'pngerror.c',
                'pngget.c',
                'pngmem.c',
                'pngpread.c',
                'pngread.c',
                'pngrio.c',
                'pngrtran.c',
                'pngrutil.c',
                'pngset.c',
                'pngtrans.c',
                'pngwio.c',
                'pngwrite.c',
                'pngwtran.c',
                'pngwutil.c',
                'png.h',
                'pngconf.h',
                'pngdebug.h',
                'pnginfo.h',
                'pnglibconf.h',
                'pngpriv.h',
                'pngstruct.h'
            ]
        }
    ]        
}
