/**
 * @file network.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2018-12-15
 * @bug No known bugs.
 */

#include <iostream>
#include <iomanip>
#include <network.h>

namespace abc {
    static Abc_Obj_t *PLUS_Abc_NtkDupObj(Abc_Ntk_t *pNtkNew, Abc_Obj_t *pObj, int fCopyName) {
        Abc_Obj_t *pObjNew;
        // create the new object
        pObjNew = Abc_NtkCreateObj(pNtkNew, (Abc_ObjType_t) pObj->Type);
        // transfer names of the terminal objects
        if (fCopyName) {
            if (Abc_ObjIsCi(pObj)) {
                if (!Abc_NtkIsNetlist(pNtkNew))
                    Abc_ObjAssignName(pObjNew, Abc_ObjName(Abc_ObjFanout0Ntk(pObj)), nullptr);
            } else if (Abc_ObjIsCo(pObj)) {
                if (!Abc_NtkIsNetlist(pNtkNew)) {
                    if (Abc_ObjIsPo(pObj))
                        Abc_ObjAssignName(pObjNew, Abc_ObjName(Abc_ObjFanin0Ntk(pObj)), nullptr);
                    else {
                        assert(Abc_ObjIsLatch(Abc_ObjFanout0(pObj)));
                        Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), nullptr);
                    }
                }
            } else if (Abc_ObjIsBox(pObj) || Abc_ObjIsNet(pObj))
                Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), nullptr);
        }
        // copy functionality/names
        if (Abc_ObjIsNode(pObj)) // copy the function if functionality is compatible
        {
            if (fCopyName) {
                Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), nullptr);  //MODIFIED: copy node's name as expected
            }
            if (pNtkNew->ntkFunc == pObj->pNtk->ntkFunc) {
                if (Abc_NtkIsStrash(pNtkNew)) {}
                else if (Abc_NtkHasSop(pNtkNew) || Abc_NtkHasBlifMv(pNtkNew))
                    pObjNew->pData = Abc_SopRegister((Mem_Flex_t *) pNtkNew->pManFunc, (char *) pObj->pData);
#ifdef ABC_USE_CUDD
                else if (Abc_NtkHasBdd(pNtkNew))
                    pObjNew->pData = Cudd_bddTransfer((DdManager *) pObj->pNtk->pManFunc, (DdManager *) pNtkNew->pManFunc,
                                                      (DdNode *) pObj->pData), Cudd_Ref((DdNode *) pObjNew->pData);
#endif
                else if (Abc_NtkHasAig(pNtkNew))
                    pObjNew->pData = Hop_Transfer((Hop_Man_t *) pObj->pNtk->pManFunc, (Hop_Man_t *) pNtkNew->pManFunc,
                                                  (Hop_Obj_t *) pObj->pData, Abc_ObjFaninNum(pObj));
                else if (Abc_NtkHasMapping(pNtkNew))
                    pObjNew->pData = pObj->pData, pNtkNew->nBarBufs2 += !pObj->pData;
                else
                    assert(0);
            }
        } else if (Abc_ObjIsNet(pObj)) // copy the name
        {
        } else if (Abc_ObjIsLatch(pObj)) // copy the reset value
            pObjNew->pData = pObj->pData;
        // transfer HAIG
//    pObjNew->pEquiv = pObj->pEquiv;
        // remember the new node in the old node
        pObj->pCopy = pObjNew;
        return pObjNew;
    }

    static Abc_Ntk_t *PLUS_Abc_NtkDup(Abc_Ntk_t *pNtk) {
        Abc_Ntk_t *pNtkNew;
        Abc_Obj_t *pObj, *pFanin;
        int i, k;
        if (pNtk == nullptr)
            return nullptr;
        // start the network
        pNtkNew = Abc_NtkStartFrom(pNtk, pNtk->ntkType, pNtk->ntkFunc);
        // copy the internal nodes
        if (Abc_NtkIsStrash(pNtk)) {
            // copy the AND gates
            Abc_AigForEachAnd(pNtk, pObj, i)pObj->pCopy = Abc_AigAnd((Abc_Aig_t *) pNtkNew->pManFunc, Abc_ObjChild0Copy(pObj),
                                                                     Abc_ObjChild1Copy(pObj));
            // relink the choice nodes
            Abc_AigForEachAnd(pNtk, pObj, i) if (pObj->pData)
                    pObj->pCopy->pData = ((Abc_Obj_t *) pObj->pData)->pCopy;
            // relink the CO nodes
            Abc_NtkForEachCo(pNtk, pObj, i)Abc_ObjAddFanin(pObj->pCopy, Abc_ObjChild0Copy(pObj));
            // get the number of nodes before and after
            if (Abc_NtkNodeNum(pNtk) != Abc_NtkNodeNum(pNtkNew))
                printf("Warning: Structural hashing during duplication reduced %d nodes (this is a minor bug).\n",
                       Abc_NtkNodeNum(pNtk) - Abc_NtkNodeNum(pNtkNew));
        } else {
            // duplicate the nets and nodes (CIs/COs/latches already dupped)
            Abc_NtkForEachObj(pNtk, pObj, i) if (pObj->pCopy == nullptr)
                    PLUS_Abc_NtkDupObj(pNtkNew, pObj, 1);
            // reconnect all objects (no need to transfer attributes on edges)
            Abc_NtkForEachObj(pNtk, pObj, i) if (!Abc_ObjIsBox(pObj) && !Abc_ObjIsBo(pObj))
                    Abc_ObjForEachFanin(pObj, pFanin, k)Abc_ObjAddFanin(pObj->pCopy, pFanin->pCopy);
        }
        // duplicate the EXDC Ntk
        if (pNtk->pExdc)
            pNtkNew->pExdc = Abc_NtkDup(pNtk->pExdc);
        if (pNtk->pExcare)
            pNtkNew->pExcare = Abc_NtkDup((Abc_Ntk_t *) pNtk->pExcare);
        // duplicate timing manager
        if (pNtk->pManTime)
            Abc_NtkTimeInitialize(pNtkNew, pNtk);
        if (pNtk->vPhases)
            Abc_NtkTransferPhases(pNtkNew, pNtk);
        if (pNtk->pWLoadUsed)
            pNtkNew->pWLoadUsed = Abc_UtilStrsav(pNtk->pWLoadUsed);
        // check correctness
        if (!Abc_NtkCheck(pNtkNew))
            fprintf(stdout, "Abc_NtkDup(): Network check has failed.\n");
        pNtk->pCopy = pNtkNew;
        return pNtkNew;
    }

    static Abc_Ntk_t *PLUS_Abc_NtkDupDfs(Abc_Ntk_t *pNtk) {
        Vec_Ptr_t *vNodes;
        Abc_Ntk_t *pNtkNew;
        Abc_Obj_t *pObj, *pFanin;
        int i, k;
        if (pNtk == nullptr)
            return nullptr;
        assert(!Abc_NtkIsStrash(pNtk) && !Abc_NtkIsNetlist(pNtk));
        // start the network
        pNtkNew = Abc_NtkStartFrom(pNtk, pNtk->ntkType, pNtk->ntkFunc);
        // copy the internal nodes
        vNodes = Abc_NtkDfs(pNtk, 0);
        Vec_PtrForEachEntry(Abc_Obj_t *, vNodes, pObj, i)PLUS_Abc_NtkDupObj(pNtkNew, pObj, 1);
        Vec_PtrFree(vNodes);
        // reconnect all objects (no need to transfer attributes on edges)
        Abc_NtkForEachObj(pNtk, pObj, i) if (!Abc_ObjIsBox(pObj) && !Abc_ObjIsBo(pObj))
                Abc_ObjForEachFanin(pObj, pFanin, k)if (pObj->pCopy && pFanin->pCopy)
                        Abc_ObjAddFanin(pObj->pCopy, pFanin->pCopy);
        // duplicate the EXDC Ntk
        if (pNtk->pExdc)
            pNtkNew->pExdc = Abc_NtkDup(pNtk->pExdc);
        if (pNtk->pExcare)
            pNtkNew->pExcare = Abc_NtkDup((Abc_Ntk_t *) pNtk->pExcare);
        // duplicate timing manager
        if (pNtk->pManTime)
            Abc_NtkTimeInitialize(pNtkNew, pNtk);
        if (pNtk->vPhases)
            Abc_NtkTransferPhases(pNtkNew, pNtk);
        if (pNtk->pWLoadUsed)
            pNtkNew->pWLoadUsed = Abc_UtilStrsav(pNtk->pWLoadUsed);
        // check correctness
        if (!Abc_NtkCheck(pNtkNew))
            fprintf(stdout, "Abc_NtkDup(): Network check has failed.\n");
        pNtk->pCopy = pNtkNew;
        return pNtkNew;
    }

    static inline int Abc_ObjFanoutId(Abc_Obj_t *pObj, int i) { return pObj->vFanouts.pArray[i]; }
}

namespace abc_plus {
    //---------------------------------------------------------------------------
    // Network Functions
    //---------------------------------------------------------------------------
    void NtkPrintInfo(NtkPtr ntk, bool verbose) {
        std::string ntk_types[] = {"ABC_NTK_NONE",      // 0:  unknown
                                   "ABC_NTK_NETLIST",   // 1:  network with PIs/POs, latches, nodes, and nets
                                   "ABC_NTK_LOGIC",     // 2:  network with PIs/POs, latches, and nodes
                                   "ABC_NTK_STRASH",    // 3:  structurally hashed AIG (two input AND gates with c-attributes on edges)
                                   "ABC_NTK_OTHER"};    // 4:  unused
        std::cout << "Network Type: " << ntk_types[ntk->ntkType] << std::endl;
        for (auto const &obj : NtkObjs(ntk)) {
            if (verbose)
                std::cout << "---------------------------------------------------------------------------" << std::endl;
            ObjPrintInfo(obj, verbose);
        }
    }

    NtkPtr NtkReadBlif(const std::string &i_file) {
        NtkPtr tmp, ntk = abc::Io_ReadBlif((char *) i_file.c_str(), 1);
        ntk = Abc_NtkToLogic(tmp = ntk);
        abc::Abc_NtkDelete(tmp);
        return ntk;
    }

    NtkPtr NtkReadBench(const std::string &i_file) {
        NtkPtr tmp, ntk = abc::Io_ReadBench((char *) i_file.c_str(), 1);
        ntk = abc::Abc_NtkToLogic(tmp = ntk);
        abc::Abc_NtkDelete(tmp);
        return ntk;
    }

    void NtkWriteBlif(NtkPtr ntk, const std::string &o_file) {
        abc::Io_WriteBlifLogic(abc::PLUS_Abc_NtkDup(ntk), (char *) o_file.c_str(), 0); // CHECK
    }

    NtkPtr NtkDuplicate(NtkPtr ntk) { return abc::PLUS_Abc_NtkDup(ntk); }

    NtkPtr NtkDuplicateDFS(NtkPtr ntk) { return abc::PLUS_Abc_NtkDupDfs(ntk); }

    void NtkDelete(NtkPtr ntk) { abc::Abc_NtkDelete(ntk); }

    std::vector<ObjPtr> NtkObjs(NtkPtr ntk) {
        abc::Abc_Obj_t *obj;
        int i;
        std::vector<ObjPtr> objs;
        Abc_NtkForEachObj(ntk, obj, i) {
                objs.push_back(obj);
            }
        return objs;
    }

    std::vector<ObjPtr> NtkNodes(NtkPtr ntk) {
        abc::Abc_Obj_t *node;
        int i;
        std::vector<ObjPtr> nodes;
        Abc_NtkForEachNode(ntk, node, i) {
                nodes.push_back(node);
            }
        return nodes;
    }

    std::vector<ObjPtr> NtkPIs(NtkPtr ntk) {
        abc::Abc_Obj_t *pi;
        int i;
        std::vector<ObjPtr> pis;
        Abc_NtkForEachPi(ntk, pi, i) {
            pis.push_back(pi);
        }
        return pis;
    }

    std::vector<ObjPtr> NtkPOs(NtkPtr ntk) {
        abc::Abc_Obj_t *po;
        int i;
        std::vector<ObjPtr> pos;
        Abc_NtkForEachPo(ntk, po, i) {
            pos.push_back(po);
        }
        return pos;
    }

    ObjPtr NtkObjbyID(NtkPtr ntk, int id) { return abc::Abc_NtkObj(ntk, id); }

    ObjPtr NtkPIbyName(NtkPtr ntk, std::string name) { return abc::Abc_NtkFindCi(ntk, (char *) name.c_str()); }

    ObjPtr NtkNodebyName(NtkPtr ntk, std::string name) { return abc::Abc_NtkFindNode(ntk, (char *) name.c_str()); }

    ObjPtr NtkPObyName(NtkPtr ntk, std::string name) { return abc::Abc_NtkFindCo(ntk, (char *) name.c_str()); }

    //---------------------------------------------------------------------------
    // Object Functions
    //---------------------------------------------------------------------------
    void ObjPrintInfo(ObjPtr obj, bool verbose) {
        std::string obj_types[12] = {"ABC_OBJ_NONE",       //  0:  unknown
                                     "ABC_OBJ_CONST1",     //  1:  constant 1 node (AIG only)
                                     "ABC_OBJ_PI",         //  2:  primary input terminal
                                     "ABC_OBJ_PO",         //  3:  primary output terminal
                                     "ABC_OBJ_BI",         //  4:  box input terminal
                                     "ABC_OBJ_BO",         //  5:  box output terminal
                                     "ABC_OBJ_NET",        //  6:  net
                                     "ABC_OBJ_NODE",       //  7:  node
                                     "ABC_OBJ_LATCH",      //  8:  latch
                                     "ABC_OBJ_WHITEBOX",   //  9:  box with known contents
                                     "ABC_OBJ_BLACKBOX",   // 10:  box with unknown contents
                                     "ABC_OBJ_NUMBER"};    // 11:  unused
        std::cout << "Obj:" << std::setw(4) << ObjName(obj)
                  << " ID:" << std::setw(3) << ObjID(obj)
                  << " Type: " << std::setw(12) << obj_types[obj->Type] << std::endl;
        if (verbose) {
            std::cout << "Fanins:  ";
            for (auto const &fan_in : ObjFanins(obj))
                std::cout << ObjName(fan_in) << " ";
            std::cout << std::endl << "Fanouts: ";
            for (auto const &fan_out : ObjFanouts(obj))
                std::cout << ObjName(fan_out) << " ";
            std::cout << std::endl;
        }
    }

    bool ObjIsPI(ObjPtr obj) {
        return (bool) abc::Abc_ObjIsPi(obj);
    }

    bool ObjIsPO(ObjPtr obj) {
        return (bool) abc::Abc_ObjIsPo(obj);
    }

    bool ObjIsPONode(ObjPtr obj) {
        for (auto const &fan_out : ObjFanouts(obj))
            if (ObjIsPO(fan_out))
                return true;
        return false;
    }

    bool ObjIsNode(ObjPtr obj) {
        return (bool) abc::Abc_ObjIsNode(obj);
    }

    std::string ObjName(ObjPtr obj) {
        return std::string(Abc_ObjName(obj));
    }

    unsigned int ObjID(ObjPtr obj) {
        return abc::Abc_ObjId(obj);
    }

    NtkPtr ObjHostNtk(ObjPtr obj) {
        return abc::Abc_ObjNtk(obj);
    }

    ObjPtr ObjFanin0(ObjPtr obj) {
        return abc::Abc_ObjFanin0(obj);
    }

    ObjPtr ObjFanin1(ObjPtr obj) {
        return abc::Abc_ObjFanin1(obj);
    }

    std::vector<ObjPtr> ObjFanins(ObjPtr obj) {
        abc::Abc_Obj_t *fan_in;
        int i;
        std::vector<ObjPtr> fan_ins;
        Abc_ObjForEachFanin(obj, fan_in, i) {
            fan_ins.push_back(fan_in);
        }
        return fan_ins;
    }

    std::vector<ObjPtr> ObjFanouts(ObjPtr obj) {
        abc::Abc_Obj_t *fan_out;
        int i;
        std::vector<ObjPtr> fan_outs;
        Abc_ObjForEachFanout(obj, fan_out, i) {
            fan_outs.push_back(fan_out);
        }
        return fan_outs;
    }

    void ObjReplace(ObjPtr obj_old, ObjPtr obj_new) { abc::Abc_ObjTransferFanout(obj_old, obj_new); }

    void ObjRecover(ObjPtr obj, ObjPtr obj_bak) {
        for (const auto &fan_out_bak : ObjFanouts(obj_bak)) {
            auto fan_out = NtkObjbyID(ObjHostNtk(obj), ObjID(fan_out_bak));
            abc::Abc_ObjRemoveFanins(fan_out);
            for (const auto &fan_in_bak : ObjFanins(fan_out_bak)) {
                auto fan_in = NtkObjbyID(ObjHostNtk(obj), ObjID(fan_in_bak));
                abc::Abc_ObjAddFanin(fan_out, fan_in);
            }
        }
    }

    ObjPtr ObjCreateInv(ObjPtr fan_in) { return abc::Abc_NtkCreateNodeInv(ObjHostNtk(fan_in), fan_in); }

    void ObjDelete(ObjPtr obj) { abc::Abc_NtkDeleteObj(obj); }
}
