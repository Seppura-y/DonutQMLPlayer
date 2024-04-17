#ifndef OPENGL_BUFFERS_H
#define OPENGL_BUFFERS_H

#include "renderer/buffers.h"

#include <stdint.h>
#include <QOpenGLFunctions_4_5_Core>


namespace Donut
{

	class OpenGLVertexBuffer : public VertexBuffer, public QOpenGLFunctions_4_5_Core
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void setLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& getLayout() const override;

		virtual void setData(const void* data, uint32_t size) override;

		virtual void bind() override;
		virtual void unBind()  override;

	private:
		uint32_t object_id_;
		BufferLayout layout_;
	};

	class OpenGLIndexBuffer : public IndexBuffer, public QOpenGLFunctions_4_5_Core
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() override;
		virtual void unBind() override;

		virtual uint32_t getIndicesCount() const override;
	private:
		uint32_t object_id_;
		uint32_t indices_count_;
	};
}
#endif