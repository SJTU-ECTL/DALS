/**
 * @file utils.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-15
 * @bug No known bugs.
 */

#include <utils.h>

namespace abc_plus {
    std::vector<ObjPtr> NtkTopoSortPINode(NtkPtr ntk) {
        abc::Vec_Ptr_t *abc_objs = abc::Abc_NtkDfs(ntk, 0);
        std::vector<ObjPtr> sorted_objs;

        for (int i = 0; i < abc_objs->nSize; ++i) {
            sorted_objs.push_back((abc::Abc_Obj_t *) abc_objs->pArray[i]);
        }
        Vec_PtrFree(abc_objs);

        auto pis = NtkPIs(ntk);
        sorted_objs.insert(sorted_objs.begin(),
                           std::make_move_iterator(pis.begin()),
                           std::make_move_iterator(pis.end()));
        return sorted_objs;
    }

    std::vector<ObjPtr> NtkTopoSortNode(NtkPtr ntk) {
        abc::Vec_Ptr_t *abc_objs = abc::Abc_NtkDfs(ntk, 0);
        std::vector<ObjPtr> sorted_objs;

        for (int i = 0; i < abc_objs->nSize; ++i) {
            sorted_objs.push_back((abc::Abc_Obj_t *) abc_objs->pArray[i]);
        }
        Vec_PtrFree(abc_objs);

        return sorted_objs;
    }
}
