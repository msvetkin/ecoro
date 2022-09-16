# The MIT License (MIT)
#
# Copyright (c) 2022 Mikhail Svetkin
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

ecoro_module(core)

target_sources(${ecoro_current_target}
  PRIVATE
    include/ecoro/detail/intrusive/list.hpp
    include/ecoro/detail/compiler.hpp
    include/ecoro/detail/invoke_or_pass.hpp
    include/ecoro/detail/std_concepts.hpp
    include/ecoro/detail/sync_wait_task.hpp
    include/ecoro/detail/task_awaitable.hpp
    include/ecoro/detail/task_promise_impl.hpp
    include/ecoro/detail/traits.hpp
    include/ecoro/awaitable_concepts.hpp
    include/ecoro/awaitable_traits.hpp
    include/ecoro/awaiter_concepts.hpp
    include/ecoro/awaiter_traits.hpp
    include/ecoro/concepts.hpp
    include/ecoro/config.hpp
    include/ecoro/coroutine.hpp
    include/ecoro/manual_reset_event.hpp
    include/ecoro/scheduler.hpp
    include/ecoro/scope.hpp
    include/ecoro/scope_guard.hpp
    include/ecoro/stop_token.hpp
    include/ecoro/sync_wait.hpp
    include/ecoro/task.hpp
    include/ecoro/task_promise.hpp
    include/ecoro/this_coro.hpp
    include/ecoro/when_all.hpp
    include/ecoro/when_any.hpp
    include/ecoro/when_first.hpp
    manual_reset_event.cpp
    scope.cpp
    stop_token.cpp
)

target_compile_features(${ecoro_current_target} PUBLIC cxx_std_20)

if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${ecoro_current_target} PUBLIC -fcoroutines)
  find_package(Threads)
  target_link_libraries(${ecoro_current_target} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
  target_compile_definitions(${ecoro_current_target}
    PUBLIC
      ECORO_HACK_NOINLINE=
      $<$<CONFIG:Debug>:>
      $<$<CONFIG:RelWithDebInfo>:SYMMETRIC_TRANSFER>
      $<$<CONFIG:Release>:SYMMETRIC_TRANSFER>
      $<$<CONFIG:MinSizeRel>:SYMMETRIC_TRANSFER>
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT ANDROID)
  target_compile_options(${ecoro_current_target} PUBLIC -stdlib=libc++ -fcoroutines-ts)
  target_link_options(${ecoro_current_target} PUBLIC -stdlib=libc++ -lc++abi)
  target_compile_definitions(${ecoro_current_target}
    PUBLIC
      SYMMETRIC_TRANSFER
      ECORO_HACK_NOINLINE=
  )
elseif (MSVC)
  target_compile_definitions(${ecoro_current_target}
    PUBLIC
      SYMMETRIC_TRANSFER
      ECORO_WORKAROUND_MSVC_FREE_CO_AWAIT_CONCEPT
      $<$<CONFIG:Debug>:ECORO_HACK_NOINLINE=>
      $<$<CONFIG:RelWithDebInfo>:ECORO_HACK_NOINLINE=ECORO_NOINLINE>
      $<$<CONFIG:Release>:ECORO_HACK_NOINLINE=ECORO_NOINLINE>
      $<$<CONFIG:MinSizeRel>:ECORO_HACK_NOINLINE=>
  )
endif()
