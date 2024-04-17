#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

namespace Donut
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
		virtual void setVsync(bool state) = 0;
		virtual bool isVsync() = 0;
	private:

	};
}

#endif