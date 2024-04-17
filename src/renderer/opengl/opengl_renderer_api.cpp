#include "opengl_renderer_api.h"
#include "opengl/opengl_context.h"

#include <QOpenGLContext>

//#include <glad/glad.h>
namespace Donut
{
	OpenGLRendererAPI::OpenGLRendererAPI()
		: QOpenGLFunctions_4_5_Core()
	{
		//initializeOpenGLFunctions();
	}

	void OpenGLRendererAPI::init(void* ctx)
	{
		//DN_PROFILE_FUNCTION();
		//initializeOpenGLFunctions();
		//QOpenGLContext* context = QOpenGLContext::currentContext();
		//if (context)
		//{
		//	glEnable(GL_BLEND);
		//}
		graphics_ctx_ = static_cast<OpenGLContext*>(ctx);
		surface_ = graphics_ctx_->getSurface();
		auto context = graphics_ctx_->getContext();
		context->makeCurrent(surface_);

		initializeOpenGLFunctions();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::init()
	{
		context_ = new QOpenGLContext;

		QSurfaceFormat format;
		format.setMajorVersion(4);
		format.setMinorVersion(5);
		format.setProfile(QSurfaceFormat::CoreProfile);

		//auto format = QSurfaceFormat::defaultFormat();
		//format.setProfile(QSurfaceFormat::CoreProfile);
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
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		//auto context = graphics_ctx_->getContext();
		//context->makeCurrent(surface_);
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear()
	{
		//auto context = graphics_ctx_->getContext();
		//context->makeCurrent(surface_);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count)
	{
		va->bind();
		va->getIndexBuffer()->bind();
		uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
		glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	void OpenGLRendererAPI::drawLines(const Donut::Ref<VertexArray>& va, uint32_t vertex_count)
	{
		va->bind();
		glDrawArrays(GL_LINES, 0, vertex_count);
	}

	void OpenGLRendererAPI::setLineWidth(float width)
	{
		glLineWidth(width);
	}
}