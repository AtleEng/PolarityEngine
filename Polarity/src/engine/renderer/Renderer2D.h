#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace Polarity
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void OnWindowResize(uint32_t width, uint32_t height);

		//Draw quads

		static void DrawQuad(const glm::mat4& transform,	const Ref<Texture2D>& texture,	const glm::vec4& tint = glm::vec4(1.0f), const float textureScale = 1.0f);
		static void DrawQuad(const Ref<Texture2D>& texture, const glm::vec3& position,		const glm::vec2& size = glm::vec2(1.0f), const float rotation = 0, const glm::vec4& tint = glm::vec4(1.0f), const float textureScale = 1.0f);
		static void DrawQuad(const Ref<Texture2D>& texture, const glm::vec2& position,		const glm::vec2& size = glm::vec2(1.0f), const float rotation = 0, const glm::vec4& tint = glm::vec4(1.0f), const float textureScale = 1.0f);

		static void DrawQuad(const glm::mat4& transform,	const glm::vec4& tint = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position,		const glm::vec2& size = glm::vec2(1.0f), const float rotation = 0, const glm::vec4& tint = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position,		const glm::vec2& size = glm::vec2(1.0f), const float rotation = 0, const glm::vec4& tint = glm::vec4(1.0f));
	private:
		static void StartBatch();
		static void NextBatch();
	};
}

