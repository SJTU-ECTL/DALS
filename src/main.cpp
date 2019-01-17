/**
 * @file main.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-13
 * @bug No known bugs.
 */

#include <iostream>
#include <boost/filesystem.hpp>
#include <abc_plus.h>
#include <playground.h>

using namespace boost::filesystem;
using namespace abc_plus;

int main(int argc, char *argv[]) {
    PrintNodeInfo();
    ApproximateSubstitution();
    StaticTimingAnalysis();
    Visualization();
    return 0;
}
