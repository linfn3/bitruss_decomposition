#ifndef EDGE_H
#define EDGE_H

#include <iostream>
#include <tuple>
#include <vector>

typedef std::pair<int, unsigned int> pair_t;
typedef unsigned int ui;
typedef unsigned long long ul;
// typedef std::pair<int, pair_t> affect_bloom_t;
typedef std::tuple<int, pair_t, long long, ui> affect_bloom_t;

struct TwinInfo {
  public:
    long long twinEdgeID{0};
    ui hostBloomIndex{0};

    TwinInfo(long long _id, ui _index)
        : twinEdgeID(_id), hostBloomIndex(_index) {}
};

class Edge {
  private:
    int butterflySupport{0};
    int targetValue{0};
    int slackValue{0};
    int accumulatedValue{0};


    std::vector<int> hostBloom;// bloom id
    std::vector<long long> twinEdge;
    std::vector<pair_t> reverseIndexInHostBloom; //x:bucket（） index y: 该bucket里面的第几个元素
    std::vector<ui> hostBloomIndexInTwin;

    pair_t reverseIndexInExtraBloom;

  public:
    long long id{0};
    int node1, node2;
    bool isDT{true};
    bool isPeel{false};
    Edge() {}

    Edge(long long _id) : id(_id) {}

    ~Edge();

    ui add_host_bloom_and_twin_edge(int bloomID, long long twinEdgeID);

    inline void add_host_bloom_index_of_twin_edge(ui hostBloomIndex) {
        hostBloomIndexInTwin.emplace_back(hostBloomIndex);
    }

    inline void add_butterfly_support(const int support) {
        butterflySupport += support;
    }

    inline void decrease_butterfly_support(const int support) {
        butterflySupport -= support;
    }

    inline const int get_butterfly_support() { return butterflySupport; }

    void compute_slack_value();

    void compute_slack_value(const int _target);

    inline const int get_slack_value() { return slackValue; }

    inline const ui get_host_bloom_number() { return hostBloom.size(); }

    inline const int get_host_bloom_id_by_index(ui index) {
        return hostBloom[index];
    }

    inline const pair_t get_reverse_index_in_host_bloom_by_index(
        const ui index) {
        return reverseIndexInHostBloom[index];
    }
    inline bool check_maturity() {
        if (accumulatedValue < targetValue) {
            return false;
        } else {
            accumulatedValue = 0;
            return true;
        }
    }
    inline const TwinInfo get_twin_edge_info_by_index(const ui index) {
        return TwinInfo(twinEdge[index], hostBloomIndexInTwin[index]);
    }

    inline void set_twin_index_by_index(const ui index, const ui twinIndex) {
        hostBloomIndexInTwin[index] = twinIndex;
    }

    inline void add_reverse_index_in_host_bloom(const pair_t index) {
        reverseIndexInHostBloom.push_back(index);
    }

    inline void accumulate_value(const int value) { accumulatedValue += value; }



    affect_bloom_t remove_host_bloom_by_index(ui index);

    inline void set_reverse_index_by_index(ui index, pair_t reverseIndex) {
        reverseIndexInHostBloom[index] = reverseIndex;
    }

    inline void set_reverse_index_in_extra_bloom(pair_t reverseIndex) {
        reverseIndexInExtraBloom = reverseIndex;
    }

    inline pair_t get_reverse_index_in_extra_bloom() {
        return reverseIndexInExtraBloom;
    }
};

#endif