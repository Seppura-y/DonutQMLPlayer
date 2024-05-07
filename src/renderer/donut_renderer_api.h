#ifndef DONUT_RENDERER_API_H
#define DONUT_RENDERER_API_H

#include "opengl_vertex_array.h"

#include <glm/glm.hpp>

namespace Donut
{
	class DonutRendererApi
	{
	public:
		enum class RendererAPIType
		{
			None = 0,
			OpenGL = 1
		};

		inline static RendererAPIType getCurrentAPIType() { return type_; }

		virtual ~DonutRendererApi() = default;

		virtual void init() = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void drawIndices(const Donut::Ref<OpenGLVertexArray>& vertex_array, uint32_t count = 0) = 0;
		virtual void drawArrays(uint32_t count = 0) = 0;
		inline static RendererAPIType getApiType() { return type_; }
	private:
		static RendererAPIType type_;
	};
}


#endif