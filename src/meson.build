# meson.build for src dir
# SRB - 2023

flex = find_program('flex', required : true)
bison = find_program('bison', required : true)

parser_prefix = meson.global_build_root() / 'parser'
parser_src = '.'.join(parser_prefix, 'tab.c')
run_command(bison, '-b', parser_prefix, '-d', 'parser.y', check : true)

lexer = meson.global_build_root() / 'lexer.c'
run_command(flex, '-o', lexer, 'lexer.l', check : true)

src_files = files('pc.c', 'number.c', 'utils.c', parser_src, lexer)

unittest_sources = files('number.c', 'utils.c')
unittest_lib = library('unittest_lib',
                           unittest_sources,
                           include_directories : inc,
                           c_args : '-DUNIT_TEST' )

