#include "graph/graph_batch.h"

void GraphBatch::construct_index() {
    Graph::construct_index();
    bloomCounter = new int[bloomCount];
    isAffected = new char[m];
    std::memset(bloomCounter, 0, sizeof(int) * bloomCount);
    std::memset(isAffected, 0, sizeof(char) * m);
    affectedBloom.reserve(bloomCount / 10);
    affectEdge.reserve(m / 100);
}

void GraphBatch::bitruss_decomposition() {
    long long visitedEdge = 0;
    std::vector<long long> peelList;
    std::vector<long long> peelListTmp;
    std::vector<long long> matureList;
    double start = get_current_time();
    std::cout << "bitruss decomposing in batches..." << std::endl;
    while (visitedEdge < edgeToPeel) {
        if (peelList.empty()) {
            extraBloom.send_value_to_member(1, matureList, edge, isAffected);
            extraBloom.increse_counter(1);
            for (ui i = 0; i < matureList.size(); i++) {
                long long edgeID = matureList[i];
                check_mature_edge(edgeID, peelList);
                isAffected[edgeID] = 0;
            }
            matureList.clear();
        } else {
            peel_edge(peelList, peelListTmp);
            visitedEdge += (int)peelList.size();
            peelList.swap(peelListTmp);
            peelListTmp.clear();
        }
    }
    std::cout << std::fixed << std::setprecision(6)
              << "Bitruss decomposition time:\t" << get_current_time() - start
              << "sec\n";
}

void GraphBatch::peel_edge(std::vector<long long> edgeList,
                           std::vector<long long> &peelList) {
    affectedBloom.clear();
    affectEdge.clear();
    for (ui i = 0, size = edgeList.size(); i < size; i++) {
        long long edgeID = edgeList[i];
        auto *peelEdge = &edge[edgeID];
        pair_t index = peelEdge->get_reverse_index_in_extra_bloom();
        remove_edge_from_extra_bloom_by_index(index);
        for (ui j = 0; j < peelEdge->get_host_bloom_number(); j++) {
            int bloomID = peelEdge->get_host_bloom_id_by_index(j);
            TwinInfo twinEdgeInfo = peelEdge->get_twin_edge_info_by_index(j);
            pair_t reverseIndex =
                peelEdge->get_reverse_index_in_host_bloom_by_index(j);
            auto *currentBloom = &bloom[bloomID];
            int bloomNumber = currentBloom->bloomNumber;
            long long twinEdgeID = twinEdgeInfo.twinEdgeID;
            remove_edge_from_bloom_by_index(bloomID, reverseIndex);

            ui indexInTwinEdge = twinEdgeInfo.hostBloomIndex;
            pair_t indexInHostBloom =
                edge[twinEdgeID].get_reverse_index_in_host_bloom_by_index(
                    indexInTwinEdge);

            if (bloomNumber <= 1) {
                remove_edge_from_bloom_by_index(bloomID, indexInHostBloom);
                remove_bloom_from_edge_by_index(twinEdgeID, indexInTwinEdge);
                edge[twinEdgeID].decrease_butterfly_support(
                    currentBloom->get_counter());
                currentBloom->bloomNumber--;
                continue;
            }

            // deal with twin edge
            currentBloom->send_value_to_member(bloomNumber - 1,
                                               indexInHostBloom, edge);
            remove_edge_from_bloom_by_index(bloomID, indexInHostBloom);
            remove_bloom_from_edge_by_index(twinEdgeID, indexInTwinEdge);
            edge[twinEdgeID].decrease_butterfly_support(
                currentBloom->get_counter() + bloomNumber - 1);
            if (edge[twinEdgeID].check_maturity() && !isAffected[twinEdgeID]) {
                affectEdge.emplace_back(twinEdgeID);
                isAffected[twinEdgeID] = 1;
            }
            if (bloomCounter[bloomID] == 0) {
                affectedBloom.emplace_back(bloomID);
            }
            bloomCounter[bloomID]++;
        }
        bitrussNumber[edgeID] = extraBloom.get_counter();
    }

    for (ui i = 0; i < affectedBloom.size(); i++) {
        int bloomID = affectedBloom[i];
        bloom[bloomID].bloomNumber -= bloomCounter[bloomID];
        if (bloom[bloomID].bloomNumber == 0) {
            bloomCounter[bloomID]--;
        }
        bloom[bloomID].send_value_to_member(bloomCounter[bloomID], affectEdge,
                                            edge, isAffected);
        bloom[bloomID].increse_counter(bloomCounter[bloomID]);
        bloomCounter[bloomID] = 0;
    }

    for (ui i = 0; i < affectEdge.size(); i++) {
        long long currentEdgeID = affectEdge[i];
        check_mature_edge(currentEdgeID, peelList);
        isAffected[currentEdgeID] = 0;
    }
}