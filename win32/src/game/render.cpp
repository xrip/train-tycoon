// Packed-sprite UI and text drawing.
// renderPackedSprite draws the menu/dialog layouts (state 20). renderFontString
// draws text from the `ca[]` font atlas. u() resolves per-slot sub-sprites.
#include "game_internal.h"

#define RGB3(r, g, b) (((r) << 16) | ((g) << 8) | (b))
static int imax(int a, int b) { return a > b ? a : b; }

void Renderer::draw_font_string(Surface& surface_value, const Assets& assets,
                                int32_t x, int32_t baselineY,
                                std::span<const int8_t> codes,
                                bool preserveClip) const {
    Surface *surface = &surface_value;
    int drawX = x;
    midp_set_color(surface, 0);
    const int len = static_cast<int>(codes.size());
    for (int i = 0; i < len; ++i) {
        int8_t code = codes[i];
        if (preserveClip) {
            midp_set_clip(surface, clipX, clipY, clipWidth, clipHeight);
            midp_clip_rect(surface, drawX, baselineY - 4, 5, 12);
        } else {
            midp_set_clip(surface, drawX, baselineY - 4, 5, 12);
        }
        if (code == 0) { drawX += 6; continue; }
        if (code < 32) {
            midp_draw_image(surface, assets.font_images[(code - 1) / 16].get(), drawX - 5 * ((code - 1) % 16), baselineY, 20);
            drawX += 6;
            continue;
        }
        if (code < 40) {
            int8_t markYOffset = (int8_t)(codes[i + 1] >= 40 && codes[i + 1] <= 65 ? 1 : 0);
            switch (code) {
                case 32:
                    midp_draw_line(surface, drawX + 2, baselineY - 2 + markYOffset, drawX + 2, baselineY - 2 + markYOffset);
                    break;
                case 35:
                    midp_draw_line(surface, drawX + 2, baselineY - 4 + markYOffset, drawX + 2, baselineY - 4 + markYOffset);
                    /* fall through */
                case 34:
                    midp_draw_line(surface, drawX + 2, baselineY - 2 + markYOffset, drawX + 1, baselineY - 3 + markYOffset);
                    /* fall through */
                case 33:
                    midp_draw_line(surface, drawX + 2, baselineY - 2 + markYOffset, drawX + 3, baselineY - 3 + markYOffset);
                    break;
                case 37:
                    midp_draw_line(surface, drawX + 1, baselineY - 2 + markYOffset, drawX + 1, baselineY - 2 + markYOffset);
                    midp_draw_line(surface, drawX + 3, baselineY - 2 + markYOffset, drawX + 3, baselineY - 2 + markYOffset);
                    break;
            }
            continue;
        }
        if (code < 70) {
            midp_draw_image(surface, assets.font_images[2 + (code - 40) / 16].get(), drawX - 5 * ((code - 40) % 16), baselineY, 20);
            drawX += 6;
            continue;
        }
        if (code >= 100) continue;
        midp_draw_image(surface, assets.font_images[4 + (code - 70) / 16].get(), drawX - 5 * ((code - 70) % 16), baselineY, 20);
        drawX += 6;
    }
    if (preserveClip) {
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
Image *Game::wagon_car_sprite(int32_t contextCode, int32_t slotIndex) {
    Game *game = this;
    if (contextCode == 23) {
        if (slotIndex == 0 && game->build.wagonType >= 0 && game->build.wagonType < 3)
            return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[136])].get();
        if (slotIndex == 0 && game->build.wagonType >= 3 && game->build.wagonType < 6)
            return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[146])].get();
        if (slotIndex > 0) {
            switch (game->build.wagonOptionSlots[slotIndex - 1]) {
                case -1: return NULL;
                case 0: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[162])].get();
                case 1: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[167])].get();
                case 2: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[172])].get();
                case 3: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[182])].get();
            }
        }
    }
    if (contextCode == 26 && game->fleet.count_cars(game->trainAtCursor) > slotIndex) {
        int carSlot = game->trainAtCursor;
        for (int i = 0; i < slotIndex; ++i) carSlot = game->fleet.trainCars[carSlot].nextCar;
        int carType = train_flag::type(game->fleet.trainCars[carSlot].flags);
        switch (carType) {
            case -1: return NULL;
            case 0: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[136])].get();
            case 1: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[146])].get();
            case 2: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[162])].get();
            case 3: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[167])].get();
            case 4: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[172])].get();
            case 5: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[177])].get();
            case 6: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[182])].get();
            case 7: return game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[187])].get();
        }
    }
    return NULL;
}

void Renderer::draw_text_box(Surface& surface_value, const Assets& assets,
                             int32_t x, int32_t y, int32_t colorSet,
                             int32_t alignment,
                             std::span<const int8_t> codes) const {
    Surface *surface = &surface_value;
    int textWidth = 0;
    const int len = static_cast<int>(codes.size());
    for (int i = 0; i < len; ++i) {
        if (codes[i] >= 32 && codes[i] <= 39) continue;
        ++textWidth;
    }
    textWidth *= 6;
    if ((alignment & 1) == 1) x -= textWidth / 2 + 3;
    if ((alignment & 2) == 2) y -= 5;
    if ((alignment & 4) == 4) x -= textWidth + 6;
    if (alignment == 37) { x = 0; y = 15 + gameplayViewportHeight - 15; }
    midp_set_color(surface, graphColors[3 * colorSet]);
    midp_fill_rect(surface, x, y, textWidth + 5, 13);
    midp_set_color(surface, graphColors[3 * colorSet + 1]);
    midp_draw_rect(surface, x, y, textWidth + 5, 13);
    midp_set_color(surface, graphColors[3 * colorSet + 2]);
    midp_draw_rect(surface, x, y + 13, textWidth + 5, 0);
    midp_draw_rect(surface, x + textWidth + 5, y, 0, 13);
    draw_font_string(*surface, assets, x + 4, y + 5, codes, false);
}

void Renderer::draw_selected_train_label(Surface& surface_value,
                                         const Assets& assets,
                                         const UiController& ui, int32_t x,
                                         int32_t y, int8_t selectionIndex) {
    Surface *surface = &surface_value;
    const auto engine_codes = std::span<const int8_t>(assets.spriteBitmapTable[
        assets.spriteIndexTable[80 + (ui.trainSelectionIndex[selectionIndex] >> 8 & 0xFF)]]);
    const auto wagon_codes = std::span<const int8_t>(assets.spriteBitmapTable[
        assets.spriteIndexTable[86 + (ui.trainSelectionIndex[selectionIndex] & 0xFF)]]);
    std::vector<int8_t> train_codes;
    train_codes.reserve(wagon_codes.size() + engine_codes.size());
    train_codes.insert(train_codes.end(), wagon_codes.begin(), wagon_codes.end());
    train_codes.insert(train_codes.end(), engine_codes.begin(), engine_codes.end());
    int textWidth = 0;
    for (const int8_t code : train_codes) {
        if (code >= 32 && code <= 39) continue;
        ++textWidth;
    }
    textWidth *= 6;
    x -= textWidth / 2 + 3;
    y -= 5;
    midp_set_color(surface, RGB3(255, 176, 98));
    midp_fill_rect(surface, x, y, textWidth + 5, 11);
    midp_set_color(surface, RGB3(255, 210, 170));
    midp_draw_rect(surface, x, y, textWidth + 5, 11);
    midp_set_color(surface, RGB3(255, 130, 0));
    midp_draw_rect(surface, x, y + 11, textWidth + 5, 0);
    midp_draw_rect(surface, x + textWidth + 5, y, 0, 11);
    clipX = 0;
    clipY = 15;
    clipWidth = canvasWidth;
    clipHeight = gameplayViewportHeight;
    draw_font_string(*surface, assets, x + 4, y + 3, train_codes, true);
}

namespace {

class PackedDialogReader {
public:
    PackedDialogReader(Assets& assets, int offset)
        : assets_(assets), offset_(offset) {}

    int byte() { return assets_.spriteData[offset_++]; }

    int u16() {
        const int low = unsignedByte(assets_.spriteData[offset_++]);
        const int high = unsignedByte(assets_.spriteData[offset_++]);
        return low + 256 * high;
    }

    int rgb() {
        const int red = unsignedByte(assets_.spriteData[offset_++]);
        const int green = unsignedByte(assets_.spriteData[offset_++]);
        const int blue = unsignedByte(assets_.spriteData[offset_++]);
        return 65536 * red + 256 * green + blue;
    }

    std::vector<int8_t> text() {
        std::vector<int8_t> codes = assets_.read_packed_text_codes(offset_);
        skip(1 + unsignedByte(assets_.spriteData[offset_]));
        return codes;
    }

    void skip(int count) { offset_ += count; }

private:
    Assets& assets_;
    int offset_;
};

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

PackedDialogHeader readDialogHeader(PackedDialogReader& reader) {
    PackedDialogHeader header{};
    header.flags = reader.byte();
    header.backgroundColor = reader.rgb();
    header.edgeColor = reader.rgb();
    header.borderColor = reader.rgb();
    header.selectionColor = reader.rgb();
    header.width = static_cast<uint8_t>(reader.byte());
    header.height = static_cast<uint8_t>(reader.byte());
    header.spriteId = reader.byte();
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

PackedContentItem readContentItem(Game& game, PackedDialogReader& reader) {
    PackedContentItem item{};
    item.flags = reader.byte();
    item.image = (item.flags & 0x80) != 0;
    item.hasResourceCondition = (item.flags & 0x40) != 0;
    if (!item.image) {
        item.xOffset = (item.flags & 4) != 0 ? 0 : reader.byte();
        item.yOffset = (item.flags & 2) != 0 ? 0 : reader.byte();
        item.hasDynamicValue = (item.flags & 0x20) != 0;
        item.resourceId = item.hasResourceCondition ? reader.byte() : 0;
        item.valueId = item.hasDynamicValue ? reader.byte() : 0;
        if ((item.flags & 0x10) != 0) {
            item.actionCode = reader.byte();
            item.selectionLeft = reader.byte();
            item.selectionTop = reader.byte();
            item.selectionRight = reader.byte();
            item.selectionBottom = reader.byte();
        }
        item.textCodes = item.hasDynamicValue
            ? game.resolve_dialog_value(game.renderer.currentSpriteId, item.valueId)
            : std::optional<std::vector<int8_t>>(reader.text());
        return item;
    }
    item.slotIndex = reader.byte();
    item.xOffset = (item.flags & 0x20) != 0 ? 0 : reader.byte();
    item.yOffset = (item.flags & 0x10) != 0 ? 0 : reader.byte();
    item.resourceId = item.hasResourceCondition ? reader.byte() : 0;
    return item;
}

struct PackedDrawingCommand {
    int flags;
    int type;
    int color;
    int x;
    int y;
    int widthOrX2;
    int heightOrY2;
    bool hasResourceCondition;
    int resourceId;
};

PackedDrawingCommand readDrawingCommand(Game& game, PackedDialogReader& reader) {
    PackedDrawingCommand command{};
    command.flags = reader.byte();
    command.type = command.flags & 7;
    command.color = reader.rgb();
    command.x = reader.u16();
    command.y = reader.u16();
    command.widthOrX2 = reader.u16();
    command.heightOrY2 = reader.u16();
    if ((command.flags & 8) != 0)
        command.x = static_cast<int16_t>(game.ui.report_graph_value(
            game.renderer.currentSpriteId, command.x));
    if ((command.flags & 0x10) != 0)
        command.y = static_cast<int16_t>(game.ui.report_graph_value(
            game.renderer.currentSpriteId, command.y));
    if ((command.flags & 0x20) != 0)
        command.widthOrX2 = static_cast<int16_t>(game.ui.report_graph_value(
            game.renderer.currentSpriteId, command.widthOrX2));
    if ((command.flags & 0x40) != 0)
        command.heightOrY2 = static_cast<int16_t>(game.ui.report_graph_value(
            game.renderer.currentSpriteId, command.heightOrY2));
    command.hasResourceCondition = (command.flags & 0x80) != 0;
    command.resourceId = command.hasResourceCondition ? reader.byte() : 0;
    return command;
}

class PackedDialogRender {
public:
    PackedDialogRender(Game& gameValue, Surface& surfaceValue, int32_t spriteIdValue)
        : game(&gameValue), surface(&surfaceValue), spriteId(spriteIdValue),
          reader(gameValue.assets, gameValue.assets.spriteOffsetTable[spriteIdValue]) {}

    void setupFrame();
    void renderContent();
    void renderCommands();
    void finishFrame();

private:
    Game *game;
    Surface *surface;
    int32_t spriteId;
    PackedDialogReader reader;
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

void PackedDialogRender::setupFrame() {
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
    const PackedDialogHeader header = readDialogHeader(reader);
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
    if ((header.flags & 1) == 1) {
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
    if ((header.flags & 2) == 2) {
        if (game->renderer.needsRepaint) midp_set_color(surface, borderColor);
        if (game->renderer.needsRepaint) midp_draw_rect(surface, dialogX + 2, dialogY + 13, dialogWidth - 4, 0);
        game->renderer.clipX = dialogX; game->renderer.clipY = dialogY; game->renderer.clipWidth = dialogWidth; game->renderer.clipHeight = dialogHeight;
        if (game->renderer.needsRepaint)
            game->renderer.draw_font_string(*surface, game->assets, dialogX + 3, dialogY + 5,
                                  reader.text(), true);
        else
            reader.text();
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

void PackedDialogRender::renderContent() {
    const int contentItemCount = reader.byte();
    for (int itemIndex = 0; itemIndex < contentItemCount; ++itemIndex) {
        int itemY = dialogY + textContentY - game->ui.cursorScale;
        const PackedContentItem item = readContentItem(*game, reader);
        const int itemFlags = item.flags;
        const int xOffset = item.xOffset;
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
                if ((itemFlags & 0x10) == 0x10) {
                    if (game->ui.cursorDragY == -3) game->ui.cursorDragY = itemIndex;
                    // Capture every visible text row as a full-width hit band, including
                    // the topmost row whose band starts above the clip and so fails the
                    // fully-in-clip test below. Without this the default-selected first
                    // menu item is drawn and keyboard-selectable but not mouse-clickable.
                    game->clickMap.add_menu_row(game->renderer.clipX, itemY + yOffset - selectionTop - 3,
                                                game->renderer.clipWidth, 10 + selectionTop + selectionBottom,
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
                    if (game->renderer.needsRepaint && (itemFlags & 8) == 8) {
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

void PackedDialogRender::renderCommands() {
    const int drawingCommandCount = reader.byte();
    for (int commandIndex = 0; commandIndex < drawingCommandCount; ++commandIndex) {
        int commandBaseY = dialogY + drawingContentY - game->ui.cursorScale;
        const PackedDrawingCommand command = readDrawingCommand(*game, reader);
        const int commandType = command.type;
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
        switch (commandType) {
            case 0:
                midp_set_color(surface, color);
                midp_draw_line(surface, contentX + x, commandBaseY + y,
                               contentX + widthOrX2, commandBaseY + heightOrY2);
                lowestY = imax(commandBaseY + y, commandBaseY + heightOrY2);
                break;
            case 1:
                midp_set_color(surface, color);
                midp_draw_rect(surface, contentX + x, commandBaseY + y, widthOrX2, heightOrY2);
                break;
            case 2:
                midp_set_color(surface, color);
                midp_fill_rect(surface, contentX + x, commandBaseY + y, widthOrX2, heightOrY2);
                break;
            case 3:
                midp_set_color(surface, color);
                midp_draw_arc(surface, contentX + x, commandBaseY + y, widthOrX2, heightOrY2, 0, 360);
                break;
            case 4:
                midp_set_color(surface, color);
                midp_fill_arc(surface, contentX + x, commandBaseY + y, widthOrX2, heightOrY2, 0, 360);
                break;
        }
        if (lowestY < game->renderer.clipY + game->renderer.clipHeight) continue;
        game->ui.cursorAllowZoomOut = true;
    }
}

void PackedDialogRender::finishFrame() {
    if (game->renderer.needsRepaint) {
        if (game->ui.cursorScale == 0 && game->ui.cursorSelectY == -2) game->ui.cursorSelectY = -1;
        if (game->ui.cursorScale > 0) game->ui.cursorOverUI = true;
    }
    game->renderer.needsRepaint = true;
    if (game->renderer.spriteJustChanged > 0) --game->renderer.spriteJustChanged;
}

} // namespace

void Game::render_packed_sprite(Surface& surface_value, int32_t spriteId) {
    Game *game = this;
    Surface *surface = &surface_value;
    PackedDialogRender render(*game, *surface, spriteId);
    render.setupFrame();
    render.renderContent();
    render.renderCommands();
    render.finishFrame();
}
