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

DonutGLItemRenderer::~DonutGLItemRenderer()
{
    delete m_program;
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

void DonutGLItemRenderer::init()
{
    if (!m_program) {
        QSGRendererInterface* rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
            "attribute highp vec4 vertices;"
            "varying highp vec2 coords;"
            "void main() {"
            "    gl_Position = vertices;"
            "    coords = vertices.xy;"
            "}");
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
            "uniform lowp float t;"
            "varying highp vec2 coords;"
            "void main() {"
            "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
            "    i = smoothstep(t - 0.8, t + 0.8, i);"
            "    i = floor(i * 20.) / 20.;"
            "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
            "}");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();

    }
}


void DonutGLItemRenderer::paint()
{
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();

    m_program->bind();

    m_program->enableAttributeArray(0);

    glClearColor(1.0f, 0.8f, 0.58f, 1.0f);

    float values[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };

    // This example relies on (deprecated) client-side pointers for the vertex
    // input. Therefore, we have to make sure no vertex buffer is bound.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    m_program->setUniformValue("t", (float)m_t);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    QQuickOpenGLUtils::resetOpenGLState();

    m_window->endExternalCommands();
}

