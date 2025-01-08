#include "bloom/bloom.h"
#include<iostream>
using  namespace  std;
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
    if(!edge[edgeID].isDT){
        memberEdge[0].emplace_back(edgeID);
        reverseIndexInMemberEdge[0].emplace_back(indexInMemberEdge);
        return std::make_pair(0,memberEdge[0].size()-1);
    }
    int slackValue = edge[edgeID].get_slack_value();
    if (slackValue >= bloomNumber)
        return std::make_pair(-1, 0);
    int bucket = log2_32(slackValue);// edge future value：slackValue+counter
    memberEdge[bucket].emplace_back(edgeID);
    reverseIndexInMemberEdge[bucket].emplace_back(indexInMemberEdge);
    return std::make_pair(bucket, memberEdge[bucket].size() - 1);
}

pair_t Bloom::add_member_edge(long long edgeID, Edge *edge) {
    if(!edge[edgeID].isDT){
        memberEdge[0].emplace_back(edgeID);
        //reverseIndexInMemberEdge[0].emplace_back(indexInMemberEdge);
        return std::make_pair(0, memberEdge[0].size()-1);
    }
    int slackValue = edge[edgeID].get_slack_value();
    if (slackValue >= bloomNumber)
        return std::make_pair(-1, 0);
    int bucket = log2_32(slackValue);
    memberEdge[bucket].emplace_back(edgeID);
    return std::make_pair(bucket, memberEdge[bucket].size() - 1);
}

void Bloom::send_value_to_member(std::vector<long long> &matureList, std::vector<long long> &peelList,
                                 Edge *edge) {
    int old_counter = counter;  // Store the old counter
    counter++;  // Increment counter

    // Step 1: Process all elements in bucket 0
    for (int i = 0; i < memberEdge[0].size(); i++) {
        long long edgeID = memberEdge[0][i];
        edge[edgeID].accumulate_value(1);
        if (edge[edgeID].check_maturity()) {
            edge[edgeID].isPeel = true;
            peelList.emplace_back(edgeID);
        }
    }
    if(counter <= 3){
        return;
    }
    // Step 2: Check the difference between old_counter and new counter
    int diff = old_counter ^ counter;  // XOR to find the bit positions that changed
    int bucket = 2;  // Start processing from bucket 2 (right to left third bit)

    // Process each bucket where the corresponding bit in diff is 1
    diff >>= 2;
    while (diff) {
        //cout<<"counter:"<<counter<<" old counter:"<<old_counter<<" diff:"<<diff<<endl;
        if (diff & 1) {  // If the current bit is 1, process the corresponding bucket
            for (long long edgeID : memberEdge[bucket]) {
                //std::cout<<"slackValue"<<edge[edgeID].get_slack_value()<<" counter:"<<counter<< " BloomID:"<<id<<" bucket:"<<bucket <<endl;
                edge[edgeID].accumulate_value(1);
                //std::cout<<"slackValue"<<edge[edgeID].get_slack_value()<<" counter:"<<counter<< " BloomID:"<<id<<" bucket:"<<bucket <<endl;
                if (edge[edgeID].check_maturity()) {
                    matureList.emplace_back(edgeID);
                }
            }
        }
        bucket++;  // Move to the next bucket
        diff >>= 1;  // Right shift to check the next bit
    }
}

/*
if(edgeID == 1 && id == 0){
    std::cout<<"slackValue"<<edge[edgeID].get_slack_value()<<" counter:"<<counter<< " BloomID:"<<id<<" bucket:"<<bucket <<endl;
}
*/

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
    int bucket = index.first;

    if (bucket == -1)
        return;
    long long edgeID = memberEdge[bucket][index.second];
    if(bucket == 0){
        edge[edgeID].accumulate_value(deltaValue);
        return;
    }
    int bucketValue = pow2[bucket];

    int sendValue = 1 + (counter + deltaValue - bucketValue)/edge[edgeID].get_slack_value();

    //if(edgeID == 1 && id == 0) cout<<"memberEdge.size:"<<memberEdge.size()<<" memberEdge[0].size:"<<memberEdge[0].size()<<" index.first"<<index.first<<" index.second"<<index.second<<endl;


    edge[edgeID].accumulate_value(sendValue);
}

affect_edge_t Bloom::remove_member_by_index(pair_t index) {
    if (index.first == -1) {
        return std::make_pair(-1, 0);
    }
    //cout<<index.first<<" "<<index.second<<endl;
    int bucket = index.first;
    ui length = memberEdge[bucket].size();
    if (index.second < length - 1) {
        long long affectEdgeID = memberEdge[bucket][length - 1];
        ui affectedIndex = reverseIndexInMemberEdge[bucket][length - 1];//该位置的edge所对应的bloom
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