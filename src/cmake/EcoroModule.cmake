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

set(project_prefix ecoro)
set(targets_export_name ${project_prefix}-targets)

function(ecoro_module name)
  set(target ${project_prefix}-${name})
  set(alias ${project_prefix}::${name})

  add_library(${target})

  target_include_directories(
    ${target}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${name})
  add_library(${alias} ALIAS ${target})

  install(TARGETS ${target} EXPORT ${targets_export_name})
  install(DIRECTORY include/${project_prefix} TYPE INCLUDE)

  set(${project_prefix}_current_target ${target} PARENT_SCOPE)
endfunction()
