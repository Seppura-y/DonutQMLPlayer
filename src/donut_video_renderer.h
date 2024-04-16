#ifndef DONUT_VIDEO_RENDERER_H
#define DONUT_VIDEO_RENDERER_H

#include "opengl/opengl_shader.h"

#include <glm/glm.hpp>

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class DonutVideoRenderer : public QOpenGLFunctions
{
public:
	DonutVideoRenderer();
	~DonutVideoRenderer();

	void init();
	void paint();
	void resize(int width, int height);
	

	void initShader();
private:
	QOpenGLShaderProgram program_;
	Donut::OpenGLShader shader_;

	QVector<QVector3D>	 mVertices;
	QVector<QVector4D>	 mColors;
	QVector<QVector2D>	 mTexcoords;

	QMatrix4x4 model_matrix_;
	QMatrix4x4 view_matrix_;
	QMatrix4x4 projection_matrix_;

	int					 mModelMatHandle, mViewMatHandle, mProjectMatHandle;
	int					 mVerticesHandle;
	int					 mTexCoordHandle;
	int					 mColorHandle;
};

#endif