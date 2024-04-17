#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "renderer/orthographic_camera.h"
#include "renderer/texture.h"
#include "renderer/subtexture.h"
#include "renderer/camera.h"
#include "renderer/editor_camera.h"
//#include "renderer/font.h"

//#include "scene/components.h"

#include "render_global.h"

namespace Donut
{
	class Renderer2D
	{
	public:
		static void init();
		static void shutdown();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="camera"></param>
		/// <param name="transform">����������ת������</param>
		static void beginScene(const Camera& camera, const glm::mat4& transform);
		static void beginScene(const OrthographicCamera& camera);
		static void beginScene(const EditorCamera& camera);
		static void endScene();
		static void flush();

		static void drawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void drawRectangle(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void drawRotatedRectangle(const glm::vec2& position, const glm::vec2& size, float rotation, glm::vec4& color);
		static void drawRotatedRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, glm::vec4& color);

		static void drawRectangle(const glm::vec2& position, const glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));
		static void drawRectangle(const glm::vec3& position, const glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));

		static void drawRotatedRectangle(const glm::vec2& position, const glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));
		static void drawRotatedRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));
	
		static void drawRectangle(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Subtexture>& subtexture, float tiling_factor = 1.0f, const glm::vec4& tincolor = glm::vec4(1.0f));
		static void drawRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Subtexture>& subtexture, float tiling_factor = 1.0f, const glm::vec4& tincolor = glm::vec4(1.0f));
	
		static void drawRectangle(const glm::mat4 transform, const glm::vec4& color, int entity_id = -1);
		static void drawRectangle(const glm::mat4 transform, const Ref<Texture2D>& texture, float tiling_factor = 1.0f, const glm::vec4& tint_color = glm::vec4(1.0f), int entity_id = -1);
	
		//static void drawSprite(const glm::mat4& trnasform, SpriteRendererComponent& component, int entity_id);

		static void drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entity_id = -1);
	
		//static void drawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entity_id = -1);
		//static void drawRectangleWithLines(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entity_id = -1);
		//static void drawRectangleWithLines(const glm::mat4& transform, const glm::vec4& color, int entity_id = -1);
		//
		struct TextParams
		{
			glm::vec4 color_{ 1.0f };
			float kerning_{ 0.0f };
			float line_spacing_{ 0.0f };
		};
		//static void drawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& text_params, int entity_id = -1);
		//static void drawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entity_id = -1);
		//
		static float getLineWidth();
		static void setLineWidth(float width);

	public:
		struct Statistics
		{
			uint32_t drawcalls_ = 0;
			uint32_t rect_count_ = 0;

			uint32_t getTotalVertexCount() { return rect_count_ * 4; }
			uint32_t getTotalIndexCount() { return rect_count_ * 6; }
		};

		static void resetStatistics();
		static Statistics getStatistics();
	private:

		static void flushAndReset();
	};
}
#endif