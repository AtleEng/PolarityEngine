#pragma once

#include "engine/renderer/Shader.h"
#include <glm/glm.hpp>

typedef unsigned int GLenum; //temp

namespace Polarity
{
	class OpenGLShader: public Shader
	{
	public:
		OpenGLShader(const std::string& shaderPath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		virtual void Bind()		const override;
		virtual void UnBind()	const override;

		virtual const std::string& GetName() const override { return m_name; }

		// ----- Upload Uniforms -----
		void UploadUniformInt		(const std::string& name, const float value);

		void UploadUniformFloat		(const std::string& name, const float value);
		void UploadUniformFloat2	(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3	(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4	(const std::string& name, const glm::vec4& value);

		void UploadUniformMat4		(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_rendererID;
		std::string m_name;
	};
}
