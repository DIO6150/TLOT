#include "shader.hpp"

#include <regex>
#include <fstream>
#include <sstream>

#include "glad/glad.h"
#include "utils.hpp"

static bool CreateVertex (std::string path, unsigned int *object) {
	int status;
	char infoLog[512];

	std::string vertex;

	if ((vertex = ReadFile (path)) == "") return (false);

	std::cout << vertex << "\n";

	const char* vertex_code = vertex.c_str ();

	unsigned int vertex_id = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vertex_id, 1, &vertex_code, NULL);
	glCompileShader (vertex_id);

	glGetShaderiv (vertex_id, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_id, 512, NULL, infoLog);
		// TODO add logging
		std::cout << "VERTEX SHADER COMPILE ERROR : " << infoLog << "\n";
		return (false);
	}

	*object = vertex_id;

	return (true);
}


static bool CreateFragment (std::string path, unsigned int *object) {
	int status;
	char infoLog[512];

	std::string fragment;

	if ((fragment = ReadFile (path)) == "") return (false);

	const char* fragment_code = fragment.c_str ();

	unsigned int fragment_id = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fragment_id, 1, &fragment_code, NULL);
	glCompileShader (fragment_id);

	glGetShaderiv (fragment_id, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_id, 512, NULL, infoLog);
		// TODO add logging
		std::cout << "FRAGMENT SHADER COMPILE ERROR : " << infoLog << "\n";
		return (false);
	}

	*object = fragment_id;

	return (true);
}

Engine::Shader::Shader (std::initializer_list <std::string> paths) {
	std::regex rgx("<([^>]+)>");
	std::smatch match;

	unsigned int shader_object;

	unsigned int shader;
	shader = glCreateProgram ();

	int status;
	char infoLog[512];

	// TODO : save individual shader_object for hot reloading

	for (auto& e : paths)
	{
		if (std::regex_search(e, match, rgx)) {
			std::string path = std::regex_replace(e, rgx, "");
			std::cout << match [0] << " [" << match [1] << "]" << path <<"\n";
			if (match[1] == "vs") {
				if (!CreateVertex (path, &shader_object)) continue;
				std::cout << "VertexSha is ok !\n";
				glAttachShader (shader, shader_object);
				m_paths [VERTEX_SHADER] = path;
			}
			else if (match[1] == "fs") {
				if (!CreateFragment (path, &shader_object)) continue;
				std::cout << "FragmentSha is ok !\n";
				glAttachShader (shader, shader_object);
				m_paths [FRAGMENT_SHADER] = path;
			}
			else {
				continue;
			}
		}
	}

	glLinkProgram (shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	if(!status) {
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Shader Linkage Error : " << infoLog << "\n";
		return;
	}

	m_object = shader;
}

Engine::Shader::Shader () {

}

void Engine::Shader::Use () {
	glUseProgram (m_object);
}