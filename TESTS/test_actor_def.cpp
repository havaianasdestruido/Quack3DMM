#include "actor/actor_def.h"

int main() {
    q3d::actor::ActorDefinition def;
    def.id = "test";
    def.name = "Test Actor";
    q3d::actor::ActorDefinitionLoader::save(def, "test.actor");
    auto loaded = q3d::actor::ActorDefinitionLoader::load("test.actor");
    return loaded ? 0 : 1;
}
