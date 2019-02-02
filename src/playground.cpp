/**
 * @file playground.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-15
 * @bug No known bugs.
 */

#include <playground.h>
#include <iostream>
#include <boost/timer/timer.hpp>
#include <abc_plus.h>
#include <sta.h>
#include <dinic.h>

using namespace boost::filesystem;
using namespace abc_plus;

std::shared_ptr<Playground> Playground::GetPlayground() {
    static std::shared_ptr<Playground> playground(new Playground);
    return playground;
}

void Playground::PrintNodeInfo() {
    path benchmark_path = benchmark_dir_ / "c17.blif";
    NtkPtr ntk = NtkReadBlif(benchmark_path.string());

    std::cout << "Objs' names & types:\n";
    for (auto const &obj : NtkObjs(ntk))
        std::cout << ObjName(obj) << ":" << obj->Type << " ";
    std::cout << std::endl;
    std::cout << "Sorted PIs & Nodes' names & types:\n";
    for (auto const &obj : NtkTopoSortPINode(ntk))
        std::cout << ObjName(obj) << ":" << obj->Type << " ";
    std::cout << std::endl;
}

void Playground::ApproximateSubstitution() {
    path benchmark_path = benchmark_dir_ / "c17.blif";
    NtkPtr origin_ntk = NtkReadBlif(benchmark_path.string());
    NtkPtr approx_ntk = NtkDuplicate(origin_ntk);

    auto target_node = NtkNodebyName(approx_ntk, "23");
    auto target_node_bak = NtkObjbyID(origin_ntk, ObjID(target_node));

    auto sub_node = NtkNodebyName(approx_ntk, "n9");
    auto sub_inv = ObjCreateInv(sub_node);

    ObjReplace(target_node, sub_inv);
    std::cout << "Approximate Substitution: " << SimER(origin_ntk, approx_ntk) << std::endl;

    ObjRecover(target_node, target_node_bak);
    std::cout << "Recovered: " << SimER(origin_ntk, approx_ntk) << std::endl;
}

void Playground::StaticTimingAnalysis() {
    path benchmark_path = benchmark_dir_ / "c17.blif";
    NtkPtr ntk = NtkReadBlif(benchmark_path.string());
    CalcSlack(ntk, true);
    GetKMostCriticalPaths(ntk, 10, true);
}

void Playground::Visualization() {
    path out_dir = project_source_dir_ / "out";
    path benchmark_path = benchmark_dir_ / "c17.blif";
    path dot_path = out_dir / "c17.dot";
    NtkPtr ntk = NtkReadBlif(benchmark_path.string());
    NtkWriteDot(ntk, dot_path.string());
}

void Playground::MaxFlowMinCut() {
    Dinic dinic(6);
    dinic.AddEdge(0, 1, 16);
    dinic.AddEdge(0, 2, 13);
    dinic.AddEdge(1, 2, 10);
    dinic.AddEdge(1, 3, 12);
    dinic.AddEdge(2, 1, 4);
    dinic.AddEdge(2, 4, 14);
    dinic.AddEdge(3, 2, 9);
    dinic.AddEdge(3, 5, 20);
    dinic.AddEdge(4, 3, 7);
    dinic.AddEdge(4, 5, 4);
//    std::cout << dinic.MaxFlow(0, 5) << std::endl;
//    for (auto e : dinic.E)
//        std::cout << "u=" << e.u << " v=" << e.v << " cap=" << e.cap << " flow=" << e.flow << std::endl;

    for (auto e : dinic.MinCut(0, 5))
        std::cout << "u=" << e.u << " v=" << e.v << " cap=" << e.cap << " flow=" << e.flow << std::endl;
}

Playground::~Playground() = default;

Playground::Playground() : project_source_dir_(PROJECT_SOURCE_DIR) {
    benchmark_dir_ = project_source_dir_ / "benchmark" / "blif";
}
