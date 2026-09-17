#include "mod/mod.h"
#include "mod/mod_manifest.h"
#include <cassert>
#include <string>

int main() {
    const char* toml =
        "schema_version = 2\n"
        "id = \"cool-character-pack\"\n"
        "name = \"Cool Character Pack\"\n"
        "version = \"1.2.3\"\n"
        "description = \"Test mod\"\n"
        "engine_version = \"^1.0.0\"\n"
        "runtime = \"lua54\"\n"
        "sandbox = true\n"
        "memory_budget_mb = 256\n";

    auto manifest = q3d::mod::ManifestParser::parse(toml, "tests/mod");
    auto errors = q3d::mod::ManifestParser::validate(manifest);

    q3d::mod::Version v = q3d::mod::Version::parse("1.2.3");
    bool ok1 = v.satisfies("^1.2.0");
    bool ok2 = v.satisfies("~1.2.0");
    bool ok3 = v.satisfies(">=1.0.0 <2.0.0");

    auto toml_out = q3d::mod::ManifestWriter::to_toml(manifest);
    auto manifest2 = q3d::mod::ManifestParser::parse(toml_out.c_str(), "tests/mod2");

    return (errors.empty() && ok1 && ok2 && ok3 && manifest.id == manifest2.id) ? 0 : 1;
}