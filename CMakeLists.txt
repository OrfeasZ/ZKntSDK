cmake_minimum_required(VERSION 3.21)

project(ZKntSdk VERSION 0.1.0 LANGUAGES CXX)

set(GAME_INSTALL_PATH "" CACHE PATH "Path to 007 First Light installation folder.")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(minhook CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(directx-headers CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_subdirectory(DirectInputProxy)
add_subdirectory(ZKntSdk)

set(MODS
        NoPause
        FreeCam
        #SkipIntro
        Cheats
)

foreach (MOD IN LISTS MODS)
  add_subdirectory("Mods/${MOD}")
endforeach ()

# Install ZKntSdk, dinput8, and all the mods in the game folder.
if (EXISTS "${GAME_INSTALL_PATH}/Retail")
  install(FILES "$<TARGET_FILE:DirectInputProxy>" DESTINATION "${GAME_INSTALL_PATH}/Retail")
  install(FILES "$<TARGET_FILE:ZKntSdk>" DESTINATION "${GAME_INSTALL_PATH}/Retail")

  foreach (MOD IN LISTS MODS)
    install(FILES "$<TARGET_FILE:${MOD}>" DESTINATION "${GAME_INSTALL_PATH}/Retail/mods")
  endforeach ()
endif ()

# Packageable install layout (always emitted to ${CMAKE_INSTALL_PREFIX}).
# Mirrors the in-game folder structure so the staged tree can be zipped and
# extracted directly into the game's Retail/ directory.
install(FILES "$<TARGET_FILE:DirectInputProxy>" DESTINATION "Retail")
install(FILES "$<TARGET_PDB_FILE:DirectInputProxy>" DESTINATION "Retail" OPTIONAL)
install(FILES "$<TARGET_FILE:ZKntSdk>" DESTINATION "Retail")
install(FILES "$<TARGET_PDB_FILE:ZKntSdk>" DESTINATION "Retail" OPTIONAL)

foreach (MOD IN LISTS MODS)
  install(FILES "$<TARGET_FILE:${MOD}>" DESTINATION "Retail/mods")
  install(FILES "$<TARGET_PDB_FILE:${MOD}>" DESTINATION "Retail/mods" OPTIONAL)
endforeach ()

if (EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
  install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION ".")
endif ()

# Collect all licenses from the vcpkg-installed share tree into licenses/<lib>.txt
set(_ZKNT_VCPKG_SHARE "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share")
install(CODE "set(VCPKG_BUILD_ROOT \"${_ZKNT_VCPKG_SHARE}/..\")")
install(CODE "set(INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")")
install(CODE [[
	message(STATUS "Collecting licenses from ${VCPKG_BUILD_ROOT}/share/")
	file(GLOB_RECURSE LICENSES "${VCPKG_BUILD_ROOT}/share/*/copyright")

	foreach(LICENSE IN LISTS LICENSES)
		# Get the name of the library (containing folder).
		get_filename_component(LICENSE_DIR "${LICENSE}" DIRECTORY)
		get_filename_component(LICENSE_DIR "${LICENSE_DIR}" NAME)

		message(STATUS "Installing ${LICENSE} as licenses/${LICENSE_DIR}.txt")
		file(COPY "${LICENSE}" DESTINATION "${INSTALL_DIR}/licenses")
		file(RENAME "${INSTALL_DIR}/licenses/copyright" "${INSTALL_DIR}/licenses/${LICENSE_DIR}.txt")
	endforeach()
]])

# Install licenses from external projects.
install(FILES "External/IconFontCppHeaders/licence.txt" DESTINATION licenses RENAME IconFontCppHeaders.txt)
