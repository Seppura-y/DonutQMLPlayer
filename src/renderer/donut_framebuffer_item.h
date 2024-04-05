#ifndef DONUT_FRAMEBUFFER_ITEM
#define DONUT_FRAMEBUFFER_ITEM

#include <QQuickFramebufferObject>
#include <QQuickItem>

class DonutFramebufferItem : public QQuickFramebufferObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DonutFramebufferItem(QQuickItem* parent = nullptr);

	virtual Renderer* createRenderer() const override;
};

#endif