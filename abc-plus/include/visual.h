/**
 * @file visual.h
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-17
 * @bug No known bugs.
 */

#ifndef ABC_PLUS_VISUAL_H
#define ABC_PLUS_VISUAL_H

#include <abc_api.h>
#include <network.h>

namespace abc_plus {
    void NtkWriteDot(NtkPtr ntk, const std::string &o_file);
}

#endif
