#ifndef I_DONUT_AV_BASE_HANDLER_H
#define I_DONUT_AV_BASE_HANDLER_H

#include "i_donut_thread.h"

#include <vector>

namespace Donut
{
	class IDonutAVBaseHandler : public IDonutThread
	{
	public:
		IDonutAVBaseHandler() {}
		virtual ~IDonutAVBaseHandler() {}

		void addNode(IDonutAVBaseHandler* node);
		void notify(void* data);
		virtual void updateHandler(void* data) {}
	protected:
		virtual void threadLoop() = 0;
	protected:
		std::vector<IDonutAVBaseHandler*> handler_nodes_;
	};
}

#endif