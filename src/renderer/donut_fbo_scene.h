#ifndef DONUT_FBO_SCENE_H
#define DONUT_FBO_SCENE_H

#include <QQuickFramebufferObject>

class DonutFramebufferScene : public QQuickFramebufferObject
{
	Q_OBJECT
public:


protected:
	Renderer* createRenderer() const override;
private:

};

#endif