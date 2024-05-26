#include "i_donut_av_base_handler.h"

#include <iostream>
#include <sstream>

namespace Donut
{

    void IDonutAVBaseHandler::addNode(IDonutAVBaseHandler* node)
    {
        if (!node) return;
        std::lock_guard<std::mutex> lock(mtx_);
        handler_nodes_.emplace_back(node);
    }

    void IDonutAVBaseHandler::notify(void* data)
    {
        if (!data) return;
        std::lock_guard<std::mutex> lock(mtx_);

        for (auto node : handler_nodes_)
        {
            node->updateHandler(data);
        }
    }

}
