unittest_sources += files('unittest_utils.c')
unittest_lib = library('unittest_lib',
                           unittest_sources,
                           include_directories : inc,
                           c_args : ['-DUNIT_TEST', '-DDEBUG'] )

run_command('cp', '-rf', 'inputs', meson.global_build_root() / 'test' / 'unittests', check : true)

test_addition_exe = executable( 'test_addition', 'addition.c',
  include_directories : inc,
  link_with : unittest_lib,
  c_args : '-DUNIT_TEST' )

test('addition', test_addition_exe)

test_subtraction_exe = executable( 'test_subtraction', 'subtraction.c',
  include_directories : inc,
  link_with : unittest_lib,
  c_args : '-DUNIT_TEST' )

test('subtraction', test_subtraction_exe)

test_twos_comp_exe = executable( 'test_twos_comp', 'twos_comp.c',
  include_directories : inc,
  link_with : unittest_lib,
  c_args : '-DUNIT_TEST' )

test('twos_comp', test_twos_comp_exe)

test_and_exe = executable( 'test_and', 'and.c',
  include_directories : inc,
  link_with : unittest_lib,
  c_args : '-DUNIT_TEST' )

test('bitwise AND', test_and_exe)

test_or_exe = executable( 'test_or', 'or.c',
  include_directories : inc,
  link_with : unittest_lib,
  c_args : '-DUNIT_TEST' )

test('bitwise OR', test_or_exe)

# test_wide_exe = executable( 'test_wide', 'wide.c',
#   include_directories : inc,
#   link_with : unittest_lib,
#   c_args : ['-DUNIT_TEST', '-DDEBUG'] )
#
# test('wide number', test_wide_exe)

# test_dec_print = executable( 'test_decimal', 'print.c',
#   include_directories : inc,
#   link_with : unittest_lib )
# test('test_dec', test_dec_print)

test_lshift = executable( 'test_lshift', 'lshift.c',
  include_directories : inc,
  link_with : unittest_lib)
test('test_lshift', test_lshift, args : meson.global_build_root() / 'test' / 'unittests' / 'inputs' / 'lshift.txt' )

test_rshift = executable( 'test_rshift', 'rshift.c',
  include_directories : inc,
  link_with : unittest_lib)
test('test_rshift', test_rshift, args : meson.global_build_root() / 'test' / 'unittests' / 'inputs' / 'rshift.txt' )

test_division = executable( 'test_div', 'divide.c',
  include_directories : inc,
  link_with : unittest_lib)
test('test_div', test_division, args : meson.global_build_root() / 'test' / 'unittests' / 'inputs' / 'divide.txt', timeout : 5 )

test_mod = executable( 'test_mod', 'modulo.c',
  include_directories : inc,
  link_with : unittest_lib)
test('test_mod', test_mod, args : meson.global_build_root() / 'test' / 'unittests' / 'inputs' / 'modulo.txt', timeout : 5 )

test_mult = executable( 'test_mult', 'multiply.c',
  include_directories : inc,
  link_with : unittest_lib)
test('test_mult', test_mult, args : meson.global_build_root() / 'test' / 'unittests' / 'inputs' / 'multiply.txt', timeout : 5 )
