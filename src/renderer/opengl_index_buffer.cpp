#include "opengl_index_buffer.h"

#include "donut_scene.h"
#include "renderer_global.h"

namespace Donut
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: indices_count_(count)
	{
		OPENGL_EXTRA_FUNCTIONS(glGenBuffers(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteBuffers(1, &object_id_));
	}

	void OpenGLIndexBuffer::bind() const
	{
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_));
	}

	void OpenGLIndexBuffer::unBind() const
	{
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	void OpenGLIndexBuffer::setData(uint32_t* indices, uint32_t size)
	{
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW));
	}

	uint32_t OpenGLIndexBuffer::getIndicesCount() const
	{
		return indices_count_;
	}
}