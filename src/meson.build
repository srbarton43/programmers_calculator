# meson.build for src dir
# SRB - 2023

flex = '/opt/homebrew/opt/flex/bin/flex'
bison = '/opt/homebrew/opt/bison/bin/bison'

parser_prefix = '/'.join(meson.global_build_root(), 'parser')
parser_src = '.'.join(parser_prefix, 'tab.c')
parser_header = '.'.join(parser_prefix, 'tab.h')
run_command(bison, '-b', parser_prefix, '-d', 'parser.y', check : true)

lexer = '/'.join(meson.global_build_root(), 'lexer.c')
run_command(flex, '-o', lexer, 'lexer.l', check : true)

ut_sources = files('number.c', 'utils.c')

src_files = files('main.c', 'number.c', 'utils.c', parser_src, lexer)
