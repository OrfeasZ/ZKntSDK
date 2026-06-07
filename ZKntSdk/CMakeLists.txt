find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.c
        Src/*.hpp
        Src/*.h
)

file(GLOB_RECURSE HEADER_FILES
        CONFIGURE_DEPENDS
        Include/*.h
        Include/*.hpp
)

add_library(ZKntSdk SHARED
        ${SRC_FILES}
        ${HEADER_FILES}
)

target_compile_definitions(ZKntSdk PRIVATE
        LOADER_EXPORTS
)

target_compile_options(ZKntSdk PRIVATE -Wno-microsoft-cast)

set_target_properties(ZKntSdk PROPERTIES
        PREFIX ""
        OUTPUT_NAME ZKntSdk
)

target_include_directories(ZKntSdk PRIVATE
        ${CR_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CMAKE_SOURCE_DIR}/DirectInputProxy/Include
)

target_include_directories(ZKntSdk PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_SOURCE_DIR}/External/IconFontCppHeaders
)

target_link_libraries(ZKntSdk PRIVATE
        minhook::minhook
        spdlog::spdlog_header_only
        imgui::imgui
        Microsoft::DirectX-Headers
        Microsoft::DirectX-Guids
        d3d12
        dxgi
        d3dcompiler
)

# Header-only interface exposed to mods.
add_library(ZKntSdkInterface INTERFACE)
target_include_directories(ZKntSdkInterface INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CR_INCLUDE_DIRS}
)
target_link_libraries(ZKntSdkInterface INTERFACE
        spdlog::spdlog_header_only
)
target_compile_options(ZKntSdkInterface INTERFACE -Wno-microsoft-cast)

add_dependencies(ZKntSdk DirectInputProxy)

# Install icon header.
install(FILES "${CMAKE_SOURCE_DIR}/External/IconFontCppHeaders/IconsMaterialDesign.h" DESTINATION include)
