#pragma once

#include <string>
#include <glm/glm.hpp>

#include "engine/core/Core.h"

namespace Polarity
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
	};

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D> texture, const glm::vec2 coords, const glm::vec2 size);

		const Ref<Texture2D>& GetTexture()	const { return m_texture; }
		const glm::vec2*	 GetTexCoords()	const { return m_textureBounds; }

		static Ref<SubTexture2D> Create(const Ref<Texture2D> texture, const glm::vec2 coords, const glm::vec2 size);
		static Ref<SubTexture2D> CreateFromUniformGrid(const Ref<Texture2D> texture, const glm::vec2 indexCoords, float gridSize);
	private:
		const Ref<Texture2D> m_texture;
		glm::vec2			 m_textureBounds[4];
	};
}