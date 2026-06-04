find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.c
        Src/*.hpp
        Src/*.h
)

add_library(Cheats SHARED ${SRC_FILES})

set_target_properties(Cheats PROPERTIES PREFIX "" OUTPUT_NAME Cheats)

target_include_directories(Cheats PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CR_INCLUDE_DIRS}
)

target_link_libraries(Cheats PRIVATE ZKntSdkInterface imgui::imgui)
