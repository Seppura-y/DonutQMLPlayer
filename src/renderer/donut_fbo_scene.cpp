#include "donut_fbo_scene.h"

#include "donut_fbo_renderer.h"

DonutFramebufferScene::DonutFramebufferScene(QQuickItem* parent)
    : QQuickFramebufferObject(parent)
{
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(5);
    format.setProfile(QSurfaceFormat::CoreProfile);

    context_ = new QOpenGLContext(this);
    context_->setFormat(format);
    if (!context_->create())
    {
        qDebug() << "context creation failed";
    }

    offscreen_surface_ = new QOffscreenSurface(nullptr, this);
    //offscreen_surface_ = new QOffscreenSurface(window()->screen());
    offscreen_surface_->create();
    context_->makeCurrent(offscreen_surface_);

    Donut::DonutRenderCommand::init();
    startTimer(1000 / 60);
}

QQuickFramebufferObject::Renderer* DonutFramebufferScene::createRenderer() const
{
    return new Donut::DonutFramebufferRenderer;
}
void DonutFramebufferScene::timerEvent(QTimerEvent* ev)
{
    update();
}

void DonutFramebufferScene::onItemInitialized()
{

}