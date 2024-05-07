#ifndef DONUT_RENDER_COMMAND_H
#define DONUT_RENDER_COMMAND_H

#include "donut_renderer_api.h"
#include "donut_gl_renderer_api.h"

namespace Donut
{
	class DonutRenderCommand
	{
	public:
		DonutRenderCommand() = default;
		~DonutRenderCommand() = default;

		static void init();

		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void setClearColor(const glm::vec4& color);
		static void clear();

		static void drawIndices(const Donut::Ref<OpenGLVertexArray>& va, uint32_t count = 0);

		static void drawArrays(uint32_t count);

		static DonutGLRendererApi* getRendererApi() { return renderer_api_; }
	private:
		static DonutGLRendererApi* renderer_api_;
	};
}

#endif