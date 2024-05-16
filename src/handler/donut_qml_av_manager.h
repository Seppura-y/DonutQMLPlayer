#ifndef DONUT_QML_AV_MANAGER_H
#define DONUT_QML_AV_MANAGER_H


#include "i_donut_av_manager.h"

#include <QQuickItem>
namespace Donut
{
	class DonutQMLAVManager :  public IDonutAVManager
	{
		Q_OBJECT
		QML_ELEMENT
	public:
		DonutQMLAVManager(QObject* parent = nullptr);
		virtual ~DonutQMLAVManager();


	};
}
#endif