#ifndef DONUT_FBO_SCENE_H
#define DONUT_FBO_SCENE_H

#include <QQuickFramebufferObject>
#include <QOffscreenSurface>
class DonutFramebufferScene : public QQuickFramebufferObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DonutFramebufferScene(QQuickItem* parent = nullptr);
signals:
	void sigItemInitialized();

private slots:
	void onItemInitialized();
protected:
	Renderer* createRenderer() const override;

	void timerEvent(QTimerEvent* ev) override;
private:
	QOpenGLContext* context_ = nullptr;
	QOffscreenSurface* offscreen_surface_ = nullptr;
};

#endif