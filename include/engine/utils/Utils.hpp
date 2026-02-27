#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <deque>
#include <stdint.h>
#include <filesystem>

#if _WIN32
#include <windows.h>
#endif

// Fonction de callback, merci ChatGPT j'avais la grosse flemme de donner un équivalent str aux enums
inline void GLAPIENTRY openglDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	__attribute__((unused)) GLsizei length,
	const GLchar* message,
	__attribute__((unused)) const void* userParam) {
	if (type != GL_DEBUG_TYPE_ERROR) return;

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


// TODO: uhhh le hash en question
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

inline std::filesystem::path getExecutableDir() {
#ifdef _WIN32
    wchar_t path[1024];
    GetModuleFileNameW(nullptr, path, 1024);
    return std::filesystem::path(path).parent_path();
#else
    return std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
}

inline bool load_texture (const std::string & url, unsigned char *& data, size_t & width, size_t & height) {
	int nb_channels;
	int _width, _height;

	data = stbi_load(url.c_str(), &_width, &_height, &nb_channels, 4);

	if (!data) {
		printf ("%s(%s) : (%s) %s\n", __func__, __FILE__, url.c_str (), stbi_failure_reason ());
		return (false);
	}

	width  = _width;
	height = _height;
	
	return (true);
}

inline void printMat4(const glm::mat4& m, const std::string& name = "Matrix") {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "  [ ";
        for (int j = 0; j < 4; j++) {
            // Note : GLM utilise un stockage en colonnes (column-major)
            // m[j][i] permet d'afficher la matrice de façon "visuelle" (ligne par ligne)
            std::cout << std::fixed << std::setprecision(3) << std::setw(8) << m[j][i] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

#include <vector>

template<class T>
inline void printVector(const std::vector<T> & vec, const std::string & name = "Vector") {
	std::cout << name << " : [";
	for (const auto & e : vec) {
		std::cout << e << ",\n";
	}
	std::cout << "]\n";
}