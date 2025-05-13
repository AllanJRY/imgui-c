#include "imgui.h"

void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer, Imgui_Input* input) {
    // TODO: Create the context before, like, inside the platform code ?
    Imgui_Context ctx = {0};
    imgui_context_init(&ctx, offscreen_buffer);

    // TODO: ASSERT(memory->permanent_size >= sizeof(Imgui_Context))


    // PASSES START

    imgui_element_open(
        &ctx,
        Horizontal,
        0x00FF00,
        10,
        (Imgui_Padding) {10, 10, 10, 10},
        (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
        (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Fit }, .height = (Imgui_Sizing_Axis) { .type = Fit } }
    );

    imgui_element_open(
        &ctx,
        Horizontal,
        0xFF0000,
        10,
        (Imgui_Padding) {10, 10, 10, 10},
        (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
        (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .size.min = 100, .type = Fixed }, .height = (Imgui_Sizing_Axis) { .size.min = 100, .type = Fixed } }
    );
    imgui_element_close(&ctx);

    imgui_element_open(
        &ctx,
        Horizontal,
        0x0000FF,
        10,
        (Imgui_Padding) {10, 10, 10, 10},
        (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
        (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .size.min = 100, .type = Fixed }, .height = (Imgui_Sizing_Axis) { .size.min = 100, .type = Fixed } }
    );
    imgui_element_close(&ctx);

    imgui_element_close(&ctx);

    // PASSES END

    imgui_draw_rect(offscreen_buffer, 0, 0, ctx.root.sizing.width.size.min, ctx.root.sizing.height.size.min, ctx.root.bg_color);
    for (uint8_t i = 0; i < ctx.root.children.len; i += 1) {
        Imgui_Element element = ctx.root.children.elements[i];
        imgui_draw_rect(offscreen_buffer, element.position.x, element.position.y, element.sizing.width.size.min, element.sizing.height.size.min, element.bg_color);
    }

    // TODO: Clear the context and allocated arrays ? Should be a lot simpler with a custom 
    // allocator on a pre-allocated buffer (here the temp buffer should suit us perfectly)
}

void imgui_context_init(Imgui_Context* ctx, Imgui_Offscreen_Buffer* offscreen_buffer) {
    ctx->root.layout_direction        = Horizontal;
    ctx->root.position                = (Imgui_Position) {.type = Absolute, .x = 0, .y = 0};
    ctx->root.sizing.width.type       = Fixed;
    ctx->root.sizing.width.size.min   = offscreen_buffer->width;
    ctx->root.sizing.width.size.max   = offscreen_buffer->width;
    ctx->root.sizing.height.type      = Fixed;
    ctx->root.sizing.height.size.min  = offscreen_buffer->height;
    ctx->root.sizing.height.size.max  = offscreen_buffer->height;
    ctx->root.bg_color                = 0xFFFFFF;
    ctx->root.child_gap               = 0;
    ctx->root.padding                 = (Imgui_Padding) {0, 0, 0, 0};
    ctx->root.children.cap            = 2;
    ctx->root.children.len            = 0;
    ctx->root.children.elements       = malloc(ctx->root.children.cap * sizeof(Imgui_Element)); // replace by usage of custom allocator on Imgui_Memory.

    ctx->open_stack.cap = 2;
    ctx->open_stack.len = 0;
    // TODO use custom allocator
    ctx->open_stack.elements = malloc(ctx->open_stack.cap * sizeof(Imgui_Element));

    if (ctx->open_stack.elements == NULL) {
        exit(1);
    }

    // TODO use one from custom allocator
    // memset(ctx->open_stack.elements, 0, ctx->open_stack.cap * sizeof(Imgui_Element));
}

void imgui_element_open(
    Imgui_Context*         ctx, 
    Imgui_Layout_Direction layout_direction,
    uint32_t               bg_color,
    uint8_t                child_gap,
    Imgui_Padding          padding,
    Imgui_Position         position,
    Imgui_Sizing           sizing
) {
    if (ctx == NULL || ctx->open_stack.elements == NULL) {
        // TODO: log.
        return;
    }

    Imgui_Element element;
    element.layout_direction = layout_direction;
    element.bg_color         = bg_color;
    element.child_gap        = child_gap;
    element.padding          = padding;
    element.position         = position;
    element.sizing           = sizing;

    imgui_element_stack_push(&ctx->open_stack, &element);

}

void imgui_element_close(Imgui_Context* ctx) {
    if (ctx == NULL || ctx->open_stack.len == 0) {
        // TODO: log.
        return;
    }

    Imgui_Element element;
    (void) imgui_element_stack_pop(&ctx->open_stack, &element);

    // TODO: this is more margin than padding, change this.
    Imgui_Padding padding = element.padding;
    element.sizing.width.size.min += padding.left + padding.right;
    element.sizing.height.size.min += padding.top + padding.bottom;

    Imgui_Element* parent = imgui_element_stack_peek(&ctx->open_stack);
    if (parent == NULL) {
        imgui_element_array_push(&ctx->root.children, &element);
        return;
    }

    imgui_element_array_push(&parent->children, &element);

    if (parent->layout_direction == Horizontal) {
        if (parent->children.len > 1) {
            parent->sizing.width.size.min += parent->child_gap;
        }

        if (parent->sizing.width.type == Fit) {
            parent->sizing.width.size.min += element.sizing.width.size.min;
        }

        if (parent->sizing.height.type == Fit) {
            if (parent->sizing.height.size.min < element.sizing.height.size.min) {
                parent->sizing.height.size.min = element.sizing.height.size.min;
            }
        }
    } else {
        if (parent->children.len > 1) {
            parent->sizing.height.size.min += parent->child_gap;
        }

        if (parent->sizing.height.type == Fit) {
            parent->sizing.height.size.min += element.sizing.height.size.min;
        }

        if (parent->sizing.width.type == Fit) {
            if (parent->sizing.width.size.min < element.sizing.width.size.min) {
                parent->sizing.width.size.min = element.sizing.width.size.min;
            }
        }
    }
}

void imgui_element_array_push(Imgui_Element_Array* array, Imgui_Element* element) {
    if (array == NULL) {
        // TODO: log.
        return;
    }

    if (array->len == array->cap) {
        array->elements = imgui_backing_buffer_grow(array->elements, sizeof(Imgui_Element) * (array->cap * 2));
        array->cap *= 2;
    }

    array->elements[array->len] = *element;
    array->len += 1;
}

void imgui_element_stack_push(Imgui_Element_Stack* stack, Imgui_Element* element) {
    if (stack == NULL) {
        // TODO: log.
        return;
    }

    if (stack->len == stack->cap) {
        stack->elements = imgui_backing_buffer_grow(stack->elements, sizeof(Imgui_Element) * (stack->cap * 2));
        stack->cap *= 2;
    }

    stack->elements[stack->len] = *element;
    stack->len += 1;
}

Imgui_Element* imgui_element_stack_peek(Imgui_Element_Stack* stack) {
    if (stack->len == 0) {
        return NULL;
    }

    return stack->elements + (stack->len - 1);
}

bool imgui_element_stack_pop(Imgui_Element_Stack* stack, Imgui_Element* poped_element) {
    if(stack == NULL || stack->len == 0) {
        return false;
    }

    *poped_element = stack->elements[stack->len - 1];
    stack->len -= 1;
    return true;
}

void* imgui_backing_buffer_grow(void* backing_buffer, int new_size) {
    // TODO: use custom allocator.
    void* new_mem = (Imgui_Element*) realloc(backing_buffer, new_size);
    if (new_mem == NULL) {
        exit(1);
    }

    // memset to 0 the new mem space.

    return new_mem;
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