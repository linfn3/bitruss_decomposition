#include "bloom/bloom.h"

Bloom::~Bloom() {
    for (auto v : memberEdge) {
        v.clear();
        v.shrink_to_fit();
    }
    memberEdge.clear();
    memberEdge.shrink_to_fit();
    for (auto v : reverseIndexInMemberEdge) {
        v.clear();
        v.shrink_to_fit();
    }
}

void Bloom::initialize_space() {
    maxSize = log2_32(bloomNumber - 1) + 1;
    memberEdge.resize(maxSize);
    reverseIndexInMemberEdge.resize(maxSize);
}

void Bloom::initialize_space(int value) {
    maxSize = log2_32(value) + 1;
    memberEdge.resize(maxSize);
    reverseIndexInMemberEdge.resize(maxSize);
}

void Bloom::initialize_space_member_edge_only() {
    maxSize = log2_32(bloomNumber - 1) + 1;
    memberEdge.resize(maxSize);
}

void Bloom::initialize_space_member_edge_only(int value) {
    maxSize = log2_32(value) + 1;
    memberEdge.resize(maxSize);
}

pair_t Bloom::add_member_edge(long long edgeID, ui indexInMemberEdge,
                              Edge *edge) {
    int slackValue = edge[edgeID].get_slack_value();
    if (slackValue >= bloomNumber)
        return std::make_pair(-1, 0);
    int bucket = log2_32(slackValue+counter);
    memberEdge[bucket].emplace_back(edgeID);
    reverseIndexInMemberEdge[bucket].emplace_back(indexInMemberEdge);
    return std::make_pair(bucket, memberEdge[bucket].size() - 1);
}

pair_t Bloom::add_member_edge(long long edgeID, Edge *edge) {
    int slackValue = edge[edgeID].get_slack_value();
    if (slackValue >= bloomNumber)
        return std::make_pair(-1, 0);
    int bucket = log2_32(slackValue);
    memberEdge[bucket].emplace_back(edgeID);
    return std::make_pair(bucket, memberEdge[bucket].size() - 1);
}

void Bloom::send_value_to_member(int deltaValue,
                                 std::vector<long long> &matureList,
                                 Edge *edge) {
    int bucket = 0;
    int bucketValue = 1;
    while (bucket < maxSize) {
        int base = counter / bucketValue;
        if (memberEdge[bucket].empty()) {
            bucket++;
            bucketValue <<= 1;
            continue;
        }
        if (counter + deltaValue < (base + 1) * bucketValue) {
            break;
        }
        int sendValue = 1;
        sendValue +=
                (counter + deltaValue - (base + 1) * bucketValue) / bucketValue;
        for (ui i = 0; i < memberEdge[bucket].size(); i++) {
            long long edgeID = memberEdge[bucket][i];
            edge[edgeID].accumulate_value(sendValue);
            if (edge[edgeID].check_maturity()) {
                matureList.emplace_back(edgeID);
            }
        }
        bucket++;
        bucketValue <<= 1;
    }
}


void Bloom::send_value_to_member_1(int bucket, std::vector<long long> &matureList,
                                 Edge *edge) {
        if(bucket > memberEdge.size()){
            return;
        }

        //std::cout<<"id:"<<id<<"bucketsize:"<<memberEdge[bucket].size();
        for (ui i = 0; i < memberEdge[bucket].size(); i++) {
        //std::cout<<memberEdge.size()<<std::endl;
        //std::cout<<"bucket:"<<bucket<<" i:"<<i<<std::endl;

        long long edgeID = memberEdge[bucket][i];
        //std::cout<<edgeID<<std::endl;
        edge[edgeID].accumulate_value(1);

        if (edge[edgeID].check_maturity()) {
            matureList.emplace_back(edgeID);
        }
    }


}

void Bloom::send_value_to_member(int deltaValue,
                                 std::vector<long long> &matureList, Edge *edge,
                                 char *isAffected) {
    int bucket = 0;
    int bucketValue = 1;
    while (bucket < maxSize) {
        int base = counter / bucketValue;
        if (memberEdge[bucket].empty()) {
            bucket++;
            bucketValue <<= 1;
            continue;
        }
        if (counter + deltaValue < (base + 1) * bucketValue) {
            break;
        }
        int sendValue = 1;
        sendValue +=
                (counter + deltaValue - (base + 1) * bucketValue) / bucketValue;
        for (ui i = 0; i < memberEdge[bucket].size(); i++) {
            long long edgeID = memberEdge[bucket][i];
            edge[edgeID].accumulate_value(sendValue);
            if (edge[edgeID].check_maturity() && !isAffected[edgeID]) {
                matureList.emplace_back(edgeID);
                isAffected[edgeID] = 1;
            }
        }
        bucket++;
        bucketValue <<= 1;
    }
}

void Bloom::send_value_to_member(int deltaValue, pair_t index, Edge *edge) {
    if (index.first == -1)
        return;
    int bucketValue = pow2[index.first];
    int base = counter / bucketValue;
    if (counter + deltaValue < (base + 1) * bucketValue) {
        return;
    }
    int sendValue = 1;
    sendValue +=
            (counter + deltaValue - (base + 1) * bucketValue) / bucketValue;
    long long edgeID = memberEdge[index.first][index.second];
    edge[edgeID].accumulate_value(sendValue);
}

affect_edge_t Bloom::remove_member_by_index(pair_t index) {
    if (index.first == -1) {
        return std::make_pair(-1, 0);
    }
    int bucket = index.first;
    ui length = memberEdge[bucket].size();
    if (index.second < length - 1) {
        long long affectEdgeID = memberEdge[bucket][length - 1];
        ui affectedIndex = reverseIndexInMemberEdge[bucket][length - 1];
        memberEdge[bucket][index.second] = affectEdgeID;
        reverseIndexInMemberEdge[bucket][index.second] = affectedIndex;
        memberEdge[bucket].pop_back();
        reverseIndexInMemberEdge[bucket].pop_back();
        return std::make_pair(affectEdgeID, affectedIndex);
    } else {
        memberEdge[bucket].pop_back();
        reverseIndexInMemberEdge[bucket].pop_back();
        return std::make_pair(-1, 0);
    }
}

long long Bloom::remove_member_by_index_id_only(pair_t index) {
    if (index.first == -1) {
        return -1;
    }
    int bucket = index.first;
    ui length = memberEdge[bucket].size();
    if (index.second < length - 1) {
        long long affectEdgeID = memberEdge[bucket][length - 1];
        memberEdge[bucket][index.second] = affectEdgeID;
        memberEdge[bucket].pop_back();
        return affectEdgeID;
    } else {
        memberEdge[bucket].pop_back();
        return -1;
    }
}

int log2_32(uint32_t value) {
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}