#ifndef DONUT_GL_RENDERER_API_H
#define DONUT_GL_RENDERER_API_H

#include "donut_renderer_api.h"

#include <QOpenGLFunctions_4_5_Core>


namespace Donut
{
	class DonutGLRendererApi : public DonutRendererApi, public QOpenGLFunctions_4_5_Core
	{
	public:
		DonutGLRendererApi();
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndices(const Donut::Ref<OpenGLVertexArray>& va, uint32_t count = 0) override;
		virtual void drawArrays(uint32_t count = 0) override;
	private:
	
	};
}

#endif