#include "donut_framebuffer_item.h"
#include "donut_video_renderer.h"
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QDebug>
//************TaoItemRender************//
class DonutFramebufferItemRenderer : public QQuickFramebufferObject::Renderer
{
public:
	DonutFramebufferItemRenderer();

	void					  render() override;
	QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
	void					  synchronize(QQuickFramebufferObject*) override;

private:
	DonutVideoRenderer	  m_render;
	QQuickWindow* m_window = nullptr;
};

DonutFramebufferItemRenderer::DonutFramebufferItemRenderer()
{
	qDebug() << "DonutFramebufferItemRenderer::DonutFramebufferItemRenderer()";
	 m_render.init();
}

void DonutFramebufferItemRenderer::render()
{
	qDebug() << "DonutFramebufferItemRenderer::render()";
	// m_render.paint();
	//m_window->resetOpenGLState();
}

QOpenGLFramebufferObject* DonutFramebufferItemRenderer::createFramebufferObject(const QSize& size)
{
	qDebug() << "DonutFramebufferItemRenderer::createFramebufferObject(const QSize& size)";
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setSamples(4);
	m_render.resize(size.width(), size.height());
	return new QOpenGLFramebufferObject(size, format);
}

void DonutFramebufferItemRenderer::synchronize(QQuickFramebufferObject* item)
{
	DonutFramebufferItem* pItem = qobject_cast<DonutFramebufferItem*>(item);
	if (pItem)
	{
		if (!m_window)
		{
			m_window = pItem->window();
		}
		if (pItem->infoDirty())
		{
			m_render.updateTextureInfo(pItem->videoWidth(), pItem->videoHeght(), pItem->videoFormat());
			pItem->makeInfoDirty(false);
		}
		bool	got = false;
		//YUVData data = pItem->getFrame(got);
		//if (got)
		//{
		//	m_render.updateTextureData(data);
		//}
	}
}

//************TaoItem************//
DonutFramebufferItem::DonutFramebufferItem(QQuickItem* parent)
	: QQuickFramebufferObject(parent),
		camera_controller_(1600.0f / 900.0f, true)
{
	//m_decoderController = std::make_unique<TaoDecoderController>();
	//connect(m_decoderController.get(), &TaoDecoderController::videoInfoReady, this, &TaoItem::onVideoInfoReady);
	// 按每秒60帧的帧率更新界面
	startTimer(1000 / 60);

	qDebug() << "DonutFramebufferItem::DonutFramebufferItem(QQuickItem* parent)";
}

void DonutFramebufferItem::timerEvent(QTimerEvent* event)
{
	Q_UNUSED(event);
	update();
}

//YUVData DonutFramebufferItem::getFrame(bool& got)
//{
//	return m_decoderController->getFrame(got);
//}

void DonutFramebufferItem::loadFile(const QUrl& url)
{
	//m_decoderController->load(url.toLocalFile());
}

void DonutFramebufferItem::onVideoInfoReady(int width, int height, int format)
{
	if (m_videoWidth != width)
	{
		m_videoWidth = width;
		makeInfoDirty(true);
	}
	if (m_videoHeight != height)
	{
		m_videoHeight = height;
		makeInfoDirty(true);
	}
	if (m_videoFormat != format)
	{
		m_videoFormat = format;
		makeInfoDirty(true);
	}
}

QQuickFramebufferObject::Renderer* DonutFramebufferItem::createRenderer() const
{
	qDebug() << "DonutFramebufferItemRenderer::createFramebufferObject(const QSize& size)";
	return new DonutFramebufferItemRenderer;
}

void DonutFramebufferItem::initCamera(float left, float right, float bottom, float top)
{
}

Q_INVOKABLE void DonutFramebufferItem::mouseClickedInUI()
{
	
}
