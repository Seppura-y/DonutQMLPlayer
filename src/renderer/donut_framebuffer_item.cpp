#include "donut_framebuffer_item.h"

#include "renderer_2d.h"
#include "renderer.h"

#include "editor_camera.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>
#include <QQuickWindow>
#include <QQuickOpenGLUtils>
#include <QOpenGLFunctions>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	//Donut::Renderer::init();
	//Donut::Renderer2D::init();
	camera_controller_.setZoomLevel(350.0);
}

void DonutFramebufferItemRenderer::render()
{
	//window_->beginExternalCommands();
	// Ö´ÐÐÄúµÄäÖÈ¾ÃüÁî
	Donut::Renderer2D::resetStatistics();
	Donut::RenderCommand::setClearColor(glm::vec4{ 0.0f });
	//Donut::RenderCommand::setClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	Donut::RenderCommand::clear();

	Donut::EditorCamera camera = Donut::EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	Donut::Renderer2D::beginScene(camera);
	for (float y = -5.0f; y < 5.0f; y += 0.5f)
	{
		for (float x = -5.0f; x < 5.0f; x += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
			//Donut::Renderer2D::drawRectangle(
			//	const glm::vec2(1.0f),
			//	const glm::vec2(1.0f),
			//	color
			//);
			//Donut::Renderer2D::drawRectangle(glm::vec2{ x, y }, glm::vec2{ 0.45, 0.45f }, color);
			//OPENGL_EXTRA_FUNCTIONS(glLineWidth(1));
			//Donut::Renderer2D::drawRectangle(glm::vec3{ 0.5f, 0.5f, 0.0f }, glm::vec2{ 0.5f, 0.75f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
		}
	}
	//Donut::Renderer2D::drawRectangle(glm::mat4{ 1.0f }, glm::vec4{ 0.8f, 0.3f, 0.2f, 1.0f });
	Donut::Renderer2D::setLineWidth(1);
	Donut::Renderer2D::drawLine(glm::vec3{ 0.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f }, glm::vec4{ 0.8f, 0.3f, 0.2f, 1.0f });


	Donut::Renderer2D::endScene();
	//window_->endExternalCommands();
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
		//Donut::Renderer2D::beginScene(camera_controller_.getCamera());
		qDebug() << "DonutFramebufferItemRenderer::synchronize";
		//Donut::Renderer2D::endScene();
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
	//update();
	//qDebug() << "DonutFramebufferItem update";
}
