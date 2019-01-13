/**
 * @file playground.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-15
 * @bug No known bugs.
 */

#include <playground.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>
#include <abc_plus.h>

using namespace boost::filesystem;
using namespace abc_plus;

void PrintNodeInfo() {
    path project_source_dir(PROJECT_SOURCE_DIR);
    path benchmark_dir = project_source_dir / "benchmark";
    path benchmark_path = benchmark_dir / "C17.blif";

    NtkPtr origin_ntk = NtkReadBlif(benchmark_path.string());
    NtkPtr approx_ntk = NtkDuplicate(origin_ntk);

    std::cout << "Objs' names & types:\n";
    for (auto obj : NtkObjs(approx_ntk))
        std::cout << ObjName(obj) << ":" << obj->Type << " ";
    std::cout << std::endl;
    std::cout << "Sorted PIs & Nodes' names & types:\n";
    for (auto obj : NtkTopoSortPINode(origin_ntk))
        std::cout << ObjName(obj) << ":" << obj->Type << " ";
    std::cout << std::endl;
}

void ApproximateSubstitution() {
    path project_source_dir(PROJECT_SOURCE_DIR);
    path benchmark_dir = project_source_dir / "benchmark";
    path benchmark_path = benchmark_dir / "C17.blif";

    NtkPtr origin_ntk = NtkReadBlif(benchmark_path.string());
    NtkPtr approx_ntk = NtkDuplicate(origin_ntk);

    auto target_node = NtkNodebyName(approx_ntk, "G23gat");
    auto target_node_bak = NtkObjbyID(origin_ntk, ObjID(target_node));

    auto sub_node = NtkNodebyName(approx_ntk, "G19gat");
    auto sub_inv = ObjCreateInv(sub_node);

    ObjReplace(target_node, sub_inv);
    std::cout << "Approximate Substitution: " << SimER(origin_ntk, approx_ntk) << std::endl;

    ObjRecover(target_node, target_node_bak);
    std::cout << "Recovered: " << SimER(origin_ntk, approx_ntk) << std::endl;
}
