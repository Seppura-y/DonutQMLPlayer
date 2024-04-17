#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "renderer/graphics_context.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QOffscreenSurface>
#include <QSurfaceFormat>
#include <QOpenGLContext>

struct GLFWwindow;

namespace Donut
{
	class OpenGLContext : public GraphicsContext, public QOpenGLFunctions_4_5_Core
	{
	public:
		OpenGLContext(QOpenGLContext* context, QOffscreenSurface* surface);
		~OpenGLContext();
		virtual void init() override;
		virtual void swapBuffers() override;

		void setVsync(bool state) override;
		bool isVsync() override;

		QOpenGLContext* getContext() { return context_; }
		QOffscreenSurface* getSurface() { return surface_; }
	private:
		QOpenGLContext* context_;
		QOffscreenSurface* surface_;
	
		bool is_vsync_ = false;
	};
}
#endif