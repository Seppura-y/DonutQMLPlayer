#include "donut_render_command.h"

#include "donut_gl_renderer_api.h"

namespace Donut
{
	DonutGLRendererApi* DonutRenderCommand::renderer_api_ = nullptr;

	void DonutRenderCommand::init()
	{
		if(!renderer_api_)
			renderer_api_ = new DonutGLRendererApi();
	}

	void DonutRenderCommand::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		renderer_api_->setViewport(x, y, width, height);
	}

	void DonutRenderCommand::setClearColor(const glm::vec4& color)
	{
		renderer_api_->setClearColor(color);
	}

	void DonutRenderCommand::clear()
	{
		renderer_api_->clear();
	}

	void DonutRenderCommand::drawIndices(const Donut::Ref<OpenGLVertexArray>& va, uint32_t count)
	{
		renderer_api_->drawIndices(va, count);
	}

	void DonutRenderCommand::drawArrays(uint32_t count)
	{
		renderer_api_->drawArrays(count);
	}
}