#ifndef DONUT_AV_STREAM_H
#define DONUT_AV_STREAM_H

#include "core.h"
#include "donut_av_timer.h"

#include <memory>

struct AVCodecParameters;

namespace Donut{
class DONUT_API DonutAVStream
{
public:
	~DonutAVStream();

	static DonutAVStream* createInstance();
	AVCodecParameters* getParameters() { return para_; };
private:
	AVCodecParameters* para_ = nullptr;
	DonutAVRational* timebase_ = nullptr;
	DonutAVStream();
};




}

#endif