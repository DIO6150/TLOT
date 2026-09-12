cmake_minimum_required(VERSION 4.3.3)

project(TetoEngine LANGUAGES C CXX)
add_library(${PROJECT_NAME} STATIC)
target_compile_features(${PROJECT_NAME}	PUBLIC cxx_std_26)

#set(CMAKE_CXX_STANDARD 26)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_sources(${PROJECT_NAME}
	PRIVATE
		# ENGINE
		${CMAKE_CURRENT_SOURCE_DIR}/src/AssetManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/GeometryBuffer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/GeometryLocation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/GlyphAtlas.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/IndexLocation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/InputManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/MaterialInstance.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/RenderableObject.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/RenderContext.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/SceneInspector.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderProgram.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderSource.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/SSBO.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/TextureAtlas.cpp

		# VENDORED
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/stb.c
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c

		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui.cpp

		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<INSTALL_INTERFACE:include/imgui>
		
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends>
)


# EXTERNAL DEPENDENCY
find_package(Freetype REQUIRED)
find_package(glm      REQUIRED)
find_package(glfw3    REQUIRED)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Freetype::Freetype
		glm::glm
		glfw

		stdc++exp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY
		${CMAKE_BINARY_DIR}

	LIBRARY_OUTPUT_DIRECTORY
		${CMAKE_BINARY_DIR}

	ARCHIVE_OUTPUT_DIRECTORY
		${CMAKE_BINARY_DIR}
)


# INSALL LIBRARY
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
	TARGETS TetoEngine
    EXPORT TetoEngineTargets

    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# INSTALL PUBLIC HEADERS
install(
	DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/include/

	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)

# temporary solution to bad library architecture
# supposedly none of those vendored libs are exposed to the user
# but right now im trying to make the cmake build work, not cleanup the code
# that would be for later
install(
	DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include/glad

	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include/KHR

	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb

	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui

	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}
)

# INSTALL CMAKE TARGET EXPORT
install(
    EXPORT TetoEngineTargets
	FILE TetoEngineTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TetoEngine
    NAMESPACE TetoEngine::
)

# GENREATE CMAKE PROJECT CONFIG
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/TetoEngineConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/TetoEngineConfig.cmake

	INSTALL_DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/TetoEngine
)

# TODO : generate version config file

# INSTALL CMAKE PROJECT CONFIG
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/TetoEngineConfig.cmake

	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/TetoEngine
)
	
#include(${CMAKE_CURRENT_LIST_DIR}/TetoEngineTargets.cmake)