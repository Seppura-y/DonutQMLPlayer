#include "donut_render_command.h"

#include "donut_gl_renderer_api.h"

namespace Donut
{
	DonutRendererApi* DonutRenderCommand::renderer_api_ = new DonutGLRendererApi();

}