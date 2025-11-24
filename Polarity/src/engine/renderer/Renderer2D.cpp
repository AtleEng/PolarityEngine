#include "polpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#include "platform/openGL/OpenGLShader.h" //temp

namespace Polarity
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> whiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		POLARITY_PROFILE_FUNCTION();

		s_Data = new Renderer2DStorage();

		s_Data->QuadVertexArray = VertexArray::Create();


		float quadVertexes[5 * 4] = {
			//	vertex		  		 // UV
			-0.5f, -0.5f, 0.0f, 	 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 	 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 	 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 	 0.0f, 1.0f
		};
		Ref<VertexBuffer> squareVB = VertexBuffer::Create(quadVertexes, sizeof(quadVertexes));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord"    }
			});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);


		uint32_t quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, sizeof(quadIndices));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->whiteTexture = Texture2D::Create(1, 1);
		uint32_t textureData = 0xffffffff;
		s_Data->whiteTexture->SetData(&textureData, sizeof(uint32_t));

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		POLARITY_PROFILE_FUNCTION();

		delete s_Data;
	}

	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		POLARITY_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
	{

	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size,rotation, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		POLARITY_PROFILE_FUNCTION();

		s_Data->whiteTexture->Bind();

		float radians = glm::radians(rotation);
		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) * 
			glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->TextureShader->SetFloat4("u_Color", color);	

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		DrawQuad(texture, { position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawQuad(const Ref<Texture2D>& texture, const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		POLARITY_PROFILE_FUNCTION();

		float radians = glm::radians(rotation);

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->TextureShader->SetFloat4("u_Color", color);

		texture->Bind();

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
}