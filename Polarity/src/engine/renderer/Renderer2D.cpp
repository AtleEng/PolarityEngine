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
	};

	struct Renderer2DStorage
	{
		//For a single Draw call
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;

		Ref<Shader> TextureShader;
		Ref<Texture2D> whiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
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
			{ ShaderDataType::Float2, "a_TexCoord" }
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
		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetInt("u_Texture", 0);
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

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{
		POLARITY_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		POLARITY_PROFILE_FUNCTION();
		
		
		
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}

	void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
	{

	}

	// Draw Quad

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(s_Data.whiteTexture, { position.x, position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(s_Data.whiteTexture, { position.x, position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const float textureScale)
	{
		DrawQuad(texture, { position.x, position.y, 0.0f }, size, color, textureScale);
	}
	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const float textureScale)
	{
		POLARITY_PROFILE_FUNCTION();

		{
			POLARITY_PROFILE_SCOPE("Set QuadVertexBufferPtr:s");

			s_Data.QuadVertexBufferPtr->Position = position;
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
			s_Data.QuadVertexBufferPtr++;

			s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
			s_Data.QuadVertexBufferPtr++;

			s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
			s_Data.QuadVertexBufferPtr++;

			s_Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
			s_Data.QuadVertexBufferPtr++;

			s_Data.QuadIndexCount += 6;
		}
		/*
		glm::mat4 transform(1.0f);
		{
			POLARITY_PROFILE_SCOPE("Transform calc (pos, size)");

			transform = glm::translate(transform, position);
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}

		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetFloat("u_TexScale", textureScale);
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		texture->Bind();

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
		*/
	}
	
	// Draw Rotated Quad
	
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(s_Data.whiteTexture, { position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(s_Data.whiteTexture, { position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const Ref<Texture2D>& texture, const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color, const float textureScale)
	{
		DrawRotatedQuad(texture, { position.x, position.y, 0.0f }, size, rotation, color, textureScale);
	}
	void Renderer2D::DrawRotatedQuad(const Ref<Texture2D>& texture, const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color, const float textureScale)
	{
		POLARITY_PROFILE_FUNCTION();

		glm::mat4 transform(1.0f);
		if (rotation != 0) //For preformance we don't calculate rotation if we dont have to
		{
			POLARITY_PROFILE_SCOPE("Transform calc (pos, size, rot)");

			transform = glm::translate(transform, position);
			transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0, 0, 1));
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}
		else
		{
			POLARITY_PROFILE_SCOPE("Transform calc (pos, size)");

			transform = glm::translate(transform, position);
			transform = glm::scale(transform, glm::vec3(size, 1.0f));
		}

		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetFloat("u_TexScale", textureScale);
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		texture->Bind();

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

}