#include "donut_scene.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QQuickOpenGLUtils>
#include <QRunnable>

#include <QDebug>

DonutSceneRenderer* DonutScene::s_renderer_ = nullptr;
Donut::DonutGLRendererApi* DonutScene::renderer_api_ = nullptr;

DonutScene::DonutScene(QQuickItem* parent)
    : QQuickItem(parent)
    , delta_t_(0)
    //, s_renderer_(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &DonutScene::onWindowChanged);
    connect(this, &DonutScene::sigItemInitialized, this, &DonutScene::onItemInitialized);
    startTimer(1000 / 60);
    setFlag(ItemHasContents, true);

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
    //renderer_api_ = new Donut::DonutGLRendererApi();
    //renderer_api_->init();

    //renderer_api_->glCreateProgram();
}

DonutScene::~DonutScene()
{
}



void DonutScene::setT(qreal t)
{
    if (t == delta_t_)
        return;
    delta_t_ = t;
    emit tChanged();
    if (window())
        window()->update();
}

void DonutScene::setViewportSize(const QSize& size)
{
    viewport_size_ = { size.width(), size.height()};

    width_ = size.width();
    height_ = size.height();
    aspect_ratio_ = (float)width_ / (float)height_;

    scene_camera_.setOrthographicSize(height_);
    scene_camera_.setViewportSize(width_, height_);
    qDebug() << "DonutSceneRenderer::setViewportSize width : " << size.width() << " height: " << size.height();
}

void DonutScene::onItemInitialized()
{
    qDebug() << "DonutScene::onItemInitialized()";
    if (!s_renderer_) {
        s_renderer_ = new DonutSceneRenderer();
        //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::init, Qt::DirectConnection);
        //connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutSceneRenderer::paint, Qt::DirectConnection);
        //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForRectRender, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForVideoRender, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, this, &DonutScene::onUpdate, Qt::DirectConnection);
    }
}

void DonutScene::onUpdate()
{
    //window()->beginExternalCommands();
    s_renderer_->beginScene(scene_camera_);

    //s_renderer_->drawFlatRectangle(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec2{ 0.5f, 0.5f });
    //s_renderer_->drawFlatRectangle(glm::vec3{ 0.5f, 0.5f, 0.0f }, glm::vec2{ 1.0f, 1.0f });
    //s_renderer_->drawFlatRectangle(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f });
    //s_renderer_->drawFlatRectangle(glm::vec3{ -1.0f, -1.0f, 0.0f }, glm::vec2{ 0.5f, 0.5f });
    s_renderer_->drawRectangle(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.8f, 0.5f, 0.3f, 1.0f });
    //s_renderer_->drawRectangle(glm::vec3{ -1.0f, -1.0f, 0.0f }, glm::vec2{ 1000.5f, 1000.5f }, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

    s_renderer_->endScene();
    //s_renderer_->paint();
    //window()->endExternalCommands();
}

void DonutScene::timerEvent(QTimerEvent* ev)
{
    update();
}

void DonutScene::mousePressEvent(QMouseEvent* ev)
{
    qDebug() << "DonutScene::mousePressEvent(QMouseEvent* ev)";
}

void DonutScene::wheelEvent(QWheelEvent* ev)
{
    qDebug() << "DonutScene::wheelEvent(QWheelEvent* ev)";
}

void DonutScene::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    qDebug() << "DonutScene::geometryChange()" << newGeometry.width() << " : " << newGeometry.height();

}


void DonutScene::onWindowChanged(QQuickWindow* win)
{

    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &DonutScene::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &DonutScene::cleanup, Qt::DirectConnection);

        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor(Qt::black);
    }
}



void DonutScene::cleanup()
{
    delete s_renderer_;
    s_renderer_ = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(DonutSceneRenderer* renderer) : s_renderer_(renderer) { }
    void run() override { delete s_renderer_; }
private:
    DonutSceneRenderer* s_renderer_;
};


void DonutScene::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(s_renderer_), QQuickWindow::BeforeSynchronizingStage);
    s_renderer_ = nullptr;
}



void DonutScene::sync()
{
    if (!s_renderer_) {
        s_renderer_ = new DonutSceneRenderer();
        connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutSceneRenderer::paint, Qt::DirectConnection);
    }
    //qDebug() << "width : " << window()->size().width() << " height: " << window()->size().height();
    //qDebug() << "ratio : " << window()->devicePixelRatio();
    //qDebug() << "effective ratio : " << window()->effectiveDevicePixelRatio();

    s_renderer_->setViewportSize(window()->size() * window()->devicePixelRatio());
    s_renderer_->setT(delta_t_);
    s_renderer_->setWindow(window());
}
