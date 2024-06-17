﻿#include "donut_scene.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QQuickOpenGLUtils>
#include <QRunnable>

#include <QDebug>

extern"C"
{
#include <libavcodec/avcodec.h>
}

namespace Donut
{
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
        if (decoded_frame_)
        {
            av_frame_unref(decoded_frame_);
            av_frame_free(&decoded_frame_);
        }
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
        viewport_size_ = { size.width(), size.height() };

        width_ = size.width();
        height_ = size.height();
        aspect_ratio_ = (float)width_ / (float)height_;

        scene_camera_.setOrthographicSize(height_);
        scene_camera_.setViewportSize(width_, height_);
        qDebug() << "DonutSceneRenderer::setViewportSize width : " << size.width() << " height: " << size.height();
    }

    void DonutScene::setQMLAvManager(DonutQMLAVManager* manager)
    {
        av_manager_ = manager;
    }

    void DonutScene::onItemInitialized()
    {
        qDebug() << "DonutScene::onItemInitialized()";
        if (!s_renderer_) {
            //s_renderer_ = new DonutSceneRenderer();
            //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::init, Qt::DirectConnection);
            //connect(window(), &QQuickWindow::beforeRenderPassRecording, s_renderer_, &DonutSceneRenderer::paint, Qt::DirectConnection);

            // glDrawArrays 测试 ---- initForRectRender
            //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForRectRender, Qt::DirectConnection);

            // glDrawElements 绘制 ---- initForVideoRender
            //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForVideoRender, Qt::DirectConnection);
            //connect(window(), &QQuickWindow::beforeRenderPassRecording, this, &DonutScene::onUpdate, Qt::DirectConnection);

            //connect(window(), &QQuickWindow::afterSynchronizing, s_renderer_, &DonutSceneRenderer::initForVideoRender, Qt::DirectConnection);
            //connect(window(), &QQuickWindow::afterRenderPassRecording, this, &DonutScene::onUpdate, Qt::DirectConnection);

            connect(window(), &QQuickWindow::sceneGraphInitialized, this, [this]() {

                if (decoded_frame_ == nullptr)
                {
                    decoded_frame_ = av_frame_alloc();
                }

                test_texture_ = std::make_shared<Donut::OpenGLTexture2D>("assets/textures/cat.jpg");
                //test_texture_ = std::make_shared<Donut::OpenGLTexture2D>("assets/textures/RPG Base/RPGpack_sheet_2X.png");
                }, Qt::DirectConnection);
        }
    }

    void DonutScene::onUpdate()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!frame_updated_ && decoded_frame_ && decoded_frame_->data[0])
        {
            if (decoded_frame_->format == AV_PIX_FMT_YUV420P ||
                decoded_frame_->format == AV_PIX_FMT_YUVJ420P)
            {
                if (!y_texture_)
                {
                    y_texture_ = std::make_shared<Donut::OpenGLTexture2D>(decoded_frame_->width, decoded_frame_->height, TextureFormat::TEXTURE_FORMAT_YUV420);
                }

                if (!u_texture_)
                {
                    u_texture_ = std::make_shared<Donut::OpenGLTexture2D>(decoded_frame_->width / 2, decoded_frame_->height / 2, TextureFormat::TEXTURE_FORMAT_YUV420);
                }

                if (!v_texture_)
                {
                    v_texture_ = std::make_shared<Donut::OpenGLTexture2D>(decoded_frame_->width / 2, decoded_frame_->height / 2, TextureFormat::TEXTURE_FORMAT_YUV420);
                }

                y_texture_->setData(decoded_frame_->data[0], decoded_frame_->linesize[0] * decoded_frame_->height);

                u_texture_->setData(decoded_frame_->data[1], decoded_frame_->linesize[1] * decoded_frame_->height / 2);

                v_texture_->setData(decoded_frame_->data[2], decoded_frame_->linesize[2] * decoded_frame_->height / 2);
            }

            frame_updated_ = true;
        }

        av_frame_unref(decoded_frame_);

        s_renderer_->beginScene(scene_camera_);

        if (test_texture_)
        {
            s_renderer_->drawTexturedRectangle(glm::vec3{ 0.0f, 0.0f, 0.3f }, glm::vec2{ 1.0f, 1.0f }, test_texture_, glm::vec4{ 1.f, 1.f, 1.f, 1.f });
        }

        if (y_texture_ && u_texture_ && v_texture_)
        {
            s_renderer_->drawYuvData(glm::vec3{ 0.0f, 0.0f, 0.4f }, glm::vec2{ 1.0f, 1.0f }, y_texture_, u_texture_, v_texture_);
        }

        s_renderer_->endScene();
    }


    void DonutScene::updateHandler(void* data)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        av_frame_unref(decoded_frame_);
        if (data)
        {
            auto frame = static_cast<AVFrame*>(data);

            {

                if (!decoded_frame_)
                {
                    decoded_frame_ = av_frame_alloc();
                }

                av_frame_ref(decoded_frame_, frame);
                frame_updated_ = false;
                lock.unlock();
            }

            av_frame_unref(frame);
        }
    }


    void DonutScene::threadLoop()
    {
    }

    void DonutScene::timerEvent(QTimerEvent* ev)
    {
        QQuickItem::update();
    }

    void DonutScene::mousePressEvent(QMouseEvent* ev)
    {

    }

    void DonutScene::wheelEvent(QWheelEvent* ev)
    {
        QPoint delta = ev->angleDelta();
        if (!delta.isNull())
        {
            // right handed
            // get closer, then, we should decrease the z level of the camera
            // QML中设置其他控件的z值为100，但是这里zoomlevel小于0.12，就开始会覆盖GUI，所以暂时限制为最小0.15
            if (delta.y() > 0 && zoom_level_ > 0.15f)
            {
                zoom_level_ -= 0.01f;
                scene_camera_.setZoomLevel(zoom_level_);
            }
            else if (delta.y() < 0 && zoom_level_ < 1.5f)
            {
                zoom_level_ += 0.01f;
                scene_camera_.setZoomLevel(zoom_level_);
            }
        }
    }

    void DonutScene::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
    {
        scene_camera_.setViewportSize(newGeometry.width(), newGeometry.height());
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

            connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initAll, Qt::DirectConnection);
            //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForYuvRender, Qt::DirectConnection);
            //connect(window(), &QQuickWindow::beforeRendering, s_renderer_, &DonutSceneRenderer::initForSpriteRender, Qt::DirectConnection);
            connect(window(), &QQuickWindow::beforeRenderPassRecording, this, &DonutScene::onUpdate, Qt::DirectConnection);
        }
        //qDebug() << "width : " << window()->size().width() << " height: " << window()->size().height();
        //qDebug() << "ratio : " << window()->devicePixelRatio();
        //qDebug() << "effective ratio : " << window()->effectiveDevicePixelRatio();

        s_renderer_->setViewportSize(window()->size() * window()->devicePixelRatio());
        //s_renderer_->setT(delta_t_);
        s_renderer_->setWindow(window());
    }
}