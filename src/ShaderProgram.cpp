
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include <Core/Logger.hpp>

#include <OpenGL/ShaderProgram.hpp>

#include <glm/glm.hpp>

using namespace TLOT;

ShaderProgram::ShaderProgram ()
{
	
}

void ShaderProgram::CompileVertex (std::string source)
{
	if (source == "") return;

	if (m_vertexShader != 0)
	{
		Logger::log (LogLevel::Info, "Recompiling Vertex Shader : {}", m_program);
		glDeleteShader (m_vertexShader);
	}

	m_vertexShader   = glCreateShader (GL_VERTEX_SHADER);

	char const * vertexCode = source.c_str ();
	glShaderSource (m_vertexShader, 1, &vertexCode, NULL);
	glCompileShader (m_vertexShader);

	int status;
	char infoLog[512];
	glGetShaderiv (m_vertexShader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		glGetShaderInfoLog(m_vertexShader, 512, NULL, infoLog);
		Logger::log (LogLevel::Error, "Vertex Shader Compilation : {}", infoLog);
	}
}

void ShaderProgram::CompileFragment (std::string source)
{
	if (source == "") return;

	if (m_fragmentShader != 0)
	{
		Logger::log (LogLevel::Info, "Recompiling Fragment Shader : {}", m_program);
		glDeleteShader (m_fragmentShader);
	}

	m_fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);

	char const * fragmentCode = source.c_str ();
	glShaderSource (m_fragmentShader, 1, &fragmentCode, NULL);
	glCompileShader (m_fragmentShader);

	int status;
	char infoLog[512];
	glGetShaderiv (m_fragmentShader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		glGetShaderInfoLog(m_fragmentShader, 512, NULL, infoLog);
		Logger::log (LogLevel::Error, "Fragment Shader Compilation : {}", infoLog);
	}
}

void ShaderProgram::Link ()
{
	if (m_program)
	{
		glDeleteProgram (m_program);
		Logger::log (LogLevel::Info, "Recompiling Shader Program : {}", m_program);
	}

	m_program = glCreateProgram ();
	
	glAttachShader (m_program, m_vertexShader);
	glAttachShader (m_program, m_fragmentShader);
	glLinkProgram (m_program);
	
	int status;
	char infoLog[512];
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if(!status)
	{
		glGetProgramInfoLog(m_program, 512, NULL, infoLog);
		std::cout << "ShaderProgram Linkage Error : " << infoLog << "\n";
		m_program = 0;
	}
}

void ShaderProgram::Use () const {
	glUseProgram (m_program);
}

void ShaderProgram::Cancel () const {
	glUseProgram (0);
}

// TODO-add : create a cache for uniform locations
void ShaderProgram::UploadMatrix4 (std::string key, const glm::mat4& matrix) const {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniformMatrix4fv (location, 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::Upload1i (std::string key, int i) const {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniform1i (location, i);
}

void ShaderProgram::Upload3fv (std::string key, glm::vec3 vec) const {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniform3fv (location, 1, &vec[0]);
}

void ShaderProgram::Upload1f (std::string key, float f) const {
    int location;

    location = glGetUniformLocation (m_program, key.c_str ());
    glUniform3fv (location, 1, &f);
}
