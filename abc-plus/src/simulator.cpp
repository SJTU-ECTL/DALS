/**
 * @file simulator.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-16
 * @bug No known bugs.
 */

#include <iostream>
#include <unordered_map>
#include <functional>
#include <random>
#include <chrono>
#include <bitset>
#include <boost/progress.hpp>
#include <simulator.h>

namespace abc_plus {
    enum struct GateType : int {
        CONST0, CONST1, WIRE, AND, INV
    };

    enum struct AndType : int {
        AND0, AND1, AND2, AND3 // 00 01 10 11
    };

    struct GateInfo {
        GateType gate_type;
        AndType and_type;
        bool is_complement;
        uint64_t val;

        GateInfo(GateType gate_type, AndType and_type, bool is_complement) : gate_type(gate_type), and_type(and_type),
                                                                             is_complement(is_complement), val(0) {}

        ~GateInfo() = default;
    };

    std::ostream &operator<<(std::ostream &os, GateType gt) {
        switch (gt) {
            case GateType::CONST0:
                os << "CONST0";
                break;
            case GateType::CONST1:
                os << "CONST1";
                break;
            case GateType::WIRE:
                os << "WIRE";
                break;
            case GateType::AND:
                os << "AND";
                break;
            case GateType::INV:
                os << "INV";
                break;
            default:
                os.setstate(std::ios_base::failbit);
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, AndType at) {
        switch (at) {
            case AndType::AND0:
                os << "AND0";
                break;
            case AndType::AND1:
                os << "AND1";
                break;
            case AndType::AND2:
                os << "AND2";
                break;
            case AndType::AND3:
                os << "AND3";
                break;
            default:
                os.setstate(std::ios_base::failbit);
        }
        return os;
    }

    GateInfo *ObjCreateGateInfo(ObjPtr obj) {
        GateType gate_type = GateType::WIRE;
        AndType and_type = AndType::AND0;
        bool is_complement = false;
        if (ObjIsNode(obj)) {
            auto pSop = (char *) obj->pData;
            is_complement = (bool) abc::Abc_SopIsComplement(pSop);
            if (abc::Abc_SopIsConst0(pSop))
                gate_type = GateType::CONST0;
            else if (abc::Abc_SopIsConst1(pSop))
                gate_type = GateType::CONST1;
            else if (pSop[0] == pSop[2]) {
                gate_type = GateType::WIRE;
            } else if (abc::Abc_SopIsInv(pSop))
                gate_type = GateType::INV;
            else {
                gate_type = GateType::AND;
                and_type = (AndType) ((int) AndType::AND0 + (pSop[0] - '0') * 2 + (pSop[1] - '0'));
            }
        }
        return new GateInfo(gate_type, and_type, is_complement);
    }

    static inline uint64_t &ObjVal(ObjPtr obj) { return ((GateInfo *) obj->pTemp)->val; }

    void ObjSim(ObjPtr obj) {
        switch (((GateInfo *) obj->pTemp)->gate_type) {
            case GateType::CONST0:
                ObjVal(obj) = 0;
                break;
            case GateType::CONST1:
                ObjVal(obj) = 1;
                break;
            case GateType::WIRE:
                ObjVal(obj) = ObjVal(ObjFanin0(obj));
                break;
            case GateType::AND: {
                uint64_t input0 = ObjVal(ObjFanin0(obj));
                uint64_t input1 = ObjVal(ObjFanin1(obj));
                switch (((GateInfo *) obj->pTemp)->and_type) {
                    case AndType::AND0: // 00
                        ObjVal(obj) = ~input0 & ~input1;
                        break;
                    case AndType::AND1: // 01
                        ObjVal(obj) = ~input0 & input1;
                        break;
                    case AndType::AND2: // 10
                        ObjVal(obj) = input0 & ~input1;
                        break;
                    case AndType::AND3: // 11
                        ObjVal(obj) = input0 & input1;
                        break;
                }
                break;
            }
            case GateType::INV:
                ObjVal(obj) = ~ObjVal(ObjFanin0(obj));
                break;
        }
        if (((GateInfo *) obj->pTemp)->is_complement)
            ObjVal(obj) = ~ObjVal(obj);
    }

    std::unordered_map<ObjPtr, std::vector<uint64_t>> SimTruthVec(NtkPtr ntk, bool show_progress_bar, int sim_64_cycles) {
        std::unordered_map<ObjPtr, std::vector<uint64_t>> truth_vec;
        std::default_random_engine generator((unsigned) std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
        auto dice = std::bind(distribution, generator);
        auto objs = NtkObjs(ntk);
        auto pis = NtkPIs(ntk);
        auto pos = NtkPOs(ntk);
        auto topo_nodes = NtkTopoSortNode(ntk);

        for (auto const &obj : objs) {
            obj->pTemp = ObjCreateGateInfo(obj);
            truth_vec.emplace(obj, std::vector<uint64_t>());
            truth_vec.at(obj).reserve(sim_64_cycles);
        }

        boost::progress_display *pd = nullptr;
        if (show_progress_bar)
            pd = new boost::progress_display((unsigned long) sim_64_cycles);

        for (int _ = 0; _ < sim_64_cycles; ++_) {
            if (show_progress_bar)
                ++(*pd);
            for (auto const &pi : pis)
                ObjVal(pi) = dice();
            for (auto const &obj : topo_nodes)
                ObjSim(obj);
            for (auto const &po : pos)
                ObjVal(po) = ObjVal(ObjFanin0(po));
            for (auto const &obj : objs)
                truth_vec.at(obj).push_back(ObjVal(obj));
        }

        for (auto obj : NtkNodes(ntk))
            delete (GateInfo *) obj->pTemp;

        return truth_vec;
    }

    double SimER(NtkPtr origin, NtkPtr approx, bool show_progress_bar, int sim_cycles) {
        std::default_random_engine generator((unsigned) std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
        auto dice = std::bind(distribution, generator);
        int er = 0;
        auto origin_pis = NtkPIs(origin);
        auto origin_pos = NtkPOs(origin);
        auto origin_topo_nodes = NtkTopoSortNode(origin);
        auto approx_pis = NtkPIs(approx);
        auto approx_pos = NtkPOs(approx);
        auto approx_topo_nodes = NtkTopoSortNode(approx);
        for (auto const &obj : NtkObjs(origin))
            obj->pTemp = ObjCreateGateInfo(obj);
        for (auto const &obj : NtkObjs(approx))
            obj->pTemp = ObjCreateGateInfo(obj);

        boost::progress_display *pd = nullptr;
        if (show_progress_bar)
            pd = new boost::progress_display((unsigned long) sim_cycles / 64);

        for (int _ = 0; _ < sim_cycles / 64; ++_) {
            if (show_progress_bar)
                ++(*pd);
            for (int i = 0; i < (int) origin_pis.size(); i++) {
                uint64_t rand_val = dice();
                ObjVal(origin_pis[i]) = rand_val;
                ObjVal(approx_pis[i]) = rand_val;
            }
            for (auto const &obj : origin_topo_nodes)
                ObjSim(obj);
            for (auto const &po : origin_pos)
                ObjVal(po) = ObjVal(ObjFanin0(po));
            for (auto const &obj : approx_topo_nodes)
                ObjSim(obj);
            for (auto const &po : approx_pos)
                ObjVal(po) = ObjVal(ObjFanin0(po));
            uint64_t res = 0;
            for (int i = 0; i < (int) origin_pos.size(); i++)
                res = res | (ObjVal(origin_pos[i]) ^ ObjVal(approx_pos[i]));
            er += std::bitset<64>(res).count();
        }

        for (auto const &obj : NtkNodes(origin))
            delete (GateInfo *) obj->pTemp;
        for (auto const &obj : NtkNodes(approx))
            delete (GateInfo *) obj->pTemp;
        return (double) er / (double) sim_cycles;
    }
}
