#ifndef DONUT_SDL_AUDIO_PLAYER_H
#define DONUT_SDL_AUDIO_PLAYER_H

#include "i_donut_audio_player.h"

#include <SDL2/SDL.h>


struct AVFrame;
struct AVCodecParameters;

namespace Donut {

class DonutSDLAudioPlayer : public IDonutAudioPlayer
{
public:
    DonutSDLAudioPlayer();

    void updateHandler(void* data) override;
    void threadLoop() override;

    void pause(bool is_pause);

    bool open(DonutAudioSpec& spec);
    void close();
    void callback(unsigned char* stream, int len);

    long long getCurrentPts() override;
private:
    long long cur_pts_ = 0; //��ǰ����λ��
    long long last_ms_ = 0;  //�ϴε�ʱ���
    long long pause_begin_ = 0;//��ͣ��ʼʱ���
};


}
#endif