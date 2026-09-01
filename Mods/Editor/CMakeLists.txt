find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.c
        Src/*.hpp
        Src/*.h
)

add_library(Editor SHARED ${SRC_FILES})

set_target_properties(Editor PROPERTIES PREFIX "" OUTPUT_NAME Editor)

find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")
find_path(USOCKETS_LIB "uSockets.lib")

set(USOCKETS_SEARCH_PATH "${VCPKG_BUILD_ROOT}/lib")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(USOCKETS_SEARCH_PATH "${VCPKG_BUILD_ROOT}/debug/lib")
endif ()

find_library(USOCKETS_LIB
        NAMES uSockets
        PATHS "${USOCKETS_SEARCH_PATH}"
        NO_DEFAULT_PATH
        REQUIRED
)

target_include_directories(Editor PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CR_INCLUDE_DIRS}
        ${UWEBSOCKETS_INCLUDE_DIRS}
)

target_link_libraries(Editor PRIVATE
        ZKntSdkInterface
        imgui::imgui
        imguizmo::imguizmo
        implot::implot
        ResourceLib_KNT
        simdjson::simdjson
        Microsoft::DirectXTex
        ws2_32
        ZLIB::ZLIB
        $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
        "${USOCKETS_LIB}"
)

target_compile_definitions(Editor PRIVATE ZKNT_HAS_IMPLOT)
