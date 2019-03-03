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

void Playground::ApproximateSubstitution(bool verbose) {
    path benchmark_file = benchmark_dir_ / "c17.blif";
    NtkPtr origin_ntk = NtkReadBlif(benchmark_file.string());
    NtkPtr approx_ntk = NtkDuplicate(origin_ntk);

    auto target_node = NtkNodebyName(approx_ntk, "23");
    auto sub_node = NtkNodebyName(approx_ntk, "n9");

    if (verbose) {
        NtkPrintInfo(approx_ntk);
        std::cout << "ObjNumMax: " << abc::Abc_NtkObjNumMax(approx_ntk) << std::endl;
        std::cout << "ObjNum: " << abc::Abc_NtkObjNum(approx_ntk) << std::endl;
    }
    auto alc = new ALC(target_node, sub_node, true);
    alc->Do();
    if (verbose) {
        NtkPrintInfo(approx_ntk);
        std::cout << "ObjNumMax: " << abc::Abc_NtkObjNumMax(approx_ntk) << std::endl;
        std::cout << "ObjNum: " << abc::Abc_NtkObjNum(approx_ntk) << std::endl;
    }
    std::cout << "Substitution: " << SimER(origin_ntk, approx_ntk) << std::endl;
    alc->Recover();
    if (verbose) {
        NtkPrintInfo(approx_ntk);
        std::cout << "ObjNumMax: " << abc::Abc_NtkObjNumMax(approx_ntk) << std::endl;
        std::cout << "ObjNum: " << abc::Abc_NtkObjNum(approx_ntk) << std::endl;
    }
    std::cout << "Recovered: " << SimER(origin_ntk, approx_ntk) << std::endl;
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

void Playground::CriticalGraph() {
    path benchmark_file = benchmark_dir_ / "c17.blif";
    NtkPtr ntk = NtkReadBlif(benchmark_file.string());
    for (auto &path : GetKMostCriticalPaths(ntk)) {
        for (auto &obj : path.objs)
            std::cout << ObjName(obj) << "-" << ObjID(obj) << " ";
        std::cout << std::endl;
    }
    for (auto &[u, vs] : GetCriticalGraph(ntk)) {
        std::cout << u << ": ";
        for (auto &v : vs) std::cout << v << " ";
        std::cout << std::endl;
    }
}

void Playground::CriticalErrorNetwork() {
    path benchmark_file = benchmark_dir_ / "c1355.blif";
    NtkPtr ntk = NtkReadBlif(benchmark_file.string());

    int N = abc::Abc_NtkObjNumMax(ntk) + 1;
    int source = 0, sink = N - 1;
    Dinic dinic(N * 2);

    auto time_info = CalcSlack(ntk);
    std::vector<ObjPtr> pis_nodes_0, nodes_0;
    for (auto const &obj : NtkTopoSortPINode(ntk))
        if (time_info.at(obj).slack == 0) {
            pis_nodes_0.push_back(obj);
            if (ObjIsNode(obj)) nodes_0.push_back(obj);
        }

    for (const auto &obj_0 : pis_nodes_0) {
        int u = ObjID(obj_0);
        if (ObjIsPI(obj_0))
            dinic.AddEdge(source, u, std::numeric_limits<double>::max());
        else {
            dinic.AddEdge(u, u + N, 1);
            if (ObjIsPONode(obj_0))
                dinic.AddEdge(u + N, sink, std::numeric_limits<double>::max());
        }
    }

    for (auto &[u, vs] : GetCriticalGraph(ntk)) {
        for (auto &v : vs) {
            if (ObjIsPI(NtkObjbyID(ntk, u)))
                dinic.AddEdge(u, v, std::numeric_limits<double>::max());
            else
                dinic.AddEdge(u + N, v, std::numeric_limits<double>::max());
        }
    }

    std::cout << "Max Flow: " << dinic.MaxFlow(source, sink) << std::endl;
}

Playground::~Playground() = default;

Playground::Playground() : project_source_dir_(PROJECT_SOURCE_DIR) {
    benchmark_dir_ = project_source_dir_ / "benchmark" / "blif";
    out_dir_ = project_source_dir_ / "out";
}
