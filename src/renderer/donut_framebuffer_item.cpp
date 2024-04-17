#include "donut_framebuffer_item.h"

#include "renderer_2d.h"
#include "renderer.h"

#include "editor_camera.h"

#include "donut_video_renderer.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>
#include <QQuickWindow>
#include <QQuickOpenGLUtils>
#include <QOpenGLFunctions>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
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
	Donut::EditorCamera editor_camera_;

	DonutVideoRenderer renderer_;
};

DonutFramebufferItemRenderer::DonutFramebufferItemRenderer()
	: camera_controller_(1600.0f / 900.0f, true)
{
	qDebug() << "DonutFramebufferItemRenderer created";
	//Donut::Renderer::init();
	//Donut::Renderer2D::init();
	camera_controller_.setZoomLevel(350.0);
	editor_camera_ = Donut::EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	glm::mat4x4 viewMatrix = glm::mat4x4(1.0);
	glm::mat4x4 projectionMatrix = glm::mat4x4(1.0);
	glm::vec3     m_eye = glm::vec3(0, 0, 1);
	glm::vec3     m_center = glm::vec3(0, 0, 0);
	glm::vec3     m_up = glm::vec3(0, 1, 0);

	projectionMatrix = glm::ortho(-1.0, 1.0, -1.0, 1.0, 0.1, 1000.0);

	//projectionMatrix=glm::perspective(25,64/48,0.1,1000);
	viewMatrix = glm::lookAt(m_eye, m_center, m_up);
	//editor_camera_.setViewMatirx(projectionMatrix);


	//renderer_.init();
}

void DonutFramebufferItemRenderer::render()
{
	editor_camera_.setViewportSize(this->window_->width(), this->window_->height());
	//window_->beginExternalCommands();

	Donut::Renderer2D::resetStatistics();
	Donut::RenderCommand::setClearColor(glm::vec4{ 0.0f });
	Donut::RenderCommand::clear();


	Donut::Renderer2D::beginScene(editor_camera_);

	Donut::Renderer2D::drawRectangle(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.5f, 0.5f }, glm::vec4{ 0.8f, 0.3f, 0.2f, 1.0f });
	//Donut::Renderer2D::drawRectangle(glm::mat4{ 1.0f }, glm::vec4{ 0.8f, 0.3f, 0.2f, 1.0f });
	//Donut::Renderer2D::setLineWidth(10);
	//Donut::Renderer2D::drawLine(glm::vec3{ 0.0f, 0.0f, 0.2f }, glm::vec3{ 1.0f, 1.0f, 0.2f }, glm::vec4{ 1.0f });


	Donut::Renderer2D::endScene();
	//window_->endExternalCommands();
	qDebug() << "DonutFramebufferItemRenderer render";

	//window_->beginExternalCommands();
	//renderer_.paint();
	//window_->endExternalCommands();
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
	//renderer_.resize(size.width(), size.height());
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
