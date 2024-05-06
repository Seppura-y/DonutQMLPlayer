#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "renderer_global.h"
#include "buffer_defs.h"
#include "opengl_vertex_buffer.h"
#include "opengl_index_buffer.h"


#include <vector>

namespace Donut
{
	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void addVertexBuffer(const Ref<OpenGLVertexBuffer>& buffer);
		virtual void setIndexBuffer(const Ref<OpenGLIndexBuffer>& buffer);

		virtual void bind() const;
		virtual void unBind() const;

		virtual const std::vector<Ref<OpenGLVertexBuffer>>& getVertexBuffers() const { return vertex_buffers_; }
		virtual const Ref<OpenGLIndexBuffer>& getIndexBuffer() const { return index_buffer_; }
	private:
		unsigned int object_id_;
		unsigned int vertex_buffer_index_ = 0;
		std::vector<Ref<OpenGLVertexBuffer>> vertex_buffers_;
		Ref<OpenGLIndexBuffer> index_buffer_;
	};
}
#endif