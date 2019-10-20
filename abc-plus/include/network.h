/**
 * @file network.h
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-15
 * @bug No known bugs.
 */

#ifndef ABC_PLUS_NETWORK_H
#define ABC_PLUS_NETWORK_H

#include <abc_api.h>
#include <string>
#include <vector>

namespace abc_plus {
    //---------------------------------------------------------------------------
    // Type Alias
    //---------------------------------------------------------------------------
    using NtkPtr = abc::Abc_Ntk_t *;
    using ObjPtr = abc::Abc_Obj_t *;

    //---------------------------------------------------------------------------
    // Network Functions
    //---------------------------------------------------------------------------
    void NtkPrintInfo(NtkPtr ntk, bool verbose = false);

    NtkPtr NtkReadBlif(const std::string &i_file);

    NtkPtr NtkReadBench(const std::string &i_file);

    void NtkWriteBlif(NtkPtr ntk, const std::string &o_file);

    NtkPtr NtkDuplicate(NtkPtr ntk);

    NtkPtr NtkDuplicateDFS(NtkPtr ntk);

    void NtkDelete(NtkPtr ntk);

    std::vector<ObjPtr> NtkObjs(NtkPtr ntk);

    std::vector<ObjPtr> NtkNodes(NtkPtr ntk);

    std::vector<ObjPtr> NtkPIs(NtkPtr ntk);

    std::vector<ObjPtr> NtkPOs(NtkPtr ntk);

    ObjPtr NtkObjbyID(NtkPtr ntk, int id);

    ObjPtr NtkPIbyName(NtkPtr ntk, std::string name);

    ObjPtr NtkNodebyName(NtkPtr ntk, std::string name);

    ObjPtr NtkPObyName(NtkPtr ntk, std::string name);

    //---------------------------------------------------------------------------
    // Object Functions
    //---------------------------------------------------------------------------
    void ObjPrintInfo(ObjPtr obj, bool verbose = false);

    bool ObjIsPI(ObjPtr obj);

    bool ObjIsPO(ObjPtr obj);

    bool ObjIsPONode(ObjPtr obj);

    bool ObjIsNode(ObjPtr obj);

    std::string ObjName(ObjPtr obj);

    unsigned int ObjID(ObjPtr obj);

    NtkPtr ObjHostNtk(ObjPtr obj);

    ObjPtr ObjFanin0(ObjPtr obj);

    ObjPtr ObjFanin1(ObjPtr obj);

    std::vector<ObjPtr> ObjFanins(ObjPtr obj);

    std::vector<ObjPtr> ObjFanouts(ObjPtr obj);

    void ObjReplace(ObjPtr obj_old, ObjPtr obj_new);

    void ObjRecover(ObjPtr obj, ObjPtr obj_bak);

    ObjPtr ObjCreateInv(ObjPtr fan_in);

    void ObjDelete(ObjPtr obj);
}

#endif
