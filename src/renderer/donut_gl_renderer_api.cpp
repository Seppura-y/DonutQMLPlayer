#include "donut_gl_renderer_api.h"


Donut::DonutGLRendererApi::DonutGLRendererApi()
	: QOpenGLFunctions_4_5_Core()
{
	initializeOpenGLFunctions();
}

void Donut::DonutGLRendererApi::init()
{
	//initializeOpenGLFunctions();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
}

void Donut::DonutGLRendererApi::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void Donut::DonutGLRendererApi::setClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Donut::DonutGLRendererApi::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Donut::DonutGLRendererApi::drawIndices(const Donut::Ref<OpenGLVertexArray>& va, uint32_t count)
{
	uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Donut::DonutGLRendererApi::drawArrays(uint32_t count)
{
	glDrawArrays(GL_TRIANGLE_STRIP, 0, count);
}
