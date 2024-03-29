#ifndef DONUT_SDL_MANAGER_H
#define DONUT_SDL_MANAGER_H

#include "i_donut_av_manager.h"

#include "donut_av_demux_handler.h"
#include "donut_av_player_handler.h"
#include "donut_av_video_decode_handler.h"
#include "donut_av_video_encode_handler.h"
#include "donut_av_mux_handler.h"


#include <QObject>
#include <QTime>
#include <QTimerEvent>

#include <vector>
#include <mutex>
#include <string>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace Donut {
	class DONUT_API DonutSDLManager : public IDonutAVManager
	{
		Q_OBJECT

	public:
		DonutSDLManager(QObject* parent = nullptr);
		~DonutSDLManager();

		static DonutSDLManager* getInstance();

		int openMediaFile(QString file_path, void* hwnd) override;

		virtual int initManager() override;
		virtual int initManager(int width, int height, DonutPixFormat fmt, void* win_id = nullptr) override;
		virtual int initManager(int width, int height, int fmt, void* win_id = nullptr) override;

		virtual void destroyManager() override;
		virtual void resetManager() override;

		virtual int renderPackedData(uint8_t* buffer, int size) override;
		virtual int renderYuvData(uint8_t* y_buffer, int y_size, uint8_t* u_buffer, int u_size, uint8_t* v_buffer, int v_size) override;
		
		virtual void setTargetUrl(std::string url) override { target_url_ = url; }
		virtual void update() override;
	public:
		void mediaEndReached() override;

		void updateTimePos(qint64 value) override;
		void updateTotalDuration(QTime value) override;
		void updateSoundVolume(int value) override;

		void setPlaying() override;
		void setPause() override;
		void setStop() override;

		void setTimePos(double value) override;
		void setSoundVolume(int value) override;
	protected:
		virtual void handle(void* data) override {};
		virtual void threadLoop() override;
	protected:
		void timerEvent(QTimerEvent* ev) override;

	public slots:

		void onSeekingTimePos(double value) override;
		void onSeekForward() override;
		void onSeekBackward() override;

		void onSetSoundVolume(int value) override;
		void onPlayOrPause(bool status) override;
		void onStop() override;

		void onSetPlaybackRate(float rate) override;

	private:
		int initSdlLibrary();
		void onTimerTimeout();
	private:
		static DonutSDLManager* instance_;
		bool is_initialized_ = false;

		SDL_Window* window_ = nullptr;
		SDL_Texture* texture_ = nullptr;
		SDL_Renderer* renderer_ = nullptr;


		bool is_playing_ = false;

		qint64 cur_time_ = -1;

		float playback_rate_ = 1.0f;
		int startup_volume_ = 100;

		std::string target_url_;
		//DonutAVDemuxHandler* demux_handler_ = nullptr;
		DonutAVPlayerHandler* demux_handler_ = nullptr;
		DonutAVVideoDecodeHandler* v_decode_handler_ = nullptr;
		DonutAVVideoEncodeHandler* v_encode_handler_ = nullptr;
		DonutAVMuxHandler* mux_handler_ = nullptr;
	};
}
#endif