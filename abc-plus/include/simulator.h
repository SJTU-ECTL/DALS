/**
 * @file simulator.h
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-16
 * @bug No known bugs.
 */

#ifndef ABC_PLUS_SIMULATOR_H
#define ABC_PLUS_SIMULATOR_H

#include <cstdint>
#include <unordered_map>
#include <abc_api.h>
#include <network.h>
#include <utils.h>

namespace abc_plus {
    std::unordered_map<ObjPtr, std::vector<uint64_t >> SimTruthVec(NtkPtr ntk, bool show_progress_bar = false, int sim_64_cycles = 10000);

    double SimER(NtkPtr origin, NtkPtr approx, bool show_progress_bar = false, int sim_cycles = 100000);
}


#endif
