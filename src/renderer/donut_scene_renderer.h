#ifndef DONUT_SCENE_RENDERER_H
#define DONUT_SCENE_RENDERER_H


#include "opengl_shader.h"
#include "opengl_vertex_array.h"
#include "opengl_uniform_buffer.h"
#include "opengl_texture_2d.h"
#include "opengl_framebuffer.h"

#include "camera.h"
#include "donut_scene_camera.h"
#include "donut_render_command.h"

#include <QObject>
#include <QQuickWindow>


struct RectangleVertex
{
	glm::vec2 position_;
	glm::vec4 color_;
	glm::vec2 tex_coordinate_;

	float texture_index_;
	//float tiling_factor_;

	//int entity_id_;
};


struct BatchRenderData
{
	static const uint32_t max_rects_ = 20000;
	static const uint32_t max_vertices_ = max_rects_ * 4;
	static const uint32_t max_indices_ = max_rects_ * 6;
	static const uint32_t max_texture_slots_ = 32;

	std::shared_ptr <Donut::OpenGLTexture2D> white_texture_;

	std::shared_ptr <Donut::OpenGLShader> rect_shader_;
	std::shared_ptr <Donut::OpenGLVertexArray> rect_vao_;
	std::shared_ptr <Donut::OpenGLVertexBuffer> rect_vbo_;
	std::shared_ptr <Donut::OpenGLIndexBuffer> rect_ebo_;
	std::shared_ptr <Donut::OpenGLUniformBuffer> rect_ubo_;

	uint32_t rect_indices_count_ = 0;
	RectangleVertex* rect_vertex_buffer_base_ = nullptr;
	RectangleVertex* rect_vertex_buffer_ptr_ = nullptr;

	std::array<std::shared_ptr<Donut::OpenGLTexture2D>, max_texture_slots_> texture_slots_;
	uint32_t texture_index_ = 1;

	glm::vec4 rect_vertex_positions_[4];

	glm::mat4 view_projection_;
	std::shared_ptr<Donut::OpenGLUniformBuffer> camera_ubo_;
};

class DonutSceneRenderer : public QObject
{
	Q_OBJECT

public:
	DonutSceneRenderer() : delta_t_(0) {}
	~DonutSceneRenderer();


	void setT(qreal t) { delta_t_ = t; }
	void setViewportSize(const QSize& size);
	void setWindow(QQuickWindow* window) { window_ = window; }


	//void setViewport(int x, int y, int width, int height);

	void drawRectangle(glm::vec3 position, glm::vec2 size, glm::vec4 color);

public slots:
	void init();
	void initForVideoRender();
	void paint();

public:
	void flush();
	void flushAndReset();

	void beginScene();

	void beginScene(Donut::Camera camera);
	void beginScene(Donut::Camera camera, const glm::mat4& transform);

	void endScene();
private:
	void calculateAspectRatio();

private:
	//void drawIndices(const Donut::Ref<Donut::OpenGLVertexArray>& va, uint32_t vertex_count);
private:

	QSize viewport_size_;
	glm::vec2 viewport_;
	qreal delta_t_;

	QQuickWindow* window_;

	BatchRenderData batch_data_;

	std::shared_ptr<Donut::OpenGLFramebuffer> framebuffer_;

	Donut::DonutSceneCamera scene_camera_;
	//Donut::OrthographicCameraController camera_controller_;

	int width_;
	int height_;
	float aspect_ratio_;
	
};

#endif