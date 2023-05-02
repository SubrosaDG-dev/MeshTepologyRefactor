cmake_minimum_required(VERSION 3.25)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(mesh_tepology_refactor LANGUAGES CXX)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_COLOR_DIAGNOSTICS ON)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

cmake_host_system_information(RESULT NUMBER_OF_PHYSICAL_CORES QUERY NUMBER_OF_PHYSICAL_CORES)
message(STATUS "Number of physical cores: ${NUMBER_OF_PHYSICAL_CORES}")

set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "cmake --verbose on" FORCE)

find_program(LLD NAMES ld.lld)
if(LLD)
    set(CMAKE_LINKER "${LLD}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
    message_tool_version(lld "${LLD}")
endif()

find_program(CLANG_TIDY NAMES clang-tidy)
if(CLANG_TIDY)
    set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
    message(STATUS "Enable clang-tidy in ${CLANG_TIDY}")
endif()

find_program(CLANG_FORMAT NAMES clang-format)
if(CLANG_FORMAT)
    include(${PROJECT_SOURCE_DIR}/cmake/clang-format.cmake)
    message_tool_version(clang-format "${CLANG_FORMAT}")
endif()

find_program(CLANGD NAMES clangd)
if(CLANGD)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    message_tool_version(clangd "${CLANGD}")
endif()

find_program(IWYU NAMES include-what-you-use iwyu)
if(IWYU)
    set(CMAKE_C_INCLUDE_WHAT_YOU_USE "${IWYU}")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU}")
    message_tool_version(include-what-you-use "${IWYU}")
endif()
find_program(IWYU_TOOL NAMES iwyu_tool.py HINTS ${PROJECT_SOURCE_DIR}/utils/include-what-you-use)
find_program(IWYU_FIX_TOOL NAMES fix_includes.py HINTS ${PROJECT_SOURCE_DIR}/utils/include-what-you-use)
if(IWYU_TOOL AND IWYU_FIX_TOOL)
    include(${PROJECT_SOURCE_DIR}/cmake/iwyu.cmake)
    list(APPEND CMAKE_CXX_INCLUDE_WHAT_YOU_USE "-Xiwyu"
        "--mapping_file=${CMAKE_CURRENT_LIST_DIR}/.iwyu.imp")
endif()

find_program(CCACHE NAMES ccache)
if(CCACHE)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE}")
    message_tool_version(ccache "${CCACHE}")
endif()

set(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/include")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(MESH_TEPOLOGY_REFACTOR_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE MESH_TEPOLOGY_REFACTOR_SOURCES CONFIGURE_DEPENDS
    ${MESH_TEPOLOGY_REFACTOR_SOURCES_DIR}/*.cpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/example/main.cpp
    ${MESH_TEPOLOGY_REFACTOR_SOURCES}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORT)
    if(IPO_SUPPORT)
        message(STATUS "Enable interprocedural optimization")
        set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()

set(MESH_TEPOLOGY_REFACTOR_CXX_FLAGS)
set(MESH_TEPOLOGY_REFACTOR_CXX_WARNING_FLAGS)
set(MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_DEBUG)
set(MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_RELEASE)
set(MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_RELWITHDEBINFO)

find_library(LIBCXX NAMES libc++.so)
if(LIBCXX)
    string(APPEND MESH_TEPOLOGY_REFACTOR_CXX_FLAGS "-stdlib=libc++ ")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-stdlib=libc++")
endif()

string(APPEND MESH_TEPOLOGY_REFACTOR_CXX_FLAGS "-mfma -Wno-narrowing")
string(APPEND MESH_TEPOLOGY_REFACTOR_CXX_WARNING_FLAGS "-Wall -Wshadow -Wextra -Wconversion -Wdocumentation")
string(APPEND MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_DEBUG "${MESH_TEPOLOGY_REFACTOR_CXX_FLAGS} ${MESH_TEPOLOGY_REFACTOR_CXX_WARNING_FLAGS} -O0 -g3")
string(APPEND MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_RELEASE "${MESH_TEPOLOGY_REFACTOR_CXX_FLAGS} -O3 -DNDEBUG -march=native")

set(MESH_TEPOLOGY_REFACTOR_FLAGS "$<$<CONFIG:Debug>:${MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_DEBUG}> \
    $<$<CONFIG:Release>:${MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_RELEASE}> \
    $<$<CONFIG:RelWithDebInfo>:${MESH_TEPOLOGY_REFACTOR_CXX_FLAGS_RELWITHDEBINFO}>")
string(REPLACE " " ";" MESH_TEPOLOGY_REFACTOR_FLAGS ${MESH_TEPOLOGY_REFACTOR_FLAGS})
target_compile_options(${PROJECT_NAME} PUBLIC ${MESH_TEPOLOGY_REFACTOR_FLAGS})

configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake.h.in ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/cmake.h @ONLY)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_INCLUDE_OUTPUT_DIRECTORY})

find_path(TQDM_DIR NAMES tqdm.hpp PATHS ${PROJECT_SOURCE_DIR}/libs/tqdm-cpp)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${TQDM_DIR})

find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)

find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
