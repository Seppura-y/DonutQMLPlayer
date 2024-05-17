#include "donut_qml_av_manager.h"

#include <QTime>

Donut::DonutQMLAVManager::DonutQMLAVManager(QQuickItem* parent)
	: QQuickItem(parent)
{
}


Donut::DonutQMLAVManager::~DonutQMLAVManager()
{
}

int Donut::DonutQMLAVManager::initManager()
{
	return 0;
}

int Donut::DonutQMLAVManager::initManager(int width, int height, DonutPixFormat fmt, void* win_id)
{
	return 0;
}

int Donut::DonutQMLAVManager::initManager(int width, int height, int fmt, void* win_id)
{
	return 0;
}

void Donut::DonutQMLAVManager::destroyManager()
{
}

void Donut::DonutQMLAVManager::resetManager()
{
}

void Donut::DonutQMLAVManager::setTargetUrl(std::string url)
{
}

void Donut::DonutQMLAVManager::updateTimePos(qint64 value)
{
}

void Donut::DonutQMLAVManager::updateTotalDuration(QTime value)
{
}

void Donut::DonutQMLAVManager::updateSoundVolume(int value)
{
}

void Donut::DonutQMLAVManager::mediaEndReached()
{
}

void Donut::DonutQMLAVManager::setPlaying()
{
}

void Donut::DonutQMLAVManager::setPause()
{
}

void Donut::DonutQMLAVManager::setStop()
{
}

void Donut::DonutQMLAVManager::setTimePos(double value)
{
}

void Donut::DonutQMLAVManager::setSoundVolume(int value)
{
}

void Donut::DonutQMLAVManager::onSeekingTimePos(double value)
{

}

void Donut::DonutQMLAVManager::onSeekForward()
{
}

void Donut::DonutQMLAVManager::onSeekBackward()
{
}

void Donut::DonutQMLAVManager::onSetSoundVolume(int value)
{
}

void Donut::DonutQMLAVManager::onPlayOrPause(bool status)
{
}

void Donut::DonutQMLAVManager::onStop()
{
}

void Donut::DonutQMLAVManager::onSetPlaybackRate(float rate)
{
}

void Donut::DonutQMLAVManager::onVideoViewInitialized(QObject* view)
{

	this->video_view_ = dynamic_cast<IDonutVideoView*>(view);
	if (this->video_view_)
	{
		video_view_->updateHandler(nullptr);
	}
	else
	{
		qDebug() << "video_view_ is nullptr";
	}
}
