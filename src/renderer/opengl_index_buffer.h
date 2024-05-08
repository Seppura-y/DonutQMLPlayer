#ifndef OPENGL_INDEX_BUFFER_H
#define OPENGL_INDEX_BUFFER_H

#include <stdint.h>

namespace Donut
{
	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() const;
		virtual void unBind() const;

		virtual void setData(uint32_t* indices, uint32_t size);

		virtual uint32_t getId() const { return object_id_; };

		virtual uint32_t getIndicesCount() const;
	private:
		uint32_t object_id_;
		uint32_t indices_count_;
	};
}
#endif