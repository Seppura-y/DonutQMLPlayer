#include "donut_fbo_renderer.h"

#include "donut_fbo_scene.h"

#include <QOpenGLFramebufferObjectFormat>
namespace Donut
{

DonutSceneRenderer * DonutFramebufferRenderer::s_renderer_ = nullptr;

DonutFramebufferRenderer::DonutFramebufferRenderer()
{
    if (!s_renderer_)
    {
        s_renderer_ = new DonutSceneRenderer();
        //s_renderer_->setWindow(window_);
        //s_renderer_->initForVideoRender();
    }
}

void DonutFramebufferRenderer::render()
{
    s_renderer_->beginScene(scene_camera_);

    //s_renderer_->drawRectangle(glm::vec3{ 0.0f, 0.0f, 0.1f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.8f, 0.5f, 0.3f, 1.0f });
    //s_renderer_->drawRectangle(glm::vec3{ -1.0f, -1.0f, 0.2f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.5f, 0.8f, 0.3f, 1.0f });
    //s_renderer_->drawRectangle(glm::vec3{ 0.6f, 0.6f, 0.3f }, glm::vec2{ 1.5f, 1.5f }, glm::vec4{ 0.3f, 0.5f, 0.8f, 0.5f });

    s_renderer_->endScene();
}

QOpenGLFramebufferObject* DonutFramebufferRenderer::createFramebufferObject(const QSize& size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    s_renderer_->setViewportSize(size);
    return new QOpenGLFramebufferObject(size, format);
}

void DonutFramebufferRenderer::synchronize(QQuickFramebufferObject* item)
{
    DonutFramebufferScene* fbo_scene = qobject_cast<DonutFramebufferScene*>(item);

    if (fbo_scene)
    {
        if (!window_)
        {
            window_ = fbo_scene->window();
            s_renderer_->setWindow(window_);
            s_renderer_->initForVideoRender();
        }

    }
}





}