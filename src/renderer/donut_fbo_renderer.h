#ifndef DONUT_FBO_RENDERER_H
#define DONUT_FBO_RENDERER_H

#include "donut_scene_renderer.h"
#include "donut_scene_camera.h"

#include <QQuickFramebufferObject>
#include <QQuickWindow>

namespace Donut
{
	class DonutFramebufferRenderer : public QQuickFramebufferObject::Renderer
	{
	public:
		DonutFramebufferRenderer();
		void render() override;
		QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
		void synchronize(QQuickFramebufferObject*) override;
	protected:

	private:
		static DonutSceneRenderer* s_renderer_;
		DonutSceneCamera scene_camera_;
		QQuickWindow* window_ = nullptr;
	};
}


#endif