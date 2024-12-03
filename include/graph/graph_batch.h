#ifndef GRAPH_BATCH_H
#define GRAPH_BATCH_H

#include "graph/graph.h"

class GraphBatch : public Graph {
protected:
    int *visited{nullptr};
    int *bloomCounter{nullptr};
    char *isAffected{nullptr};
    std::vector<int> affectedBloom;
    std::vector<long long> affectEdge;

public:
    GraphBatch(const std::string filePath) : Graph(filePath) {}

    void construct_index() override;

    void bitruss_decomposition() override;

    void peel_edge(std::vector<long long> edgeList,
                   std::vector<long long> &peelList);
};

#endif