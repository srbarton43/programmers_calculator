run_command('cp', '-rf', 'inputs', meson.global_build_root() / 'test' / 'unittests', check : true)

unittest_sources += files('unittest_utils.c')
unittest_lib = library('unittest_lib',
                           unittest_sources,
                           include_directories : inc,
                           c_args : '-DUNIT_TEST' )

#test_addition_exe = executable( 'test_addition', 'addition.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('addition', test_addition_exe)
#
#test_subtraction_exe = executable( 'test_subtraction', 'subtraction.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('subtraction', test_subtraction_exe)
#
#test_shifts_exe = executable( 'test_shifts', 'shifts.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('lshift and rshift', test_shifts_exe)
#
#test_twos_comp_exe = executable( 'test_twos_comp', 'twos_comp.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('twos_comp', test_twos_comp_exe)
#
#test_copy_num_exe = executable( 'test_copy_number', 'copy_number.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('copy_number', test_copy_num_exe)
#
#test_and_exe = executable( 'test_and', 'and.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('bitwise AND', test_and_exe)
#
#test_or_exe = executable( 'test_or', 'or.c',
#  include_directories : inc,
#  link_with : unittest_lib,
#  c_args : '-DUNIT_TEST' )
#
#test('bitwise OR', test_or_exe)

test_wide_exe = executable( 'test_wide', 'wide.c',
  include_directories : inc,
  link_with : unittest_lib,
  c_args : ['-DUNIT_TEST', '-DDEBUG'] )

test('wide number', test_wide_exe)
