#ifndef DONUT_GL_MANAGER_H
#define DONUT_GL_MANAGER_H

#include "i_donut_av_manager.h"

#include <QObject>
#include <QTime>
#include <vector>
namespace Donut {
	class DONUT_API DonutGLManager : public IDonutAVManager
	{
		Q_OBJECT

	public:
		DonutGLManager(QObject* parent = nullptr);
		~DonutGLManager();

		static DonutGLManager* getInstance();
		int openMediaFile(QString file_path, void* hwnd) override;

		virtual int initManager() override;
		virtual int initManager(int width, int height, DonutPixFormat fmt, void* win_id = nullptr) override;
		virtual int initManager(int width, int height, int fmt, void* win_id = nullptr) override { return 0; };

		virtual void destroyManager() override {};
		virtual void resetManager() override {};

		virtual int renderPackedData(uint8_t* buffer, int size) override { return -1; };
		virtual int renderYuvData(uint8_t* y_buffer, int y_size, uint8_t* u_buffer, int u_size, uint8_t* v_buffer, int v_size) override { return -1; };
		
		virtual void setTargetUrl(std::string url) override {}
		virtual void update() override {}
	public:
		void updateTimePos(qint64 value) override;
		void updateTotalDuration(QTime value) override;
		void updateSoundVolume(int value) override;
		void mediaEndReached() override;

		void setPlaying() override;
		void setPause() override;
		void setStop() override;

		void setTimePos(double value) override;
		//void seekForward() override;
		//void seekBackward() override;

		void setSoundVolume(int value) override;

	protected:
		virtual void handle(void* data) override {};
		void threadLoop() override {}

	public slots:

		void onSeekingTimePos(double value) override;
		void onSeekForward() override;
		void onSeekBackward() override;

		void onSetSoundVolume(int value) override;
		void onPlayOrPause(bool status) override;
		void onStop() override;

		void onSetPlaybackRate(float rate) override;
	private:
		static DonutGLManager* instance_;

		bool is_playing_;
		bool is_initialized_;

		qint64 cur_time_ = -1;

		float playback_rate_ = 1.0f;
		int startup_volume_ = 100;
	};
}
#endif