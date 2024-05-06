#ifndef DONUT_RENDER_COMMAND_H
#define DONUT_RENDER_COMMAND_H

#include "donut_renderer_api.h"

namespace Donut
{
	class DonutRenderCommand
	{
	public:
		DonutRenderCommand() = default;
		~DonutRenderCommand() = default;

		inline static void init()
		{
		}
	private:
		static DonutRendererApi* renderer_api_;
	};
}

#endif