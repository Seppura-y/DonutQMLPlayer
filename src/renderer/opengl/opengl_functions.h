#ifndef OPENGL_FUNCTINOS_H
#define OPENGL_FUNCTIONS_H

#include "render_global.h"
#include <QOpenGLFunctions_4_5_Core>

namespace Donut
{
	class OpenGLFunctions : public QOpenGLFunctions_4_5_Core
	{
	public:
		~OpenGLFunctions();

		static Scope<OpenGLFunctions> getInstance();
		OpenGLFunctions();
		static Scope<OpenGLFunctions> create();
	private:

	private:
	};
}


#endif