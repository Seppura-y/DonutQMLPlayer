#include "donut_av_decode_handler.h"

#include "log.h"

extern"C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/time.h>
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

        decoder_.setStreamIndex(stream_index_);

        return 0;
    }

    int Donut::DonutAVDecodeHandler::openDecoder(std::shared_ptr<DonutAVParamWarpper> param)
    {
        timebase_ = *param->time_base;
        framerate_ = param->framerate_;
        return openDecoder(param->para);
    }

    void Donut::DonutAVDecodeHandler::updateHandler(void* data)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (packet_queue_)
        {
            //auto pkt = static_cast<AVPacket*>(data);

            //if(packet_queue_->packetQueueGetStreamIndex(0) == pkt->stream_index
            //    && stream_index_ == pkt->stream_index)
            //{
            //    std::shared_ptr<DonutAVPacket> d_pkt = std::make_shared<DonutAVPacket>(pkt, true);
            //    packet_queue_->packetQueuePut(d_pkt);
            //    av_packet_unref(pkt);
            //}
        }
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

    void DonutAVDecodeHandler::setClocks(std::shared_ptr<DonutAVClock>& master, std::shared_ptr<DonutAVClock>& clock)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        master_clock_ = master;
        clock_ = clock;
    }

    void DonutAVDecodeHandler::setSleepTime(bool need_sync, double time)
    {
        is_need_sync_ = need_sync;
        sleep_ms_ = time * 1000000;
    }

    void DonutAVDecodeHandler::setVideoFramerate(int num, int den)
    {
        framerate_ = { num, den };
    }

    double DonutAVDecodeHandler::getFrameDiffTime(AVFrame* frame)
    {
        if (!master_clock_ || !clock_) return 0;

        if (master_clock_ == clock_)
        {
            is_need_sync_ = false;
            return 0;
        }
        else
        {
            is_need_sync_ = true;
        }

        double pts = frame->pts;
        if (pts == AV_NOPTS_VALUE)
        {
            pts = 0;
        }
        auto para = decoder_.copyCodecParam();
        AVRational* timebase = para->time_base;
        pts *= av_q2d(*timebase);
        if (pts > 0)
        {
            clock_->setClockAt(pts, 0, pts);
        }

        double diff = master_clock_->pts_ - clock_->pts_;

        return diff;
    }

    double DonutAVDecodeHandler::getDelayTime(double diff)
    {
        double delay_time = 0;
        double default_delay_time = 0.04;
        if (diff > 0.003)
        {
            // master超前，调整睡眠时间为2/3
            delay_time = delay_time * 2 / 3;
            if (delay_time < default_delay_time / 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2/3
                delay_time = default_delay_time * 2 / 3;
            }
            else if (delay_time > default_delay_time * 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2倍
                delay_time = default_delay_time * 2;
            }
        }
        else if (diff < -0.003)
        {
            // master落后，调整为delay time的1.5倍
            delay_time = delay_time * 3 / 2;
            if (delay_time < default_delay_time / 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2/3
                delay_time = default_delay_time * 2 / 3;
            }
            else if (delay_time > default_delay_time * 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2倍
                delay_time = default_delay_time * 2;
            }
        }

        // master超前500ms
        if (diff >= 0.5)
        {
            // 直接跳过本帧
            delay_time = 0;
        }
        else if (diff <= -0.5)
        {
            // master落后超过500ms，2倍默认值(2 * 40ms)
            delay_time = default_delay_time * 2;
        }

        // master超前10秒，不进行同步，直接清空slave队列
        if (diff >= 10)
        {
            packet_queue_->packetQueueFlush();
            delay_time = default_delay_time;
        }

        // master落后10秒，不进行同步，直接清空master队列
        if (diff <= -10)
        {
            packet_queue_->packetQueueFlush();
            delay_time = default_delay_time;
        }

        return delay_time;
    }

    void DonutAVDecodeHandler::setPaused(bool pause)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        is_paused_ = pause;
    }

    void Donut::DonutAVDecodeHandler::threadLoop()
    {
        AVFrame* decoded_frame = av_frame_alloc();
        while (!is_exit_)
        {
            if (is_paused_)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            auto par = decoder_.copyCodecParam();
            bool is_audio = par->para->sample_rate > 0 ? true : false;

            //bool is_audio = stream_index_ == 0 ? true : false;
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

                    if (last_serial_ == -1)
                    {
                        last_serial_ = serial;
                        frame_queue_->setFrameTimer(av_gettime_relative() / 1000000.0);
                    }


                    if (serial != last_serial_)
                    {
                        decoder_.decoderFlush();
                        pkt.reset();
                        last_serial_ = serial;
                        continue;
                    } 

                    decoder_.sendPacket(pkt);

                    
                    if (decoder_.recvFrame(decoded_frame) == 0)
                    {
                        //if (!isnan((double)decoded_frame->pts))
                        //{

                        //    if (decoded_frame->channels > 0 && decoded_frame->sample_rate > 0)
                        //    {
                        //        double pts = decoded_frame->pts + (double)decoded_frame->nb_samples / decoded_frame->sample_rate;
                        //        audio_clock_->pts_ = pts;
                        //    }

                        //}

                        std::shared_ptr<DonutAVFrame> frame = frame_queue_->frameQueuePeekWritable();
                        if (!frame)
                        {
                            av_frame_unref(decoded_frame);
                            std::this_thread::sleep_for(std::chrono::microseconds(1));
                            continue;
                        }
                        //auto new_frame = std::make_shared<DonutAVFrame>(decoded_frame);
                        //frame = new_frame;

                        if (framerate_.num != 0 && framerate_.den != 0)
                        {
                            frame->duration_ = av_q2d(AVRational { framerate_.den, framerate_.num });
                            frame->pts_ = (decoded_frame->pts == AV_NOPTS_VALUE) ? NAN : decoded_frame->pts * av_q2d(timebase_);
                        }

                        double pts = decoded_frame->pts;
                        if (pts == AV_NOPTS_VALUE)
                        {
                            pts = 0;
                        }
                        auto para = decoder_.copyCodecParam();
                        //AVRational* timebase = para->time_base;
                        pts *= av_q2d(timebase_);
                        if (pts > 0)
                        {
                            bool is_audio = para->para->sample_rate > 0 ? true : false;
                            //if (is_audio)
                            //    DN_CORE_ERROR("clock_ : {}", clock_->pts_);

                            clock_->setClockAt(pts, serial, av_gettime_relative() / 1000000.0);
                            //if (is_audio)
                            //    DN_CORE_ERROR("clock_->setClockAt({})", pts);
                            //is_audio ? DN_CORE_ERROR("{} clock_->setClockAt({})", is_audio ? "audio" : "video", pts)
                            //    : 
                            //    DN_CORE_INFO("{} clock_->setClockAt({})", is_audio ? "audio" : "video", pts);
                        }

                        frame->setFrame(decoded_frame, serial, timebase_);
                        frame_queue_->frameQueuePush(frame);


                        //auto pts = decoded_frame->pts;
                        //pts *= av_q2d(timebase_);
                        //auto cur = av_gettime_relative();
                        //clock_->setClockAt(pts, 0, cur);

                        //double diff = getFrameDiffTime(decoded_frame);
                        //sleep_ms_ = (int)(getDelayTime(diff) * 1000000);
                        //if (is_need_sync_)
                        //{
                        //    std::this_thread::sleep_for(std::chrono::microseconds(sleep_ms_));
                        //}
                        //notify(decoded_frame);
                    }
                    //av_frame_unref(decoded_frame);
                    //av_frame_free(&decoded_frame);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}