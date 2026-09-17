#pragma once

#include <string>
#include <vector>
#include <memory>

namespace q3d {
namespace actor {

using std::string;
using std::vector;
using std::shared_ptr;

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float px, float py, float pz) : x(px), y(py), z(pz) {}
};

struct Mat4 {
    float m[16];
    Mat4() {
        for (int i = 0; i < 16; ++i)
            m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
    float& operator()(int row, int col) { return m[col * 4 + row]; }
    const float& operator()(int row, int col) const { return m[col * 4 + row]; }
};

class Animation {
public:
    string name;
};

struct IKChain {
    string name;
    vector<string> bones;
    int tip_index = 0;
    int pole_index = -1;
    float tolerance = 0.001f;
};

class IKRig {
public:
    string type;
    vector<IKChain> chains;
};

const IKRig& HumanoidRig();
const IKRig& QuadrupedRig();

class IKSolver {
public:
    virtual ~IKSolver() = default;
    virtual Vec3 solve(
        const vector<Mat4>& bone_transforms,
        const Vec3& target,
        const Vec3& pole_hint,
        float tolerance,
        int max_iterations
    ) = 0;
};

class CCDSolver : public IKSolver {
public:
    Vec3 solve(
        const vector<Mat4>& bone_transforms,
        const Vec3& target,
        const Vec3& pole_hint,
        float tolerance,
        int max_iterations
    ) override;
};

class FABRIKSolver : public IKSolver {
public:
    Vec3 solve(
        const vector<Mat4>& bone_transforms,
        const Vec3& target,
        const Vec3& pole_hint,
        float tolerance,
        int max_iterations
    ) override;
};

class Retargeter {
public:
    Retargeter(const IKRig& source, const IKRig& target);

    Animation retarget(const Animation& anim);

    void set_bone_mapping(const char* src_bone, const char* dst_bone);
    void set_position_offset(const char* bone, const Vec3& offset);
    void set_scale_factor(float f);

private:
    const IKRig& source_;
    const IKRig& target_;
};

} // namespace actor
} // namespace q3d
