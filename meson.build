# root meson file
# SRB - 2023

project('pcalc', 'c')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')

executable( 'pcalc', src_files,
            include_directories : inc,
            link_with : hashtable,
            #c_args : '-DDEBUG',
            link_args : '-ll' )
