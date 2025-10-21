#pragma once

#include "glad/glad.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <stdint.h>

// Fonction de callback, merci ChatGPT j'avais la grosse flemme de donner un équivalent str aux enums
inline void GLAPIENTRY openglDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	__attribute__((unused)) GLsizei length,
	const GLchar* message,
	__attribute__((unused)) const void* userParam) {
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
	glDebugMessageControl(
		GL_DONT_CARE,
		GL_DONT_CARE,
		GL_DONT_CARE,
		0,
		nullptr,
		GL_TRUE
	); // Active tout les types de messages
}

inline unsigned int getGLType (std::string e_type)
{
	if (e_type == "float") return (GL_FLOAT);
	if (e_type == "int") return (GL_INT);
	if (e_type == "uint") return (GL_UNSIGNED_INT);
	else return (GL_UNSIGNED_BYTE);
}

inline size_t getGLTypeSize (unsigned int e_type)
{
	if (e_type == GL_FLOAT) return (sizeof (float));
	if (e_type == GL_INT) return (sizeof (int));
	if (e_type == GL_UNSIGNED_INT) return (sizeof (int));
	else return (1);
}

inline std::string ReadFile (std::string path)
{
	std::ifstream file;
	file.open (path);
	if (!file.is_open ()) {
		file.close ();
		std::cout << "Couldn't open file : " << path << "\n";
		// TODO : LOGGING
		return ("");
	}

	std::stringstream buffer;
	buffer << file.rdbuf ();

	file.close ();

	return (buffer.str ());
}


// TODO: delete function
inline uint64_t compute_hash(const std::string & s) {
	const int p = 102013;
	const int m = 1e9 + 9;
	uint64_t hash_value = 0;
	uint64_t p_pow = 1;

	for (char c : s) {
		hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}

	return (hash_value);
}


// TODO: Find a better hash function ? it seems :
//	1) very destructive
//	2) not very safe ?

template<typename ... args>
inline uint64_t multi_type_hash () {
	return (static_cast<uint64_t> (typeid (args).hash_code ()) ^ ...);
}