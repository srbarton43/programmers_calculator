# root meson file
# SRB - 2023

project('pcalc', 'c')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')

executable( 'pcalc', src_files,
            link_args : '-ll' )
