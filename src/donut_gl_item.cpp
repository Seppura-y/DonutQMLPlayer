#include "donut_gl_item.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QQuickOpenGLUtils>
#include <QRunnable>

DonutGLItemRenderer* DonutGLItem::s_renderer_ = nullptr;

DonutGLItem::DonutGLItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_t(0)
    //, s_renderer_(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &DonutGLItem::handleWindowChanged);
    connect(this, &DonutGLItem::sigItemInitialized, this, &DonutGLItem::onItemInitialized);
    startTimer(1000 / 60);
    setFlag(ItemHasContents, true);
}

DonutGLItem::~DonutGLItem()
{
}



void DonutGLItem::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void DonutGLItem::onItemInitialized()
{
    qDebug() << "DonutGLItem::onItemInitialized()";
    if (!s_renderer_) {
        s_renderer_ = new DonutGLItemRenderer();
        connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutGLItemRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutGLItemRenderer::paint, Qt::DirectConnection);
    }
}

void DonutGLItem::timerEvent(QTimerEvent* ev)
{
    update();
}

void DonutGLItem::mousePressEvent(QMouseEvent* ev)
{
    qDebug() << "DonutGLItem::mousePressEvent(QMouseEvent* ev)";
}

void DonutGLItem::wheelEvent(QWheelEvent* ev)
{
    qDebug() << "DonutGLItem::wheelEvent(QWheelEvent* ev)";
}


void DonutGLItem::handleWindowChanged(QQuickWindow* win)
{

    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &DonutGLItem::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &DonutGLItem::cleanup, Qt::DirectConnection);

        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor(Qt::black);
    }
}



void DonutGLItem::cleanup()
{
    delete s_renderer_;
    s_renderer_ = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(DonutGLItemRenderer* renderer) : s_renderer_(renderer) { }
    void run() override { delete s_renderer_; }
private:
    DonutGLItemRenderer* s_renderer_;
};


void DonutGLItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(s_renderer_), QQuickWindow::BeforeSynchronizingStage);
    s_renderer_ = nullptr;
}



void DonutGLItem::sync()
{
    //if (!s_renderer_) {
    //    s_renderer_ = new DonutGLItemRenderer();
    //    connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutGLItemRenderer::init, Qt::DirectConnection);
    //    connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutGLItemRenderer::paint, Qt::DirectConnection);
    //}
    s_renderer_->setViewportSize(window()->size() * window()->devicePixelRatio());
    s_renderer_->setT(m_t);
    s_renderer_->setWindow(window());
}



