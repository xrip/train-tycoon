// In-code dialog layout table (transcoded from packed_sprites.bin).
//
// packed_sprites.bin was a declarative dialog format: a header (colours/size),
// an optional title, content items (text or image slots), and drawing commands.
// dialog_table.cpp holds the same data as an editable C++ table so dialogs can
// be changed and new ones added in source. render.cpp walks these structs; text
// is a Text catalog key resolved via catalog_codes().
//
// Field order matches the generator's designated initializers -- keep them in
// sync (or re-run tools/decode_packed_text.py codegen).
#ifndef GAME_DIALOG_TABLE_H
#define GAME_DIALOG_TABLE_H

#include <span>
#include <vector>

#include "text/text_catalog.h"

// One content row: either static/dynamic text, or an image slot (image == true).
struct DialogItem {
    int flags = 0;
    bool image = false;
    int x_offset = 0;
    int y_offset = 0;
    bool has_resource_condition = false;
    int resource_id = 0;
    bool has_dynamic_value = false;    // text resolved at runtime (resolve_dialog_value)
    int value_id = 0;
    int action_code = 0;               // selectable row (flags & 0x10)
    int sel_left = 0;
    int sel_top = 0;
    int sel_right = 0;
    int sel_bottom = 0;
    int slot_index = 0;                // image slot (wagon_car_sprite)
    Text text = Text::none;            // static text key
};

// One drawing primitive. x/y/w/h are stored unsigned (0..65535); render.cpp
// applies the signed int16 cast under the graph-substitution flags itself.
struct DialogCommand {
    int flags = 0;
    int color = 0;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    bool has_resource_condition = false;
    int resource_id = 0;
};

struct PackedDialog {
    int sprite_id = 0;                 // table slot this dialog is stored under
    int flags = 0;
    int background_color = 0;
    int edge_color = 0;
    int border_color = 0;
    int selection_color = 0;
    int width = 0;
    int height = 0;
    int nested_sprite_id = 0;          // header's own id, used for value resolution
    Text title = Text::none;
    std::vector<DialogItem> items;
    std::vector<DialogCommand> commands;
};

// The dialog stored under `sprite_id`, or nullptr if none.
const PackedDialog* dialog_for_sprite(int sprite_id);

// Every dialog, in table order.
std::span<const PackedDialog> all_dialogs();

#endif  // GAME_DIALOG_TABLE_H
