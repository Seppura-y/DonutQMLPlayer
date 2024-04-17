#include "opengl_functions.h"

namespace Donut
{
	OpenGLFunctions::OpenGLFunctions() : QOpenGLFunctions_4_5_Core()
	{
		initializeOpenGLFunctions();
	}

	OpenGLFunctions::~OpenGLFunctions()
	{
		
	}

	Scope<OpenGLFunctions> OpenGLFunctions::getInstance()
	{
		return createScope<OpenGLFunctions>();
	}


	Scope<OpenGLFunctions> OpenGLFunctions::create()
	{
		return createScope<OpenGLFunctions>();
	}

}
