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
        auto pkt = static_cast<AVPacket*>(data);
        if (packet_queue_)
        {
            std::shared_ptr<DonutAVPacket> d_pkt = std::make_shared<DonutAVPacket>(pkt, true);
            packet_queue_->packetQueuePut(d_pkt);
        }
        av_packet_unref(pkt);
    }

    AVFrame* Donut::DonutAVDecodeHandler::getDecodedFrame()
    {
        return nullptr;
    }

    void Donut::DonutAVDecodeHandler::setFrameQueue(std::shared_ptr<DonutAVFrameQueue> frame_queue)
    {
        this->frame_queue_ = frame_queue;
    }

    void Donut::DonutAVDecodeHandler::setPacketQueue(std::shared_ptr<DonutAVPacketQueue> packet_queue)
    {
        this->packet_queue_ = packet_queue;
    }

    void Donut::DonutAVDecodeHandler::threadLoop()
    {
        while (!is_exit_)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
}