#include "donut_gl_renderer.h"

#include "opengl_shader.h"

#include <QQuickOpenGLUtils>

#include <memory>

DonutGLItemRenderer::~DonutGLItemRenderer()
{
    delete m_program;
}

void DonutGLItemRenderer::init()
{
    if (!m_program) {
        QSGRendererInterface* rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        shader_ = std::make_shared<Donut::OpenGLShader>("assets/shaders/scene_graph_shader.glsl");
        //shader_->bind();
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
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();

    shader_->bind();
    //m_program->bind();

    //m_program->enableAttributeArray(0);

    //glClearColor(1.0f, 0.8f, 0.58f, 1.0f);

    //float values[] = {
    //    -1, -1,
    //    1, -1,
    //    -1, 1,
    //    1, 1
    //};

    //// This example relies on (deprecated) client-side pointers for the vertex
    //// input. Therefore, we have to make sure no vertex buffer is bound.
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    //m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    //m_program->setUniformValue("t", (float)m_t);

    //glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    //glDisable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //m_program->disableAttributeArray(0);
    //m_program->release();

    //// Not strictly needed for this example, but generally useful for when
    //// mixing with raw OpenGL.
    ////QQuickOpenGLUtils::resetOpenGLState();

    m_window->endExternalCommands();
}