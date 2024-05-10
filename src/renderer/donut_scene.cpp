#include "donut_scene.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QQuickOpenGLUtils>
#include <QRunnable>

#include <QDebug>

DonutSceneRenderer* DonutScene::s_renderer_ = nullptr;
//Donut::DonutGLRendererApi* DonutScene::renderer_api_ = nullptr;

class DonutRenderNode : public QSGRenderNode
{
public:
    DonutRenderNode(QQuickWindow* window) : window_(window)
    {

    }
    void render(const RenderState* state) override
    {
        scene_renderer_->beginScene(scene_camera_);

        //scene_renderer_->drawFlatRectangle(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec2{ 0.5f, 0.5f });
        //scene_renderer_->drawFlatRectangle(glm::vec3{ 0.5f, 0.5f, 0.0f }, glm::vec2{ 1.0f, 1.0f });

        scene_renderer_->drawRectangle(glm::vec3{ 0.0f, 0.0f, 0.3f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.8f, 0.5f, 0.3f, 0.4f });
        scene_renderer_->drawRectangle(glm::vec3{ -1.0f, -1.0f, 0.2f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.5f, 0.8f, 0.3f, 0.3f });
        scene_renderer_->drawRectangle(glm::vec3{ 0.6f, 0.6f, 0.1f }, glm::vec2{ 1.5f, 1.5f }, glm::vec4{ 0.3f, 0.5f, 0.8f, 0.5f });

        scene_renderer_->endScene();
    }

    void setRenderer(DonutSceneRenderer* renderer, Donut::DonutSceneCamera camera)
    {
        scene_renderer_ = renderer;
        scene_camera_ = camera;
    }

    //void prepare() override;
    //void render(const RenderState* state) override;
    void releaseResources() override
    {

    }

    // 必须重写这个函数，否则程序崩溃
    RenderingFlags flags() const override
    {
        //We are rendering 2D content directly into the scene graph using QRhi, no
        //direct usage of a 3D API.Hence NoExternalRendering.This is a minor
        //optimization.
        //Additionally, the node takes the item transform into account by relying
        //on projectionMatrix() and matrix() (see prepare()) and never rendering at
        //other Z coordinates.Hence DepthAwareRendering.This is a potentially
        //bigger optimization.
        return QSGRenderNode::NoExternalRendering | QSGRenderNode::DepthAwareRendering;
    }
    QSGRenderNode::StateFlags changedStates() const override
    {
        return QSGRenderNode::StateFlag::ViewportState | QSGRenderNode::StateFlag::CullState;
    }
private:
    DonutSceneRenderer* scene_renderer_;
    QQuickWindow* window_;
    Donut::DonutSceneCamera scene_camera_;
};

DonutScene::DonutScene(QQuickItem* parent)
    : QQuickItem(parent)
    , delta_t_(0)
    //, s_renderer_(nullptr)
{
    connect(this, &DonutScene::sigSceneUpdate, this, &DonutScene::update);
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
        
        // glDrawArrays 测试 ---- initForRectRender
        //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForRectRender, Qt::DirectConnection);
        
        // glDrawElements 绘制 ---- initForVideoRender
        connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForVideoRender, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, this, &DonutScene::onUpdate, Qt::DirectConnection);
    }
}

void DonutScene::onUpdate()
{
    //s_renderer_->beginScene(scene_camera_);

    //s_renderer_->drawRectangle(glm::vec3{ 0.0f, 0.0f, 0.1f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.8f, 0.5f, 0.3f, 1.0f });
    //s_renderer_->drawRectangle(glm::vec3{ -1.0f, -1.0f, 0.2f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{ 0.5f, 0.8f, 0.3f, 1.0f });
    //s_renderer_->drawRectangle(glm::vec3{ 0.6f, 0.6f, 0.3f }, glm::vec2{ 1.5f, 1.5f }, glm::vec4{ 0.3f, 0.5f, 0.8f, 0.5f });

    //s_renderer_->endScene();
}

void DonutScene::timerEvent(QTimerEvent* ev)
{
    update();
    emit sigSceneUpdate();
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

QSGNode* DonutScene::updatePaintNode(QSGNode* old, UpdatePaintNodeData* data)
{
    DonutRenderNode* node = static_cast<DonutRenderNode*>(old);
    if (!node) {
        node = new DonutRenderNode(window());
    }

    // ... 设置node的属性和准备渲染数据 ...

    node->setRenderer(s_renderer_, scene_camera_);
    node->markDirty(QSGNode::DirtyForceUpdate);

    return node;
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
        //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::init, Qt::DirectConnection);
        //connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutSceneRenderer::paint, Qt::DirectConnection);
    }
    //qDebug() << "width : " << window()->size().width() << " height: " << window()->size().height();
    //qDebug() << "ratio : " << window()->devicePixelRatio();
    //qDebug() << "effective ratio : " << window()->effectiveDevicePixelRatio();

    s_renderer_->setViewportSize(window()->size() * window()->devicePixelRatio());
    s_renderer_->setT(delta_t_);
    s_renderer_->setWindow(window());
}
