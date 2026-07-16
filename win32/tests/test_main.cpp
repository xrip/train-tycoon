#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "midp.h"
#include "support/resource_reader.h"
#include "support/record_store.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace filesystem = std::filesystem;

int main(int argc, char** argv) {
    const filesystem::path test_root = filesystem::current_path();
    const filesystem::path resource_root = test_root / "resources";
    const filesystem::path save_root = test_root / "saves";
    if (!filesystem::exists(resource_root / "scenarios.bin")) {
        std::cerr << "Test resource root is missing: " << resource_root << '\n';
        return 2;
    }

    filesystem::create_directories(save_root);
    filesystem::remove(save_root / "TTStore.rms");
    filesystem::remove(save_root / "RulesStore.rms");
    resources::set_root(resource_root);
    RecordStore::set_root(save_root);

    doctest::Context context(argc, argv);
    return context.run();
}
