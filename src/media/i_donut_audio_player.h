#ifndef I_DONUT_AUDIO_PLAYER_H
#define I_DONUT_AUDIO_PLAYER_H

#include "core.h"
#include "donut_av_datas.h"


#include <vector>
#include <list>
#include <mutex>

struct DonutAudioSpec
{
    int sample_rate = 44100;
    unsigned short format = AUDIO_S16SYS;
    unsigned char channels = 2;
    unsigned short samples = 1024;
};

struct DonutAudioData
{
    std::vector<unsigned char> data;
    int offset = 0;
    long long pts = 0;
};

struct AVFrame;
struct AVCodecParameters;

namespace Donut {

class DONUT_API IDonutAudioPlayer
{
public:
    static IDonutAudioPlayer* getInstance();

    virtual void pause(bool is_pause) = 0;

    virtual void push(AVFrame* frame);
    virtual bool open(AVCodecParameters* para);
    virtual bool open(DonutAVParamWarpper& para);

    virtual bool open(DonutAudioSpec& spec) = 0;
    virtual void close() = 0;
    virtual void clear();

    virtual long long getCurrentPts() = 0;

    void push(const unsigned char* data, int size, long long pts);

    virtual void setSpeed(float s);

    void setVolume(int v){ volume_ = v; }
    void setTimebase(double b) { timebase_ = b; }

protected:
    IDonutAudioPlayer();
    virtual void callback(unsigned char* stream, int len) = 0;
    static void audioCallback(void* userdata, unsigned char* stream, int len)
    {
        auto ap = (IDonutAudioPlayer*)userdata;
        ap->callback(stream, len);
    }
protected:
    std::mutex mtx_;
    DonutAudioSpec spec_;
    std::list<DonutAudioData> audio_datas_;
    float speed_ = 1.;
    double timebase_ = 0;
    unsigned char volume_ = 128;// 0~128
};


}
#endif