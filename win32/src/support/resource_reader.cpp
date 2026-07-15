#include "support/resource_reader.h"

#include <utility>

namespace filesystem = std::filesystem;

namespace {

filesystem::path resource_root = "resources";

std::string_view without_leading_slash(std::string_view name) {
    if (!name.empty() && name.front() == '/') {
        name.remove_prefix(1);
    }
    return name;
}

} // namespace

ResourceReader::ResourceReader(std::ifstream stream)
    : stream_(std::move(stream)) {}

ResourceReader::operator bool() const {
    return stream_.is_open();
}

int ResourceReader::read_byte() {
    return stream_.get();
}

namespace resources {

void set_root(filesystem::path root) {
    resource_root = std::move(root);
}

filesystem::path path_for(std::string_view name) {
    return resource_root / without_leading_slash(name);
}

ResourceReader open(std::string_view name) {
    return ResourceReader{std::ifstream(path_for(name), std::ios::binary)};
}

std::optional<std::vector<std::uint8_t>> read_all(std::string_view name) {
    std::ifstream stream(path_for(name), std::ios::binary | std::ios::ate);
    if (!stream) {
        return std::nullopt;
    }

    const std::streampos end = stream.tellg();
    if (end < 0) {
        return std::nullopt;
    }

    std::vector<std::uint8_t> data(static_cast<std::size_t>(end));
    stream.seekg(0);
    if (!data.empty()) {
        stream.read(reinterpret_cast<char*>(data.data()),
                    static_cast<std::streamsize>(data.size()));
        if (!stream) {
            return std::nullopt;
        }
    }
    return data;
}

} // namespace resources
