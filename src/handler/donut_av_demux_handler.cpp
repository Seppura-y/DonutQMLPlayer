#include "donut_av_demux_handler.h"

namespace Donut
{


	Donut::DonutAVDemuxHandler::DonutAVDemuxHandler()
	{
	}

	Donut::DonutAVDemuxHandler::~DonutAVDemuxHandler()
	{
	}

	int Donut::DonutAVDemuxHandler::openAVSource(const char* url, int timeout)
	{
		return 0;
	}

	std::shared_ptr<DonutAVParamWarpper> Donut::DonutAVDemuxHandler::copyVideoParameters()
	{
		return std::shared_ptr<DonutAVParamWarpper>();
	}

	std::shared_ptr<DonutAVParamWarpper> Donut::DonutAVDemuxHandler::copyAudioParameters()
	{
		return std::shared_ptr<DonutAVParamWarpper>();
	}

	int Donut::DonutAVDemuxHandler::copyCodecExtraData(uint8_t* buffer, int& size)
	{
		return 0;
	}

	int Donut::DonutAVDemuxHandler::seek(long long pts)
	{
		return 0;
	}

	int Donut::DonutAVDemuxHandler::getVideoFramerate()
	{
		return 0;
	}

	void Donut::DonutAVDemuxHandler::threadLoop()
	{
	}

}