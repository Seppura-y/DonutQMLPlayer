#ifndef DONUT_AV_AUDIO_PLAYER_H
#define DONUT_AV_AUDIO_PLAYER_H

#include "donut_av_data_tools.h"

#define AUDIO_U8        0x0008  /**< Unsigned 8-bit samples */
#define AUDIO_S8        0x8008  /**< Signed 8-bit samples */
#define AUDIO_U16LSB    0x0010  /**< Unsigned 16-bit samples */
#define AUDIO_S16LSB    0x8010  /**< Signed 16-bit samples */
#define AUDIO_U16MSB    0x1010  /**< As above, but big-endian byte order */
#define AUDIO_S16MSB    0x9010  /**< As above, but big-endian byte order */
#define AUDIO_U16       AUDIO_U16LSB
#define AUDIO_S16       AUDIO_S16LSB
/* @} */

/**
 *  \name int32 support
 */
 /* @{ */
#define AUDIO_S32LSB    0x8020  /**< 32-bit integer samples */
#define AUDIO_S32MSB    0x9020  /**< As above, but big-endian byte order */
#define AUDIO_S32       AUDIO_S32LSB
/* @} */

/**
 *  \name float32 support
 */
 /* @{ */
#define AUDIO_F32LSB    0x8120  /**< 32-bit floating point samples */
#define AUDIO_F32MSB    0x9120  /**< As above, but big-endian byte order */
#define AUDIO_F32       AUDIO_F32LSB
/* @} */

/**
 *  \name Native audio byte ordering
 */
 /* @{ */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO_U16SYS    AUDIO_U16LSB
#define AUDIO_S16SYS    AUDIO_S16LSB
#define AUDIO_S32SYS    AUDIO_S32LSB
#define AUDIO_F32SYS    AUDIO_F32LSB
#else
#define AUDIO_U16SYS    AUDIO_U16MSB
#define AUDIO_S16SYS    AUDIO_S16MSB
#define AUDIO_S32SYS    AUDIO_S32MSB
#define AUDIO_F32SYS    AUDIO_F32MSB
#endif
#include <vector>
#include <list>
#include <mutex>
struct DonutAVAudioSpec
{
    int sample_rate = 44100;
    unsigned short format = AUDIO_S16SYS;
    unsigned char channels = 2;
    unsigned short samples = 1024;
};

struct DonutAVAudioData
{
    std::vector<unsigned char> data;
    int offset = 0;
    long long pts = 0;
};

struct AVFrame;
struct AVCodecParameters;

namespace Donut {

class DONUT_API DonutAVAudioPlayer
{
public:
    static DonutAVAudioPlayer* getInstance();

    virtual void pause(bool is_pause) = 0;

    virtual void push(AVFrame* frame);
    virtual bool open(AVCodecParameters* para);
    virtual bool open(DonutAVParamWarpper& para);

    virtual bool open(DonutAVAudioSpec& spec) = 0;
    virtual void close() = 0;
    virtual void clear()
    {
        close();
        setSpeed(speed_);
    }

    virtual long long getCurrentPts() = 0;

    void push(const unsigned char* data, int size, long long pts)
    {
        std::unique_lock<std::mutex> lock(mux_);
        audio_datas_.push_back(DonutAVAudioData());
        audio_datas_.back().pts = pts;
        audio_datas_.back().data.assign(data, data + size);
    }

    virtual void setSpeed(float s)
    {
        speed_ = s;
        auto spec = spec_;
        auto old_sample_rate = spec.sample_rate;
        spec.sample_rate *= s;
        open(spec);
        spec_.sample_rate = old_sample_rate;
    }

    void setVolume(int v)
    {
        volume_ = v;
    }

    void setTimebase(double b) { timebase_ = b; }
protected:
    DonutAVAudioPlayer();
    virtual void callback(unsigned char* stream, int len) = 0;
    static void audioCallback(void* userdata, unsigned char* stream, int len)
    {
        auto ap = (DonutAVAudioPlayer*)userdata;
        ap->callback(stream, len);
    }

protected:
    std::mutex mux_;
    DonutAVAudioSpec spec_;
    std::list<DonutAVAudioData> audio_datas_;
    float speed_ = 1.;
    double timebase_ = 0;
    unsigned char volume_ = 128;// 0~128
};


}
#endif