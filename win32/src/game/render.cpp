// Packed-sprite UI and text drawing.
// renderPackedSprite draws the menu/dialog layouts (state 20). renderFontString
// draws text from the `ca[]` font atlas. u() resolves per-slot sub-sprites.
#include "game.h"
#include "support/numeric.h"
#include "game/dialog_table.h"
#include "text/text_catalog.h"

#define RGB3(red, green, blue) (((red) << 16) | ((green) << 8) | (blue))
static int max_value(int left_value, int right_value) {
    return left_value > right_value ? left_value : right_value;
}

namespace font_mark {
// These codes draw small marks instead of loading a font sprite. The names
// describe the recovered geometry; their exact alphabet mapping belongs to
// the original font atlas.
constexpr int8_t kDot = 32;
constexpr int8_t kRisingStroke = 33;
constexpr int8_t kCaret = 34;
constexpr int8_t kCaretWithTopDot = 35;
constexpr int8_t kDoubleDot = 37;
}  // namespace font_mark

namespace font_layout {
constexpr int8_t kFirstMarkCode = font_mark::kDot;
constexpr int8_t kLastMarkCode = 39;
constexpr int8_t kFirstLowercaseCode = 40;
constexpr int8_t kLastLowercaseCode = 65;
constexpr int8_t kFirstUppercaseCode = 70;
constexpr int8_t kAfterLastCode = 100;
constexpr int kFontPageWidth = 16;
constexpr int kGlyphCellWidth = 5;
constexpr int kGlyphAdvance = 6;
constexpr int kGlyphClipTopOffset = 4;
constexpr int kGlyphClipHeight = 12;
constexpr int kMarkCodeCount = 40;
}  // namespace font_layout

namespace text_box_layout {
constexpr int32_t kCenterAlignment = 1;
constexpr int32_t kBottomAlignment = 2;
constexpr int32_t kRightAlignment = 4;
constexpr int32_t kStatusBarAlignment = 37;
constexpr int32_t kHorizontalPadding = 5;
constexpr int32_t kHeight = 13;
constexpr int32_t kBottomBorderOffset = 13;
}  // namespace text_box_layout

namespace dialog_text_layout {
constexpr int32_t kDynamicValueRightPadding = 6;
}  // namespace dialog_text_layout

namespace selected_train_text {
constexpr int32_t kEngineTextBase = 80;
constexpr int32_t kWagonTextBase = 86;
constexpr int32_t kByteMask = 0xFF;
}  // namespace selected_train_text

namespace wagon_sprite {
constexpr int32_t kBuildMenuContext = 23;
constexpr int32_t kTrainContext = 26;
constexpr int32_t kEngineSlot = 0;
constexpr int32_t kFirstWagonSlot = 1;
constexpr int32_t kNoSpriteCode = -1;

// These are asset-table keys, not visual tile ids. The art meaning is kept
// neutral because the sprite bundle is the source of truth.
constexpr int32_t kEngineTypeASpriteCode = 136;
constexpr int32_t kEngineTypeBSpriteCode = 146;
constexpr int32_t kPassengerMailSpriteCode = 162;
constexpr int32_t kIndustryGroup1SpriteCode = 167;
constexpr int32_t kIndustryGroup2SpriteCode = 172;
constexpr int32_t kIndustryGroup2AlternateSpriteCode = 177;
constexpr int32_t kIndustryGroup3SpriteCode = 182;
constexpr int32_t kIndustryGroup3AlternateSpriteCode = 187;

int32_t sprite_code_for_option(int32_t wagon_option) {
    switch (wagon_option) {
        case wagon::PassengerMail: return kPassengerMailSpriteCode;
        case wagon::IndustryGroup1: return kIndustryGroup1SpriteCode;
        case wagon::IndustryGroup2: return kIndustryGroup2SpriteCode;
        case wagon::IndustryGroup3: return kIndustryGroup3SpriteCode;
        default: return kNoSpriteCode;
    }
}

int32_t sprite_code_for_car_type(int32_t car_type) {
    switch (car_type) {
        case wagon::EngineTypeA: return kEngineTypeASpriteCode;
        case wagon::EngineTypeB: return kEngineTypeBSpriteCode;
        case wagon::PassengerMailCar: return kPassengerMailSpriteCode;
        case wagon::IndustryGroup1Car: return kIndustryGroup1SpriteCode;
        case wagon::IndustryGroup2CarA: return kIndustryGroup2SpriteCode;
        case wagon::IndustryGroup2CarB:
            return kIndustryGroup2AlternateSpriteCode;
        case wagon::IndustryGroup3CarA: return kIndustryGroup3SpriteCode;
        case wagon::IndustryGroup3CarB:
            return kIndustryGroup3AlternateSpriteCode;
        default: return kNoSpriteCode;
    }
}

Image* image_for_sprite_code(const Assets& assets, int32_t sprite_code) {
    if (sprite_code == kNoSpriteCode) return nullptr;
    const int sprite_index =
        unsigned_byte(assets.tileSpriteIndex[sprite_code]);
    return assets.tile_sprites[sprite_index].get();
}
}  // namespace wagon_sprite

void Renderer::draw_font_string(Surface& target_surface, const Assets& assets,
                                int32_t start_x, int32_t baseline_y,
                                std::span<const int8_t> codes,
                                bool preserve_clip) const {
    Surface *surface = &target_surface;
    int glyph_x = start_x;
    midp_set_color(surface, 0);
    const int code_count = static_cast<int>(codes.size());
    for (int code_index = 0; code_index < code_count; ++code_index) {
        const int8_t glyph_code_value = codes[code_index];
        if (preserve_clip) {
            midp_set_clip(surface, clipX, clipY, clipWidth, clipHeight);
            midp_clip_rect(surface, glyph_x, baseline_y - font_layout::kGlyphClipTopOffset,
                           font_layout::kGlyphCellWidth,
                           font_layout::kGlyphClipHeight);
        } else {
            midp_set_clip(surface, glyph_x, baseline_y - font_layout::kGlyphClipTopOffset,
                          font_layout::kGlyphCellWidth,
                          font_layout::kGlyphClipHeight);
        }
        if (glyph_code_value == 0) {
            glyph_x += font_layout::kGlyphAdvance;
            continue;
        }
        if (glyph_code_value < font_layout::kFirstMarkCode) {
            midp_draw_image(surface,
                            assets.font_images[(glyph_code_value - 1)
                                               / font_layout::kFontPageWidth].get(),
                            glyph_x - font_layout::kGlyphCellWidth
                                * ((glyph_code_value - 1)
                                   % font_layout::kFontPageWidth),
                            baseline_y, 20);
            glyph_x += font_layout::kGlyphAdvance;
            continue;
        }
        if (glyph_code_value < font_layout::kMarkCodeCount) {
            const int8_t mark_y_offset = static_cast<int8_t>(
                codes[code_index + 1] >= font_layout::kFirstLowercaseCode
                && codes[code_index + 1] <= font_layout::kLastLowercaseCode
                    ? 1 : 0);
            switch (glyph_code_value) {
                case font_mark::kDot:
                    midp_draw_line(surface, glyph_x + 2,
                                   baseline_y - 2 + mark_y_offset,
                                   glyph_x + 2,
                                   baseline_y - 2 + mark_y_offset);
                    break;
                case font_mark::kCaretWithTopDot:
                    midp_draw_line(surface, glyph_x + 2,
                                   baseline_y - 4 + mark_y_offset,
                                   glyph_x + 2,
                                   baseline_y - 4 + mark_y_offset);
                    /* fall through */
                case font_mark::kCaret:
                    midp_draw_line(surface, glyph_x + 2,
                                   baseline_y - 2 + mark_y_offset,
                                   glyph_x + 1,
                                   baseline_y - 3 + mark_y_offset);
                    /* fall through */
                case font_mark::kRisingStroke:
                    midp_draw_line(surface, glyph_x + 2,
                                   baseline_y - 2 + mark_y_offset,
                                   glyph_x + 3,
                                   baseline_y - 3 + mark_y_offset);
                    break;
                case font_mark::kDoubleDot:
                    midp_draw_line(surface, glyph_x + 1,
                                   baseline_y - 2 + mark_y_offset,
                                   glyph_x + 1,
                                   baseline_y - 2 + mark_y_offset);
                    midp_draw_line(surface, glyph_x + 3,
                                   baseline_y - 2 + mark_y_offset,
                                   glyph_x + 3,
                                   baseline_y - 2 + mark_y_offset);
                    break;
            }
            continue;
        }
        if (glyph_code_value < font_layout::kFirstUppercaseCode) {
            midp_draw_image(surface,
                            assets.font_images[2 + (glyph_code_value
                                                    - font_layout::kFirstLowercaseCode)
                                               / font_layout::kFontPageWidth].get(),
                            glyph_x - font_layout::kGlyphCellWidth
                                * ((glyph_code_value
                                    - font_layout::kFirstLowercaseCode)
                                   % font_layout::kFontPageWidth),
                            baseline_y, 20);
            glyph_x += font_layout::kGlyphAdvance;
            continue;
        }
        if (glyph_code_value >= font_layout::kAfterLastCode) continue;
        midp_draw_image(surface,
                        assets.font_images[4 + (glyph_code_value
                                                - font_layout::kFirstUppercaseCode)
                                           / font_layout::kFontPageWidth].get(),
                        glyph_x - font_layout::kGlyphCellWidth
                            * ((glyph_code_value
                                - font_layout::kFirstUppercaseCode)
                               % font_layout::kFontPageWidth),
                        baseline_y, 20);
        glyph_x += font_layout::kGlyphAdvance;
    }
    if (preserve_clip) {
        midp_set_clip(surface, clipX, clipY, clipWidth, clipHeight);
    } else {
        midp_set_clip(surface, 0, 0, canvasWidth, canvasHeight);
    }
}

// Returns the tile sprite image for a wagon/car slot. contextCode 23 is the
// build-menu wagon preview (slotIndex 0 = the engine chosen by wagonType, higher
// slots = the offered wagon in wagonOptionSlots); contextCode 26 is the
// slotIndex-th car of the train under the cursor. Returns NULL when the slot has
// no sprite.
Image* Game::wagon_car_sprite(int32_t context_code, int32_t slot_index) {
    Game *game = this;
    if (context_code == wagon_sprite::kBuildMenuContext) {
        if (slot_index == wagon_sprite::kEngineSlot
            && game->build.wagonType >= train_era::First
            && game->build.wagonType <= train_era::Third)
            return wagon_sprite::image_for_sprite_code(
                game->assets, wagon_sprite::kEngineTypeASpriteCode);
        if (slot_index == wagon_sprite::kEngineSlot
            && game->build.wagonType >= train_era::Fourth
            && game->build.wagonType <= train_era::Sixth)
            return wagon_sprite::image_for_sprite_code(
                game->assets, wagon_sprite::kEngineTypeBSpriteCode);
        if (slot_index >= wagon_sprite::kFirstWagonSlot) {
            const int32_t sprite_code = wagon_sprite::sprite_code_for_option(
                game->build.wagonOptionSlots[slot_index - wagon_sprite::kFirstWagonSlot]);
            return wagon_sprite::image_for_sprite_code(game->assets, sprite_code);
        }
    }
    if (context_code == wagon_sprite::kTrainContext
        && game->fleet.count_cars(game->trainAtCursor) > slot_index) {
        int car_slot = game->trainAtCursor;
        for (int wagon_offset = 0; wagon_offset < slot_index; ++wagon_offset)
            car_slot = game->fleet.trainCars[car_slot].nextCar;
        const int car_type =
            train_flag::type(game->fleet.trainCars[car_slot].flags);
        return wagon_sprite::image_for_sprite_code(
            game->assets, wagon_sprite::sprite_code_for_car_type(car_type));
    }
    return nullptr;
}

void Renderer::draw_text_box(Surface& surface_value, const Assets& assets,
                             int32_t x, int32_t y, int32_t color_set,
                             int32_t alignment,
                             std::span<const int8_t> codes) const {
    Surface *surface = &surface_value;
    int text_width = 0;
    const int code_count = static_cast<int>(codes.size());
    for (int code_index = 0; code_index < code_count; ++code_index) {
        if (codes[code_index] >= font_layout::kFirstMarkCode
            && codes[code_index] <= font_layout::kLastMarkCode)
            continue;
        ++text_width;
    }
    text_width *= font_layout::kGlyphAdvance;
    if ((alignment & text_box_layout::kCenterAlignment)
        == text_box_layout::kCenterAlignment)
        x -= text_width / 2 + 3;
    if ((alignment & text_box_layout::kBottomAlignment)
        == text_box_layout::kBottomAlignment)
        y -= 5;
    if ((alignment & text_box_layout::kRightAlignment)
        == text_box_layout::kRightAlignment)
        x -= text_width + font_layout::kGlyphAdvance;
    if (alignment == text_box_layout::kStatusBarAlignment) {
        x = 0;
        y = 15 + gameplayViewportHeight - 15;
    }
    const int graph_color_index = 3 * color_set;
    const int box_width = text_width + text_box_layout::kHorizontalPadding;
    midp_set_color(surface, graphColors[graph_color_index]);
    midp_fill_rect(surface, x, y, box_width, text_box_layout::kHeight);
    midp_set_color(surface, graphColors[graph_color_index + 1]);
    midp_draw_rect(surface, x, y, box_width, text_box_layout::kHeight);
    midp_set_color(surface, graphColors[graph_color_index + 2]);
    midp_draw_rect(surface, x, y + text_box_layout::kBottomBorderOffset,
                   box_width, 0);
    midp_draw_rect(surface, x + box_width, y, 0, text_box_layout::kHeight);
    draw_font_string(*surface, assets, x + 4, y + 5, codes, false);
}

void Renderer::draw_selected_train_label(Surface& surface_value,
                                         const Assets& assets,
                                         const UiController& ui, int32_t x,
                                         int32_t y, int8_t selection_index) {
    Surface *surface = &surface_value;
    const std::vector<int8_t> engine_codes =
        assets.index_text_codes(
            selected_train_text::kEngineTextBase
            + (ui.trainSelectionIndex[selection_index] >> 8
               & selected_train_text::kByteMask));
    const std::vector<int8_t> wagon_codes =
        assets.index_text_codes(
            selected_train_text::kWagonTextBase
            + (ui.trainSelectionIndex[selection_index]
               & selected_train_text::kByteMask));
    std::vector<int8_t> train_codes;
    train_codes.reserve(wagon_codes.size() + engine_codes.size());
    train_codes.insert(train_codes.end(), wagon_codes.begin(), wagon_codes.end());
    train_codes.insert(train_codes.end(), engine_codes.begin(), engine_codes.end());
    int text_width = 0;
    for (const int8_t code : train_codes) {
        if (code >= font_layout::kFirstMarkCode
            && code <= font_layout::kLastMarkCode)
            continue;
        ++text_width;
    }
    text_width *= font_layout::kGlyphAdvance;
    x -= text_width / 2 + 3;
    y -= 5;
    midp_set_color(surface, RGB3(255, 176, 98));
    midp_fill_rect(surface, x, y, text_width + text_box_layout::kHorizontalPadding, 11);
    midp_set_color(surface, RGB3(255, 210, 170));
    midp_draw_rect(surface, x, y, text_width + text_box_layout::kHorizontalPadding, 11);
    midp_set_color(surface, RGB3(255, 130, 0));
    midp_draw_rect(surface, x, y + 11,
                   text_width + text_box_layout::kHorizontalPadding, 0);
    midp_draw_rect(surface, x + text_width + text_box_layout::kHorizontalPadding,
                   y, 0, 11);
    clipX = 0;
    clipY = 15;
    clipWidth = canvasWidth;
    clipHeight = gameplayViewportHeight;
    draw_font_string(*surface, assets, x + 4, y + 3, train_codes, true);
}

namespace {

// The dialog layout now comes from the in-code table (game/dialog_table.h),
// transcoded from packed_sprites.bin. These adapters map a table entry onto the
// structs the render loop below already used, so that loop is unchanged.

struct PackedDialogHeader {
    int flags;
    int backgroundColor;
    int edgeColor;
    int borderColor;
    int selectionColor;
    int width;
    int height;
    int spriteId;
};

PackedDialogHeader headerFromDialog(const PackedDialog& dialog) {
    PackedDialogHeader header{};
    header.flags = dialog.flags;
    header.backgroundColor = dialog.background_color;
    header.edgeColor = dialog.edge_color;
    header.borderColor = dialog.border_color;
    header.selectionColor = dialog.selection_color;
    header.width = dialog.width;
    header.height = dialog.height;
    header.spriteId = dialog.nested_sprite_id;
    return header;
}

struct PackedContentItem {
    int flags = 0;
    bool image = false;
    int xOffset = 0;
    int yOffset = 0;
    bool hasResourceCondition = false;
    bool hasDynamicValue = false;
    int resourceId = 0;
    int valueId = 0;
    int actionCode = 0;
    int selectionLeft = 0;
    int selectionTop = 0;
    int selectionRight = 0;
    int selectionBottom = 0;
    int slotIndex = 0;
    std::optional<std::vector<int8_t>> textCodes;
};

PackedContentItem itemFromDialog(const DialogItem& source) {
    PackedContentItem item{};
    item.flags = source.flags;
    item.image = source.image;
    item.hasResourceCondition = source.has_resource_condition;
    item.xOffset = source.x_offset;
    item.yOffset = source.y_offset;
    item.hasDynamicValue = source.has_dynamic_value;
    item.resourceId = source.resource_id;
    item.valueId = source.value_id;
    item.actionCode = source.action_code;
    item.selectionLeft = source.sel_left;
    item.selectionTop = source.sel_top;
    item.selectionRight = source.sel_right;
    item.selectionBottom = source.sel_bottom;
    item.slotIndex = source.slot_index;
    // Static text comes from the catalog; dynamic text is resolved by the caller.
    if (!source.image && !source.has_dynamic_value)
        item.textCodes = catalog_codes(source.text);
    return item;
}

namespace drawing_command {
enum Type : int32_t {
    Line = 0,
    OutlineRectangle = 1,
    FilledRectangle = 2,
    OutlineArc = 3,
    FilledArc = 4,
};

constexpr int32_t kFullCircleStartAngle = 0;
constexpr int32_t kFullCircleSweepAngle = 360;

namespace flag {
constexpr int TypeMask = 0x07;
constexpr int XUsesReportValue = 0x08;
constexpr int YUsesReportValue = 0x10;
constexpr int WidthUsesReportValue = 0x20;
constexpr int HeightUsesReportValue = 0x40;
constexpr int ResourceCondition = 0x80;
}  // namespace flag
}  // namespace drawing_command

struct PackedDrawingCommand {
    int flags;
    drawing_command::Type type;
    int color;
    int x;
    int y;
    int widthOrX2;
    int heightOrY2;
    bool hasResourceCondition;
    int resourceId;
};

PackedDrawingCommand commandFromDialog(const UiController& ui,
                                       int32_t sprite_id,
                                       const DialogCommand& source) {
    PackedDrawingCommand command{};
    command.flags = source.flags;
    command.type = static_cast<drawing_command::Type>(
        command.flags & drawing_command::flag::TypeMask);
    command.color = source.color;
    command.x = source.x;
    command.y = source.y;
    command.widthOrX2 = source.w;
    command.heightOrY2 = source.h;
    if ((command.flags & drawing_command::flag::XUsesReportValue) != 0)
        command.x = static_cast<int16_t>(ui.report_graph_value(
            sprite_id, command.x));
    if ((command.flags & drawing_command::flag::YUsesReportValue) != 0)
        command.y = static_cast<int16_t>(ui.report_graph_value(
            sprite_id, command.y));
    if ((command.flags & drawing_command::flag::WidthUsesReportValue) != 0)
        command.widthOrX2 = static_cast<int16_t>(ui.report_graph_value(
            sprite_id, command.widthOrX2));
    if ((command.flags & drawing_command::flag::HeightUsesReportValue) != 0)
        command.heightOrY2 = static_cast<int16_t>(ui.report_graph_value(
            sprite_id, command.heightOrY2));
    command.hasResourceCondition =
        (command.flags & drawing_command::flag::ResourceCondition) != 0;
    command.resourceId = source.resource_id;
    return command;
}

}  // namespace

class Game::PackedDialogRender {
public:
    PackedDialogRender(Game& gameValue, Surface& surfaceValue, int32_t spriteIdValue)
        : game(&gameValue), surface(&surfaceValue), spriteId(spriteIdValue),
          dialog(dialog_for_sprite(spriteIdValue)) {}

    bool valid() const { return dialog != nullptr; }
    void setupFrame();
    void renderContent();
    void renderCommands();
    void finishFrame();

private:
    Game *game;
    Surface *surface;
    int32_t spriteId;
    const PackedDialog *dialog;
    int backgroundColor = 0;
    int edgeColor = 0;
    int borderColor = 0;
    int selectionColor = 0;
    int dialogWidth = 0;
    int dialogHeight = 0;
    int dialogX = 0;
    int dialogY = 0;
    int textContentY = 6;
    int drawingContentY = 6;
    int scrollButtonY = 6;
    int contentX = 0;
};

void Game::PackedDialogRender::setupFrame() {
    if (game->renderer.lastRenderedSpriteId != spriteId) {
        game->renderer.spriteJustChanged = 1;
        game->renderer.lastRenderedSpriteId = spriteId;
    }
    if (game->renderer.spriteJustChanged != 0) game->renderer.needsRepaint = false;
    if (game->renderer.mapRedrawPending) {
        game->ui.menuDirty = true;
        game->render_map(*surface);
        game->renderer.mapRedrawPending = false;
    }
    const PackedDialogHeader header = headerFromDialog(*dialog);
    backgroundColor = header.backgroundColor;
    edgeColor = header.edgeColor;
    borderColor = header.borderColor;
    selectionColor = header.selectionColor;
    dialogWidth = header.width;
    dialogHeight = header.height;
    if (game->renderer.dialogContentHeight < dialogHeight) game->renderer.dialogContentHeight = dialogHeight;
    else dialogHeight = game->renderer.dialogContentHeight;
    if (game->ui.cursorOverUI || game->ui.cursorAllowZoomOut) dialogWidth += 6;
    game->renderer.currentSpriteId = header.spriteId;
    dialogX = (game->renderer.canvasWidth - dialogWidth) / 2;
    dialogY = (game->renderer.canvasHeight - dialogHeight) / 2;
    if (game->renderer.needsRepaint) {
        midp_set_color(surface, backgroundColor); midp_fill_rect(surface, dialogX, dialogY, dialogWidth, dialogHeight);
        midp_set_color(surface, borderColor); midp_draw_rect(surface, dialogX, dialogY, dialogWidth, dialogHeight);
        midp_set_color(surface, edgeColor); midp_draw_rect(surface, dialogX, dialogY, 0, dialogHeight);
        midp_draw_rect(surface, dialogX, dialogY, dialogWidth, 0);
    }
    game->ui.pauseMenuOpen = false;
    if ((header.flags & dialog_header_flag::CloseButton) != 0) {
        if (game->renderer.needsRepaint && game->ui.cursorDragY == -1) {
            midp_set_color(surface, selectionColor);
            midp_fill_rect(surface, dialogX + dialogWidth - 10, dialogY + 3, 7, 7);
        }
        if (game->renderer.needsRepaint) {
            midp_set_color(surface, borderColor);
            midp_draw_rect(surface, dialogX + dialogWidth - 10, dialogY + 3, 7, 7);
            midp_draw_line(surface, dialogX + dialogWidth - 8, dialogY + 5, dialogX + dialogWidth - 5, dialogY + 8);
            midp_draw_line(surface, dialogX + dialogWidth - 8, dialogY + 8, dialogX + dialogWidth - 5, dialogY + 5);
        }
        game->ui.pauseMenuOpen = true;
        scrollButtonY = 19;
        // Capture the top-right close (X) button so the pointer layer can close
        // the overlay by clicking it -- same geometry as the drawn X box above.
        game->clickMap.add_close(dialogX + dialogWidth - 10, dialogY + 3, 7, 7);
    } else if (game->ui.cursorDragY == -1) {
        game->ui.cursorDragY = -3;
    }
    if ((header.flags & dialog_header_flag::Title) != 0) {
        if (game->renderer.needsRepaint) midp_set_color(surface, borderColor);
        if (game->renderer.needsRepaint) midp_draw_rect(surface, dialogX + 2, dialogY + 13, dialogWidth - 4, 0);
        game->renderer.clipX = dialogX; game->renderer.clipY = dialogY; game->renderer.clipWidth = dialogWidth; game->renderer.clipHeight = dialogHeight;
        if (game->renderer.needsRepaint)
            game->renderer.draw_font_string(*surface, game->assets, dialogX + 3, dialogY + 5,
                                  catalog_codes(dialog->title), true);
        textContentY = 19; drawingContentY = 19; scrollButtonY = 19;
    }
    contentX = dialogX + 4;
    game->renderer.clipX = dialogX + 1;
    game->renderer.clipY = dialogY + textContentY - 4;
    game->renderer.clipWidth = dialogWidth - 2;
    game->renderer.clipHeight = dialogHeight - textContentY + 3;
    midp_set_clip(surface, game->renderer.clipX, game->renderer.clipY, game->renderer.clipWidth, game->renderer.clipHeight);
    if (game->renderer.needsRepaint && game->ui.cursorOverUI) {
        midp_set_color(surface, borderColor);
        midp_draw_rect(surface, dialogX + dialogWidth - 6, dialogY + scrollButtonY - 1, 4, 0);
        midp_draw_rect(surface, dialogX + dialogWidth - 5, dialogY + scrollButtonY - 2, 2, 0);
        midp_draw_rect(surface, dialogX + dialogWidth - 4, dialogY + scrollButtonY - 3, 0, 0);
    }
    if (game->renderer.needsRepaint && game->ui.cursorAllowZoomOut) {
        midp_set_color(surface, borderColor);
        midp_draw_rect(surface, dialogX + dialogWidth - 6, dialogY + dialogHeight - 5, 4, 0);
        midp_draw_rect(surface, dialogX + dialogWidth - 5, dialogY + dialogHeight - 4, 2, 0);
        midp_draw_rect(surface, dialogX + dialogWidth - 4, dialogY + dialogHeight - 3, 0, 0);
    }
    game->ui.cursorSelectY = -2;
    game->ui.cursorSelectX = -2;
    game->ui.cursorOverUI = false;
    game->ui.cursorAllowZoomOut = false;
    game->ui.cursorDragX = -1;
}

void Game::PackedDialogRender::renderContent() {
    const int contentItemCount = static_cast<int>(dialog->items.size());
    for (int itemIndex = 0; itemIndex < contentItemCount; ++itemIndex) {
        int itemY = dialogY + textContentY - game->ui.cursorScale;
        PackedContentItem item = itemFromDialog(dialog->items[itemIndex]);
        if (item.hasDynamicValue)
            item.textCodes = game->resolve_dialog_value(
                game->renderer.currentSpriteId, item.valueId);
        const int itemFlags = item.flags;
        int xOffset = item.xOffset;
        const int yOffset = item.yOffset;
        const bool hasResourceCondition = item.hasResourceCondition;
        const int resourceId = item.resourceId;
        if (!item.image) {
            const int actionCode = item.actionCode;
            const int selectionLeft = item.selectionLeft;
            const int selectionTop = item.selectionTop;
            const int selectionRight = item.selectionRight;
            const int selectionBottom = item.selectionBottom;
            const auto& textCodes = item.textCodes;
            int visibleCharacterCount = 0;
            if (!textCodes) continue;
            for (const int8_t code : *textCodes) {
                if (code >= 32 && code <= 39) continue;
                ++visibleCharacterCount;
            }
            if (item.hasDynamicValue) {
                const int fitting_x_offset =
                    dialogWidth - dialog_text_layout::kDynamicValueRightPadding
                    - visibleCharacterCount * font_layout::kGlyphAdvance;
                if (xOffset > fitting_x_offset)
                    xOffset = max_value(0, fitting_x_offset);
            }
            if (itemY + yOffset + 6 + selectionBottom >= game->renderer.clipY + game->renderer.clipHeight) {
                int neededHeight = dialogHeight + (itemY + yOffset + 6 + selectionBottom + selectionTop + 3
                    - (game->renderer.clipHeight + game->renderer.clipY));
                if (game->renderer.dialogContentHeight < neededHeight) game->renderer.dialogContentHeight = neededHeight;
                if (game->renderer.dialogContentHeight > game->renderer.canvasHeight - 20) game->renderer.dialogContentHeight = game->renderer.canvasHeight - 20;
            }
            if (game->renderer.needsRepaint && itemY + yOffset + 6 + selectionBottom >= game->renderer.clipY + game->renderer.clipHeight)
                game->ui.cursorAllowZoomOut = true;
            if (hasResourceCondition
                && !game->matches_resource_condition(
                    game->renderer.currentSpriteId, resourceId))
                continue;
            if (itemY + yOffset - selectionTop - 3 <= game->renderer.clipY + game->renderer.clipHeight
                && itemY + yOffset + 8 + selectionBottom >= game->renderer.clipY) {
                if ((itemFlags & dialog_item_flag::Selectable) != 0) {
                    if (game->ui.cursorDragY == -3) game->ui.cursorDragY = itemIndex;
                    // Capture the row's exact selection rectangle -- the same rect the
                    // highlight fill below draws -- so the hit region matches the visible
                    // highlight. Some dialogs lay two options out side by side on one line
                    // (same y, different x, e.g. Yes/No); a full clip-width band would make
                    // both options overlap and only the last-drawn one hittable. Captured
                    // here (outside the fully-in-clip test below) so the topmost, partly
                    // clipped default row is mouse-selectable too.
                    game->clickMap.add_menu_row(contentX + xOffset - selectionLeft,
                                                itemY + yOffset - selectionTop - 3,
                                                6 * visibleCharacterCount + selectionLeft + selectionRight - 1,
                                                10 + selectionTop + selectionBottom,
                                                itemIndex, actionCode);
                    if (itemY + yOffset + 8 + selectionBottom <= game->renderer.clipY + game->renderer.clipHeight
                        && itemY + yOffset - 3 - selectionTop >= game->renderer.clipY) {
                        if (itemIndex < game->ui.cursorDragY) game->ui.cursorSelectY = itemIndex;
                        if (game->ui.cursorSelectX == -2 && itemIndex > game->ui.cursorDragY) game->ui.cursorSelectX = itemIndex;
                    }
                    if (game->renderer.needsRepaint && game->ui.cursorDragY == itemIndex) {
                        midp_set_color(surface, selectionColor);
                        midp_fill_rect(surface, contentX + xOffset - selectionLeft,
                                       itemY + yOffset - selectionTop - 3,
                                       6 * visibleCharacterCount + selectionLeft + selectionRight - 1,
                                       10 + selectionTop + selectionBottom);
                        game->ui.cursorDragX = actionCode;
                    }
                    if (game->renderer.needsRepaint
                        && (itemFlags & dialog_item_flag::Border) != 0) {
                        midp_set_color(surface, borderColor);
                        midp_draw_rect(surface, contentX + xOffset - selectionLeft,
                                       itemY + yOffset - selectionTop - 3,
                                       6 * visibleCharacterCount + selectionLeft + selectionRight - 1,
                                       10 + selectionTop + selectionBottom);
                        midp_set_color(surface, edgeColor);
                        midp_draw_rect(surface, contentX + xOffset - selectionLeft,
                                       itemY + yOffset - selectionTop - 3, 0,
                                       10 + selectionTop + selectionBottom);
                        midp_draw_rect(surface, contentX + xOffset - selectionLeft,
                                       itemY + yOffset - selectionTop - 3,
                                       6 * visibleCharacterCount + selectionLeft + selectionRight - 1, 0);
                    }
                }
                if (game->renderer.needsRepaint)
                    game->renderer.draw_font_string(*surface, game->assets, contentX + xOffset, itemY + yOffset,
                                        *textCodes, true);
            }
            textContentY += 11 + yOffset;
            continue;
        }
        Image *image = game->wagon_car_sprite(game->renderer.currentSpriteId, item.slotIndex);
        if (image == NULL) continue;
        int imageHeight = image->height();
        if (hasResourceCondition
            && !game->matches_resource_condition(
                game->renderer.currentSpriteId, resourceId))
            continue;
        if (game->renderer.needsRepaint && itemY + yOffset <= game->renderer.clipY + game->renderer.clipHeight
            && itemY + yOffset + imageHeight >= game->renderer.clipY)
            midp_draw_image(surface, image, contentX + xOffset, itemY + yOffset, 20);
        if (game->renderer.needsRepaint && itemY + yOffset + imageHeight >= game->renderer.clipY + game->renderer.clipHeight)
            game->ui.cursorAllowZoomOut = true;
        textContentY += imageHeight + yOffset;
    }
}

void Game::PackedDialogRender::renderCommands() {
    const int drawingCommandCount = static_cast<int>(dialog->commands.size());
    for (int commandIndex = 0; commandIndex < drawingCommandCount; ++commandIndex) {
        int commandBaseY = dialogY + drawingContentY - game->ui.cursorScale;
        const PackedDrawingCommand command = commandFromDialog(
            game->ui, game->renderer.currentSpriteId, dialog->commands[commandIndex]);
        const drawing_command::Type command_type = command.type;
        const int color = command.color;
        const int x = command.x;
        const int y = command.y;
        const int widthOrX2 = command.widthOrX2;
        const int heightOrY2 = command.heightOrY2;
        const bool hasResourceCondition = command.hasResourceCondition;
        const int resourceId = command.resourceId;
        int lowestY = commandBaseY + y + heightOrY2;
        if (!game->renderer.needsRepaint
            || (hasResourceCondition
                && !game->matches_resource_condition(
                    game->renderer.currentSpriteId, resourceId)))
            continue;
        switch (command_type) {
            case drawing_command::Line:
                midp_set_color(surface, color);
                midp_draw_line(surface, contentX + x, commandBaseY + y,
                               contentX + widthOrX2, commandBaseY + heightOrY2);
                lowestY = max_value(commandBaseY + y, commandBaseY + heightOrY2);
                break;
            case drawing_command::OutlineRectangle:
                midp_set_color(surface, color);
                midp_draw_rect(surface, contentX + x, commandBaseY + y, widthOrX2, heightOrY2);
                break;
            case drawing_command::FilledRectangle:
                midp_set_color(surface, color);
                midp_fill_rect(surface, contentX + x, commandBaseY + y, widthOrX2, heightOrY2);
                break;
            case drawing_command::OutlineArc:
                midp_set_color(surface, color);
                midp_draw_arc(surface, contentX + x, commandBaseY + y,
                              widthOrX2, heightOrY2,
                              drawing_command::kFullCircleStartAngle,
                              drawing_command::kFullCircleSweepAngle);
                break;
            case drawing_command::FilledArc:
                midp_set_color(surface, color);
                midp_fill_arc(surface, contentX + x, commandBaseY + y,
                              widthOrX2, heightOrY2,
                              drawing_command::kFullCircleStartAngle,
                              drawing_command::kFullCircleSweepAngle);
                break;
        }
        if (lowestY < game->renderer.clipY + game->renderer.clipHeight) continue;
        game->ui.cursorAllowZoomOut = true;
    }
}

void Game::PackedDialogRender::finishFrame() {
    if (game->renderer.needsRepaint) {
        if (game->ui.cursorScale == 0 && game->ui.cursorSelectY == -2) game->ui.cursorSelectY = -1;
        if (game->ui.cursorScale > 0) game->ui.cursorOverUI = true;
    }
    game->renderer.needsRepaint = true;
    if (game->renderer.spriteJustChanged > 0) --game->renderer.spriteJustChanged;
}

void Game::render_packed_sprite(Surface& surface_value, int32_t spriteId) {
    Game *game = this;
    Surface *surface = &surface_value;
    PackedDialogRender render(*game, *surface, spriteId);
    if (!render.valid()) return;
    render.setupFrame();
    render.renderContent();
    render.renderCommands();
    render.finishFrame();
}
