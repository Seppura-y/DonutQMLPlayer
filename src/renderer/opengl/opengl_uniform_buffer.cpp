#include "opengl_uniform_buffer.h"



namespace Donut
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		// ����һ�����������󲢻�ȡ��ID
		OPENGL_EXTRA_FUNCTIONS(glGenBuffers(1, &object_id_));

		// �󶨻���������GL_UNIFORM_BUFFERĿ��
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_UNIFORM_BUFFER, object_id_));

		// �������ݴ洢�ռ䣬�����������ݳ�ʼ��
		OPENGL_EXTRA_FUNCTIONS(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));

		// ������������󶨵��󶨵㣬�������Ϳ��Ա���ɫ������
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