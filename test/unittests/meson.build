
test_addition_exe = executable( 'test_addition', 'addition.c',
  include_directories : inc,
  link_with : [hashtable, unittest_lib],
  c_args : '-DUNIT_TEST' )

test('addition', test_addition_exe)

test_subtraction_exe = executable( 'test_subtraction', 'subtraction.c',
  include_directories : inc,
  link_with : [hashtable, unittest_lib],
  c_args : '-DUNIT_TEST' )

test('subtraction', test_subtraction_exe)

test_shifts_exe = executable( 'test_shifts', 'shifts.c',
  include_directories : inc,
  link_with : [hashtable, unittest_lib],
  c_args : '-DUNIT_TEST' )

test('lshift and rshift', test_shifts_exe)

test_twos_comp_exe = executable( 'test_twos_comp', 'twos_comp.c',
  include_directories : inc,
  link_with : [hashtable, unittest_lib],
  c_args : '-DUNIT_TEST' )

test('twos_comp', test_twos_comp_exe)

test_copy_num_exe = executable( 'copy_number', 'copy_number.c',
  include_directories : inc,
  link_with : [hashtable, unittest_lib],
  c_args : '-DUNIT_TEST' )

test('copy_number', test_copy_num_exe)

