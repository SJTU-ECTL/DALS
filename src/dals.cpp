/**
 * @file dals.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-20
 * @bug No known bugs.
 */

#include <dals.h>

void ALC::SetError(double err) { error_ = err; }

void ALC::SetTarget(ObjPtr t) { target_ = t; }

void ALC::SetSubstitute(ObjPtr sub) { substitute_ = sub; }

void ALC::SetComplemented(bool is_complemented) { is_complemented_ = is_complemented; }

double ALC::GetError() const { return error_; }

ObjPtr ALC::GetTarget() const { return target_; }

ObjPtr ALC::GetSubstitute() const { return substitute_; }

bool ALC::IsComplemented() const { return is_complemented_; }

bool ALC::CanBeComplemented() const { return can_be_complemented_; }

void ALC::Do() {
    auto ntk = ObjHostNtk(target_);
    if (is_complemented_) {
        inv_ = ObjCreateInv(substitute_);
        ObjReplace(target_, inv_);
    } else
        ObjReplace(target_, substitute_);
}

void ALC::Recover() {
    for (auto const &fan_out : target_fan_outs_) {
        if (is_complemented_)
            abc::Abc_ObjPatchFanin(fan_out, inv_, target_);
        else
            abc::Abc_ObjPatchFanin(fan_out, substitute_, target_);
        abc::Vec_IntPush(&target_->vFanouts, ObjID(fan_out));
    }
    if (is_complemented_)
        ObjDelete(inv_);
}

ALC &ALC::operator=(const ALC &other) {
    target_ = other.target_;
    substitute_ = other.substitute_;
    is_complemented_ = other.is_complemented_;
    return *this;
}

ALC::ALC() = default;

ALC::ALC(ObjPtr t, ObjPtr s, bool can_be_complemented) : error_(1), can_be_complemented_(can_be_complemented), is_complemented_(false),
                                                         target_(t), substitute_(s), inv_(nullptr),
                                                         target_fan_outs_(std::vector<ObjPtr>()) {
    for (auto const &fan_out : ObjFanouts(target_))
        target_fan_outs_.push_back(fan_out);
}
