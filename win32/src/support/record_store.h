#ifndef SUPPORT_RECORD_STORE_H
#define SUPPORT_RECORD_STORE_H

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

class RecordStore {
public:
    static void set_root(std::filesystem::path root);
    static std::filesystem::path path_for(std::string_view name);
    static std::unique_ptr<RecordStore> open(std::string_view name,
                                             bool create_if_needed);

    ~RecordStore();
    RecordStore(const RecordStore&) = delete;
    RecordStore& operator=(const RecordStore&) = delete;
    RecordStore(RecordStore&&) = delete;
    RecordStore& operator=(RecordStore&&) = delete;

    int add_record(std::span<const std::uint8_t> data);
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> record(int id) const;
    void delete_record(int id);
    [[nodiscard]] int record_count() const;
    [[nodiscard]] int first_record_id() const;
    [[nodiscard]] int record_id_at(int index) const;
    [[nodiscard]] int record_size(int id) const;

private:
    struct Record {
        int id;
        std::vector<std::uint8_t> data;
    };

    explicit RecordStore(std::filesystem::path path);
    void load();
    void flush() const;
    [[nodiscard]] const Record* find(int id) const;

    std::filesystem::path path_;
    std::vector<Record> records_;
    int next_id_ = 1;
};

#endif
