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

		static DonutGLRendererApi* getRendererApi() { return renderer_api_; }
	private:
		static DonutGLRendererApi* renderer_api_;
	};
}

#endif