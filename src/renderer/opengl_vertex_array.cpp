#include "opengl_vertex_array.h"

#include "donut_scene.h"
#include "renderer_global.h"

namespace Donut
{
	static GLenum shaderDataTypeToGLenumType(const ShaderDataType& type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
		}

		DN_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		//DN_PROFILE_FUNCTION();

		OPENGL_EXTRA_FUNCTIONS(glGenVertexArrays(1, &object_id_));
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		//DN_PROFILE_FUNCTION();

		OPENGL_EXTRA_FUNCTIONS(glDeleteVertexArrays(1, &object_id_));
	}

	void OpenGLVertexArray::addVertexBuffer(const Donut::Ref<OpenGLVertexBuffer>& buffer)
	{
		//DN_PROFILE_FUNCTION();

		DN_CORE_ASSERT((buffer->getLayout().getBufferElements().size()) > 0, "vertex buffer is empty!");

		OPENGL_EXTRA_FUNCTIONS(glBindVertexArray(object_id_));
		buffer->bind();

		const auto& lay = buffer->getLayout();
		for (const auto& element : lay)
		{
			switch (element.type_)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				OPENGL_EXTRA_FUNCTIONS(glEnableVertexAttribArray(vertex_buffer_index_));

				auto compo_count = element.getComponentCount();
				auto type = shaderDataTypeToGLenumType(element.type_);
				OPENGL_EXTRA_FUNCTIONS(glVertexAttribPointer(vertex_buffer_index_,
					element.getComponentCount(),
					shaderDataTypeToGLenumType(element.type_),
					element.normalized_ ? GL_TRUE : GL_FALSE,
					lay.getStride(),
					(const void*)element.offset_)
				);
				vertex_buffer_index_++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				OPENGL_EXTRA_FUNCTIONS(glEnableVertexAttribArray(vertex_buffer_index_));
				OPENGL_EXTRA_FUNCTIONS(glVertexAttribIPointer(vertex_buffer_index_,
					element.getComponentCount(),
					shaderDataTypeToGLenumType(element.type_),
					lay.getStride(),
					(const void*)element.offset_));
				vertex_buffer_index_++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.getComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					OPENGL_EXTRA_FUNCTIONS(glEnableVertexAttribArray(vertex_buffer_index_));
					OPENGL_EXTRA_FUNCTIONS(glVertexAttribPointer(vertex_buffer_index_,
						count,
						shaderDataTypeToGLenumType(element.type_),
						element.normalized_ ? GL_TRUE : GL_FALSE,
						lay.getStride(),
						(const void*)(sizeof(float) * count * i)));
					OPENGL_EXTRA_FUNCTIONS(glVertexAttribDivisor(vertex_buffer_index_, 1));
					vertex_buffer_index_++;
				}
				break;
			}
			default:
				DN_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		vertex_buffers_.push_back(buffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Donut::Ref<OpenGLIndexBuffer>& buffer)
	{
		OPENGL_EXTRA_FUNCTIONS(glBindVertexArray(object_id_));
		index_buffer_ = buffer;
		index_buffer_->bind();
	}

	void OpenGLVertexArray::bind() const
	{
		//DN_PROFILE_FUNCTION();

		OPENGL_EXTRA_FUNCTIONS(glBindVertexArray(object_id_));
	}

	void OpenGLVertexArray::unBind() const
	{
		//DN_PROFILE_FUNCTION();

		OPENGL_EXTRA_FUNCTIONS(glBindVertexArray(0));
	}
}