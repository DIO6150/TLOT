cmake_minimum_required(VERSION 4.3.3)

project(TetoEngine LANGUAGES CXX)

#set(CXX_STANDARD 26)
#set(CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_26)

target_sources(${PROJECT_NAME} PRIVATE
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
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


# Vendored libs:
# stb_image, glad, imgui
target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/stb.c
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
)

# Installed libs:
# Freetype, glm, glfw3
find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype)

find_package(glm REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


# installation
include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT TetoEngineTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT TetoEngineTargets
    FILE TetoEngineTargets.cmake
    NAMESPACE TetoEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TetoEngine
)
