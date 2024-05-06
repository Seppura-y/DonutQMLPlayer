#include "opengl_vertex_buffer.h"
//#include "donut_gl_item.h"
#include "donut_scene.h"
#include "renderer_global.h"
namespace Donut
{

	///////////////////////////////////////////////////////////////////////////////////////////////
	////Vertex Buffer

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		OPENGL_EXTRA_FUNCTIONS(glGenBuffers(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ARRAY_BUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		OPENGL_EXTRA_FUNCTIONS(glGenBuffers(1, &object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ARRAY_BUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		OPENGL_EXTRA_FUNCTIONS(glDeleteBuffers(1, &object_id_));
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
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ARRAY_BUFFER, object_id_));
		OPENGL_EXTRA_FUNCTIONS(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
	}

	void OpenGLVertexBuffer::bind() const
	{
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ARRAY_BUFFER, object_id_));
	}

	void OpenGLVertexBuffer::unBind() const
	{
		OPENGL_EXTRA_FUNCTIONS(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

}