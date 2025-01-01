#include "graph/edge.h"

Edge::~Edge() {
    hostBloom.clear();
    hostBloom.shrink_to_fit();
    reverseIndexInHostBloom.clear();
    reverseIndexInHostBloom.shrink_to_fit();
    twinEdge.clear();
    twinEdge.shrink_to_fit();
    hostBloomIndexInTwin.clear();
    hostBloomIndexInTwin.shrink_to_fit();
}

ui Edge::add_host_bloom_and_twin_edge(int bloomID, long long twinEdgeID) {

    hostBloom.emplace_back(bloomID);
    twinEdge.emplace_back(twinEdgeID);
    return hostBloom.size() - 1;
}

void Edge::compute_slack_value() {
    ui hostNumber = hostBloom.size() + 1;
    slackValue = butterflySupport / 2 / hostNumber;
    if(slackValue <=3){
        isDT = false;
        targetValue = butterflySupport;
    }
    else targetValue = hostNumber;

}

void Edge::compute_slack_value(const int _target) {
    int hostNumber = (int)hostBloom.size() + 1;
    slackValue = _target / 2 / hostNumber;
    if(slackValue <=3){
        isDT = false;
        targetValue = _target;
    }
    else targetValue = hostNumber;
}



affect_bloom_t Edge::remove_host_bloom_by_index(ui index) {
    ui hostNumber = hostBloom.size();
    if (index < hostNumber - 1) {
        twinEdge[index] = twinEdge[hostNumber - 1];
        hostBloomIndexInTwin[index] = hostBloomIndexInTwin[hostNumber - 1];
        twinEdge.pop_back();
        hostBloomIndexInTwin.pop_back();

        int affectBloomID = hostBloom[hostNumber - 1];
        pair_t affectIndex = reverseIndexInHostBloom[hostNumber - 1];

        hostBloom[index] = affectBloomID;
        reverseIndexInHostBloom[index] = affectIndex;
        hostBloom.pop_back();
        reverseIndexInHostBloom.pop_back();
        return std::make_tuple(affectBloomID, affectIndex, twinEdge[index],
                               hostBloomIndexInTwin[index]);
    } else {
        twinEdge.pop_back();
        hostBloomIndexInTwin.pop_back();
        hostBloom.pop_back();
        reverseIndexInHostBloom.pop_back();
        return std::make_tuple(-1, std::make_pair(-1, 0), -1, 0);
    }
}