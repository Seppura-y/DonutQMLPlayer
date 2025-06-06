#ifndef I_DONUT_AV_MANAGER_H
#define I_DONUT_AV_MANAGER_H

#include <QObject>
#include <QTime>

#include <vector>

#include "media_types.h"
#include "i_donut_av_base_handler.h"

struct AVCodecParameters;
struct IDonutAVBaseHandler;

namespace Donut {
	class DONUT_API IDonutAVManager : public QObject, public IDonutAVBaseHandler
	{
		Q_OBJECT

	public:
		IDonutAVManager(QObject* parent = nullptr);
		~IDonutAVManager();
		static IDonutAVManager* getInstance(DonuAVManagerType type);

		virtual int openMediaFile(QString file_path, void* hwnd) = 0;

		virtual int initManager() = 0;
		virtual int initManager(AVCodecParameters* param);
		virtual int initManager(int width, int height, DonutPixFormat fmt = DonutPixFormat::DONUT_PIX_FORMAT_YUV420P, void* win_id = nullptr) = 0;
		virtual int initManager(int width, int height, int fmt, void* win_id = nullptr) = 0;

		virtual void destroyManager() = 0;
		virtual void resetManager() = 0;

		virtual void setWindowId(void* win_id) { window_id_ = win_id; };

		virtual int renderFrame(AVFrame* frame);
		virtual int renderPackedData(uint8_t* buffer, int size) = 0;
		virtual int renderYuvData(uint8_t* y_buffer, int y_size, uint8_t* u_buffer, int u_size, uint8_t* v_buffer, int v_size) = 0;

		virtual void addHandler(IDonutAVBaseHandler* handler);

		virtual void setTargetUrl(std::string url) = 0;

		virtual void update() = 0;
	public:
		virtual void updateTimePos(qint64 value) = 0;
		virtual void updateTotalDuration(QTime value) = 0;
		virtual void updateSoundVolume(int value) = 0;
		virtual void mediaEndReached() = 0;

		virtual void setPlaying() = 0;
		virtual void setPause() = 0;
		virtual void setStop() = 0;

		virtual void setTimePos(double value) = 0;

		virtual void setSoundVolume(int value) = 0;

	public slots:
		virtual void onSeekingTimePos(double value) = 0;
		virtual void onSeekForward() = 0;
		virtual void onSeekBackward() = 0;

		virtual void onSetSoundVolume(int value) = 0;
		virtual void onPlayOrPause(bool status) = 0;
		virtual void onStop() = 0;

		virtual void onSetPlaybackRate(float rate) = 0;
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
		virtual void handle(void* data) = 0;
		virtual void threadLoop() = 0;
	protected:
		std::mutex mtx_;
		std::thread worker_;

		bool is_init_ = false;
		bool is_exit_ = false;

		int width_ = -1;
		int height_ = -1;
		int scaled_width_ = -1;
		int scaled_height_ = -1;

		int64_t total_duration_ = -1;
		int64_t seek_time_ = -1;
		int seek_serial_ = -1;

		void* window_id_ = nullptr;

		DonutPixFormat pix_fmt_ = DonutPixFormat::DONUT_PIX_FORMAT_YUV420P;

		std::vector<IDonutAVBaseHandler*> handlers_;
	};
}
#endif