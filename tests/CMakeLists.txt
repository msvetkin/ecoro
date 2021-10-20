include(CTest)
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        origin/master
)

FetchContent_MakeAvailable(googletest)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

include(GoogleTest)

macro(ecoro_test TESTNAME)
    add_executable(${TESTNAME} ${TESTNAME}.cpp)
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ecoro)
    gtest_discover_tests(${TESTNAME})
endmacro()

ecoro_test(tst_task)
ecoro_test(tst_awaiter_traits)
ecoro_test(tst_awaitable_traits)
ecoro_test(tst_when_all)
ecoro_test(tst_scope)
