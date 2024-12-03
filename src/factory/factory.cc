#include "factory/factory.h"

Graph *GraphFactory::generate_graph(const std::string filePath,
                                    const std::string method) {
    Graph *rtn = nullptr;
    if (method == "standard") {
        rtn = new Graph(filePath);
    } else if (method == "batch") {
        rtn = new GraphBatch(filePath);
    }
    return rtn;
}