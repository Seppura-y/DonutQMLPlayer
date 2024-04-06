#ifndef RENDER_GLOBAL_H
#define RENDER_GLOBAL_H

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

#ifdef DN_ENABLE_ASSERTS
#define DN_ASSERT(x, ...) { if(!(x)) { DN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define DN_CORE_ASSERT(x, ...) { if(!(x)) { DN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define DN_ASSERT(x, ...)
#define DN_CORE_ASSERT(x, ...)
#endif

#define OPENGL_FUNCTIONS QOpenGLContext::currentContext()->functions()
#define OPENGL_EXTRA_FUNCTIONS QOpenGLContext::currentContext()->extraFunctions()

namespace Donut
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif