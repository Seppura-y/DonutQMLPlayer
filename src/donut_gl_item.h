#ifndef DONUT_GL_ITEM_H
#define DONUT_GL_ITEM_H

#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QQuickWindow>

class DonutGLItemRenderer : public QObject, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	DonutGLItemRenderer() : m_t(0), m_program(0){}
	~DonutGLItemRenderer();


	void setT(qreal t) { m_t = t; }
	void setViewportSize(const QSize& size) { m_viewportSize = size; }
	void setWindow(QQuickWindow* window) { m_window = window; }

public slots:
	void init();
	void paint();

private:

	QSize m_viewportSize;
	qreal m_t;
	QOpenGLShaderProgram* m_program;
	QQuickWindow* m_window;
};

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
