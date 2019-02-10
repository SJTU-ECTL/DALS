/**
 * @file sta.h
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-13
 * @bug No known bugs.
 */

#ifndef DALS_STA_H
#define DALS_STA_H

#include <unordered_map>
#include <map>
#include <set>
#include <abc_plus.h>

using namespace abc_plus;

struct TimeObject {
    int arrival_time;
    int required_time;
    int slack;

    TimeObject(int arrival, int required, int slack);
};

struct Path {
    std::vector<ObjPtr> objs;
    int max_delay;

    explicit Path(std::vector<ObjPtr> nodes = std::vector<ObjPtr>());

    Path(ObjPtr obj, int max_delay, std::vector<ObjPtr> objs = std::vector<ObjPtr>());

    void Print() const;
};

std::unordered_map<ObjPtr, TimeObject> CalcSlack(NtkPtr ntk, bool print_result = false);

std::vector<Path> GetKMostCriticalPaths(NtkPtr ntk, int k = -1, bool print_result = false);

std::map<int, std::set<int>> GetCriticalGraph(NtkPtr ntk);

#endif
