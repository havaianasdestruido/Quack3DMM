#include "skeleton.h"

namespace q3d {
namespace actor {

const IKRig& HumanoidRig() {
    static IKRig rig;
    rig.type = "humanoid";
    return rig;
}

const IKRig& QuadrupedRig() {
    static IKRig rig;
    rig.type = "quadruped";
    return rig;
}

Vec3 CCDSolver::solve(
    const vector<Mat4>& bone_transforms,
    const Vec3& target,
    const Vec3& pole_hint,
    float tolerance,
    int max_iterations
) {
    (void)bone_transforms;
    (void)target;
    (void)pole_hint;
    (void)tolerance;
    (void)max_iterations;
    return Vec3();
}

Vec3 FABRIKSolver::solve(
    const vector<Mat4>& bone_transforms,
    const Vec3& target,
    const Vec3& pole_hint,
    float tolerance,
    int max_iterations
) {
    (void)bone_transforms;
    (void)target;
    (void)pole_hint;
    (void)tolerance;
    (void)max_iterations;
    return Vec3();
}

Retargeter::Retargeter(const IKRig& source, const IKRig& target)
    : source_(source), target_(target) {}

Animation Retargeter::retarget(const Animation& anim) {
    return anim;
}

void Retargeter::set_bone_mapping(const char* src_bone, const char* dst_bone) {
    (void)src_bone;
    (void)dst_bone;
}

void Retargeter::set_position_offset(const char* bone, const Vec3& offset) {
    (void)bone;
    (void)offset;
}

void Retargeter::set_scale_factor(float f) {
    (void)f;
}

} // namespace actor
} // namespace q3d
