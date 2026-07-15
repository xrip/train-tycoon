#ifndef SUPPORT_IMAGE_H
#define SUPPORT_IMAGE_H

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <vector>

struct Surface {
    std::uint32_t* pixels = nullptr;
    int width = 0;
    int height = 0;
    bool has_alpha = false;
    int color = 0;
    int clip_x = 0;
    int clip_y = 0;
    int clip_w = 0;
    int clip_h = 0;
};

class Image {
public:
    static std::unique_ptr<Image> create_blank(int width, int height);
    static std::unique_ptr<Image> decode_png(std::span<const std::uint8_t> data);

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(Image&&) = delete;

    [[nodiscard]] int width() const { return width_; }
    [[nodiscard]] int height() const { return height_; }
    [[nodiscard]] std::span<const std::uint32_t> pixels() const { return pixels_; }
    [[nodiscard]] Surface* drawing_surface();

private:
    Image(int width, int height, std::vector<std::uint32_t> pixels,
          bool mutable_image);

    int width_;
    int height_;
    std::vector<std::uint32_t> pixels_;
    std::optional<Surface> surface_;
};

#endif
