#include "opengl_uniform_buffer.h"



namespace Donut
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		// 创建一个缓冲区对象并获取其ID
		OPENGL_EXTRA_FUNCTIONS(glGenBuffers(1, &object_id_));

		// 绑定缓冲区对象到GL_UNIFORM_BUFFER目标
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_UNIFORM_BUFFER, object_id_));

		// 分配数据存储空间，但不进行数据初始化
		OPENGL_EXTRA_FUNCTIONS(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));

		// 将缓冲区对象绑定到绑定点，这样它就可以被着色器访问
		OPENGL_EXTRA_FUNCTIONS(glBindBufferBase(GL_UNIFORM_BUFFER, binding, object_id_));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteBuffers(1, &object_id_));
	}


	void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset)
	{
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_UNIFORM_BUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBufferSubData(object_id_, offset, size, data));
	}
}