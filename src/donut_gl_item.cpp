#include "donut_gl_item.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QQuickOpenGLUtils>
#include <QRunnable>

DonutGLItem::DonutGLItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_t(0)
    , m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &DonutGLItem::handleWindowChanged);
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

void DonutGLItem::timerEvent(QTimerEvent* ev)
{
    update();
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
    delete m_renderer;
    m_renderer = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(DonutGLItemRenderer* renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    DonutGLItemRenderer* m_renderer;
};


void DonutGLItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}



void DonutGLItem::sync()
{
    if (!m_renderer) {
        m_renderer = new DonutGLItemRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &DonutGLItemRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &DonutGLItemRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}



