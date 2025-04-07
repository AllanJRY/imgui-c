#include "imgui.h"

void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer, Imgui_Input* input) {
    Imgui_Context ctx = {0};
    // TODO: ASSERT(memory->permanent_size >= sizeof(Imgui_Context))

    // static uint32_t gradient_x_offset = 0;
    // static uint32_t gradient_y_offset = 0;

    // if (input->move_left) {
    //     gradient_x_offset -= 50;
    // } else if (input->move_right) {
    //     gradient_x_offset += 50;
    // } else if (input->move_top) {
    //     gradient_y_offset -= 50;
    // } else if (input->move_bottom) {
    //     gradient_y_offset += 50;
    // }

    // Render a gradient which move due to the given offset.
    /*
    uint8_t* row = (uint8_t*) offscreen_buffer->mem;
    for (int y = 0; y < offscreen_buffer->height; y += 1) {
        uint32_t* pixel = (uint32_t*) row;
        for (int x = 0; x < offscreen_buffer->width; x += 1) {
            uint8_t blue  = (uint8_t) x + gradient_x_offset;
            uint8_t green = (uint8_t) y + gradient_y_offset;
            *pixel = (green << 8) | blue;
            pixel += 1;
        }

        row += BYTES_PER_PIXEL * offscreen_buffer->width;
    }
    */

    ctx.root.layout_direction        = Horizontal;
    ctx.root.position                = (Imgui_Position) {.type = Fixed, .x = 0, .y = 0};
    ctx.root.sizing.width.type       = Fixed;
    ctx.root.sizing.width.size.min   = offscreen_buffer->width;
    ctx.root.sizing.width.size.max   = offscreen_buffer->width;
    ctx.root.sizing.height.type      = Fixed;
    ctx.root.sizing.height.size.min  = offscreen_buffer->height;
    ctx.root.sizing.height.size.max  = offscreen_buffer->height;
    ctx.root.bg_color                = 0xFFFFFF;
    ctx.root.child_gap               = 10;
    ctx.root.padding                 = (Imgui_Padding) {10, 10, 10, 10};
    ctx.root.children.cap            = 10;
    ctx.root.children.len            = 0;
    ctx.root.children.elements       = malloc(ctx.root.children.cap * sizeof(Imgui_Element)); // replace by usage of custom allocator on Imgui_Memory.

    // PASSES START

    // Calc sizing
    uint32_t x_offset = (uint32_t) ctx.root.padding.left;
    uint32_t y_offset = (uint32_t) ctx.root.padding.top;
    for (int i = 0; i < 3; i += 1) {
        imgui_new_element(
            &ctx.root,
            Horizontal,
            0x0000FF << i,
            0,
            (Imgui_Padding){0},
            (Imgui_Position) {.type = Relative, },
            (Imgui_Sizing) {.width = {.type = Grow}, .height = {.type = Grow}}
        );

        x_offset += child_el.size.width + root.child_gap;
        // y_offset += child_el.size.height;  ONLY IF DIRECTION = VERTICAL
    }
    // PASSES END

    imgui_draw_rect(offscreen_buffer, 0, 0, ctx.root.sizing.width.size.min, ctx.root.sizing.height.size.min, ctx.root.bg_color);
    for (uint8_t i = 0; i < ctx.root.children.len; i += 1) {
        Imgui_Element element = ctx.root.children.elements[i];
        imgui_draw_rect(offscreen_buffer, element.position.x, element.position.y, element.sizing.width.size.min, element.sizing.height.size.min, element.bg_color);
    }
}

void imgui_draw_rect(Imgui_Offscreen_Buffer* offscreen_buffer, uint32_t pos_x, uint32_t pos_y, uint32_t width, uint32_t height, uint32_t color) {
    int32_t x_overflow = (pos_x + width) - ((uint32_t) offscreen_buffer->width);
    int32_t y_overflow = (pos_y + height) - ((uint32_t) offscreen_buffer->height);

    uint32_t inbound_width = width;
    if (x_overflow > 0) {
        inbound_width -= x_overflow;
    }

    uint32_t inbound_height = height;
    if (y_overflow > 0) {
        inbound_height -= y_overflow;
    }

    uint32_t* row = (uint32_t*) offscreen_buffer->mem;

    row += pos_x;
    row += pos_y * (uint32_t) offscreen_buffer->width;
    for (uint32_t y = 0; y < inbound_height; y += 1) {
        uint32_t* pixel = (uint32_t*) row;
        for (uint32_t x = 0; x < inbound_width; x += 1) {
            *pixel = color;
            pixel += 1;
        }

        row += offscreen_buffer->width;
    }
}

void imgui_new_element(
    Imgui_Element*         parent,
    Imgui_Layout_Direction layout_direction,
    uint32_t               bg_color,
    uint8_t                child_gap,
    Imgui_Padding          padding,
    Imgui_Position         position,
    Imgui_Sizing           sizing
) {
    if (parent->children.cap <= parent->children.len) {
        // TODO grow until a HARD defined LIMIT is reach ?
        return;
    }

    Imgui_Element element = parent->children.elements[parent->children.len];
    
    element.layout_direction = layout_direction;
    element.bg_color         = bg_color;
    element.child_gap        = child_gap;
    element.padding          = padding;
    element.position         = position;
    element.sizing           = sizing;

    parent->children.len += 1;
}
