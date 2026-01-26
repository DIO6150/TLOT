
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include <engine/shader.hpp>
#include <engine/utils.hpp>


static bool CreateVertex (std::string path, unsigned int *object) {
	int status;
	char infoLog[512];

	std::string vertex;

	if ((vertex = ReadFile (path)) == "") return (false);

	//std::cout << "Vertex code:\n" << vertex << "\n";

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

	//std::cout << "Fragment code:\n" << fragment << "\n";

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

Engine::Shader::Shader () {
	
}

Engine::Shader::Shader (const std::string & vertex_url, const std::string & frag_url) {
	createShader (vertex_url, ShaderType::VERTEX);
	createShader (frag_url, ShaderType::FRAGMENT);
	compile ();
}

void Engine::Shader::createShader (const std::string & url, const ShaderType & type) {
	unsigned int shader_object;

	bool result = true;

	switch (type) {
	case ShaderType::VERTEX :
		result = CreateVertex (url, & shader_object);
		break;

	case ShaderType::FRAGMENT :
		result = CreateFragment (url, & shader_object);
		break;
	
	default:
		break;
	}

	if (!result) {
		return;
	}

	m_shaders[type] = shader_object;
	m_urls[type] = url;
}

void Engine::Shader::compile () {
	unsigned int shader;
	shader = glCreateProgram ();

	int status;
	char infoLog[512];

	for (auto & [sha_type, sha] : m_shaders) {
		glAttachShader (shader, sha);
	}

	glLinkProgram (shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	if(!status) {
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Shader Linkage Error : " << infoLog << "\n";
		return;
	}

	m_program = shader;
}

void Engine::Shader::use () {
	glUseProgram (m_program);
}

void Engine::Shader::cancel () {
	glUseProgram (0);
}

// TODO-add : create a cache for uniform locations
void Engine::Shader::uploadMatrix4 (std::string key, glm::mat4& matrix) {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniformMatrix4fv (location, 1, GL_FALSE, &matrix[0][0]);
}

void Engine::Shader::upload1i (std::string key, int i) {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniform1i (location, i);
}

void Engine::Shader::upload3fv (std::string key, glm::vec3 vec) {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniform3fv (location, 1, &vec[0]);
}