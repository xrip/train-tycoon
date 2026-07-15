#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "midp.h"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::filesystem::path test_root = std::filesystem::current_path();
    const std::filesystem::path resource_root = test_root / "resources";
    const std::filesystem::path save_root = test_root / "saves";
    if (!std::filesystem::exists(resource_root / "1")) {
        std::cerr << "Test resource root is missing: " << resource_root << '\n';
        return 2;
    }

    std::filesystem::create_directories(save_root);
    std::filesystem::remove(save_root / "TTStore.rms");
    const std::string resource_root_text = resource_root.string();
    const std::string save_root_text = save_root.string();
    midp_resource_set_root(resource_root_text.c_str());
    midp_rms_set_root(save_root_text.c_str());

    doctest::Context context(argc, argv);
    return context.run();
}
