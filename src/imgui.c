#include "imgui.h"

void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer, Imgui_Input* input) {
    static uint32_t x_offset = 0;
    static uint32_t y_offset = 0;

    if (input->move_left) {
        x_offset -= 50;
    } else if (input->move_right) {
        x_offset += 50;
    } else if (input->move_top) {
        y_offset -= 50;
    } else if (input->move_bottom) {
        y_offset += 50;
    }

    // Render a gradient which move due to the given offset.
    uint8_t* row = (uint8_t*) offscreen_buffer->mem;
    for (int y = 0; y < offscreen_buffer->height; y += 1) {
        uint32_t* pixel = (uint32_t*) row;
        for (int x = 0; x < offscreen_buffer->width; x += 1) {
            uint8_t blue  = (uint8_t) x + x_offset;
            uint8_t green = (uint8_t) y + y_offset;
            *pixel = (green << 8) | blue;
            pixel += 1;
        }

        row += BYTES_PER_PIXEL * offscreen_buffer->width;
    }
}
