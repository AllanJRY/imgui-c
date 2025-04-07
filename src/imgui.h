#ifndef IMGUI_H

// TODO REMOVE those imports, only used to remove fucking lsp error (all are already imported before this file in the main unity build).
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // TODO replace with allocator on Imgui_Memory

#define BITS_PER_PIXEL  32
#define BYTES_PER_PIXEL 4

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

typedef enum Imgui_Position_Type {
    Fixed,
    Relative
} Imgui_Position_Type;

typedef struct Imgui_Position {
    uint32_t x, y;
    Imgui_Position_Type type;
} Imgui_Position;

typedef struct Imgui_Padding {
    uint8_t top, right, bottom, left;
} Imgui_Padding;

// TODO: replace enums by FLAGS, to have something more compact ?
typedef enum Imgui_Layout_Direction {
    Horizontal,
    Vertical,
} Imgui_Layout_Direction;

typedef enum Imgui_Sizing_Type {
    Fit,
    Fixed,
    Grow,
    Percent,
} Imgui_Sizing_Type;

typedef struct Imgui_Sizing_Axis {
    union {
        struct { uint32_t min, max; };
        float percent;
    } size;
    Imgui_Sizing_Type type;
} Imgui_Sizing_Axis;

typedef struct Imgui_Sizing {
    Imgui_Sizing_Axis width, height;
} Imgui_Sizing;

typedef struct Imgui_Element {
    Imgui_Layout_Direction     layout_direction;
    uint32_t                   bg_color;
    uint8_t                    child_gap;
    Imgui_Padding              padding;
    Imgui_Position             position;
    Imgui_Sizing               sizing;
    struct Imgui_Element_Array children;
} Imgui_Element;

typedef struct Imgui_Element_Array {
    Imgui_Element* elements;
    uint8_t        cap;
    uint8_t        len;
} Imgui_Element_Array;

typedef struct Imgui_Context {
    Imgui_Element root;
    // TODO: add open element stack (or make a static var)
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

void imgui_new_element(
    Imgui_Element*         parent,
    Imgui_Layout_Direction layout_direction,
    uint32_t               bg_color,
    uint8_t                child_gap,
    Imgui_Padding          padding,
    Imgui_Position         position,
    Imgui_Sizing           sizing
);

#define IMGUI_H
#endif
