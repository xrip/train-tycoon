#ifndef SUPPORT_RESOURCE_READER_H
#define SUPPORT_RESOURCE_READER_H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string_view>
#include <vector>

class ResourceReader {
public:
    ResourceReader() = default;
    explicit ResourceReader(std::ifstream stream);

    ResourceReader(const ResourceReader&) = delete;
    ResourceReader& operator=(const ResourceReader&) = delete;
    ResourceReader(ResourceReader&&) noexcept = default;
    ResourceReader& operator=(ResourceReader&&) noexcept = default;

    explicit operator bool() const;
    int read_byte();

private:
    std::ifstream stream_;
};

namespace resources {

void set_root(std::filesystem::path root);
std::filesystem::path path_for(std::string_view name);
ResourceReader open(std::string_view name);
std::optional<std::vector<std::uint8_t>> read_all(std::string_view name);

} // namespace resources

#endif
