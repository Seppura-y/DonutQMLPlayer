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

	Q_INVOKABLE void mouseClickedInUI();


	bool	infoDirty() const
	{
		return m_infoChanged;
	}
	void makeInfoDirty(bool dirty)
	{
		m_infoChanged = dirty;
	}
	int videoWidth() const
	{
		return m_videoWidth;
	}
	int videoHeght() const
	{
		return m_videoHeight;
	}
	int videoFormat() const
	{
		return m_videoFormat;
	}

public slots:
	void loadFile(const QUrl& url);

protected slots:
	void onVideoInfoReady(int width, int height, int format);

protected:
	void timerEvent(QTimerEvent* ev) override;

private:
	Donut::OrthographicCameraController camera_controller_;
	int									  m_videoWidth;
	int									  m_videoHeight;
	int									  m_videoFormat;
	bool								  m_infoChanged = false;
};


#endif