#include "donut_av_decode_handler.h"

#include "log.h"

extern"C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
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
        if (!param)
        {
            DN_CORE_WARN("param is null");
            return -1;
        }
        std::unique_lock<std::mutex> lock(mtx_);
        AVCodecContext* codec_ctx = decoder_.createContext(param->codec_id, true);
        if (!codec_ctx)
        {
            DN_CORE_WARN("create context failed");
            return -1;
        }
        avcodec_parameters_to_context(codec_ctx, param);
        decoder_.setCodecContext(codec_ctx);
        if (decoder_.openContext() != 0)
        {
            DN_CORE_ERROR("open context failed");
            return -1;
        }

        if (packet_queue_)
        {
            packet_queue_->packetQueueSetStreamIndex(stream_index_);
        }

        return 0;
    }

    int Donut::DonutAVDecodeHandler::openDecoder(std::shared_ptr<DonutAVParamWarpper> param)
    {
        return openDecoder(param->para);
    }

    void Donut::DonutAVDecodeHandler::updateHandler(void* data)
    {
        auto pkt = static_cast<AVPacket*>(data);
        if (packet_queue_)
        {
            std::lock_guard<std::mutex> lock(mtx_);
            {
                std::shared_ptr<DonutAVPacket> d_pkt = std::make_shared<DonutAVPacket>(pkt, true);
                packet_queue_->packetQueuePut(d_pkt);
            }
        }
        av_packet_unref(pkt);
    }

    void DonutAVDecodeHandler::setStream(AVStream* stream)
    {
        if (packet_queue_)
        {
            packet_queue_->packetQueueSetStream(stream);
        }
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
        AVFrame* decoded_frame = av_frame_alloc();
        while (!is_exit_)
        {
            int serial = -1;
            {
                if (packet_queue_->packetQueueHasEnoughPackets())
                {
                    auto pkt = packet_queue_->packetQueueGet(0, &serial);
                    if (pkt->getStreamIndex() != stream_index_)
                    {
                        pkt.reset();
                        continue;
                    }

                    decoder_.sendPacket(pkt);

                    
                    if (decoder_.recvFrame(decoded_frame) == 0)
                    {
                        notify(decoded_frame);
                    }
                    //av_frame_unref(decoded_frame);
                    //av_frame_free(&decoded_frame);
                }
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}