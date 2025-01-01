#ifndef BLOOM_H
#define BLOOM_H

#include <cstdint>
#include "graph/edge.h"

typedef std::pair<long long, ui> affect_edge_t;

class Bloom {
private:
    int counter{0};
    int maxSize{0};
    //std::vector<std::vector<long long>> memberEdge;
    std::vector<std::vector<ui>> reverseIndexInMemberEdge;
    std::vector<long long>NoDT_member;

public:
    std::vector<std::vector<long long>> memberEdge;
    int id{0};
    int bloomNumber{0};
    bool bigBloom{true};

    Bloom() {}

    Bloom(int _id) : id(_id) {}

    ~Bloom();

    void initialize_space();

    void initialize_space(int value);

    void initialize_space_member_edge_only();

    void initialize_space_member_edge_only(int value);

    pair_t add_member_edge(long long edgeID, ui indexInMemberEdge, Edge *edge);

    pair_t add_member_edge(long long edgeID, Edge *edge);

    void send_value_to_member(std::vector<long long> &matureList, std::vector<long long> &peelList,Edge *edge);
    //void send_value_to_member_1(int bucket, std::vector<long long> &matureList, Edge *edge);

    void send_value_to_member(int deltaValue,
                              std::vector<long long> &matureList, Edge *edge,
                              char *isAffected);

    void send_value_to_member(int deltaValue, pair_t index, Edge *edge);

    affect_edge_t remove_member_by_index(pair_t index);

    long long remove_member_by_index_id_only(pair_t index);

    inline void set_reverse_index_by_index(pair_t index, ui reverseIndex) {
        reverseIndexInMemberEdge[index.first][index.second] = reverseIndex;
    }

    inline int get_counter() { return counter; }

    inline void increse_counter(const int c) { counter += c; }
};

int log2_32(uint32_t value);

const int pow2[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

const int tab32[32] = {0,  9,  1,  10, 13, 21, 2,  29, 11, 14, 16,
                       18, 22, 25, 3,  30, 8,  12, 20, 28, 15, 17,
                       24, 7,  19, 27, 23, 6,  26, 5,  4,  31};

#endif