#ifndef DONUT_GL_RENDERER_H
#define DONUT_GL_RENDERER_H

#include "opengl_shader.h"

#include <QObject>
#include <QQuickWindow>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>

class DonutGLItemRenderer : public QObject, public QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	DonutGLItemRenderer() : m_t(0), m_program(0) {}
	~DonutGLItemRenderer();


	void setT(qreal t) { m_t = t; }
	void setViewportSize(const QSize& size) { m_viewportSize = size; }
	void setWindow(QQuickWindow* window) { m_window = window; }

public slots:
	void init();
	void paint();

private:

	QSize m_viewportSize;
	qreal m_t;
	QOpenGLShaderProgram* m_program;
	QQuickWindow* m_window;

	std::shared_ptr<Donut::OpenGLShader> shader_;
};

#endif