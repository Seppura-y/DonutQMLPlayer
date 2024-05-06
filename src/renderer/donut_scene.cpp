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
    connect(this, &QQuickItem::windowChanged, this, &DonutScene::handleWindowChanged);
    connect(this, &DonutScene::sigItemInitialized, this, &DonutScene::onItemInitialized);
    startTimer(1000 / 60);
    setFlag(ItemHasContents, true);

    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
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

    renderer_api_ = new Donut::DonutGLRendererApi();
    renderer_api_->init();

    renderer_api_->glCreateProgram();
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

void DonutScene::onItemInitialized()
{
    qDebug() << "DonutScene::onItemInitialized()";
    if (!s_renderer_) {
        s_renderer_ = new DonutSceneRenderer();
        connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutSceneRenderer::paint, Qt::DirectConnection);
    }
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


void DonutScene::handleWindowChanged(QQuickWindow* win)
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
