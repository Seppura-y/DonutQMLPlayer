#ifndef DONUT_QML_AV_MANAGER_H
#define DONUT_QML_AV_MANAGER_H

#include "i_donut_video_view.h"
#include "donut_av_demux_handler.h"
#include "donut_av_decode_handler.h"

#include "media_type.h"

#include <QQuickItem>

namespace Donut
{
	class DonutQMLAVManager : public QQuickItem, public IDonutThread
	//class DonutQMLAVManager : public QObject, public IDonutAVManager
	{
		Q_OBJECT
		QML_ELEMENT
	public:
		//DonutQMLAVManager(QObject* parent = nullptr);
		DonutQMLAVManager(QQuickItem* parent = nullptr);
		virtual ~DonutQMLAVManager();

		virtual void threadLoop() {}
		virtual void update() {}


		virtual int initManager();
		//virtual int initManager(AVCodecParameters* param);
		virtual int initManager(int width, int height, DonutPixFormat fmt = DonutPixFormat::DONUT_PIX_FORMAT_YUV420P, void* win_id = nullptr);
		virtual int initManager(int width, int height, int fmt, void* win_id = nullptr);

		virtual void destroyManager();
		virtual void resetManager();

		virtual void setWindowId(void* win_id) { window_id_ = win_id; };

		//virtual int renderFrame(AVFrame* frame);
		//virtual int renderPackedData(uint8_t* buffer, int size) = 0;
		//virtual int renderYuvData(uint8_t* y_buffer, int y_size, uint8_t* u_buffer, int u_size, uint8_t* v_buffer, int v_size) = 0;

		virtual void setTargetUrl(std::string url);

		// for vlc
	public:
		virtual void updateTimePos(qint64 value);
		virtual void updateTotalDuration(QTime value);
		virtual void updateSoundVolume(int value);
		virtual void mediaEndReached();
		// general
	public:
		virtual void setPlaying();
		virtual void setPause();
		virtual void setStop();

		virtual void setTimePos(double value);

		virtual void setSoundVolume(int value);

	public slots:
		virtual void onSeekingTimePos(double value);
		virtual void onSeekForward();
		virtual void onSeekBackward();

		virtual void onSetSoundVolume(int value);
		virtual void onPlayOrPause(bool status);
		virtual void onStop();

		virtual void onSetPlaybackRate(float rate);

	signals:
		void sigUpdateTimePos(QTime value);
		void sigUpdateTotalDuration(QTime value);
		void sigUpdateSoundVolume(int value);
		//void sigMediaEndReached();

		void sigStopFinished();
		void sigMediaFinished();
		void sigMediaEOF();

		void sigVideoPlaySeconds(int nSeconds);
		void sigVideoTotalSeconds(int nSeconds);

		void sigPlayMsg(QString strMsg);// 错误信息
		void sigFrameDimensionsChanged(int nFrameWidth, int nFrameHeight); // 视频宽高发生变化
		void sigStartPlay(QString strFileName);
		//void sigStopFinished();

		void sigVideoVolume(double dPercent);
		void sigPauseStat(bool bPaused);
		void sigStop();


	protected:
		int width_ = -1;
		int height_ = -1;

		int scaled_width_ = -1;
		int scaled_height_ = -1;

		int64_t total_duration_ = -1;

		int64_t seek_time_ = -1;
		int seek_serial_ = -1;

		void* window_id_ = nullptr;

		DonutPixFormat pix_fmt_ = DonutPixFormat::DONUT_PIX_FORMAT_YUV420P;

		IDonutVideoView* video_view_ = nullptr;
		DonutAVDemuxHandler* demux_handler_ = nullptr;
		DonutAVDecodeHandler* a_decode_handler_ = nullptr;
		DonutAVDecodeHandler* v_decode_handler_ = nullptr;
	};
}
#endif