#pragma once

#include <string>

namespace Polarity
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind()		const = 0;
		virtual void UnBind()	const = 0;

		static Ref<Shader> Create(const std::string& shaderPath);
		static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}