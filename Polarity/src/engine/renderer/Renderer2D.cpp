#include "polpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#include "platform/openGL/OpenGLShader.h" //temp

namespace Polarity
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TexScale;
	};

	struct Renderer2DStorage
	{
		//Maximum amount of quads for a single Draw call
		const uint32_t MaxQuads = 10000;
		//Maximum amount of vertices for a single Draw call
		const uint32_t MaxVertices = MaxQuads * 4;
		//Maximum amount of indices for a single Draw call
		const uint32_t MaxIndices = MaxQuads * 6;
		//Maximum amount of diffrent textures per Draw call
		static const uint32_t MaxTextureSlots = 32; //TODO RenderCaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;

		Ref<Shader> TextureShader;
		Ref<Texture2D> whiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		// Index of the texture slot array starts on 1 (0 = blank texture)
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];


		Renderer2D::Statistics Stats;
	};

	static Renderer2DStorage s_Data;


	void Renderer2D::Init()
	{
		POLARITY_PROFILE_FUNCTION();

		// Vertex Array
		s_Data.QuadVertexArray = VertexArray::Create();

		// Vertex Buffer
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"	},
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TexScale" }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		// Index Buffer
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Generate whiteTexture
		s_Data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t textureData = 0xffffffff;
		s_Data.whiteTexture->SetData(&textureData, sizeof(uint32_t));

		// Texture
		int32_t samplers[s_Data.MaxTextureSlots];
		for (int32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}
		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.whiteTexture;


		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		POLARITY_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		POLARITY_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.whiteTexture->Bind();

		

		s_Data.TextureSlotIndex = 1;

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		POLARITY_PROFILE_FUNCTION();

		Flush();
	}

	//Draw batch
	void Renderer2D::Flush()
	{
		POLARITY_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		//Bind textures
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

		//Draw quadbatch
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
	{

	}

	// Texture
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tint, const float textureScale)
	{
		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t vertIdx = 0; vertIdx < 4; vertIdx++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[vertIdx];
			s_Data.QuadVertexBufferPtr->Color = tint;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[vertIdx];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TexScale = textureScale;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& tint, const float textureScale)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform = glm::translate(transform, position);
			transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0, 0, 1));
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		else
		{
			transform = glm::translate(transform, position);
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		DrawQuad(transform, texture, tint, textureScale);
	}

	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& tint, const float textureScale)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform = glm::translate(transform, { position.x, position.y, 0.0f });
			transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0, 0, 1));
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		else
		{
			transform = glm::translate(transform, { position.x, position.y, 0.0f });
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		DrawQuad(transform, texture, tint, textureScale);
	}

	// Flat Color
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& tint)
	{
		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			NextBatch();

		const float texIndex = 0.0f;
		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float texScale = 1.0f;

		for (size_t vertIdx = 0; vertIdx < 4; vertIdx++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[vertIdx];
			s_Data.QuadVertexBufferPtr->Color = tint;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[vertIdx];
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TexScale = texScale;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform = glm::translate(transform, position);
			transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0, 0, 1));
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		else
		{
			transform = glm::translate(transform, position);
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		DrawQuad(transform, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& tint)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform = glm::translate(transform, { position.x, position.y, 0.0f });
			transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0, 0, 1));
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		else
		{
			transform = glm::translate(transform, { position.x, position.y, 0.0f });
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		DrawQuad(transform, tint);
	}


	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}