#ifndef DONUT_QML_AV_MANAGER_H
#define DONUT_QML_AV_MANAGER_H

#include "i_donut_video_view.h"
#include "i_donut_audio_player.h"
#include "i_donut_av_base_handler.h"

#include "donut_av_demux_handler.h"
#include "donut_av_decode_handler.h"

#include "media_type.h"

#include <QQuickItem>
#include <QDateTime>

namespace Donut
{

	class DonutQMLAVManager : public QQuickItem, public IDonutAVBaseHandler
	{
		Q_OBJECT
		QML_ELEMENT
	public:
		explicit DonutQMLAVManager(QQuickItem* parent = nullptr);
		virtual ~DonutQMLAVManager();

		static DonutQMLAVManager* getInstance();
		//static QObject* singletonProvider(QQmlEngine* engine, QJSEngine* scriptEngine);


		//virtual void update() {}
		virtual void threadLoop() override;
		virtual void updateHandler(void* data) override;


		Q_INVOKABLE virtual int resetManager();
		Q_INVOKABLE virtual int initManager();
		//virtual int initManager(AVCodecParameters* param);
		virtual int initManager(int width, int height, DonutPixFormat fmt = DonutPixFormat::DONUT_PIX_FORMAT_YUV420P, void* win_id = nullptr);
		virtual int initManager(int width, int height, int fmt, void* win_id = nullptr);

		virtual void destroyManager();

		virtual void setWindowId(void* win_id) { window_id_ = win_id; };

		//virtual int renderFrame(AVFrame* frame);
		//virtual int renderPackedData(uint8_t* buffer, int size) = 0;
		//virtual int renderYuvData(uint8_t* y_buffer, int y_size, uint8_t* u_buffer, int u_size, uint8_t* v_buffer, int v_size) = 0;

		virtual void setTargetUrl(std::string url);

	public:
		virtual void updateTimePos(qint64 value);
		virtual void updateTotalDuration(QTime value);
		virtual void updateSoundVolume(int value);
		virtual void mediaEndReached();
		// general

		int getSerial() { return serial_; };
	public:
		virtual void setPlaying();
		virtual void setPause();
		virtual void setStop();

		virtual void setTimePos(double value);

		Q_INVOKABLE void setSoundVolume(int value);

	public slots:
		virtual void onSeekForward();
		virtual void onSeekBackward();

		virtual void onPlayOrPause(bool status);
		virtual void onStop();

		virtual void onSetPlaybackRate(float rate);

		void setMute();

		//********************* QML *********************
	public:
		Q_INVOKABLE void setDemuxer(DonutAVDemuxHandler* demuxer);
		Q_INVOKABLE void setVideoDecoder(DonutAVDecodeHandler* decoder);
		Q_INVOKABLE void setAudioDecoder(DonutAVDecodeHandler* decoder);
		Q_INVOKABLE void setVideoView(IDonutVideoView* view);

		Q_INVOKABLE void setPlaybackRate(float value);

		Q_INVOKABLE void setPlayOrPause(bool pause);

		Q_INVOKABLE float getPlaybackRate();
		Q_INVOKABLE void setPlayMode(int value);
		Q_INVOKABLE void changePlayMode();

		Q_INVOKABLE void setRotationX(float value);
		Q_INVOKABLE void setRotationY(float value);
		Q_INVOKABLE void setRotationZ(float value);
	public slots:
		void onVideoViewInitialized(QObject* view);

		int onOpenMediaFile(QString path);
		int onMediaPlayStart();

		void onMediaEOF();
		Q_INVOKABLE virtual void onSeekingTimePos(double value);
		//********************* QML *********************

	signals:
		void sigUpdateTimePos(QTime value);
		void sigUpdateTimePosSec(int value);
		void sigUpdateTotalDuration(QTime value);
		void sigUpdateTotalDurationSec(int value);
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
		void timerEvent(QTimerEvent* ev) override;

	private:
		double getFrameDiffTime(AVFrame* frame);
		double getDelayTime(double diff);

	public:
		enum class Playmode
		{
			PLAYMODE_FILE_ONCE = 0,
			PLAYMODE_FILE_LOOP = 1,
			PLAYMODE_LIST_ONCE = 2,
			PLAYMODE_LIST_LOOP = 3
		};

	protected:
		int width_ = -1;
		int height_ = -1;

		int scaled_width_ = -1;
		int scaled_height_ = -1;

		int64_t total_duration_ = -1;

		int64_t seek_time_ = -1;
		int serial_ = -1;

		void* window_id_ = nullptr;

		DonutPixFormat pix_fmt_ = DonutPixFormat::DONUT_PIX_FORMAT_YUV420P;

		DonutAVDemuxHandler* demux_handler_ = nullptr;
		IDonutVideoView* video_view_ = nullptr;
		IDonutAudioPlayer* audio_player_ = nullptr;

		DonutAVDecodeHandler* a_decode_handler_ = nullptr;
		DonutAVDecodeHandler* v_decode_handler_ = nullptr;

		static DonutQMLAVManager* s_instance_;

		std::shared_ptr<DonutAVPacketQueue> v_packet_queue_;
		std::shared_ptr<DonutAVFrameQueue>	v_frame_queue_;
		std::shared_ptr<DonutAVClock> v_clock_;

		std::shared_ptr<DonutAVPacketQueue> a_packet_queue_;
		std::shared_ptr<DonutAVFrameQueue>	a_frame_queue_;
		std::shared_ptr<DonutAVClock> a_clock_;

		std::string current_url_;

		float playback_speed_ = 1.0f;

		int video_index_ = -1;
		int audio_index_ = -1;

		std::shared_ptr<DonutAVClock> audio_clock_;
		std::shared_ptr<DonutAVClock> video_clock_;

		bool is_loop_ = false;

		double frame_timer_ = 0;
		double max_frame_duration_ = 10.0;
		//double max_frame_duration_ = 3600.0;

		bool is_paused_ = true;

		Playmode playmode_ = Playmode::PLAYMODE_FILE_ONCE;
		int sound_volume_ = 100;

		friend class IDonutVideoView;
		friend class DonutScene;
	};
}

#endif