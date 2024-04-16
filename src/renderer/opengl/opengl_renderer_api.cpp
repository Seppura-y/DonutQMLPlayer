#include "opengl_renderer_api.h"
#include "render_global.h"

#include <QDebug>

namespace Donut
{
	void OpenGLRendererAPI::init()
	{
		context_ = new QOpenGLContext;
		auto format = QSurfaceFormat::defaultFormat();
		format.setProfile(QSurfaceFormat::CoreProfile);
		context_->setFormat(format);

		if (!context_->create())
		{
			qDebug() << "context create failed";
		}

		offscreen_surface_ = new QOffscreenSurface();
		offscreen_surface_->create();
		context_->makeCurrent(offscreen_surface_);


		initializeOpenGLFunctions();
		OPENGL_EXTRA_FUNCTIONS(glEnable(GL_BLEND));
		OPENGL_EXTRA_FUNCTIONS(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		OPENGL_EXTRA_FUNCTIONS(glEnable(GL_DEPTH_TEST));
		OPENGL_EXTRA_FUNCTIONS(glEnable(GL_LINE_SMOOTH));
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		OPENGL_EXTRA_FUNCTIONS(glViewport(x, y, width, height));
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		OPENGL_EXTRA_FUNCTIONS(glClearColor(color.r, color.g, color.b, color.a));
	}

	void OpenGLRendererAPI::clear()
	{
		OPENGL_EXTRA_FUNCTIONS(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void OpenGLRendererAPI::drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count)
	{
		va->bind();
		uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
		//OPENGL_EXTRA_FUNCTIONS(glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr));
		OPENGL_EXTRA_FUNCTIONS(glDrawElements(GL_TRIANGLES, 1, GL_UNSIGNED_INT, nullptr));
		GLenum error = OPENGL_EXTRA_FUNCTIONS(glGetError());
		if (error != GL_NO_ERROR) {
			// ¥Ú”°¥ÌŒÛ–≈œ¢
			qDebug() << "OpenGL error after glDrawElements: " << error;
		}
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	void OpenGLRendererAPI::drawLines(const Donut::Ref<VertexArray>& va, uint32_t vertex_count)
	{
		va->bind();
		OPENGL_EXTRA_FUNCTIONS(glDrawArrays(GL_LINES, 0, vertex_count));
	}

	void OpenGLRendererAPI::setLineWidth(float width)
	{
		OPENGL_EXTRA_FUNCTIONS(glLineWidth(width));
	}
}