include(CTest)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

macro(ecoro_test TESTNAME)
  add_executable(${TESTNAME} ${TESTNAME}.cpp)
  target_link_libraries(${TESTNAME}
    PRIVATE
      GTest::gtest
      GTest::gtest_main
      ecoro
  )
  gtest_discover_tests(${TESTNAME})
endmacro()

add_subdirectory(detail)

ecoro_test(tst_awaitable_concepts)
ecoro_test(tst_awaitable_traits)
ecoro_test(tst_awaiter_traits)
ecoro_test(tst_awaiter_concepts)
ecoro_test(tst_manual_reset_event)
ecoro_test(tst_scope)
ecoro_test(tst_stop_token)
ecoro_test(tst_task)
ecoro_test(tst_when_all)
ecoro_test(tst_when_any)
ecoro_test(tst_when_first)
