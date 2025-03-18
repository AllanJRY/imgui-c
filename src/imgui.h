#ifndef IMGUI_H

typedef struct Imgui_Offscreen_Buffer {
    void* mem;
    int   width;
    int   height;
} Imgui_Offscreen_Buffer;

void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer);

#define IMGUI_H
#endif
