#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "bloom/bloom.h"
#include "graph/edge.h"
#include "utils/current_time.h"

class Graph {
protected:
    long long m;
    int n, n1;
    long long edgeToPeel{0};
    int bloomCount{0};
    int *degree{nullptr};
    int *nbrAll{nullptr};
    int **nbr{nullptr};

    int *bitrussNumber{nullptr};
    char *isInPeelList{nullptr};

    Edge *edge{nullptr};
    Bloom *bloom{nullptr};
    Bloom extraBloom;

public:
    Graph(const std::string path);

    ~Graph();

    virtual void construct_index();

    void remove_edge_from_bloom_by_index(int bloomID, pair_t index);

    void remove_edge_from_extra_bloom_by_index(pair_t index);

    void remove_bloom_from_edge_by_index(long long edgeID, ui index);

    virtual void bitruss_decomposition();

    void peel_edge(long long edgeID, std::vector<long long> &peelList);

    int collect_counter(long long edgeID);

    void compute_and_restart(long long edgeID, const int trackValue);

    void check_mature_edge(long long edgeID, std::vector<long long> &peelList);

    void output_bitruss_number(std::string outputDir);
};

#endif