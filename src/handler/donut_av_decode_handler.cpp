#include "donut_av_decode_handler.h"

extern"C"
{
    #include <libavformat/avformat.h>
}
namespace Donut
{
    Donut::DonutAVDecodeHandler::DonutAVDecodeHandler()
    {
    }

    Donut::DonutAVDecodeHandler::~DonutAVDecodeHandler()
    {
    }

    int Donut::DonutAVDecodeHandler::openDecoder(AVCodecParameters* param)
    {
        return 0;
    }

    int Donut::DonutAVDecodeHandler::openDecoder(std::shared_ptr<DonutAVParamWarpper> param)
    {
        return 0;
    }

    void Donut::DonutAVDecodeHandler::updateHandler(void* data)
    {
        auto pkt = reinterpret_cast<AVPacket*>(data);
        av_packet_unref(pkt);
    }

    AVFrame* Donut::DonutAVDecodeHandler::getDecodedFrame()
    {
        return nullptr;
    }

    void Donut::DonutAVDecodeHandler::setFrameQueue(std::shared_ptr<DonutAVFrameQueue> frame_queue)
    {
    }

    void Donut::DonutAVDecodeHandler::setPacketQueue(std::shared_ptr<DonutAVPacketQueue>& packet_queue)
    {
    }

    void Donut::DonutAVDecodeHandler::threadLoop()
    {
    }
}