#include <iostream>

#include "factory/factory.h"
#include "utils/flags.h"

using namespace std;

int main(int argc, char **argv) {
    //gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto filepath = "../dataset/marvel";
    auto *factory = new GraphFactory();
    auto *graph = factory->generate_graph(filepath, "standard");
    graph->construct_index();
    graph->bitruss_decomposition();
    graph->output_bitruss_number("../dataset/marvel");
}