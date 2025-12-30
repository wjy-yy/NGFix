#include "ngfixlib/graph/hnsw_ngfix.h"
#include "tools/data_loader.h"
#include <iostream>
using namespace ngfixlib;

// Test program for NGFixBy2Hop method
// This demonstrates the usage of the new 2-hop neighbor NGFix edge construction
int main(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> paths;
    for (int i = 0; i < argc - 1; i++) {
        std::string arg = argv[i];
        if (arg == "--base_graph_path")
            paths["base_graph_path"] = argv[i + 1];
        if (arg == "--metric")
            paths["metric"] = argv[i + 1];
        if (arg == "--result_index_path")
            paths["result_index_path"] = argv[i + 1];
    }

    std::string base_index_path = paths["base_graph_path"];
    std::cout << "base_graph_path: " << base_index_path << "\n";
    std::string result_index_path = paths["result_index_path"];
    std::cout << "result_index_path: " << result_index_path << "\n";
    std::string metric_str = paths["metric"];

    Metric metric;
    if(metric_str == "ip_float") {
        std::cout << "metric ip\n";
        metric = IP_float;
    } else if(metric_str == "l2_float") {
        std::cout << "metric l2\n";
        metric = L2_float;
    } else {
        throw std::runtime_error("Error: Unsupported metric type.");
    }

    // Load base graph
    auto hnsw_ngfix = new HNSW_NGFix<float>(metric, base_index_path);

    std::cout << "HNSW Bottom Layer Information:\n";
    hnsw_ngfix->printGraphInfo();
    std::cout << "\n";

    auto start = std::chrono::high_resolution_clock::now();

    // Apply NGFixBy2Hop to all nodes
    std::cout << "Applying NGFixBy2Hop to all nodes...\n";
    hnsw_ngfix->NGFixBy2HopAll(32);

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "NGFixBy2Hop latency: " << diff << " ms.\n\n";

    std::cout << "HNSW_NGFix Information after NGFixBy2Hop:\n";
    hnsw_ngfix->printGraphInfo();
    std::cout << "\n";

    // Save the modified graph
    hnsw_ngfix->StoreIndex(result_index_path);
    
    delete hnsw_ngfix;
    return 0;
}
