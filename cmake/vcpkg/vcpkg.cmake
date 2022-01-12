if (DEFINED VCPKG_ROOT)
  set(VCPKG_ROOTS ${VCPKG_ROOT})
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux|Darwin")
  set(VCPKG_ROOTS
    "/usr/local/share/vcpkg"
    "/opt/vcpkg/"
  )
else()
  set(VCPKG_ROOTS
    "C:/vcpkg"
  )
endif()

foreach(VCPKG_ROOT IN LISTS VCPKG_ROOTS)
  if (EXISTS ${VCPKG_ROOT})
    include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    return()
  endif()
endforeach()
