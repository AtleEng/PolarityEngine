#include "polpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Polarity
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TexScale;

		//Editor
		int EntityID;
	};
	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		//Editor
		int EntityID;
	};

	struct Renderer2DStorage
	{
		//Maximum amount of quads for a single Draw call
		const uint32_t MaxQuads = 20000;
		//Maximum amount of vertices for a single Draw call
		const uint32_t MaxVertices = MaxQuads * 4;
		//Maximum amount of indices for a single Draw call
		const uint32_t MaxIndices = MaxQuads * 6;
		//Maximum amount of diffrent textures per Draw call
		static const uint32_t MaxTextureSlots = 32; //TODO RenderCaps

		// Vertieces

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		// buffers

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
		
		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		// Index of the texture slot array starts on 1 (0 = blank texture)
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];


		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DStorage s_Data;


	void Renderer2D::Init()
	{
		POL_PROFILE_FUNCTION();

		/// Quad
		
		// Vertex Array
		s_Data.QuadVertexArray = VertexArray::Create();

		// Vertex Buffer
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"	   },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float,  "a_TexIndex" },
			{ ShaderDataType::Float,  "a_TexScale" },
			{ ShaderDataType::Int,    "a_EntityID" }
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

		/// Line
		
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		/// Shaders

		s_Data.TextureShader = Shader::Create("assets/shaders/2D_Quad.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/2D_Line.glsl");
		
		/// Textures
		
		// Generate whiteTexture
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t textureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&textureData, sizeof(uint32_t));

		// Texture
		int32_t samplers[s_Data.MaxTextureSlots];
		for (int32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;


		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DStorage::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		POL_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		POL_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		POL_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		StartBatch();
	}
	/*
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		POL_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		StartBatch();
	}
	*/

	void Renderer2D::EndScene()
	{
		POL_PROFILE_FUNCTION();

		Flush();
		//StartBatch(); // om problem med clean up dyker upp, ta bort efter mass render test
	}

	//reset batch
	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.LineIndexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	//Draw batch
	void Renderer2D::Flush()
	{
		POL_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount)
		{
			if (s_Data.QuadIndexCount == 0)
				return;

			uint32_t dataSize =
				(uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr -
					(uint8_t*)s_Data.QuadVertexBufferBase);

			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.TextureShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

			s_Data.Stats.DrawCalls++;
		}
		if (s_Data.LineIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	//Render Quads
	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::mat4& transform, const glm::vec4& tint, const float textureScale, const glm::vec2* texCoords)
	{
		POL_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;

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
			if (s_Data.TextureSlotIndex >= Renderer2DStorage::MaxTextureSlots)
				NextBatch();

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

	// Texture
	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::mat4& transform, const glm::vec4& tint, const float textureScale)
	{
		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		DrawQuad(texture, transform, tint, textureScale, texCoords);
	}

	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& tint, const float textureScale)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		else
		{
			transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		DrawQuad(texture, transform, tint, textureScale);
	}

	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& tint, const float textureScale)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform =
				glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) *
				glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		else
		{
			transform =
				glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		DrawQuad(texture, transform, tint, textureScale);
	}

	// SubTexture
	void Renderer2D::DrawQuad(const Ref<SubTexture2D>& subTexture, const glm::mat4& transform, const glm::vec4& tint, const float textureScale)
	{
		DrawQuad(subTexture->GetTexture(), transform, tint, textureScale, subTexture->GetTexCoords());
	}

	void Renderer2D::DrawQuad(const Ref<SubTexture2D>& subTexture, const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& tint, const float textureScale)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		else
		{
			transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		DrawQuad(subTexture, transform, tint, textureScale);
	}
	
	void Renderer2D::DrawQuad(const Ref<SubTexture2D>& subTexture, const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& tint, const float textureScale)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform =
				glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) *
				glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		else
		{
			transform =
				glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		DrawQuad(subTexture, transform, tint, textureScale);
	}

	// Flat Color
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& tint)
	{
		POL_PROFILE_FUNCTION();

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
			transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		else
		{
			transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		DrawQuad(transform, color);
	}
	
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& tint)
	{
		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			transform =
				glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) *
				glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		else
		{
			transform =
				glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) *
				glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		}
		DrawQuad(transform, tint);
	}
	void Renderer2D::DrawQuadID(const Ref<Texture2D>& texture, const glm::mat4& transform, const glm::vec4& tint, const float textureScale, int entityID)
	{
		POL_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			NextBatch();

		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = 0.0f;

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
			if (s_Data.TextureSlotIndex >= Renderer2DStorage::MaxTextureSlots)
				NextBatch();

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
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawQuadID(const glm::mat4& transform, const glm::vec4& tint, int entityID)
	{
		POL_PROFILE_FUNCTION();

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
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteComponent& sc, int entityID)
	{
		if (sc.Texture)
			DrawQuadID(sc.Texture, transform, sc.Color, sc.Scale, entityID);
		else
			DrawQuadID(transform, sc.Color, entityID);
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineIndexCount += 2;
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