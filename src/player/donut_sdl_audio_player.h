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

private:
    int getSoundTouchData(void** pcm_buffer);
private:
    long long cur_pts_ = 0; //当前播放位置
    long long last_ms_ = 0;  //上次的时间戳
    long long pause_begin_ = 0;//暂停开始时间戳

    soundtouch::SoundTouch* sound_touch_ = nullptr;
    // 经soundtouch处理后的音频数据
    soundtouch::SAMPLETYPE* st_sample_buffer_ = nullptr;

    int nb_resampled_ = 0;
    // ffmpeg接口重采样后的音频数据
    uint8_t* resampled_buffer_ = nullptr;

    float pitch_ = 1.0f;

    DonutAVAudioResampler resampler_;

    AVFrame* resampled_frame_ = nullptr;
    //AVFrame* processed_frame_ = nullptr;

    bool is_resampler_init_ = false;


};


}
#endif