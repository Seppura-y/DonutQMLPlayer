#include "render_command.h"

#include "opengl/opengl_renderer_api.h"

namespace Donut
{
	RendererAPI* RenderCommand::renderer_api_ = new OpenGLRendererAPI();
}