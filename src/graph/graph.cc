#include "graph/graph.h"

Graph::Graph(const std::string path) {
    std::ifstream fin;
    std::cout<<"path:"<<path<<std::endl;
    fin.open(path + "/graph-sort.bin", std::ios::binary | std::ios::in);
    fin.read((char *)&n, sizeof(int));
    fin.read((char *)&n, sizeof(int));
    fin.read((char *)&m, sizeof(long long));
    degree = new int[n];
    nbrAll = new int[m];
    nbr = new int *[n];
    fin.read((char *)degree, sizeof(int) * n);
    fin.read((char *)nbrAll, sizeof(int) * m);
    fin.close();

    std::cout << "n = " << n << ", m = " << m / 2 << std::endl;

    long long p = 0;
    for (int i = 0; i < n; i++) {
        nbr[i] = nbrAll + p;
        p += degree[i];
    }

    m /= 2;
    edge = new Edge[m];
    bitrussNumber = new int[m];
    isInPeelList = new char[m];
    edgeToPeel = m;
    for (long long i = 0; i < m; i++) {
        edge[i].id = i;
    }
    std::memset(bitrussNumber, 0, sizeof(int) * m);
    std::memset(isInPeelList, 0, sizeof(char) * m);
}

Graph::~Graph() {
    delete[] degree;
    degree = nullptr;
    delete[] nbrAll;
    nbrAll = nullptr;
    nbr = nullptr;
    delete[] edge;
    edge = nullptr;
    delete[] bloom;
    bloom = nullptr;
}

void Graph::construct_index() {
    long long **e = new long long *[n];
    for (int u = 0; u < n; u++) {
        e[u] = new long long[degree[u]];
    }
    double start = get_current_time();
    long long edgeIdx = 0;
    for (int u = 0; u < n; u++) {
        for (int i = 0; i < degree[u]; i++) {
            int v = nbr[u][i];
            if (u < v) {
                e[u][i] = edgeIdx++;
            } else {
                int l = 0, r = degree[v] - 1, mid = 0;
                while (l < r) {
                    mid = l + (r - l) / 2;
                    if (nbr[v][mid] < u) {
                        l = mid + 1;
                    } else {
                        r = mid;
                    }
                }
                e[u][i] = e[v][l];
            }
        }
    }
    std::cout << std::fixed << std::setprecision(6)
              << "Edge ID assignment time:\t" << get_current_time() - start
              << "sec\n";
    std::vector<int> bloomNumber;
    int u, v, w;
    int *lastUseVertex = new int[n], *lastCount = new int[n],
            *visitedStatusForW = new int[n];
    std::memset(lastCount, 0, sizeof(int) * n);
    std::memset(visitedStatusForW, -1, sizeof(int) * n);

    int **butterflyCount = new int *[n];
    for (int i = 0; i < n; i++) {
        butterflyCount[i] = new int[degree[i]];
        std::memset(butterflyCount[i], 0, sizeof(int) * degree[i]);
    }
    start = get_current_time();
    int lastUseIdx = 0;
    for (u = 0; u < n; u++) {
        for (int j = 0; j < lastUseIdx; j++) {
            lastCount[lastUseVertex[j]] = 0;
            visitedStatusForW[lastUseVertex[j]] = -1;
        }
        lastUseIdx = 0;

        for (int i = 0; i < degree[u]; i++) {
            v = nbr[u][i];
            for (int j = 0; j < degree[v]; j++) {
                w = nbr[v][j];
                if (w >= u || w >= v)
                    break;
                lastCount[w]++;
                if (lastCount[w] == 1)
                    lastUseVertex[lastUseIdx++] = w;
            }
        }
        for (int i = 0; i < degree[u]; i++) {
            v = nbr[u][i];
            for (int j = 0; j < degree[v]; j++) {
                w = nbr[v][j];
                if (w >= u || w >= v)
                    break;
                int lastCountNumber = lastCount[w];
                if (lastCountNumber > 1) {
                    butterflyCount[u][i] += lastCountNumber - 1;
                    butterflyCount[v][j] += lastCountNumber - 1;
                } else
                    continue;
                if (visitedStatusForW[w] == -1) {
                    ui indexV = edge[e[u][i]].add_host_bloom_and_twin_edge(
                            bloomCount, e[v][j]);
                    ui indexU = edge[e[v][j]].add_host_bloom_and_twin_edge(
                            bloomCount, e[u][i]);
                    edge[e[u][i]].add_host_bloom_index_of_twin_edge(indexU);
                    edge[e[v][j]].add_host_bloom_index_of_twin_edge(indexV);
                    bloomNumber.emplace_back(lastCountNumber);
                    visitedStatusForW[w] = bloomCount++;
                } else {

                    ui indexV = edge[e[u][i]].add_host_bloom_and_twin_edge(
                            visitedStatusForW[w], e[v][j]);
                    ui indexU = edge[e[v][j]].add_host_bloom_and_twin_edge(
                            visitedStatusForW[w], e[u][i]);
                    edge[e[u][i]].add_host_bloom_index_of_twin_edge(indexU);
                    edge[e[v][j]].add_host_bloom_index_of_twin_edge(indexV);
                }
            }
        }
    }

    std::cout << std::fixed << std::setprecision(6)
              << "Butterfly counting time:\t" << get_current_time() - start
              << "sec\n";
    delete[] lastUseVertex;
    lastUseVertex = nullptr;
    delete[] lastCount;
    lastCount = nullptr;
    delete[] visitedStatusForW;
    visitedStatusForW = nullptr;

    start = get_current_time();
    bloom = new Bloom[bloomCount];
    for (int i = 0; i < bloomCount; i++) {
        bloom[i].id = i;
        bloom[i].bloomNumber = bloomNumber[i];
        bloom[i].initialize_space();
    }
    bloomNumber.clear();
    bloomNumber.shrink_to_fit();
    for (u = 0; u < n; u++) {
        for (int i = 0; i < degree[u]; i++) {
            edge[e[u][i]].add_butterfly_support(butterflyCount[u][i]);
        }
    }

    int maxSlackValue = 0;
    int maxButterflySupport = 0;
    for (long long i = 0; i < m; i++) {
        if (edge[i].get_butterfly_support() == 0) {
            edgeToPeel--;
            continue;
        }
        auto *currentEdge = &edge[i];
        currentEdge->compute_slack_value();
        int slackValue = currentEdge->get_slack_value();
        maxSlackValue = std::max(maxSlackValue, currentEdge->get_slack_value());
        maxButterflySupport =
                std::max(maxButterflySupport, currentEdge->get_butterfly_support());
    }
    extraBloom.id = bloomCount;
    extraBloom.bloomNumber = maxButterflySupport;
    std::cout<<"maxButterflySupport:"<<maxButterflySupport<<std::endl;
    extraBloom.initialize_space_member_edge_only();
    for (long long i = 0; i < m; i++) {
        if (edge[i].get_butterfly_support() == 0) {
            continue;
        }
        auto *currentEdge = &edge[i];
        for (int j = 0; j < currentEdge->get_host_bloom_number(); j++) {
            int bloomID = currentEdge->get_host_bloom_id_by_index(j);
            pair_t index = bloom[bloomID].add_member_edge(i, j, edge);
            currentEdge->add_reverse_index_in_host_bloom(index);
        }
        pair_t index = extraBloom.add_member_edge(i, edge);
        currentEdge->set_reverse_index_in_extra_bloom(index);
    }
    std::cout << std::fixed << std::setprecision(6)
              << "Index construction time:\t" << get_current_time() - start
              << "sec\n";
    for (u = 0; u < n; u++) {
        delete[] e[u];
        delete[] butterflyCount[u];
    }
    delete[] e;
    delete[] butterflyCount;
    e = nullptr;
    butterflyCount = nullptr;
}

void Graph::remove_edge_from_bloom_by_index(int bloomID, pair_t index) {
    affect_edge_t affectEdgeInfo = bloom[bloomID].remove_member_by_index(index);
    if (affectEdgeInfo.first == -1) {
        return;
    } else {
        long long affectEdgeID = affectEdgeInfo.first;
        ui affectIndex = affectEdgeInfo.second;
        edge[affectEdgeID].set_reverse_index_by_index(affectIndex, index);
    }
}

void Graph::remove_edge_from_extra_bloom_by_index(pair_t index) {
    long long affectEdgeID = extraBloom.remove_member_by_index_id_only(index);
    if (affectEdgeID == -1) {
        return;
    } else {
        edge[affectEdgeID].set_reverse_index_in_extra_bloom(index);
    }
}

void Graph::remove_bloom_from_edge_by_index(long long edgeID, ui index) {
    affect_bloom_t affectBloomInfo =
            edge[edgeID].remove_host_bloom_by_index(index);
    if (std::get<0>(affectBloomInfo) == -1) {
        return;
    } else {
        if (std::get<1>(affectBloomInfo).first != -1)
            bloom[std::get<0>(affectBloomInfo)].set_reverse_index_by_index(
                    std::get<1>(affectBloomInfo), index);
        edge[std::get<2>(affectBloomInfo)].set_twin_index_by_index(
                std::get<3>(affectBloomInfo), index);
    }
}

void Graph::bitruss_decomposition() {
    /*
    std::ofstream fout;
    fout.open("../dataset/marvel/temp.txt", std::ios::out);
    for(int i = 0;i < bloomCount;i++){
        fout<<"bloomid:"<<bloom[i].id<<", size:"<<bloom[i].bloomNumber<<std::endl;
    }

    fout.close();
    fout.open("../dataset/marvel/temp2.txt", std::ios::out);
    for(int i = 0;i < m;i++){
        fout<<"edgeID:"<<i<<", size:"<<edge[i].get_butterfly_support()<<" ,hostbloom:"<<edge[i].get_host_bloom_number()<<std::endl;
    }
    fout.close();
    */
    long long visitedEdge = 0;
    std::vector<long long> peelList;
    std::vector<long long> peelListTmp;
    std::vector<long long> matureList;
    double start = get_current_time();
    std::cout << "bitruss decomposing..." << std::endl;
    while (visitedEdge < edgeToPeel) {
        if (peelList.empty()) {
            extraBloom.send_value_to_member(1, matureList, edge);
            extraBloom.increse_counter(1);
            for (ui i = 0; i < matureList.size(); i++) {
                long long edgeID = matureList[i];
                check_mature_edge(edgeID, peelList);
            }
            matureList.clear();
        } else {
            for (ui i = 0; i < peelList.size(); i++) {
                long long edgeID = peelList[i];
                if (bitrussNumber[edgeID] != 0)
                    continue;
                bitrussNumber[edgeID] = extraBloom.get_counter();
                peel_edge(edgeID, peelListTmp);
                visitedEdge++;
            }
            peelList.swap(peelListTmp);
            peelListTmp.clear();
        }
    }
    std::cout << std::fixed << std::setprecision(6)
              << "Bitruss decomposition time:\t" << get_current_time() - start
              << "sec\n";
}

void Graph::peel_edge(long long edgeID, std::vector<long long> &peelList) {
    auto *peelEdge = &edge[edgeID];
    pair_t index = peelEdge->get_reverse_index_in_extra_bloom();

    // remove peel edge from extra bloom
    remove_edge_from_extra_bloom_by_index(index);
    for (ui i = 0; i < peelEdge->get_host_bloom_number(); i++) {
        int bloomID = peelEdge->get_host_bloom_id_by_index(i);
        TwinInfo twinEdgeInfo = peelEdge->get_twin_edge_info_by_index(i);
        pair_t reverseIndex =
                peelEdge->get_reverse_index_in_host_bloom_by_index(i);
        auto *currentBloom = &bloom[bloomID];
        int bloomNumber = currentBloom->bloomNumber;
        long long twinEdgeID = twinEdgeInfo.twinEdgeID;
        // remove peel edge from current bloom
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
        currentBloom->send_value_to_member(bloomNumber - 1, indexInHostBloom,
                                           edge);
        remove_edge_from_bloom_by_index(bloomID, indexInHostBloom);
        remove_bloom_from_edge_by_index(twinEdgeID, indexInTwinEdge);
        edge[twinEdgeID].decrease_butterfly_support(
                currentBloom->get_counter() + bloomNumber - 1);
        if (edge[twinEdgeID].check_maturity()) {
            check_mature_edge(twinEdgeID, peelList);
        }

        currentBloom->bloomNumber--;
        std::vector<long long> matureList;
        int bucket = log2_32(currentBloom->get_counter()+1);
        currentBloom->send_value_to_member_1(bucket,matureList, edge);
        currentBloom->increse_counter(1);
        for (ui j = 0; j < matureList.size(); j++) {
            long long currentEdgeID = matureList[j];
            check_mature_edge(currentEdgeID, peelList);
        }
    }
}

int Graph::collect_counter(long long edgeID) {
    int counter = 0;
    for (ui i = 0; i < edge[edgeID].get_host_bloom_number(); i++) {
        counter +=
                bloom[edge[edgeID].get_host_bloom_id_by_index(i)].get_counter();
    }
    return counter;
}

void Graph::compute_and_restart(long long edgeID, const int trackValue) {
    edge[edgeID].compute_slack_value(trackValue);
    for (ui i = 0; i < edge[edgeID].get_host_bloom_number(); i++) {
        int bloomID = edge[edgeID].get_host_bloom_id_by_index(i);
        pair_t reverseIndex =
                edge[edgeID].get_reverse_index_in_host_bloom_by_index(i);
        remove_edge_from_bloom_by_index(bloomID, reverseIndex);
        pair_t index = bloom[bloomID].add_member_edge(edgeID, i, edge);
        edge[edgeID].set_reverse_index_by_index(i, index);
    }
    pair_t reverseIndex = edge[edgeID].get_reverse_index_in_extra_bloom();
    remove_edge_from_extra_bloom_by_index(reverseIndex);
    pair_t index = extraBloom.add_member_edge(edgeID, edge);
    edge[edgeID].set_reverse_index_in_extra_bloom(index);
}

void Graph::check_mature_edge(long long edgeID,
                              std::vector<long long> &peelList) {
    if (isInPeelList[edgeID])
        return;
    int counterSum = collect_counter(edgeID);
    if (counterSum + extraBloom.get_counter() >=
        edge[edgeID].get_butterfly_support()) {
        isInPeelList[edgeID] = 1;
        peelList.emplace_back(edgeID);
    } else {
        compute_and_restart(edgeID, edge[edgeID].get_butterfly_support() -
                                    counterSum - extraBloom.get_counter());
    }
}

void Graph::output_bitruss_number(std::string outputDir) {
    std::ofstream fout;
    fout.open(outputDir + "/bn.txt", std::ios::out);
    for (long long i = 0; i < m; i++) {
        fout << i << "\t" << bitrussNumber[i] << std::endl;
    }
    fout.close();
}
