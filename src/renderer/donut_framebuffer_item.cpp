#include "donut_framebuffer_item.h"

#include "renderer_2d.h"

#include <QOpenGLFramebufferObject>

class DonutFramebufferItemRenderer : public QQuickFramebufferObject::Renderer
{
public:
	DonutFramebufferItemRenderer();

	void render() override;
	QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
	void synchronize(QQuickFramebufferObject* obj) override;

private:
	Donut::OrthographicCameraController camera_controller_;
};

DonutFramebufferItemRenderer::DonutFramebufferItemRenderer()
	: camera_controller_(1600.0f / 900.0f, true)
{
	Donut::Renderer2D::init();
}

void DonutFramebufferItemRenderer::render()
{
	Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	Donut::Renderer2D::endScene();
}

QOpenGLFramebufferObject* DonutFramebufferItemRenderer::createFramebufferObject(const QSize& size)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setSamples(4);
	//m_render.resize(size.width(), size.height());
	return new QOpenGLFramebufferObject(size, format);
}

void DonutFramebufferItemRenderer::synchronize(QQuickFramebufferObject* obj)
{

}




DonutFramebufferItem::DonutFramebufferItem(QQuickItem* parent)
	: QQuickFramebufferObject(parent),
	camera_controller_(1600.0f / 900.0f, true)
{

}

QQuickFramebufferObject::Renderer* DonutFramebufferItem::createRenderer() const
{
	return new DonutFramebufferItemRenderer();
}

void DonutFramebufferItem::initCamera(float left, float right, float bottom, float top)
{

}