#ifndef DONUT_GL_ITEM_H
#define DONUT_GL_ITEM_H

#include "donut_gl_renderer.h"

#include <QQuickItem>

class DonutGLItem : public QQuickItem
{
	Q_OBJECT
		Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
		QML_ELEMENT
public:
	DonutGLItem(QQuickItem* parent = nullptr);
	~DonutGLItem();

	qreal t() const { return m_t; }
	void setT(qreal t);

signals:
	void tChanged();

public slots:
	void sync();
	void cleanup();

private slots:
	void handleWindowChanged(QQuickWindow* win);

protected:
	void timerEvent(QTimerEvent* ev) override;

private:
	void releaseResources() override;

	qreal m_t;
protected:
	DonutGLItemRenderer* m_renderer;
};

#endif
