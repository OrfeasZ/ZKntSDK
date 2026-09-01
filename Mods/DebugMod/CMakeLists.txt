find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.c
        Src/*.hpp
        Src/*.h
)

add_library(DebugMod SHARED ${SRC_FILES})

set_target_properties(DebugMod PROPERTIES PREFIX "" OUTPUT_NAME DebugMod)

target_include_directories(DebugMod PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CR_INCLUDE_DIRS}
)

target_link_libraries(DebugMod PRIVATE ZKntSdkInterface imgui::imgui)
