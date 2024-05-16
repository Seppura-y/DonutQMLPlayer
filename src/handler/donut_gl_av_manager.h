#ifndef DONUT_GL_AV_MANAGER_H
#define DONUT_GL_AV_MANAGER_H

#include "i_donut_av_manager.h"

class DonutGLAVManager : public IDonutAVManager
{
	Q_OBJECT

public:
	DonutGLAVManager(QObject* parent = nullptr);
	virtual ~DonutGLAVManager();


};

#endif