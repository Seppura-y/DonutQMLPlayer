#include "donut_framebuffer_item.h"

#include "renderer_2d.h"
#include "renderer.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>
#include <QQuickWindow>
#include <QQuickOpenGLUtils>
#include <QOpenGLFunctions>

class DonutFramebufferItemRenderer : public QQuickFramebufferObject::Renderer
{
public:
	DonutFramebufferItemRenderer();

	void render() override;
	QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
	void synchronize(QQuickFramebufferObject* obj) override;

private:
	QQuickWindow* window_ = nullptr;
	Donut::OrthographicCameraController camera_controller_;
};

DonutFramebufferItemRenderer::DonutFramebufferItemRenderer()
	: camera_controller_(1600.0f / 900.0f, true)
{
	qDebug() << "DonutFramebufferItemRenderer created";
	Donut::Renderer::init();
	Donut::Renderer2D::init();
}

void DonutFramebufferItemRenderer::render()
{
	window_->beginExternalCommands();
	// Ö´ÐÐÄúµÄäÖÈ¾ÃüÁî
	Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	Donut::Renderer2D::endScene();
	window_->endExternalCommands();
	qDebug() << "DonutFramebufferItemRenderer render";
}


void DonutFramebufferItemRenderer::synchronize(QQuickFramebufferObject* obj)
{
	qDebug() << "DonutFramebufferItemRenderer::synchronize";
	DonutFramebufferItem* item = qobject_cast<DonutFramebufferItem*>(obj);
	if (item)
	{
		if (!window_)
		{
			window_ = item->window();
		}
		Donut::Renderer2D::beginScene(camera_controller_.getCamera());
		qDebug() << "DonutFramebufferItemRenderer::synchronize";
		Donut::Renderer2D::endScene();
	}
}


QOpenGLFramebufferObject* DonutFramebufferItemRenderer::createFramebufferObject(const QSize& size)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setSamples(4);
	qDebug() << "DonutFramebufferItemRenderer createFramebufferObject";
	//m_render.resize(size.width(), size.height());
	return new QOpenGLFramebufferObject(size, format);
}




DonutFramebufferItem::DonutFramebufferItem(QQuickItem* parent)
	: QQuickFramebufferObject(parent),
	camera_controller_(1600.0f / 900.0f, true)
{
	qDebug() << "DonutFramebufferItem created";
	startTimer(1000 / 60);
}

QQuickFramebufferObject::Renderer* DonutFramebufferItem::createRenderer() const
{
	return new DonutFramebufferItemRenderer;
}

void DonutFramebufferItem::initCamera(float left, float right, float bottom, float top)
{

}

Q_INVOKABLE void DonutFramebufferItem::mouseClickedInUI()
{
	qDebug() << "mouseClickedInUI";
}

void DonutFramebufferItem::timerEvent(QTimerEvent* ev)
{
	Q_UNUSED(ev);
	update();
	//qDebug() << "DonutFramebufferItem update";
}
