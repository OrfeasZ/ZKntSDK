find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        Src/*.cpp
        Src/*.c
        Src/*.hpp
        Src/*.h
)

add_library(FreeCam SHARED ${SRC_FILES})

set_target_properties(FreeCam PROPERTIES PREFIX "" OUTPUT_NAME FreeCam)

target_include_directories(FreeCam PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CR_INCLUDE_DIRS}
)

target_link_libraries(FreeCam PRIVATE ZKntSdkInterface imgui::imgui)
