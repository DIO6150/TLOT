#pragma once

#include <iostream>

#include <glad/glad.h>

inline void GLAPIENTRY openglDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	__attribute__((unused)) GLsizei length,
	const GLchar* message,
	__attribute__((unused)) const void* userParam) {
	//if (type != GL_DEBUG_TYPE_ERROR) return;

	std::cerr << "=== OpenGL Debug Message ===\n";
	std::cerr << "Message: " << message << "\n";



	std::cerr << "Type: ";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: std::cerr << "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Deprecated Behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Undefined Behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Portability Issue"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Performance Issue"; break;
		case GL_DEBUG_TYPE_MARKER: std::cerr << "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: std::cerr << "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: std::cerr << "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER: std::cerr << "Other"; break;
	}


	std::cerr << "\nSeverity: ";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: std::cerr << "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW: std::cerr << "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Notification"; break;
	}


	std::cerr << "\nSource: ";
	switch (source) {
		case GL_DEBUG_SOURCE_API: std::cerr << "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: std::cerr << "Other"; break;
	}


	std::cerr << "\nID: " << id << "\n\n";
}

// À appeler après l'initialisation du contexte OpenGL
inline void enableOpenGLDebugCallback () {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglDebugCallback, nullptr);	
	//glDebugMessageControl(
	//	GL_DONT_CARE,
	//	GL_DONT_CARE,
	//	GL_DONT_CARE,
	//	0,
	//	nullptr,
	//	GL_TRUE
	//); // Active tout les types de messages
}
