#include "opengl_uniform_buffer.h"

#include "donut_scene.h"
#include "renderer_global.h"

namespace Donut
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		OPENGL_EXTRA_FUNCTIONS(glCreateBuffers(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glNamedBufferData(object_id_, size, nullptr, GL_DYNAMIC_DRAW)); // TODO: investigate usage hint
		OPENGL_EXTRA_FUNCTIONS(glBindBufferBase(GL_UNIFORM_BUFFER, binding, object_id_));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteBuffers(1, &object_id_));
	}


	void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset)
	{
		OPENGL_EXTRA_FUNCTIONS(glNamedBufferSubData(object_id_, offset, size, data));
	}
}