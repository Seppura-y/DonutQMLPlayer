#ifndef DONUT_FRAMEBUFFER_ITEM
#define DONUT_FRAMEBUFFER_ITEM

#include <QQuickFramebufferObject>
#include <QQuickItem>

#include "orthographic_camera_controller.h"

class DonutFramebufferItem : public QQuickFramebufferObject
{
	Q_OBJECT
	QML_ELEMENT
	friend class DonutFramebufferItemRenderer;
public:
	DonutFramebufferItem(QQuickItem* parent = nullptr);

	virtual QQuickFramebufferObject::Renderer* createRenderer() const override;

	void initCamera(float left, float right, float bottom, float top);

private:
	Donut::OrthographicCameraController camera_controller_;
};


#endif