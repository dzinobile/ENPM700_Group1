add_custom_target(download_test_data
  COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/test_media.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)




#Any C++ source files needed to build this target(cpp - test).
add_executable(cpp-test main.cpp test.cpp)

#Any include directories needed to build this target.
#Note : we do not need to specify the include directories for the
#dependent libraries, they are automatically included.
#target_include_directories(cpp - test PUBLIC
# #list of include directories:
#${CMAKE_SOURCE_DIR } / include
#)

#Any dependent libraires needed to build this target.
target_link_libraries(cpp-test PUBLIC gtest myLib1 ${OpenCV_LIBS})

#Enable CMake’s test runner to discover the tests included in the
#binary, using the GoogleTest CMake module.
        gtest_discover_tests(cpp-test)
find_package(OpenCV REQUIRED)

add_dependencies(cpp-test download_test_data)