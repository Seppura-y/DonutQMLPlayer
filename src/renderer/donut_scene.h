#ifndef DONUT_SCENE_H
#define DONUT_SCENE_H

#include "donut_scene_renderer.h"
#include "donut_scene_camera.h"
#include "orthographic_camera_controller.h"

#include "opengl_framebuffer.h"
#include "donut_render_command.h"
#include "donut_gl_renderer_api.h"
#include <QQuickItem>
#include <QOffscreenSurface>

class DonutScene : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(qreal delta_t_ READ t WRITE setT NOTIFY tChanged)
	QML_ELEMENT
public:
	DonutScene(QQuickItem* parent = nullptr);
	~DonutScene();

	qreal t() const { return delta_t_; }
	void setT(qreal t);

	static DonutSceneRenderer* getRenderer() { return s_renderer_; }
	static Donut::DonutGLRendererApi* getRendererApi() { return renderer_api_; }

signals:
	void tChanged();
	void sigItemInitialized();

public slots:
	void sync();
	void cleanup();

private slots:
	void handleWindowChanged(QQuickWindow* win);

	void onItemInitialized();

protected:
	void timerEvent(QTimerEvent* ev) override;
	void mousePressEvent(QMouseEvent* ev) override;
	void wheelEvent(QWheelEvent* ev) override;

	void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
private:
	void releaseResources() override;

	qreal delta_t_;
protected:
	static DonutSceneRenderer* s_renderer_;
	//Donut::DonutSceneCamera scene_camera_;
	////Donut::OrthographicCameraController camera_controller_;

	//int width_;
	//int height_;
	//float aspect_ratio_;

	//Donut::DonutRenderCommand render_command_;
	static Donut::DonutGLRendererApi* renderer_api_;
	QOpenGLContext* context_ = nullptr;
	QOffscreenSurface* offscreen_surface_ = nullptr;
};


#endif