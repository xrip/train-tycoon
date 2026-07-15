#include "support/record_store.h"

#include <algorithm>
#include <fstream>
#include <utility>

namespace filesystem = std::filesystem;

namespace {

filesystem::path save_root = ".";

bool read_i32(std::ifstream& stream, std::int32_t& value) {
    return static_cast<bool>(stream.read(
        reinterpret_cast<char*>(&value), sizeof(value)));
}

void write_i32(std::ofstream& stream, std::int32_t value) {
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

} // namespace

void RecordStore::set_root(filesystem::path root) {
    save_root = std::move(root);
}

filesystem::path RecordStore::path_for(std::string_view name) {
    return save_root / (std::string(name) + ".rms");
}

std::unique_ptr<RecordStore> RecordStore::open(std::string_view name,
                                                bool create_if_needed) {
    const filesystem::path path = path_for(name);
    if (!filesystem::is_regular_file(path) && !create_if_needed) {
        return nullptr;
    }
    auto store = std::unique_ptr<RecordStore>(new RecordStore(path));
    store->load();
    return store;
}

RecordStore::RecordStore(filesystem::path path) : path_(std::move(path)) {}

RecordStore::~RecordStore() {
    flush();
}

void RecordStore::load() {
    std::ifstream stream(path_, std::ios::binary);
    if (!stream) {
        return;
    }

    std::int32_t count = 0;
    std::int32_t next_id = 1;
    if (!read_i32(stream, count) || !read_i32(stream, next_id) || count < 0) {
        return;
    }

    for (std::int32_t index = 0; index < count; ++index) {
        std::int32_t id = 0;
        std::int32_t size = 0;
        if (!read_i32(stream, id) || !read_i32(stream, size) || size < 0) {
            break;
        }
        std::vector<std::uint8_t> data(static_cast<std::size_t>(size));
        if (size > 0 && !stream.read(reinterpret_cast<char*>(data.data()), size)) {
            break;
        }
        records_.push_back(Record{id, std::move(data)});
    }
    next_id_ = next_id;
}

void RecordStore::flush() const {
    std::ofstream stream(path_, std::ios::binary | std::ios::trunc);
    if (!stream) {
        return;
    }
    write_i32(stream, static_cast<std::int32_t>(records_.size()));
    write_i32(stream, next_id_);
    for (const Record& record : records_) {
        write_i32(stream, record.id);
        write_i32(stream, static_cast<std::int32_t>(record.data.size()));
        if (!record.data.empty()) {
            stream.write(reinterpret_cast<const char*>(record.data.data()),
                         static_cast<std::streamsize>(record.data.size()));
        }
    }
}

int RecordStore::add_record(std::span<const std::uint8_t> data) {
    const int id = next_id_++;
    records_.push_back(Record{id, {data.begin(), data.end()}});
    return id;
}

const RecordStore::Record* RecordStore::find(int id) const {
    const auto found = std::find_if(records_.begin(), records_.end(),
        [id](const Record& record) { return record.id == id; });
    return found == records_.end() ? nullptr : &*found;
}

std::optional<std::vector<std::uint8_t>> RecordStore::record(int id) const {
    const Record* found = find(id);
    return found ? std::optional{found->data} : std::nullopt;
}

void RecordStore::delete_record(int id) {
    const auto found = std::find_if(records_.begin(), records_.end(),
        [id](const Record& record) { return record.id == id; });
    if (found != records_.end()) {
        records_.erase(found);
    }
}

int RecordStore::record_count() const {
    return static_cast<int>(records_.size());
}

int RecordStore::first_record_id() const {
    return records_.empty() ? -1 : records_.front().id;
}

int RecordStore::record_id_at(int index) const {
    return index >= 0 && index < record_count() ? records_[index].id : -1;
}

int RecordStore::record_size(int id) const {
    const Record* found = find(id);
    return found ? static_cast<int>(found->data.size()) : -1;
}
