#include "opengl_context.h"

#include <iostream>
#include <sstream>

namespace Donut
{
	OpenGLContext::OpenGLContext(QOpenGLContext* context, QOffscreenSurface* surface)
		: context_(context), surface_(surface), QOpenGLFunctions_4_5_Core()
	{
		//DN_CORE_ASSERT(surface, "window handle is null");
		
	}

	OpenGLContext::~OpenGLContext()
	{
		if (context_)
		{
			delete context_;
		}

		if (surface_)
		{
			delete surface_;
		}
	}

	void OpenGLContext::init()
	{
		//DN_PROFILE_FUNCTION();
		initializeOpenGLFunctions();
		//DN_CORE_INFO("OpenGL Info : ");

		std::stringstream ss;
		glGetString(GL_VENDOR);
		//glGetString(GL_RENDERER);
		//glGetString(GL_VERSION);


		ss << glGetString(GL_VENDOR);
		//DN_CORE_INFO("Vendor	: {0}", ss.str());

		ss.clear();
		ss << glGetString(GL_RENDERER);
		//DN_CORE_INFO("Renderer	: {0}", ss.str());

		ss.clear();
		ss << glGetString(GL_VERSION);
		//DN_CORE_INFO("Version	: {0}", ss.str());
	}

	void OpenGLContext::swapBuffers()
	{
		//DN_PROFILE_FUNCTION();
		context_->makeCurrent(surface_);
		context_->swapBuffers(surface_);
	}

	void OpenGLContext::setVsync(bool state)
	{
		if (state)
		{
			context_->format().setSwapInterval(1);
		}
		else
		{
			context_->format().setSwapInterval(0);
		}
	}

	bool OpenGLContext::isVsync()
	{
		if (context_->format().swapInterval())
		{
			return true;
		}

		return false;
	}
}