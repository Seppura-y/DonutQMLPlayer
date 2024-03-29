#ifndef I_DONUT_AV_BASE_HANDLER_H
#define I_DONUT_AV_BASE_HANDLER_H

#include <thread>
#include <mutex>
#include <functional>

#include "core.h"
#include "donut_av_data_tools.h"

struct AVPacket;

namespace Donut
{
enum class DonutAVHandlerPackageType
{
	DONUT_AVHANDLER_PACKAGE_TYPE_PACKET = 0,
	DONUT_AVHANDLER_PACKAGE_TYPE_FRAME
};

enum class DonutAVHandlerPackageAVType
{
	DONUT_AVHANDLER_PACKAGE_AV_TYPE_AUDIO = 0,
	DONUT_AVHANDLER_PACKAGE_AV_TYPE_VIDEO
};

typedef struct DonutAVHandlerPackage
{
	DonutAVHandlerPackageAVType av_type_;
	DonutAVHandlerPackageType type_;
	union AVHandlerPackagePayload
	{
		AVPacket* packet_;
		AVFrame* frame_;
	} payload_;
} DonutAVHandlerPackage;


class DONUT_API IDonutAVBaseHandler
{
public:
	IDonutAVBaseHandler() {}
	virtual ~IDonutAVBaseHandler() {}

	virtual void start();
	virtual void stop();
	virtual void pause();
	virtual void handle(void* package) = 0;

	void setNextHandler(IDonutAVBaseHandler* node);

	void setAudioCallback(std::function<void(AVPacket*)> audio_callback_);
	void setVideoCallback(std::function<void(AVPacket*)> video_callback_);
	void setAudioCallbackEnable(bool status);
	void setVideoCallbackEnable(bool status);

	bool isExit() { return is_exit_; };
protected:
	virtual void threadLoop() = 0;
	IDonutAVBaseHandler* getNextHandler();
	int64_t scaleToMsec(int64_t duration, AVRational src_timebase);

protected:
	std::mutex mtx_;
	std::thread worker_;
	std::function<void(AVPacket*)> video_callback_ = nullptr;
	std::function<void(AVPacket*)> audio_callback_ = nullptr;
	bool is_video_callback_enabled_ = false;
	bool is_audio_callback_enabled_ = false;
	IDonutAVBaseHandler* next_ = nullptr;

	bool is_exit_ = true;
	bool is_pause_ = false;
	int thread_index_ = 0;

	int64_t last_proc_time_ = 0;
private:
};

}

#endif