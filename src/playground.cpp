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
#include <dals.h>

using namespace boost::filesystem;
using namespace abc_plus;

std::shared_ptr<Playground> Playground::GetPlayground() {
    static std::shared_ptr<Playground> playground(new Playground);
    return playground;
}

void Playground::ApproximateSubstitution() {
    path benchmark_file = benchmark_dir_ / "c17.blif";
    NtkPtr origin_ntk = NtkReadBlif(benchmark_file.string());
    NtkPtr approx_ntk = NtkDuplicate(origin_ntk);

    auto target_node = NtkNodebyName(approx_ntk, "23");
    auto sub_node = NtkNodebyName(approx_ntk, "n9");

//    NtkPrintInfo(approx_ntk);
//    std::cout << "ObjNumMax: " << abc::Abc_NtkObjNumMax(approx_ntk) << std::endl;
//    std::cout << "ObjNum: " << abc::Abc_NtkObjNum(approx_ntk) << std::endl;
    auto alc = new ALC(target_node, sub_node, true);
    alc->Do();
//    NtkPrintInfo(approx_ntk);
    std::cout << "Approximate Substitution: " << SimER(origin_ntk, approx_ntk) << std::endl;
//    std::cout << "ObjNumMax: " << abc::Abc_NtkObjNumMax(approx_ntk) << std::endl;
//    std::cout << "ObjNum: " << abc::Abc_NtkObjNum(approx_ntk) << std::endl;
    alc->Recover();
//    NtkPrintInfo(approx_ntk);
    std::cout << "Recovered: " << SimER(origin_ntk, approx_ntk) << std::endl;
//    std::cout << "ObjNumMax: " << abc::Abc_NtkObjNumMax(approx_ntk) << std::endl;
//    std::cout << "ObjNum: " << abc::Abc_NtkObjNum(approx_ntk) << std::endl;
}

void Playground::StaticTimingAnalysis() {
    path benchmark_file = benchmark_dir_ / "c17.blif";
    NtkPtr ntk = NtkReadBlif(benchmark_file.string());
    CalcSlack(ntk, true);
    GetKMostCriticalPaths(ntk, 10, true);
    std::cout << std::endl;
    GetKMostCriticalPaths(ntk, -1, true);
}

void Playground::Visualization() {
    path benchmark_file = benchmark_dir_ / "c17.blif";
    path dot_file = out_dir_ / "c17.dot";
    NtkPtr ntk = NtkReadBlif(benchmark_file.string());
    NtkWriteDot(ntk, dot_file.string());
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
    out_dir_ = project_source_dir_ / "out";
}
