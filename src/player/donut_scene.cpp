#include "donut_scene.h"

#include "log.h"
#include "donut_av_global.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QQuickOpenGLUtils>
#include <QRunnable>

#include <QDebug>

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/constants.hpp>

double getTime()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> tp
        = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());

    auto tmp = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch());

    double time = (long long)tmp.count() / 1000000.0;

    return time;
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
        startTimer(1, Qt::TimerType::PreciseTimer);
        setFlag(ItemHasContents, true);

        QSurfaceFormat format;
        format.setMajorVersion(4);
        format.setMinorVersion(5);
        format.setProfile(QSurfaceFormat::CoreProfile);

        format.setSwapInterval(1);

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

    void DonutScene::reset()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        //is_exit_ = true;
        //if (video_frame_queue_)
        //{
        //    while (video_frame_queue_->frameQueueNbRemaining())
        //    {
        //        video_frame_queue_->frameQueueNext();
        //    }

        //    video_frame_queue_.reset();
        //}


        //this->start();
        is_resetting_ = true;
        if (y_texture_)
        {
            y_texture_.reset();
            y_texture_ = nullptr;
        }

        if (u_texture_)
        {
            u_texture_.reset();
            u_texture_ = nullptr;
        }

        if (v_texture_)
        {
            v_texture_.reset();
            v_texture_ = nullptr;
        }

        video_frame_queue_.reset();
        video_frame_queue_ = nullptr;

        releaseResources();
    }

    void DonutScene::setPause(bool is_pause)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        is_paused_ = is_pause;
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

        //this->start();
    }

    void DonutScene::onUpdate()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!is_paused_)
        {
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
        }

        s_renderer_->beginScene(scene_camera_);

        if (test_texture_)
        {
            s_renderer_->drawTexturedRectangle(glm::vec3{ 0.0f, 0.0f, 0.3f }, glm::vec2{ 1.0f, 1.0f }, test_texture_, glm::vec4{ 1.f, 1.f, 1.f, 1.f });
        }

        if (y_texture_ && u_texture_ && v_texture_)
        {
            glm::vec4 position = glm::vec4{ 0.0f, 0.0f, 0.4f, 1.0f };
            glm::mat4 transform = glm::mat4(1.0f);
            position = position * transform;
            //s_renderer_->drawRotatedYuvData(angle_, glm::vec3{ 0.0f, 0.0f, 0.4f }, glm::vec2{ 1.0f, 1.0f }, y_texture_, u_texture_, v_texture_);
            s_renderer_->drawRotatedYuvData(glm::vec3(angle_x_, angle_y_, angle_z_), glm::vec3(position), glm::vec2{1.0f, 1.0f}, y_texture_, u_texture_, v_texture_);
        }

        s_renderer_->endScene();
    }


    void DonutScene::updateHandler(void* data)
    {

    }


    void DonutScene::threadLoop()
    {
        double remaining_time = 0.01;
        int continue_count = 0;
        while (!is_exit_)
        {
            std::unique_lock<std::mutex> lock(mtx_);
            //if (is_resetting_)
            //{
            //    lock.unlock();
            //    is_resetting_ = false;
            //    std::this_thread::sleep_for(std::chrono::microseconds(1000));
            //    continue;
            //}
            if (!is_paused_)
            {
                av_frame_unref(decoded_frame_);

            retry:
                if (video_frame_queue_ == nullptr || video_frame_queue_->frameQueueNbRemaining() == 0)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    frame_updated_ = true;

                    //if (is_resetting_)
                    //{
                    //    is_resetting_ = false;
                    //    lock.unlock();
                    //    video_frame_queue_->frameQueueReset();
                    //}
                    continue;
                }
                else
                {
                    double last_duration, duration, delay;
                    double time;
                    std::shared_ptr<DonutAVFrame> vp, last_vp;

                    last_vp = video_frame_queue_->frameQueuePeekLast();
                    vp = video_frame_queue_->frameQueuePeek();

                    //if (vp->serial_ != manager_->getSerial())
                    //{
                    //    video_frame_queue_->frameQueueNext();
                    //    goto retry;
                    //}

                    if (last_vp->getSerial() != vp->getSerial())
                    {
                        video_frame_queue_->setFrameTimer(av_gettime_relative() / 1000000.0);
                    }

                    last_duration = computeDuration(vp, last_vp);

                    delay = computeTargetDelay(last_duration);
                    if (delay == 0)
                    {
                        frame_updated_ = true;
                    }

                    time = av_gettime_relative() / 1000000.0;

                    double frame_timer = video_frame_queue_->getFrameTimer();


                    if (time < frame_timer + delay)
                    {
                        double calculate = frame_timer + delay - time;
                        remaining_time = FFMIN(calculate, remaining_time);

                        av_usleep(remaining_time * 1000000.0);

                        remaining_time = 0.01;
                        continue_count++;
                        continue;
                    }
                    else
                    {

                        //DN_CORE_ERROR("Continue Count {}", continue_count);
                        continue_count = 0;

                        //time = getTime();
                        time = av_gettime_relative() / 1000000.0;
                        video_frame_queue_->setFrameTimer(time);
                    }

                    clock_->setClockAt(vp->pts_, vp->serial_, time);

                    if (video_frame_queue_->frameQueueNbRemaining() > 1)
                    {
                        double next_duration = 0;
                        auto next_vp = video_frame_queue_->frameQueuePeekLast();
                        next_duration = computeDuration(next_vp, vp);
                        auto f_timer = video_frame_queue_->getFrameTimer();
                        auto f_end = f_timer + next_duration;
                        //auto new_time = getTime();
                        auto new_time = av_gettime_relative() / 1000000.0;
                        //if (time > video_frame_queue_->getFrameTimer() + next_duration)
                        if (new_time > f_end)
                        {
                            video_frame_queue_->frameQueueNext();
                            frame_updated_ = true;
                            continue;
                        }
                    }

                    auto frame = video_frame_queue_->frameQueuePeekReadable();

                    clock_->setClockAt(vp->pts_, vp->serial_, time);

                    frame = video_frame_queue_->frameQueuePeekLast();
                    if (frame)
                    {
                        {
                            av_frame_ref(decoded_frame_, frame->frame_);
                            frame_updated_ = false;
                        }

                        av_frame_unref(frame->frame_);
                            lock.unlock();
                        video_frame_queue_->frameQueueNext();
                    }
                }
            }
            else
            {
                lock.unlock();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void DonutScene::timerEvent(QTimerEvent* ev)
    {
        if (!frame_updated_)
        {
            QQuickItem::update();
        }
        angle_ += 0.01;
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