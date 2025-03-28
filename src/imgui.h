#ifndef IMGUI_H

#if DEBUG
#define ASSERT(expr) if(!(expr)) { *(int*) 0 = 0; }
#else
#define ASSERT(expr)
#endif

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

typedef struct Imgui_Element {
    uint32_t color;
    uint32_t x, y;
    uint32_t width, height;
} Imgui_Element;

typedef struct Imgui_Context {
    Imgui_Element* elements;
    uint32_t       elements_count;
} Imgui_Context;

// TODO: check if really needed.
typedef struct Imgui_Memory {
    uint64_t permanent_size;
    void*    permanent;
    uint64_t temporary_size;
    void*    temporary; // Really necessary in this application.
} Imgui_Memory;

// TODO: Get memory from platform.
void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer, Imgui_Input* input);

void imgui_draw_rect(
    Imgui_Offscreen_Buffer* offscreen_buffer,
    uint32_t                top,
    uint32_t                left,
    uint32_t                width,
    uint32_t                height,
    uint32_t                color
);

#define IMGUI_H
#endif
