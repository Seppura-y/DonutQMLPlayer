#include "donut_render_command.h"

#include "donut_gl_renderer_api.h"

namespace Donut
{
	DonutGLRendererApi* DonutRenderCommand::renderer_api_ = nullptr;

	void DonutRenderCommand::init()
	{
		renderer_api_ = new DonutGLRendererApi();
	}

}