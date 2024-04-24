#include "donut_gl_renderer.h"

#include "opengl_shader.h"

#include <QQuickOpenGLUtils>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <algorithm>

DonutGLItemRenderer::~DonutGLItemRenderer()
{
    delete m_program;
}

static const uint32_t max_rects_ = 20000;
static const uint32_t max_vertices_ = max_rects_ * 4;
static const uint32_t max_indices_ = max_rects_ * 6;
static const uint32_t max_texture_slots_ = 32;

glm::vec4 rect_vertex_positions_[4];

struct RectangleVertex
{
    glm::vec2 position_;
    //glm::vec4 color_;
    //glm::vec2 tex_coordinate_;

    //float texture_index_;
    //float tiling_factor_;

    //int entity_id_;
};

struct CircleVertex
{
    glm::vec3 world_position_;
    glm::vec3 local_position_;
    glm::vec4 color_;
    float thickness_;
    float fade_;

    int entity_id_;
};

struct LineVertex
{
    glm::vec3 position_;
    glm::vec4 color_;

    int entity_id_;
};

struct TextVertex
{
    glm::vec3 position_;
    glm::vec4 color_;
    glm::vec2 tex_coordinate_;

    int entity_id_;
};

//BatchRenderData DonutGLItemRenderer::s_data;

void DonutGLItemRenderer::init()
{
    if (!s_data.rect_shader_)
    {
        QSGRendererInterface* rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        s_data.rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/scene_graph_shader.glsl");

        s_data.rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();

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

        s_data.rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(values, 32);

        s_data.rect_vbo_->setLayout({
            {Donut::ShaderDataType::Float2, "vertices"}
        });
        s_data.rect_vao_->addVertexBuffer(s_data.rect_vbo_);

        s_data.rect_vertex_buffer_base_ = new RectangleVertex[max_vertices_];
    }
}



void DonutGLItemRenderer::paint()
{
    m_window->beginExternalCommands();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    //qDebug() << "setViewportSize width : " << m_viewportSize.width() << " height: " << m_viewportSize.height();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.8f, 0.58f, 0.38f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    s_data.rect_shader_->bind();
    s_data.rect_vao_->bind();

    s_data.rect_shader_->uploadUniformFloat("u_test", (float)m_t);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    QQuickOpenGLUtils::resetOpenGLState();

    m_window->endExternalCommands();
}

void DonutGLItemRenderer::setViewportSize(const QSize& size)
{
    m_viewportSize = size;
    //qDebug() << "setViewportSize width : " << size.width() << " height: " << size.height();
}

void DonutGLItemRenderer::drawIndices(const Donut::Ref<Donut::OpenGLVertexArray>& va, uint32_t count)
{
    va->bind();
    uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
}

void DonutGLItemRenderer::flush()
{
    if (s_data.rect_indices_count_)
    {
        uint32_t data_size = (uint32_t)((uint8_t*)s_data.rect_vertex_buffer_ptr_ - (uint8_t*)s_data.rect_vertex_buffer_base_);
        s_data.rect_vbo_->setData(s_data.rect_vertex_buffer_base_, data_size);


        // bind textures
        for (uint32_t i = 0; i < s_data.texture_index_; i++)
        {
            s_data.texture_slots_[i]->bind(i);
        }
        s_data.rect_shader_->bind();
        drawIndices(s_data.rect_vao_, s_data.rect_indices_count_);
        //s_data.statistics_.drawcalls_++;
    }
}

void DonutGLItemRenderer::beginScene()
{
}

void DonutGLItemRenderer::endScene()
{
}

void DonutGLItemRenderer::calculateAspectRatio()
{
    // 计算视口宽高比和视频宽高比
    float viewportAspectRatio = static_cast<float>(m_viewportSize.width()) / static_cast<float>(m_viewportSize.height());
    float videoAspectRatio = static_cast<float>(1024) / static_cast<float>(600);

    // 计算缩放比例
    float scale = 1.0f;
    glm::vec2 translation = glm::vec2(0.0f);
    if (viewportAspectRatio > videoAspectRatio) {
        // 视口较宽，视频上下留黑边
        scale = static_cast<float>(m_viewportSize.height()) / static_cast<float>(600);
        translation.x = (m_viewportSize.width() - 1024 * scale) / 2.0f;

        //scale = static_cast<float>(m_viewportSize.width()) / static_cast<float>(1024);
        //translation.y = (m_viewportSize.height() - 600 * scale) / 2.0f;
    }
    else {
        // 视口较高，视频左右留黑边
        scale = static_cast<float>(m_viewportSize.width()) / static_cast<float>(1024);
        translation.y = (m_viewportSize.height() - 600 * scale) / 2.0f;

        //scale = static_cast<float>(m_viewportSize.height()) / static_cast<float>(600);
        //translation.x = (m_viewportSize.width() - 1024 * scale) / 2.0f;
    }

    // 创建变换矩阵
    glm::mat4 transformMatrix = glm::mat4(1.0f); // 初始化为单位矩阵
    transformMatrix = glm::translate(transformMatrix, glm::vec3(translation, 0.0f)); // 平移
    transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(scale, scale, 1.0f)); // 缩放
    s_data.rect_shader_->uploadUniformMat4fv("u_translation", transformMatrix);
}
