#pragma once
#include <string>
#include <vector>
#include <memory>

class Renderer;
struct RenderContext;

class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual std::string id() const = 0;
    virtual int order() const = 0;
    virtual void setup(Renderer* r);
    virtual void execute(Renderer* r, const RenderContext& ctx) = 0;
    virtual void cleanup(Renderer* r);
};

namespace q3d {
namespace render {

class RenderPassManager {
public:
    static RenderPassManager& instance();
    void register_pass(std::shared_ptr<RenderPass> pass);
    void unregister_pass(const char* pass_id);
    void unregister_all_for_mod(const char* mod_id);
    const std::vector<std::shared_ptr<RenderPass>>& passes() const { return passes_; }
    void execute_all(Renderer* r, const RenderContext& ctx);
    void insert_pass(std::shared_ptr<RenderPass> pass, const char* after_pass_id);
    void remove_pass(const char* pass_id);
    void add_postfx(std::shared_ptr<RenderPass> pass);
    void remove_postfx(const char* pass_id);
private:
    std::vector<std::shared_ptr<RenderPass>> passes_;
};

}
}
