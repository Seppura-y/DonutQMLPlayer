#include "donut_framebuffer_item.h"

#include "renderer_2d.h"

namespace Donut
{
	class DonutFramebufferItemRenderer : public QQuickFramebufferObject::Renderer
	{
	public:
		DonutFramebufferItemRenderer();

		void render() override;
		QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
		void synchronize(QQuickFramebufferObject* obj) override;

	private:

	};

	DonutFramebufferItemRenderer::DonutFramebufferItemRenderer()
	{
		Donut::Renderer2D::init();
	}

	void DonutFramebufferItemRenderer::render()
	{
	}

	QOpenGLFramebufferObject* DonutFramebufferItemRenderer::createFramebufferObject(const QSize& size)
	{
		return nullptr;
	}

	void DonutFramebufferItemRenderer::synchronize(QQuickFramebufferObject* obj)
	{

	}

	DonutFramebufferItem::DonutFramebufferItem(QQuickItem* parent)
		: QQuickFramebufferObject(parent)
	{

	}

	QQuickFramebufferObject::Renderer* DonutFramebufferItem::createRenderer() const
	{
		return new DonutFramebufferItemRenderer();
	}

	void DonutFramebufferItem::initCamera(float left, float right, float bottom, float top)
	{

	}



}

