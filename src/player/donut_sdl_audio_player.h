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
    long long cur_pts_ = 0; //��ǰ����λ��
    long long last_ms_ = 0;  //�ϴε�ʱ���
    long long pause_begin_ = 0;//��ͣ��ʼʱ���

    soundtouch::SoundTouch* sound_touch_ = nullptr;
    // ��soundtouch��������Ƶ����
    soundtouch::SAMPLETYPE* st_sample_buffer_ = nullptr;

    int nb_resampled_ = 0;
    // ffmpeg�ӿ��ز��������Ƶ����
    uint8_t* resampled_buffer_ = nullptr;

    float pitch_ = 1.0f;

    DonutAVAudioResampler resampler_;

    AVFrame* resampled_frame_ = nullptr;
    //AVFrame* processed_frame_ = nullptr;

    bool is_resampler_init_ = false;


};


}
#endif