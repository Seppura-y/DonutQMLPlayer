#ifndef DONUT_SDL_AUDIO_PLAYER_H
#define DONUT_SDL_AUDIO_PLAYER_H

#include "i_donut_audio_player.h"
#include "donut_av_audio_resampler.h"

#include <SDL2/SDL.h>
#include "SoundTouch.h"

struct AVFrame;
struct AVCodecParameters;

namespace Donut {

class DonutSDLAudioPlayer : public IDonutAudioPlayer
{
public:
    DonutSDLAudioPlayer();
    ~DonutSDLAudioPlayer();

    void updateHandler(void* data) override;
    void threadLoop() override;

    void pause(bool is_pause);

    bool open(AudioSpec& spec);
    void close();
    void callback(unsigned char* stream, int len);

    long long getCurrentPts() override;


    bool isNormalPlaybackRate();
private:
    int getSoundTouchData(void** pcm_buffer);
private:
    long long cur_pts_ = 0;     //当前播放位置
    long long last_ms_ = 0;     //上次的时间戳
    long long pause_begin_ = 0; //暂停开始时间戳

    soundtouch::SoundTouch* sound_touch_ = nullptr;

    // 经soundtouch处理后的音频数据 sample rate * channels * sizeof(short)
    soundtouch::SAMPLETYPE* st_source_buffer_ = nullptr;
    soundtouch::SAMPLETYPE* st_resample_buffer_ = nullptr;

    // ffmpeg接口重采样后的音频数据 sample rate * channels * sizeof(short)
    uint8_t* resampled_buffer_ = nullptr;
    uint8_t* processed_buffer_ = nullptr;

    int nb_resampled_ = 0;

    float pitch_ = 1.0f;

    DonutAVAudioResampler resampler_;

    AVFrame* resampled_frame_ = nullptr;
    //AVFrame* processed_frame_ = nullptr;

    bool is_resampler_init_ = false;

    int nb_per_second_ = 0;
    int nb_storage_ = 0;
};


}
#endif