#ifndef I_DONUT_AUDIO_PLAYER_H
#define I_DONUT_AUDIO_PLAYER_H

#include "core.h"
#include "donut_av_datas.h"

#include "i_donut_av_base_handler.h"
#include "donut_av_decoder.h"

#include <vector>
#include <list>
#include <mutex>

struct AVFrame;
struct AVCodecParameters;

namespace Donut {
struct DonutAudioData
{
    ~DonutAudioData()
    {
        data.clear();
    }
    std::vector<unsigned char> data;
    int offset = 0;
    double pts = 0;
    double duration = 0;
};

class DONUT_API IDonutAudioPlayer : public IDonutAVBaseHandler
{
public:
    static IDonutAudioPlayer* getInstance();

    virtual void updateHandler(void* data) = 0;

    virtual void pause(bool is_pause) = 0;

    virtual void push(AVFrame* frame);
    virtual bool open(AVCodecParameters* para);
    virtual bool open(DonutAVParamWarpper& para);

    virtual bool open(AudioSpec& spec) = 0;
    virtual void close() = 0;
    virtual void clear();

    virtual long long getCurrentPts() = 0;

    void pushResampled(const unsigned char* data, int size, double pts, double duration);
    void pushResampled(const unsigned char* data, int size, long long pts);
    void pushAudio(const unsigned char* data, int size, long long pts);

    virtual void setSpeed(float s);

    void setVolume(int v){ volume_ = v; }
    void setMute(){}
    void setTimebase(double b) { timebase_ = b; }

    void setClocks(std::shared_ptr<DonutAVClock>& a_clock, std::shared_ptr<DonutAVClock>& v_clock);

    void updateAuidoPts(double pts, int64_t pos, int serial, double time);

    void setFrameQueue(std::shared_ptr<DonutAVFrameQueue>& queue);

    virtual void setPlaybackSpeed(float speed) = 0;

protected:
    IDonutAudioPlayer();
    virtual void callback(unsigned char* stream, int len) = 0;
    static void audioCallback(void* userdata, unsigned char* stream, int len)
    {
        auto ap = (IDonutAudioPlayer*)userdata;
        ap->callback(stream, len);
    }
protected:
    AudioSpec input_spec_;
    AudioSpec resample_spec_;
    std::list<DonutAudioData> audio_datas_;
    std::list<DonutAudioData> resampled_datas_;
    std::list<DonutAudioData> processsed_datas_;

    float playback_speed_ = 1.0;
    double timebase_ = 0;
    unsigned char volume_ = 128;// 0~128

    AVRational tb_{ 0 };
    double cur_pts_ = 0;
    double clock_ = 0;
    double last_pts_ = 0;

    bool is_finished_ = false;

    DonutAVDecoder decoder_;

    std::shared_ptr<DonutAVClock> audio_clock_;
    std::shared_ptr<DonutAVClock> video_clock_;

    std::shared_ptr<DonutAVFrameQueue> audio_frame_queue_;

    int64_t audio_callback_time_ = -1;
};


}
#endif