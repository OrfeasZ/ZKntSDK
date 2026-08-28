find_path(CR_INCLUDE_DIRS "cr.h")
find_package(directx-headers CONFIG REQUIRED)

file(GLOB_RECURSE PROXY_SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.hpp
        Src/*.h
)

add_library(DirectInputProxy SHARED
        ${PROXY_SRC_FILES}
        Src/exports.def
)

set_target_properties(DirectInputProxy PROPERTIES
        PREFIX ""
        OUTPUT_NAME dinput8
)

target_include_directories(DirectInputProxy PRIVATE
        ${CR_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CMAKE_SOURCE_DIR}/ZKntSdk/Include
)

target_include_directories(DirectInputProxy PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(DirectInputProxy PRIVATE
        minhook::minhook
        Microsoft::DirectX-Headers
        Microsoft::DirectX-Guids
        d3d12
        dxgi
)
