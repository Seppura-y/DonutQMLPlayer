#include "donut_video_renderer.h"

DonutVideoRenderer::DonutVideoRenderer()
{
	//mVertices << QVector3D(-1, 1, 1.0f) << QVector3D(1, 1, 1.0f) << QVector3D(1, -1, 1.0f) << QVector3D(-1, -1, 1.0f);
	mVertices << QVector3D(-1, 1, 0.0f) << QVector3D(1, 1, 0.0f) << QVector3D(1, -1, 0.0f) << QVector3D(-1, -1, 0.0f);
	mTexcoords << QVector2D(0, 1) << QVector2D(1, 1) << QVector2D(1, 0) << QVector2D(0, 0);

	mColors << QVector4D(0.58, 0.1, 0.3, 1.0);

	view_matrix_.setToIdentity();
	view_matrix_.lookAt(QVector3D(0.0f, 0.0f, 1.001f), QVector3D(0.0f, 0.0f, -5.0f), QVector3D(0.0f, 1.0f, 0.0f));
	model_matrix_.setToIdentity();
}

DonutVideoRenderer::~DonutVideoRenderer()
{
}

void DonutVideoRenderer::init()
{
	initializeOpenGLFunctions();
	glDepthMask(GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	initShader();
}

void DonutVideoRenderer::paint()
{
	glDepthMask(true);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//if (!mTextureAlloced)
	//{
	//	return;
	//}
	program_.bind();

	mModelMatHandle = program_.uniformLocation("u_model");
	mViewMatHandle = program_.uniformLocation("u_view");
	mProjectMatHandle = program_.uniformLocation("u_projection");
	mVerticesHandle = program_.attributeLocation("a_Position");
	//mTexCoordHandle = program_.attributeLocation("texCoord");
	mColorHandle = program_.attributeLocation("a_Color");
	// 顶点
	program_.enableAttributeArray(mVerticesHandle);
	program_.setAttributeArray(mVerticesHandle, mVertices.constData());

	program_.enableAttributeArray(mColorHandle);
	program_.setAttributeArray(mColorHandle, mColors.constData());

	// 纹理坐标
	//program_.enableAttributeArray(mTexCoordHandle);
	//program_.setAttributeArray(mTexCoordHandle, mTexcoords.constData());

	// MVP矩阵
	program_.setUniformValue(mModelMatHandle, model_matrix_);
	program_.setUniformValue(mViewMatHandle, view_matrix_);
	program_.setUniformValue(mProjectMatHandle, projection_matrix_);

	// pixFmt
	//program_.setUniformValue("pixFmt", mPixFmt);

	//// 纹理
	////  Y
	//glActiveTexture(GL_TEXTURE0);
	//mTexY->bind();

	//// U
	//glActiveTexture(GL_TEXTURE1);
	//mTexU->bind();

	//// V
	//glActiveTexture(GL_TEXTURE2);
	//mTexV->bind();

	//mProgram.setUniformValue("tex_y", 0);
	//mProgram.setUniformValue("tex_u", 1);
	//mProgram.setUniformValue("tex_v", 2);

	glDrawArrays(GL_TRIANGLE_FAN, 0, mVertices.size());

	program_.disableAttributeArray(mVerticesHandle);
	program_.disableAttributeArray(mColorHandle);
	//program_.disableAttributeArray(mTexCoordHandle);
	program_.release();
}

void DonutVideoRenderer::resize(int width, int height)
{
	glViewport(0, 0, width, height);
	float bottom = -1.0f;
	float top = 1.0f;
	float n = 1.0f;
	float f = 100.0f;
	projection_matrix_.setToIdentity();
	projection_matrix_.frustum(-1.0, 1.0, bottom, top, n, f);
}

void DonutVideoRenderer::initShader()
{
	if (!program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/q_shader_v.glsl"))
	{
		qWarning() << " add vertex shader file failed.";
		return;
	}
	if (!program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/q_shader_f.glsl"))
	{
		qWarning() << " add fragment shader file failed.";
		return;
	}

	program_.bindAttributeLocation("a_Position", 0);
	program_.bindAttributeLocation("a_Color", 1);
	program_.link();
	program_.bind();
}
