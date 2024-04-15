#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "renderer/renderer_api.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLContext>
#include <QOffscreenSurface>

namespace Donut
{
	class OpenGLRendererAPI : public RendererAPI, public QOpenGLExtraFunctions
	{
	public:
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count = 0) override;
		virtual void drawLines(const Donut::Ref<VertexArray>& va, uint32_t vertex_count) override;

		virtual void setLineWidth(float width);
	private:
		QOpenGLContext* context_ = nullptr;
		QOffscreenSurface* offscreen_surface_ = nullptr;
	};
}
#endif