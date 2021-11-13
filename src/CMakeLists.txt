add_library(ecoro scope.cpp)
add_library(ecoro::ecoro ALIAS ecoro)

target_include_directories(
  ecoro PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(ecoro PUBLIC cxx_std_20)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(ecoro PUBLIC -fcoroutines)
    find_package(Threads)
    target_link_libraries(ecoro PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    target_compile_definitions(ecoro
        PUBLIC
          ECORO_HACK_NOINLINE=
          $<$<CONFIG:Debug>:>
          $<$<CONFIG:RelWithDebInfo>:SYMMETRIC_TRANSFER>
          $<$<CONFIG:Release>:SYMMETRIC_TRANSFER>
          $<$<CONFIG:MinSizeRel>:SYMMETRIC_TRANSFER>
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT ANDROID)
    target_compile_options(ecoro PUBLIC -stdlib=libc++ -fcoroutines-ts)
    target_link_options(ecoro PUBLIC -stdlib=libc++ -lc++abi)
    target_compile_definitions(ecoro
        PUBLIC
          SYMMETRIC_TRANSFER
          ECORO_HACK_NOINLINE=
    )
elseif (MSVC)
    target_compile_definitions(ecoro
        PUBLIC
          SYMMETRIC_TRANSFER
          $<$<CONFIG:Debug>:ECORO_HACK_NOINLINE=>
          $<$<CONFIG:RelWithDebInfo>:ECORO_HACK_NOINLINE=ECORO_NOINLINE>
          $<$<CONFIG:Release>:ECORO_HACK_NOINLINE=ECORO_NOINLINE>
          $<$<CONFIG:MinSizeRel>:ECORO_HACK_NOINLINE=>
    )
endif()
