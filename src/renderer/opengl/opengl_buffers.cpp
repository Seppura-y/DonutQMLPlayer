#include "opengl_buffers.h"

#include "render_global.h"

namespace Donut
{

	///////////////////////////////////////////////////////////////////////////////////////////////
	////Vertex Buffer

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		OPENGL_EXTRA_FUNCTIONS->glGenBuffers(1, &object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ARRAY_BUFFER, object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		OPENGL_EXTRA_FUNCTIONS->glGenBuffers(1, &object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ARRAY_BUFFER, object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		OPENGL_EXTRA_FUNCTIONS->glDeleteBuffers(1, &object_id_);
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout)
	{
		layout_ = layout;
	}

	const BufferLayout& OpenGLVertexBuffer::getLayout() const
	{
		return layout_;
	}

	void OpenGLVertexBuffer::setData(const void* data, uint32_t size)
	{
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ARRAY_BUFFER, object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::bind() const
	{
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ARRAY_BUFFER, object_id_);
	}

	void OpenGLVertexBuffer::unBind() const
	{
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}












	///////////////////////////////////////////////////////////////////////////////////////////////
	////Index Buffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: indices_count_(count)
	{
		OPENGL_EXTRA_FUNCTIONS->glGenBuffers(1, &object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_);
		OPENGL_EXTRA_FUNCTIONS->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		OPENGL_EXTRA_FUNCTIONS->glDeleteBuffers(1, &object_id_);
	}

	void OpenGLIndexBuffer::bind() const
	{
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_);
	}

	void OpenGLIndexBuffer::unBind() const
	{
		OPENGL_EXTRA_FUNCTIONS->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	uint32_t OpenGLIndexBuffer::getIndicesCount() const
	{
		return indices_count_;
	}
}