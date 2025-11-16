#include "polpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

Polarity::OpenGLShader::OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource)
{
	LOG_INFO("Creating shaders...");
	// Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Send the vertex shader source code to GL
	const GLchar* source = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertexShader);

		LOG_MAJOR_ERROR("Vertex shader compilation failure !!!");

		return;
	}

	// Create an empty fragment shader handle
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	source = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		LOG_MAJOR_ERROR("Fragment shader compilation failure !!!");

		return;
	}

	m_rendererID = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(m_rendererID, vertexShader);
	glAttachShader(m_rendererID, fragmentShader);

	// Link our program
	glLinkProgram(m_rendererID);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(m_rendererID, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_rendererID, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program and shaders anymore.
		glDeleteProgram(m_rendererID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		LOG_MAJOR_ERROR("Shader link failure !!!");

		return;
	}

	glDetachShader(m_rendererID, vertexShader);
	glDetachShader(m_rendererID, fragmentShader);
}

Polarity::OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(m_rendererID);
}

void Polarity::OpenGLShader::Bind() const
{
	glUseProgram(m_rendererID);
}

void Polarity::OpenGLShader::UnBind() const
{
	glUseProgram(0);
}

void Polarity::OpenGLShader::UploadUniformInt(const std::string& name, const float value)
{
	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	glUniform1i(location, value);
}

void Polarity::OpenGLShader::UploadUniformFloat(const std::string& name, const float value)
{
	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	glUniform1f(location, value);
}

void Polarity::OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void Polarity::OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void Polarity::OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Polarity::OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
