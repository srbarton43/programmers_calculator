
unittests_exe = executable( 'unittest', 'unittest.c', ut_sources,
            include_directories : inc, 
            link_with : hashtable,
            c_args : '-DUNIT_TEST' )

test('unit tests', unittests_exe)
