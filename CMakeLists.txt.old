# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (programmers_calculator)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
  src/main.c
  src/file.c
  src/number.c
  src/stack.c
  src/parser.c
)

# Add an executable with the above sources
add_executable(programmers_calculator ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(programmers_calculator
  PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(programmers_calculator
  pcre2-8
)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")

target_compile_options(programmers_calculator INTERFACE
  "<${gcc_like_cxx}:-Wall;-pedantic>"
)