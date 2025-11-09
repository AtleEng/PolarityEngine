#include "polpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Polarity {
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Polarity::ShaderDataType::Float:	return GL_FLOAT;
		case Polarity::ShaderDataType::Float2:	return GL_FLOAT;
		case Polarity::ShaderDataType::Float3:	return GL_FLOAT;
		case Polarity::ShaderDataType::Float4:	return GL_FLOAT;
		case Polarity::ShaderDataType::Mat3:	return GL_FLOAT;
		case Polarity::ShaderDataType::Mat4:	return GL_FLOAT;
		case Polarity::ShaderDataType::Int:		return GL_INT;
		case Polarity::ShaderDataType::Int2:	return GL_INT;
		case Polarity::ShaderDataType::Int3:	return GL_INT;
		case Polarity::ShaderDataType::Int4:	return GL_INT;
		case Polarity::ShaderDataType::Bool:	return GL_BOOL;
		}

		LOG_MAJOR_ERROR("Unknown ShaderDataType !!!");
		return 0;
	}



	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		LOG_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout !!!");
		
		glBindVertexArray(m_rendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);

			index++;
		}
		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_rendererID);
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}
}