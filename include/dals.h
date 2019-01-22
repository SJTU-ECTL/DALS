/**
 * @file dals.h
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-20
 * @bug No known bugs.
 */

#ifndef DALS_DALS_H
#define DALS_DALS_H

#include <abc_plus.h>

using namespace abc_plus;

class ALC {
public:
    void SetError(double err);

    void SetTarget(ObjPtr t);

    void SetSubstitute(ObjPtr sub);

    void SetComplemented(bool is_complemented);

    double GetError() const;

    ObjPtr GetTarget() const;

    ObjPtr GetSubstitute() const;

    bool IsComplemented() const;

    bool CanBeComplemented() const;

    void Do();

    void Recover();

    ALC &operator=(const ALC &other);

    ALC();

    ALC(ObjPtr t, ObjPtr s, bool can_be_complemented);

private:
    double error_;
    bool can_be_complemented_;
    bool is_complemented_;
    ObjPtr target_;
    ObjPtr substitute_;
    ObjPtr inv_;
    std::vector<ObjPtr> target_fan_outs_;
};

class DALS {

};

#endif
