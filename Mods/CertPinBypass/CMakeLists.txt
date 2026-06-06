find_path(CR_INCLUDE_DIRS "cr.h")

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	Src/*.cpp
	Src/*.c
	Src/*.hpp
	Src/*.h
)

add_library(CertPinBypass SHARED ${SRC_FILES})

target_include_directories(CertPinBypass PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${CR_INCLUDE_DIRS}
)

target_link_libraries(CertPinBypass PRIVATE ZKntSdkInterface imgui::imgui)
