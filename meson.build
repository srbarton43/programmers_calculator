# root meson file
# SRB - 2023

project('pcalc', 'c')

cc = meson.get_compiler('c')

use_libedit = cc.has_link_argument('-ledit')
use_readline = cc.has_link_argument('-lreadline')

if use_libedit
  add_project_arguments('-DLIBEDIT', language : 'c')
  add_project_link_arguments('-ledit', language : 'c')
elif use_readline
  add_project_arguments('-DREADLINE', language : 'c')
  add_project_link_arguments('-lreadline', language : 'c')
else
  add_project_arguments('-DNOHISTORY', language : 'c')
endif

add_project_link_arguments('-ll', language : 'c')
inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')

executable( 'pcalc', src_files,
            include_directories : inc,
            #c_args : '-DDEBUG',
            )
