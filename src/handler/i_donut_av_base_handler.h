#ifndef I_DONUT_AV_BASE_HANDLER_H
#define I_DONUT_AV_BASE_HANDLER_H

#include "i_donut_thread.h"

#include <vector>

class IDonutAVBaseHandler : public IDonutThread
{
public:
	IDonutAVBaseHandler(){}
	virtual ~IDonutAVBaseHandler() {}

	void addNode(IDonutAVBaseHandler* node);
	void notify(void* data);
	virtual void update(void* data) {}
protected:
	virtual void threadLoop() = 0;
private:
	std::vector<IDonutAVBaseHandler*> hander_nodes_;
};


#endif