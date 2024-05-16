#include "i_donut_av_manager.h"

namespace Donut
{
    Donut::IDonutAVManager::IDonutAVManager(QObject* parent)
    {
    }

    Donut::IDonutAVManager::~IDonutAVManager()
    {
    }

    IDonutAVManager* Donut::IDonutAVManager::getInstance(DonuAVManagerType type)
    {
        return nullptr;
    }
}