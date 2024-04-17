#ifndef OPENGL_RENDERER_H
#define OEPNGL_RENDERER_H

#include "renderer/renderer_api.h"
#include "opengl/opengl_context.h"

#include <QOpenGLFunctions_4_5_Core>

namespace Donut
{
	class OpenGLRendererAPI : public RendererAPI, protected QOpenGLFunctions_4_5_Core
	{
	public:
		OpenGLRendererAPI();
		virtual void init(void* ctx) override;
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count = 0) override;
		virtual void drawLines(const Donut::Ref<VertexArray>& va, uint32_t vertex_count) override;

		virtual void setLineWidth(float width);
	private:
		OpenGLContext* graphics_ctx_;
		QOffscreenSurface* surface_;
		QOpenGLContext* context_ = nullptr;
		QOffscreenSurface* offscreen_surface_;
	};
}
#endif