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

typedef struct Imgui_Offscreen_Buffer Imgui_Offscreen_Buffer;
typedef struct Imgui_Input Imgui_Input;
typedef struct Imgui_Context Imgui_Context;
typedef struct Imgui_Memory Imgui_Memory;

typedef struct Imgui_Element Imgui_Element;
typedef struct Imgui_Element_Array Imgui_Element_Array;
typedef struct Imgui_Element_Stack Imgui_Element_Stack;

typedef struct Imgui_Position Imgui_Position;
typedef enum   Imgui_Position_Type { Absolute, Relative, } Imgui_Position_Type;


typedef struct Imgui_Padding Imgui_Padding;
typedef enum   Imgui_Layout_Direction { Horizontal, Vertical, } Imgui_Layout_Direction; // TODO: replace enums by FLAGS, to have something more compact ?
typedef struct Imgui_Sizing Imgui_Sizing;
typedef struct Imgui_Sizing_Axis Imgui_Sizing_Axis;
typedef enum   Imgui_Sizing_Type { Fit, Fixed, Grow, Percent, } Imgui_Sizing_Type;

struct Imgui_Offscreen_Buffer {
    void* mem;
    int   width;
    int   height;
};

struct Imgui_Input {
    bool move_bottom;
    bool move_left;
    bool move_right;
    bool move_top;
};

struct Imgui_Element_Array {
    Imgui_Element* elements;
    uint8_t        cap;
    uint8_t        len;
};

void imgui_element_array_push(Imgui_Element_Array* array, Imgui_Element* element);

struct Imgui_Element_Stack {
    Imgui_Element* elements;
    uint8_t        cap;
    uint8_t        len;
};

void imgui_element_stack_push(Imgui_Element_Stack* stack, Imgui_Element* element);

Imgui_Element* imgui_element_stack_peek(Imgui_Element_Stack* stack);

bool imgui_element_stack_pop(Imgui_Element_Stack* stack, Imgui_Element* poped_element);

struct Imgui_Position {
    uint32_t x, y;
    Imgui_Position_Type type;
};

struct Imgui_Padding {
    uint8_t top, right, bottom, left;
};

struct Imgui_Sizing_Axis {
    union {
        struct { uint32_t min, max; };
        float percent;
    } size;
    Imgui_Sizing_Type type;
};

struct Imgui_Sizing {
    Imgui_Sizing_Axis width, height;
};

// TODO: check if really needed.
struct Imgui_Memory {
    uint64_t permanent_size;
    void*    permanent;
    uint64_t temporary_size;
    void*    temporary; // Really necessary in this application.
};

struct Imgui_Element {
    Imgui_Layout_Direction layout_direction;
    uint32_t               bg_color;
    uint8_t                child_gap;
    Imgui_Padding          padding;
    Imgui_Position         position;
    Imgui_Sizing           sizing;
    Imgui_Element_Array    children;
};

void imgui_element_open(
    Imgui_Context*         ctx, 
    Imgui_Layout_Direction layout_direction,
    uint32_t               bg_color,
    uint8_t                child_gap,
    Imgui_Padding          padding,
    Imgui_Position         position,
    Imgui_Sizing           sizing
);

void imgui_element_close(Imgui_Context* ctx);

struct Imgui_Context {
    Imgui_Element       root; // or try with an flat array ordered by opened order.
    Imgui_Element_Stack open_stack;
};

void imgui_context_init(Imgui_Context* ctx, Imgui_Offscreen_Buffer* offscreen_buffer);

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

void* imgui_backing_buffer_grow(void* backing_buffer, int new_size);

#define IMGUI_H
#endif
