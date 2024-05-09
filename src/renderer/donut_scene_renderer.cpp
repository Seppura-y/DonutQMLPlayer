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
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL
            || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        //initializeOpenGLFunctions();

        batch_data_.rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/scene_graph_shader.glsl");

        batch_data_.rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();

        //float values[] = {
        //    -1.0f, -1.0f,
        //     1.0f, -1.0f,
        //    -1.0f,  1.0f,
        //     1.0f,  1.0f
        //};

        //float values[] = {
        //    -1024.0f, -600.0f,
        //     1024.0f, -600.0f,
        //    -1024.0f,  600.0f,
        //     1024.0f,  600.0f
        //};
        float values[] = {
            -0.5f, -0.5f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f
        };
        //float values[] = {
        //    -1.0f, -1.0f, 0.0f, 1.0f,
        //     1.0f, -1.0f, 0.0f, 1.0f,
        //    -1.0f,  1.0f, 0.0f, 1.0f,
        //     1.0f,  1.0f, 0.0f, 1.0f
        //};

        batch_data_.rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(values, 64);

        batch_data_.rect_vbo_->setLayout({
            {Donut::ShaderDataType::Float4, "vertices"}
            });
        batch_data_.rect_vao_->addVertexBuffer(batch_data_.rect_vbo_);

        batch_data_.rect_vertex_buffer_base_ = new RectangleVertex[max_vertices_];
    }
}



void DonutSceneRenderer::initForVideoRender()
{
    Donut::DonutRenderCommand::init();
    if (!batch_data_.rect_shader_)
    {
        QSGRendererInterface* rif = window_->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL
            || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        //int samplers[batch_data_.max_texture_slots_];
        //for (uint32_t i = 0; i < batch_data_.max_texture_slots_; i++)
        //{
        //    samplers[i] = i;
        //}

        batch_data_.rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/texture.glsl");
        batch_data_.rect_shader_->bind();
        //batch_data_.rect_shader_->setIntArray("u_textures", samplers, batch_data_.max_texture_slots_);

        batch_data_.rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();
        batch_data_.rect_vao_->bind();

        batch_data_.rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(batch_data_.max_vertices_ * sizeof(RectangleVertex));
        batch_data_.rect_vbo_->setLayout({
            { Donut::ShaderDataType::Float4, "a_Position" },
            { Donut::ShaderDataType::Float4, "a_Color" },
            { Donut::ShaderDataType::Float2, "a_TexCoord" },
            { Donut::ShaderDataType::Float, "a_TexIndex" }
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

        // 这里用Qt的OpenGL接口，无法在创建EBO时同时传递数据，要另外调用setData，原因未知
        batch_data_.rect_ebo_ = std::make_shared<Donut::OpenGLIndexBuffer>(Donut::OpenGLIndexBuffer(rectangle_indices, batch_data_.max_indices_));
        batch_data_.rect_ebo_->setData(rectangle_indices, batch_data_.max_indices_);
        batch_data_.rect_vao_->setIndexBuffer(batch_data_.rect_ebo_);
        delete[] rectangle_indices;

        //batch_data_.white_texture_ = std::make_shared<Donut::OpenGLTexture2D>(1, 1);
        //uint32_t white_texture_data = 0xffffffff;
        //batch_data_.white_texture_->setData(&white_texture_data, sizeof(uint32_t));

        //batch_data_.texture_slots_[0] = batch_data_.white_texture_;

        batch_data_.rect_vertex_positions_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        batch_data_.rect_vertex_positions_[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        batch_data_.rect_vertex_positions_[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        batch_data_.rect_vertex_positions_[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
}

void DonutSceneRenderer::initForRectRender()
{
    Donut::DonutRenderCommand::init();
    if (!batch_data_.rect_shader_)
    {
        QSGRendererInterface* rif = window_->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL
            || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        int samplers[batch_data_.max_texture_slots_];
        for (uint32_t i = 0; i < batch_data_.max_texture_slots_; i++)
        {
            samplers[i] = i;
        }

        batch_data_.rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/flatcolor.glsl");
        batch_data_.rect_shader_->bind();


        batch_data_.rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();
        batch_data_.rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(batch_data_.max_vertices_ * sizeof(FlatColorVertex));
        batch_data_.rect_vbo_->setLayout({
            { Donut::ShaderDataType::Float4, "a_Position" }
        });

        batch_data_.rect_vao_->addVertexBuffer(batch_data_.rect_vbo_);
        batch_data_.flat_vertex_buffer_base_ = new FlatColorVertex[batch_data_.max_vertices_];
                    
        batch_data_.rect_vertex_positions_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        batch_data_.rect_vertex_positions_[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        batch_data_.rect_vertex_positions_[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        batch_data_.rect_vertex_positions_[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
}

void DonutSceneRenderer::paint()
{
    window_->beginExternalCommands();
    OPENGL_COMMAND(setViewport(0, 0, viewport_size_.width(), viewport_size_.height()));
    //OPENGL_COMMAND(glViewport(0, 0, viewport_size_.width(), viewport_size_.height()));
    //qDebug() << "setViewportSize width : " << viewport_size_.width() << " height: " << viewport_size_.height();

    OPENGL_COMMAND(setClearColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }));
    //glClearColor(0.8f, 0.58f, 0.38f, 1.0f);
    OPENGL_COMMAND(clear());

    batch_data_.rect_shader_->bind();
    batch_data_.rect_vao_->bind();

    batch_data_.rect_shader_->uploadUniformFloat("u_test", (float)delta_t_);

    calculateAspectRatio();

    OPENGL_COMMAND(glDisable(GL_DEPTH_TEST));

    //OPENGL_COMMAND(glEnable(GL_BLEND));
    OPENGL_COMMAND(glBlendFunc(GL_SRC_ALPHA, GL_ONE));

    OPENGL_COMMAND(drawArrays(4));

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    QQuickOpenGLUtils::resetOpenGLState();

    window_->endExternalCommands();
}

void DonutSceneRenderer::setViewportSize(const QSize& size)
{

    width_ = size.width();
    height_ = size.height() - 50;
    aspect_ratio_ = (float)size.width() / (float)size.height();

    viewport_size_ = QSize(width_, height_);

    OPENGL_EXTRA_FUNCTIONS(setViewport(0, -28, width_, height_));

    //scene_camera_.setOrthographicSize(height_);
    //scene_camera_.setViewportSize(width_, height_);
    //qDebug() << "DonutSceneRenderer::setViewportSize width : " << size.width() << " height: " << size.height();
}

void DonutSceneRenderer::drawRectangle(glm::vec3 position, glm::vec2 size, glm::vec4 color)
{
    if (batch_data_.rect_indices_count_ >= batch_data_.max_indices_)
    {
        flushAndReset();
    }
    const float tiling_factor = 1.0f;
    constexpr size_t rect_vertex_count = 4;
    float texture_index = 0.0f;

    constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    for (size_t i = 0; i < rect_vertex_count; i++)
    {
        batch_data_.rect_vertex_buffer_ptr_->position_ = (transform * batch_data_.rect_vertex_positions_[i]);
        batch_data_.rect_vertex_buffer_ptr_->color_ = color;
        batch_data_.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
        batch_data_.rect_vertex_buffer_ptr_->texture_index_ = texture_index;

        batch_data_.rect_vertex_buffer_ptr_++;
        uint32_t data_size = (uint32_t)((uint8_t*)batch_data_.rect_vertex_buffer_ptr_ - (uint8_t*)batch_data_.rect_vertex_buffer_base_);
        int a = 1;
    }
    uint32_t data_size = (uint32_t)((uint8_t*)batch_data_.rect_vertex_buffer_ptr_ - (uint8_t*)batch_data_.rect_vertex_buffer_base_);
    batch_data_.rect_indices_count_ += 6;
}

void DonutSceneRenderer::drawFlatRectangle(glm::vec3 position, glm::vec2 size)
{
    constexpr size_t rect_vertex_count = 4;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    for (size_t i = 0; i < rect_vertex_count; i++)
    {
        batch_data_.flat_vertex_buffer_ptr_->position_ = (transform * batch_data_.rect_vertex_positions_[i]);
        batch_data_.flat_vertex_buffer_ptr_++;
    }
}

//void DonutSceneRenderer::drawIndices(const Donut::Ref<Donut::OpenGLVertexArray>& va, uint32_t count)
//{
//    va->bind();
//    uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
//    OPENGL_COMMAND(glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr));
//}

void DonutSceneRenderer::flush()
{
    window_->beginExternalCommands();
    //OPENGL_EXTRA_FUNCTIONS(setViewport(0, 0, viewport_size_.width(), viewport_size_.height()));

    //OPENGL_EXTRA_FUNCTIONS(setClearColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }));
    //OPENGL_EXTRA_FUNCTIONS(clear());

    // 加上下面两行，否则鼠标移动到按钮时，渲染图会跟着变色
    // --glDisable(GL_DEPTH_TEST) 禁用深度测试，否则渲染图会覆盖UI控件
    // --glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    //OPENGL_EXTRA_FUNCTIONS(glDisable(GL_DEPTH_TEST));
    //OPENGL_EXTRA_FUNCTIONS(glBlendFunc(GL_SRC_ALPHA, GL_ONE));

    //OPENGL_EXTRA_FUNCTIONS(glEnable(GL_DEPTH_TEST));
    //OPENGL_EXTRA_FUNCTIONS(glDepthFunc(GL_GREATER));
    OPENGL_EXTRA_FUNCTIONS(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    if (batch_data_.rect_indices_count_)
    {
        batch_data_.rect_vao_.get()->bind();

        batch_data_.rect_ebo_->bind();

        OPENGL_EXTRA_FUNCTIONS(drawIndices(batch_data_.rect_vao_, batch_data_.rect_indices_count_));

    }


    // 以下测试flat rect 绘制
    //batch_data_.rect_vao_->bind();
    //batch_data_.rect_vbo_->bind();
    ////batch_data_.rect_shader_->bind();
    //uint32_t data_size = ((uint8_t*)batch_data_.flat_vertex_buffer_ptr_ - (uint8_t*)batch_data_.flat_vertex_buffer_base_);
    //batch_data_.rect_vbo_->setData(batch_data_.flat_vertex_buffer_base_, data_size);
    //OPENGL_EXTRA_FUNCTIONS(drawArrays(data_size / sizeof(FlatColorVertex)));

    QQuickOpenGLUtils::resetOpenGLState();
    window_->endExternalCommands();
}

void DonutSceneRenderer::flushAndReset()
{
    endScene();
    batch_data_.rect_indices_count_ = 0;
    batch_data_.rect_vertex_buffer_ptr_ = batch_data_.rect_vertex_buffer_base_;
    batch_data_.texture_index_ = 1;
}

void DonutSceneRenderer::beginScene()
{
}

void DonutSceneRenderer::beginScene(Donut::Camera camera)
{
    //window_->beginExternalCommands();

    glm::mat4 view_projection = camera.getProjection();
    batch_data_.rect_shader_->bind();
    batch_data_.rect_shader_->setMat4("u_viewProjectionMatrix", view_projection);

    batch_data_.rect_indices_count_ = 0;
    batch_data_.rect_vertex_buffer_ptr_ = batch_data_.rect_vertex_buffer_base_;
    batch_data_.flat_vertex_buffer_ptr_ = batch_data_.flat_vertex_buffer_base_;
    batch_data_.texture_index_ = 1;
}

void DonutSceneRenderer::beginScene(Donut::Camera camera, const glm::mat4& transform)
{
    //window_->beginExternalCommands();

    glm::mat4 view_projection = camera.getProjection() * glm::inverse(transform);
    batch_data_.rect_shader_->bind();
    batch_data_.rect_shader_->setMat4("u_viewProjectionMatrix", view_projection);

    batch_data_.rect_indices_count_ = 0;
    batch_data_.rect_vertex_buffer_ptr_ = batch_data_.rect_vertex_buffer_base_;
    batch_data_.flat_vertex_buffer_ptr_ = batch_data_.flat_vertex_buffer_base_;
    batch_data_.texture_index_ = 1;
}

void DonutSceneRenderer::endScene()
{
    uint32_t data_size = (uint8_t*)batch_data_.rect_vertex_buffer_ptr_ - (uint8_t*)batch_data_.rect_vertex_buffer_base_;
    batch_data_.rect_vbo_->setData(batch_data_.rect_vertex_buffer_base_, data_size);
    flush();
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
    //transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(scale, scale, 1.0f)); // 缩放
    batch_data_.rect_shader_->uploadUniformMat4fv("u_translation", transformMatrix);
}
