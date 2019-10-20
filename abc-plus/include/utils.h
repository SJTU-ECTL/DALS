/**
 * @file utils.h
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-15
 * @bug No known bugs.
 */

#ifndef ABC_PLUS_UTILS_H
#define ABC_PLUS_UTILS_H

#include <vector>
#include <network.h>

namespace abc_plus {
    std::vector<ObjPtr> NtkTopoSortPINode(NtkPtr ntk);

    std::vector<ObjPtr> NtkTopoSortNode(NtkPtr ntk);
}

#endif
