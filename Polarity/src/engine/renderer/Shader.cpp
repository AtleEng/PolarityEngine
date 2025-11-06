#include "polpch.h"
#include "Shader.h"

#include <glad/glad.h>

//https://wikis.khronos.org/opengl/Shader_Compilation#Shader_object_compilation

namespace Polarity {
	Polarity::Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		LOG_INFO("Creating shader...");

		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		LOG_INFO("  Compile Vertex shader: %s", vertexSrc);
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

			LOG_ERROR("  Vertex shader compilation failure !!!");
			LOG_MAJOR_ERROR("    %s", (const char)infoLog.data());

			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		LOG_INFO("  Compile fragment shader: %s", fragmentSrc);
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

			LOG_ERROR("  Fragment shader compilation failure !!!");
			LOG_MAJOR_ERROR("    %s", (const char)infoLog.data());

			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		m_rendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_rendererID, vertexShader);
		glAttachShader(m_rendererID, fragmentShader);

		LOG_INFO("  Linking shader...");
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

			// We don't need the program anymore.
			glDeleteProgram(m_rendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			LOG_ERROR("  Shader link failure !!!");
			LOG_MAJOR_ERROR("    %s", (const char)infoLog.data());

			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_rendererID, vertexShader);
		glDetachShader(m_rendererID, fragmentShader);

		LOG_INFO("Shader done\n");
	}

	Polarity::Shader::~Shader()
	{
		glDeleteProgram(m_rendererID);
	}

	void Polarity::Shader::Bind()
	{
		glUseProgram(m_rendererID);
	}

	void Polarity::Shader::Unbind()
	{
		glUseProgram(0);
	}
}
