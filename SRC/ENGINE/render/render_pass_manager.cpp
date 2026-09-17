#include "render_pass_manager.h"
#include <algorithm>

void RenderPass::setup(Renderer*) {}
void RenderPass::cleanup(Renderer*) {}

namespace q3d {
namespace render {

RenderPassManager& RenderPassManager::instance() { static RenderPassManager m; return m; }

void RenderPassManager::register_pass(std::shared_ptr<RenderPass> pass) {
    if (!pass) return;
    passes_.push_back(pass);
    std::sort(passes_.begin(), passes_.end(), [](const std::shared_ptr<RenderPass>& a, const std::shared_ptr<RenderPass>& b) {
        return a->order() < b->order();
    });
}

void RenderPassManager::unregister_pass(const char* pass_id) {
    remove_pass(pass_id);
}

void RenderPassManager::unregister_all_for_mod(const char* mod_id) {
    (void)mod_id;
}

void RenderPassManager::execute_all(Renderer* r, const RenderContext& ctx) {
    for (auto& p : passes_) p->execute(r, ctx);
}

void RenderPassManager::insert_pass(std::shared_ptr<RenderPass> pass, const char* after_pass_id) {
    if (!pass) return;
    for (auto it = passes_.begin(); it != passes_.end(); ++it) {
        if ((*it)->id() == after_pass_id) {
            passes_.insert(it + 1, pass);
            return;
        }
    }
    passes_.push_back(pass);
}

void RenderPassManager::remove_pass(const char* pass_id) {
    passes_.erase(std::remove_if(passes_.begin(), passes_.end(), [pass_id](const std::shared_ptr<RenderPass>& p) {
        return p->id() == pass_id;
    }), passes_.end());
}

void RenderPassManager::add_postfx(std::shared_ptr<RenderPass> pass) {
    register_pass(pass);
}

void RenderPassManager::remove_postfx(const char* pass_id) {
    remove_pass(pass_id);
}

}
}
