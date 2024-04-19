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

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_LINE_SMOOTH);

        rect_shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/scene_graph_shader.glsl");
        //rect_shader_->bind();

        rect_vao_ = std::make_shared<Donut::OpenGLVertexArray>();
        rect_vbo_ = std::make_shared<Donut::OpenGLVertexBuffer>(32);
        rect_vbo_->setLayout({
            {Donut::ShaderDataType::Float4, "vertices"}
        });
        rect_vao_->addVertexBuffer(rect_vbo_);

        rect_vertex_buffer_base_ = new RectangleVertex[max_vertices_];

        //uint32_t* rect_indices = new uint32_t[max_indices_];
        //uint32_t offset = 0;

        //for (uint32_t i = 0; i < max_indices_; i += 6)
        //{
        //    rect_indices[i + 0] = offset + 0;
        //    rect_indices[i + 1] = offset + 1;
        //    rect_indices[i + 2] = offset + 2;

        //    rect_indices[i + 3] = offset + 2;
        //    rect_indices[i + 4] = offset + 3;
        //    rect_indices[i + 5] = offset + 0;

        //    offset += 4;
        //}

        //rect_ebo_ = std::make_shared<Donut::OpenGLIndexBuffer>(rect_indices, max_indices_);
        //rect_vao_->setIndexBuffer(rect_ebo_);
        //delete[] rect_indices;

        rect_vertex_positions_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        rect_vertex_positions_[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        rect_vertex_positions_[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        rect_vertex_positions_[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
         

        //rect_ubo_ = std::make_shared<Donut::OpenGLUniformBuffer>(sizeof(qreal), 0);

        //m_program = new QOpenGLShaderProgram();
        //m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
        //    "attribute highp vec4 vertices;"
        //    "varying highp vec2 coords;"
        //    "void main() {"
        //    "    gl_Position = vertices;"
        //    "    coords = vertices.xy;"
        //    "}");
        //m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
        //    "uniform lowp float t;"
        //    "varying highp vec2 coords;"
        //    "void main() {"
        //    "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
        //    "    i = smoothstep(t - 0.8, t + 0.8, i);"
        //    "    i = floor(i * 20.) / 20.;"
        //    "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
        //    "}");

        //m_program->bindAttributeLocation("vertices", 0);
        //m_program->link();

    }
}


void DonutGLItemRenderer::paint()
{
    m_window->beginExternalCommands();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glClearColor(0.8f, 0.58f, 0.38f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    rect_shader_->bind();

    glm::vec4 color = glm::vec4{ 1, 1, 1, 1 };
    glm::vec2 size = glm::vec2{ 0.5f, 0.5f };
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{0.0f, 0.0f, 1.0f})
        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    rect_vertex_buffer_ptr_ = rect_vertex_buffer_base_;
    float values[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };
    for (size_t i = 0; i < 8; i += 2)
    {
        //auto temp = glm::vec4(values[i], values[i + 1], values[i + 2], values[i + 3]);
        rect_vertex_buffer_ptr_->position_ = glm::vec2(values[i], values[i + 1]);
        //rect_vertex_buffer_ptr_->position_ = glm::vec4(values[i], values[i + 1], values[i + 2], values[i + 3]);
        //rect_vertex_buffer_ptr_->color_ = color;
        //rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
        //rect_vertex_buffer_ptr_->texture_index_ = texture_index;
        //rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;

        rect_vertex_buffer_ptr_++;
    }

    uint32_t data_size = (uint32_t)((uint8_t*)rect_vertex_buffer_ptr_ - (uint8_t*)rect_vertex_buffer_base_);
    rect_vbo_->setData(rect_vertex_buffer_base_, data_size);

    rect_shader_->bind();
    rect_shader_->uploadUniformFloat("u_test", (float)m_t);

    //drawIndices(rect_vao_, rect_indices_count_);

    //float values[] = {
    //    -1.0f, -1.0f,
    //     1.0f, -1.0f,
    //    -1.0f,  1.0f,
    //     1.0f,  1.0f
    //};

    //rect_vao_->bind();
    //rect_vbo_->setData(values, sizeof(values));
    //rect_ubo_->setData(&m_t, sizeof(m_t));

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

//void DonutGLItemRenderer::paint()
//{
//    // Play nice with the RHI. Not strictly needed when the scenegraph uses
//    // OpenGL directly.
//    m_window->beginExternalCommands();
//
//    //m_program->bind();
//    //m_program->enableAttributeArray(0);
//
//    float values[] = {
//        -1, -1,
//        1, -1,
//        -1, 1,
//        1, 1
//    };
//
//    glClearColor(1.0f, 0.58f, 0.30f, 1.0f);
//
//    glClear(GL_COLOR_BUFFER_BIT);
//
//
//    //This example relies on (deprecated) client-side pointers for the vertex
//    //input. Therefore, we have to make sure no vertex buffer is bound.
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
//    m_program->setUniformValue("t", (float)m_t);
//
//    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
//
//    glDisable(GL_DEPTH_TEST);
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//    m_program->disableAttributeArray(0);
//    m_program->release();
//
//    // Not strictly needed for this example, but generally useful for when
//    // mixing with raw OpenGL.
//    QQuickOpenGLUtils::resetOpenGLState();
//
//    m_window->endExternalCommands();
//}