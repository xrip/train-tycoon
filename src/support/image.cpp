#include "support/image.h"

#include "stb_image.h"

#include <utility>

Image::Image(int width, int height, std::vector<std::uint32_t> pixels,
             bool mutable_image)
    : width_(width), height_(height), pixels_(std::move(pixels)) {
    if (mutable_image) {
        surface_ = Surface{pixels_.data(), width_, height_, true, 0,
                           0, 0, width_, height_};
    }
}

std::unique_ptr<Image> Image::create_blank(int width, int height) {
    if (width <= 0 || height <= 0) {
        return nullptr;
    }
    std::vector<std::uint32_t> pixels(
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height),
        0xFFFFFFFFu);
    return std::unique_ptr<Image>(
        new Image(width, height, std::move(pixels), true));
}

std::unique_ptr<Image> Image::decode_png(
    std::span<const std::uint8_t> data) {
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* rgba = stbi_load_from_memory(
        data.data(), static_cast<int>(data.size()),
        &width, &height, &channels, 4);
    if (rgba == nullptr || width <= 0 || height <= 0) {
        stbi_image_free(rgba);
        return nullptr;
    }

    std::vector<std::uint32_t> pixels(
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    for (std::size_t index = 0; index < pixels.size(); ++index) {
        const std::uint32_t red = rgba[index * 4];
        const std::uint32_t green = rgba[index * 4 + 1];
        const std::uint32_t blue = rgba[index * 4 + 2];
        const std::uint32_t alpha = rgba[index * 4 + 3];
        pixels[index] = (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
    stbi_image_free(rgba);
    return std::unique_ptr<Image>(
        new Image(width, height, std::move(pixels), false));
}

Surface* Image::drawing_surface() {
    return surface_ ? &*surface_ : nullptr;
}
