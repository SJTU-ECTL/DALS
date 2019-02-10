/**
 * @file dals.h
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-20
 * @bug No known bugs.
 */

#ifndef DALS_DALS_H
#define DALS_DALS_H

#include <map>
#include <abc_plus.h>

using namespace abc_plus;

/////////////////////////////////////////////////////////////////////////////
/// Class ALC, Approximate Local Change
/////////////////////////////////////////////////////////////////////////////

class ALC {
public:
    //---------------------------------------------------------------------------
    // Getters & Setters
    //---------------------------------------------------------------------------
    double GetError() const;

    ObjPtr GetTarget() const;

    ObjPtr GetSubstitute() const;

    bool IsComplemented() const;

    void SetError(double err);

    void SetTarget(ObjPtr t);

    void SetSubstitute(ObjPtr sub);

    void SetComplemented(bool is_complemented);

    //---------------------------------------------------------------------------
    // ALC Methods
    //---------------------------------------------------------------------------
    void Do();

    void Recover();

    //---------------------------------------------------------------------------
    // Operator Methods, Constructors & Destructors
    //---------------------------------------------------------------------------
//    ALC &operator=(const ALC &other);

    ALC();

    ALC(ObjPtr t, ObjPtr s, bool is_complemented, double error = 1);

    ~ALC();

private:
    double error_;
    bool is_complemented_;
    ObjPtr target_;
    ObjPtr substitute_;
    ObjPtr inv_;
    std::vector<ObjPtr> target_fan_outs_;
};

/////////////////////////////////////////////////////////////////////////////
/// Singleton Class DALS, Delay-Driven Approximate Logic Synthesis
/////////////////////////////////////////////////////////////////////////////

class DALS {
public:
    //---------------------------------------------------------------------------
    // Getters & Setters
    //---------------------------------------------------------------------------
    static std::shared_ptr<DALS> GetDALS();

    NtkPtr GetApproxNtk();

    void SetTargetNtk(NtkPtr ntk);

    void SetSim64Cycles(int sim_64_cycles);

    //---------------------------------------------------------------------------
    // DALS Methods
    //---------------------------------------------------------------------------
    void CalcTruthVec(bool show_progress_bar = false);

    void CalcALCs(const std::vector<ObjPtr> &target_nodes, bool show_progress = false, int top_k = 3);

    double EstSubPairError(ObjPtr target, ObjPtr substitute);

    void Run(double err_constraint = 0.15);

    //---------------------------------------------------------------------------
    // Operator Methods, Constructors & Destructors
    //---------------------------------------------------------------------------
    void operator=(DALS const &) = delete;

    DALS(Framework const &) = delete;

    ~DALS();

private:
    NtkPtr target_ntk_;
    NtkPtr approx_ntk_;
    int sim_64_cycles_;
    std::unordered_map<ObjPtr, std::vector<uint64_t>> truth_vec_;
    std::unordered_map<ObjPtr, std::vector<ALC>> cand_alcs_;
    std::unordered_map<ObjPtr, ALC> opt_alc_;

    DALS();
};

#endif
