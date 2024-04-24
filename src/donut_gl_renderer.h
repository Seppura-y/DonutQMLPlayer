#ifndef DONUT_GL_RENDERER_H
#define DONUT_GL_RENDERER_H

#include "opengl_shader.h"
#include "opengl_vertex_array.h"
#include "opengl_uniform_buffer.h"
#include "opengl_texture_2d.h"

#include <QObject>
#include <QQuickWindow>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>

struct RectangleVertex;


struct BatchRenderData
{
	static const uint32_t max_rects_ = 20000;
	static const uint32_t max_vertices_ = max_rects_ * 4;
	static const uint32_t max_indices_ = max_rects_ * 6;
	static const uint32_t max_texture_slots_ = 32;

	std::shared_ptr<Donut::OpenGLTexture2D> white_texture_;

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

class DonutGLItemRenderer : public QObject, public QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	DonutGLItemRenderer() : m_t(0), m_program(0) {}
	~DonutGLItemRenderer();


	void setT(qreal t) { m_t = t; }
	void setViewportSize(const QSize& size);
	void setWindow(QQuickWindow* window) { m_window = window; }

	void drawIndices(const Donut::Ref<Donut::OpenGLVertexArray>& va, uint32_t vertex_count);

public slots:
	void init();
	void paint();

private:
	void flush();
	void beginScene();
	void endScene();

private:

	QSize m_viewportSize;
	qreal m_t;
	QOpenGLShaderProgram* m_program;
	QQuickWindow* m_window;

	//std::shared_ptr<Donut::OpenGLShader> rect_shader_;
	//std::shared_ptr < Donut::OpenGLVertexArray> rect_vao_;
	//std::shared_ptr < Donut::OpenGLVertexBuffer> rect_vbo_;
	//std::shared_ptr < Donut::OpenGLIndexBuffer> rect_ebo_;
	//std::shared_ptr < Donut::OpenGLUniformBuffer> rect_ubo_;

	//uint32_t rect_indices_count_ = 0;
	//RectangleVertex* rect_vertex_buffer_base_ = nullptr;
	//RectangleVertex* rect_vertex_buffer_ptr_ = nullptr;

	BatchRenderData s_data;

};

#endif