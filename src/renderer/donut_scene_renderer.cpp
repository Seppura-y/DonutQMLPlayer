#include "donut_scene_renderer.h"

#include "opengl_shader.h"

#include <QQuickOpenGLUtils>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <algorithm>

DonutSceneRenderer::~DonutSceneRenderer()
{

}

static const uint32_t max_rects_ = 20000;
static const uint32_t max_vertices_ = max_rects_ * 4;
static const uint32_t max_indices_ = max_rects_ * 6;
static const uint32_t max_texture_slots_ = 32;

static glm::vec4 rect_vertex_positions_[4];

void DonutSceneRenderer::init()
{
    Donut::DonutRenderCommand::init();
    if (!batch_data_.rect_shader_)
    {
        QSGRendererInterface* rif = window_->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        batch_data_.rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/scene_graph_shader.glsl");

        batch_data_.rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();

        //float values[] = {
        //    -1.0f, -1.0f,
        //     1.0f, -1.0f,
        //    -1.0f,  1.0f,
        //     1.0f,  1.0f
        //};

        float values[] = {
            -1024.0f, -600.0f,
             1024.0f, -600.0f,
            -1024.0f,  600.0f,
             1024.0f,  600.0f
        };

        batch_data_.rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(values, 32);

        batch_data_.rect_vbo_->setLayout({
            {Donut::ShaderDataType::Float2, "vertices"}
            });
        batch_data_.rect_vao_->addVertexBuffer(batch_data_.rect_vbo_);

        batch_data_.rect_vertex_buffer_base_ = new RectangleVertex[max_vertices_];
    }
}



void DonutSceneRenderer::initForVideoRender()
{
    QSGRendererInterface* rif = window_->rendererInterface();
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

    initializeOpenGLFunctions();

    batch_data_.rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();
    batch_data_.rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(batch_data_.max_vertices_ * sizeof(RectangleVertex));
    batch_data_.rect_vbo_->setLayout({
        { Donut::ShaderDataType::Float3, "a_Position" },
        { Donut::ShaderDataType::Float4, "a_Color" },
        { Donut::ShaderDataType::Float2, "a_TexCoord" },
        { Donut::ShaderDataType::Float2, "a_TexIndex" }
    });

    batch_data_.rect_vao_->addVertexBuffer(batch_data_.rect_vbo_);
    batch_data_.rect_vertex_buffer_base_ = new RectangleVertex[batch_data_.max_vertices_];

    uint32_t* rectangle_indices = new uint32_t[batch_data_.max_indices_];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < batch_data_.max_indices_; i += 6)
    {
        rectangle_indices[i + 0] = offset + 0;
        rectangle_indices[i + 1] = offset + 1;
        rectangle_indices[i + 2] = offset + 2;

        rectangle_indices[i + 3] = offset + 2;
        rectangle_indices[i + 4] = offset + 3;
        rectangle_indices[i + 5] = offset + 0;

        offset += 4;
    }

    std::shared_ptr<Donut::OpenGLIndexBuffer> rectangle_ib;
    rectangle_ib = std::make_shared<Donut::OpenGLIndexBuffer>(Donut::OpenGLIndexBuffer(rectangle_indices, batch_data_.max_indices_));
    batch_data_.rect_vao_->setIndexBuffer(rectangle_ib);
    delete[] rectangle_indices;

    batch_data_.white_texture_ = std::make_shared<Donut::OpenGLTexture2D>(1, 1);
    uint32_t white_texture_data = 0xffffffff;
    batch_data_.white_texture_->setData(&white_texture_data, sizeof(uint32_t));

    int samplers[batch_data_.max_texture_slots_];
    for (uint32_t i = 0; i < batch_data_.max_texture_slots_; i++)
    {
        samplers[i] = i;
    }

    batch_data_.rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/video_shader_keep_ratio.glsl");
    batch_data_.rect_shader_->bind();
    batch_data_.rect_shader_->setIntArray("u_textures", samplers, batch_data_.max_texture_slots_);

    batch_data_.texture_slots_[0] = batch_data_.white_texture_;

    batch_data_.rect_vertex_positions_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    batch_data_.rect_vertex_positions_[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
    batch_data_.rect_vertex_positions_[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
    batch_data_.rect_vertex_positions_[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void DonutSceneRenderer::paint()
{
    window_->beginExternalCommands();

    glViewport(0, 0, viewport_size_.width(), viewport_size_.height());
    //qDebug() << "setViewportSize width : " << viewport_size_.width() << " height: " << viewport_size_.height();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.8f, 0.58f, 0.38f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    batch_data_.rect_shader_->bind();
    batch_data_.rect_vao_->bind();

    batch_data_.rect_shader_->uploadUniformFloat("u_test", (float)delta_t_);

    calculateAspectRatio();

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    QQuickOpenGLUtils::resetOpenGLState();

    window_->endExternalCommands();
}

void DonutSceneRenderer::setViewportSize(const QSize& size)
{
    viewport_size_ = size;

    width_ = size.width();
    height_ = size.height();
    aspect_ratio_ = (float)width_ / (float)height_;

    scene_camera_.setOrthographicSize(height_);
    scene_camera_.setViewportSize(width_, height_);
    qDebug() << "DonutSceneRenderer::setViewportSize width : " << size.width() << " height: " << size.height();
}

void DonutSceneRenderer::drawRectangle(glm::vec3 position)
{

}

void DonutSceneRenderer::drawIndices(const Donut::Ref<Donut::OpenGLVertexArray>& va, uint32_t count)
{
    va->bind();
    uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
}

void DonutSceneRenderer::flush()
{
    if (batch_data_.rect_indices_count_)
    {
        uint32_t data_size = (uint32_t)((uint8_t*)batch_data_.rect_vertex_buffer_ptr_ - (uint8_t*)batch_data_.rect_vertex_buffer_base_);
        batch_data_.rect_vbo_->setData(batch_data_.rect_vertex_buffer_base_, data_size);


        // bind textures
        for (uint32_t i = 0; i < batch_data_.texture_index_; i++)
        {
            batch_data_.texture_slots_[i]->bind(i);
        }
        batch_data_.rect_shader_->bind();
        drawIndices(batch_data_.rect_vao_, batch_data_.rect_indices_count_);
        //batch_data_.statistics_.drawcalls_++;
    }
}

void DonutSceneRenderer::beginScene()
{
}

void DonutSceneRenderer::beginScene(Donut::Camera camera)
{
    glm::mat4 view_projection = camera.getProjection();
    batch_data_.rect_shader_->bind();
    batch_data_.rect_shader_->setMat4("u_viewProjectionMatrix", view_projection);

    batch_data_.rect_indices_count_ = 0;
    batch_data_.rect_vertex_buffer_ptr_ = batch_data_.rect_vertex_buffer_base_;
    batch_data_.texture_index_ = 1;
}

void DonutSceneRenderer::beginScene(Donut::Camera camera, const glm::mat4& transform)
{
    glm::mat4 view_projection = camera.getProjection() * glm::inverse(transform);
    batch_data_.rect_shader_->bind();
    batch_data_.rect_shader_->setMat4("u_viewProjectionMatrix", view_projection);

    batch_data_.rect_indices_count_ = 0;
    batch_data_.rect_vertex_buffer_ptr_ = batch_data_.rect_vertex_buffer_base_;
    batch_data_.texture_index_ = 1;
}

void DonutSceneRenderer::endScene()
{
    uint32_t data_size = (uint8_t*)batch_data_.rect_vertex_buffer_ptr_ - (uint8_t*)batch_data_.rect_vertex_buffer_base_;
    batch_data_.rect_vbo_->setData(batch_data_.rect_vertex_buffer_base_, data_size);
}

void DonutSceneRenderer::calculateAspectRatio()
{
    // 计算视口宽高比和视频宽高比
    float viewportAspectRatio = static_cast<float>(viewport_size_.width()) / static_cast<float>(viewport_size_.height());
    float videoAspectRatio = static_cast<float>(1024) / static_cast<float>(600);

    // 计算缩放比例
    float scale = 1.0f;
    glm::vec2 translation = glm::vec2(0.0f);
    if (viewportAspectRatio > videoAspectRatio) {
        // 视口较宽，视频上下留黑边
        scale = static_cast<float>(viewport_size_.height()) / static_cast<float>(600);
        translation.x = (viewport_size_.width() - 1024 * scale) / 2.0f;

        //scale = static_cast<float>(viewport_size_.width()) / static_cast<float>(1024);
        //translation.y = (viewport_size_.height() - 600 * scale) / 2.0f;
    }
    else {
        // 视口较高，视频左右留黑边
        scale = static_cast<float>(viewport_size_.width()) / static_cast<float>(1024);
        translation.y = (viewport_size_.height() - 600 * scale) / 2.0f;

        //scale = static_cast<float>(viewport_size_.height()) / static_cast<float>(600);
        //translation.x = (viewport_size_.width() - 1024 * scale) / 2.0f;
    }

    // 创建变换矩阵
    glm::mat4 transformMatrix = glm::mat4(1.0f); // 初始化为单位矩阵
    transformMatrix = glm::translate(transformMatrix, glm::vec3(translation, 0.0f)); // 平移
    transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(scale, scale, 1.0f)); // 缩放
    batch_data_.rect_shader_->uploadUniformMat4fv("u_translation", transformMatrix);
}
