/**
 * @file sta.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-13
 * @bug No known bugs.
 */

#include <iostream>
#include <queue>
#include <boost/range/adaptor/reversed.hpp>
#include <sta.h>

static const int INF = 1000000;

TimeObject::TimeObject(int arrival, int required, int slack) : arrival_time(arrival), required_time(required), slack(slack) {}

Path::Path(std::vector<ObjPtr> nodes) { this->objs = std::move(nodes); }

Path::Path(ObjPtr obj, int max_delay, std::vector<ObjPtr> objs) : objs(std::move(objs)), max_delay(max_delay) {
    this->objs.emplace_back(obj);
}

void Path::Print() const {
    for (auto const &obj : objs)
        std::cout << ObjName(obj) << " ";
    std::cout << std::endl;
}

std::unordered_map<ObjPtr, TimeObject> CalcSlack(NtkPtr ntk, bool print_result) {
    std::vector<ObjPtr> sorted_objs = NtkTopoSortPINode(ntk);

    std::unordered_map<ObjPtr, TimeObject> t_objs;

    /* Initialization */
    for (auto const &obj : sorted_objs)
        t_objs.emplace(obj, TimeObject(-1 * INF, INF, 0));

    int max_at = -1 * INF;

    /* Update at */
    for (auto const &obj : sorted_objs) {
        if (ObjIsPI(obj))
            t_objs.at(obj).arrival_time = 1;
        else
            for (const auto &fan_in : ObjFanins(obj))
                t_objs.at(obj).arrival_time = std::max(t_objs.at(obj).arrival_time,
                                                       t_objs.at(fan_in).arrival_time + 1);
        if (t_objs.at(obj).arrival_time > max_at)
            max_at = t_objs.at(obj).arrival_time;
    }

    /* Update rat */
    for (auto const &obj : sorted_objs)
        if (ObjIsPONode(obj))
            t_objs.at(obj).required_time = max_at;

    for (auto const &obj : boost::adaptors::reverse(sorted_objs)) {
        for (auto const &fan_out : ObjFanouts(obj))
            if (!ObjIsPO(fan_out) && t_objs.find(fan_out) != t_objs.end()) {
                t_objs.at(obj).required_time = std::min(t_objs.at(obj).required_time,
                                                        t_objs.at(fan_out).required_time - 1);
            }
    }

    /* Update slack */
    for (auto const &node:sorted_objs) {
        t_objs.at(node).slack = t_objs.at(node).required_time - t_objs.at(node).arrival_time;
        if (print_result)
            std::cout << ObjName(node) << "=" << t_objs.at(node).required_time << "-" << t_objs.at(node).arrival_time
                      << "=" << t_objs.at(node).slack << " ";
    }
    if (print_result)
        std::cout << std::endl;
    return t_objs;
}

std::vector<Path> GetKMostCriticalPaths(const NtkPtr ntk, int k, bool print_result) {
    int critical_path_delay = -1;
    std::unordered_map<ObjPtr, TimeObject> time_info = CalcSlack(ntk);
    std::vector<ObjPtr> sorted_objs = NtkTopoSortPINode(ntk);
    std::unordered_map<ObjPtr, int> max_delay_to_sink;
    std::vector<Path> critical_paths;


    /* Computation of Maximum Delays to Sink */
    for (auto const &obj : sorted_objs) {
        max_delay_to_sink.emplace(obj, 0);
        if (ObjIsPONode(obj))
            max_delay_to_sink.at(obj) = 1;
    }
    for (auto const &obj : boost::adaptors::reverse(sorted_objs)) {
        for (auto const &fan_out : ObjFanouts(obj))
            if (!ObjIsPO(fan_out) && max_delay_to_sink.find(fan_out) != max_delay_to_sink.end())
                max_delay_to_sink.at(obj) = std::max(
                        max_delay_to_sink.at(obj),
                        max_delay_to_sink.at(fan_out) + 1);
    }

    /* Path Enumeration */
    auto comp = [](Path a, Path b) { return a.max_delay < b.max_delay; };

    std::priority_queue<Path, std::vector<Path>, decltype(comp)> paths(comp);

    for (auto const &node : NtkPIs(ntk))
        paths.emplace(node, max_delay_to_sink.at(node));

    while (!paths.empty() && (k == -1 || k > 0)) {
        Path path_t = paths.top();
        paths.pop();
        if (ObjIsPO(path_t.objs.back())) {
            if (k != -1)
                k--;
            else if (path_t.max_delay < critical_path_delay)
                break;
            critical_path_delay = std::max(critical_path_delay, path_t.max_delay);
            path_t.objs.pop_back();
            if (print_result) {
                std::cout << "Delay: " << path_t.max_delay << " ";
                for (auto const &obj : path_t.objs) {
                    std::cout << ObjName(obj);
//                    std::cout << "=" << time_info.at(obj).slack;
                    std::cout << " ";
                }
                std::cout << std::endl;
            }
            critical_paths.emplace_back(path_t);
        } else {
            ObjPtr obj_t = path_t.objs.back();
            for (auto const &successor : ObjFanouts(obj_t)) {
                if (!ObjIsPO(successor) && max_delay_to_sink.find(successor) != max_delay_to_sink.end())
                    paths.emplace(successor,
                                  (int) path_t.objs.size() + max_delay_to_sink.at(successor),
                                  path_t.objs);
                else
                    paths.emplace(successor,
                                  (int) path_t.objs.size() + 0,
                                  path_t.objs);
            }
        }
    }
    return critical_paths;
}
