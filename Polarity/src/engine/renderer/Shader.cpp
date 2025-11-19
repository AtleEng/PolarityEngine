#include "polpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLShader.h"

namespace Polarity {
	
	Ref<Shader> Shader::Create(const std::string& shaderPath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("RendererAPI: None is currently not supported !!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLShader>(shaderPath);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI !!!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("RendererAPI: None is currently not supported !!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI !!!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		LOG_ASSERT(!Exists(name), "Shader (%s) already exist !!!", name.c_str());
		m_shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	
	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		LOG_ASSERT(Exists(name), "Shader (%s) not found !!!", name.c_str());
		return m_shaders[name];
	}
	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_shaders.find(name) != m_shaders.end();
	}
}