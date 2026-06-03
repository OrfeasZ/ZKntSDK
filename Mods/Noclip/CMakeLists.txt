find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.c
        Src/*.hpp
        Src/*.h
)

add_library(Noclip SHARED ${SRC_FILES})

set_target_properties(Noclip PROPERTIES PREFIX "" OUTPUT_NAME Noclip)

target_include_directories(Noclip PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CR_INCLUDE_DIRS}
)

target_link_libraries(Noclip PRIVATE ZKntSdkInterface imgui::imgui)
