#ifndef IMGUI_H

typedef struct Imgui_Offscreen_Buffer {
    void* mem;
    int   width;
    int   height;
} Imgui_Offscreen_Buffer;

typedef struct Imgui_Input {
    bool move_bottom;
    bool move_left;
    bool move_right;
    bool move_top;
} Imgui_Input;

void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer, Imgui_Input* input);

#define IMGUI_H
#endif
