cmake_minimum_required(VERSION 3.13)

project(lemonc VERSION 1.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.h)
file(GLOB_RECURSE sources_test src/test/*.cpp)

find_path(LEMON_SOURCE_ROOT_DIR CMakeLists.txt
  PATHS ${CMAKE_SOURCE_DIR}/lemon
  NO_DEFAULT_PATH
  DOC "Location of LEMON source as a CMAKE subproject")

if(EXISTS ${LEMON_SOURCE_ROOT_DIR})
  add_subdirectory(${LEMON_SOURCE_ROOT_DIR} deps/lemon)
  set(LEMON_INCLUDE_DIRS
    ${LEMON_SOURCE_ROOT_DIR}
    ${CMAKE_BINARY_DIR}/deps/lemon
  )
  set(LEMON_LIBRARIES lemon)
  unset(LEMON_ROOT_DIR CACHE)
  unset(LEMON_DIR CACHE)
  unset(LEMON_INCLUDE_DIR CACHE)
  unset(LEMON_LIBRARY CACHE)
else()
  message(FATAL_ERROR "Lemon not found!")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LEMON_INCLUDE_DIRS}
)

add_library(lemonc SHARED ${sources})

target_compile_options(lemonc PUBLIC -std=c++14 -Wall)

if (WIN32)
  message("WINDOWS")
  target_link_options(lemonc PUBLIC -m64 -Wl,-allow-multiple-definition)
  target_link_libraries(lemonc PUBLIC ${LEMON_LIBRARIES} -static gcc stdc++ winpthread -dynamic)
else()
  message("MAC/LINUX")
  target_link_options(lemonc PUBLIC -m64)
  target_link_libraries(lemonc PUBLIC ${LEMON_LIBRARIES})
endif()

add_executable(lemonc-test src/main/types.h ${sources_test})

target_compile_options(lemonc-test PUBLIC -std=c++14 -Wall)

target_link_libraries(lemonc-test PUBLIC lemonc)
