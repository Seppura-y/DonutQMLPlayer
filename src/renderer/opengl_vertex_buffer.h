#ifndef OPENGL_VERTEX_BUFFERS_H
#define OPENGL_VERTEX_BUFFERS_H

#include "buffer_defs.h"

#include <stdint.h>


namespace Donut
{
	class OpenGLVertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void setLayout(const BufferLayout& layout);
		virtual const BufferLayout& getLayout() const;

		virtual void setData(const void* data, uint32_t size);

		virtual void bind() const;
		virtual void unBind() const;

	private:
		uint32_t object_id_;
		BufferLayout layout_;
	};
}

#endif