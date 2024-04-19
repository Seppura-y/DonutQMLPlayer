#include "donut_gl_renderer.h"

#include "opengl_shader.h"

#include <QQuickOpenGLUtils>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

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

void DonutGLItemRenderer::init()
{
    if (!rect_shader_)
    {
        QSGRendererInterface* rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/scene_graph_shader.glsl");

        rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();

        float values[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };

        rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(values, 32);

        rect_vbo_->setLayout({
            {Donut::ShaderDataType::Float2, "vertices"}
        });
        rect_vao_->addVertexBuffer(rect_vbo_);

        rect_vertex_buffer_base_ = new RectangleVertex[max_vertices_];
    }
}



void DonutGLItemRenderer::paint()
{
    m_window->beginExternalCommands();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.8f, 0.58f, 0.38f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    rect_shader_->bind();
    rect_vao_->bind();

    rect_shader_->uploadUniformFloat("u_test", (float)m_t);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    QQuickOpenGLUtils::resetOpenGLState();

    m_window->endExternalCommands();
}

void DonutGLItemRenderer::drawIndices(const Donut::Ref<Donut::OpenGLVertexArray>& va, uint32_t count)
{
    va->bind();
    uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
}
