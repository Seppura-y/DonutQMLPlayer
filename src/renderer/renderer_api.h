#ifndef RENDERER_API_H
#define RENDERER_API_H


#include "vertex_array.h"
#include "graphics_context.h"

#include <glm/glm.hpp>

#include "render_global.h"

namespace Donut
{
	class RendererAPI
	{
	public:
		enum class RendererAPIType
		{
			None = 0,
			OpenGL = 1
		};

		inline static RendererAPIType getCurrentAPIType() { return type_; }

		virtual ~RendererAPI() = default;

		virtual void init(void* ctx) = 0;
		virtual void init() = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void drawIndices(const Donut::Ref<VertexArray>& vertex_array, uint32_t count = 0) = 0;
		virtual void drawLines(const Donut::Ref<VertexArray>& va, uint32_t vertex_count) = 0;
		virtual void setLineWidth(float width) = 0;
		inline static RendererAPIType getApiType() { return type_; }
	private:
		static RendererAPIType type_;
	};
}
#endif