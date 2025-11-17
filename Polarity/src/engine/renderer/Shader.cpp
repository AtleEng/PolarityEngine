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

	Ref<Shader> Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("RendererAPI: None is currently not supported !!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLShader>(vertexSrc, fragmentSrc);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI !!!");
		return nullptr;
	}
}