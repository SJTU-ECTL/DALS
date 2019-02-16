/**
 * @file playground.h
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-13
 * @bug No known bugs.
 */

#ifndef DALS_PLAYGROUND_H
#define DALS_PLAYGROUND_H

#include <memory>
#include <boost/filesystem.hpp>

class Playground {
public:
    static std::shared_ptr<Playground> GetPlayground();

    void ApproximateSubstitution(bool verbose = false);

    void StaticTimingAnalysis();

    void Visualization();

    void MaxFlowMinCut();

    void CriticalGraph();

    void operator=(Playground const &) = delete;

    Playground(Playground const &) = delete;

    ~Playground();

private:
    boost::filesystem::path project_source_dir_;
    boost::filesystem::path benchmark_dir_;
    boost::filesystem::path out_dir_;

    Playground();
};

#endif
