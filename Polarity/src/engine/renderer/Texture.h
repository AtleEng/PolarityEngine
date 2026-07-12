#pragma once
#include "engine/Asset/Asset.h"
#include "engine/core/Buffer.h"
#include "engine/core/Core.h"

#include <string>
#include <glm/glm.hpp>


namespace Polarity
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(Buffer data) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());

		static AssetType GetStaticType() { return AssetType::Texture2D; }
		virtual AssetType GetType() const { return GetStaticType(); }
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