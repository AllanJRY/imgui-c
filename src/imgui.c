#include "imgui.h"

void imgui_update_and_render(Imgui_Offscreen_Buffer* offscreen_buffer, Imgui_Memory* memory, Imgui_Input* input) {
    ASSERT(offscreen_buffer != NULL, "Offscreen buffer must not be NULL");
    ASSERT(memory           != NULL, "Memory must not be NULL");
    ASSERT(input            != NULL, "Input must not be NULL");

    // TODO(AJA): Cast the context from the permanent memory buffer.
    // ASSERT(memory->permanent_size >= sizeof(Imgui_Context), "Not enough permanent memory size");
    // Imgui_Context* ctx = (Imgui_Context*) memory->permanent_size;
    // imgui_context_init(ctx, offscreen_buffer);

    // TODO(AJA): No need to reinit everything on every frame.
    Imgui_Context ctx = {0};
    gmv_arena_init(&ctx.allocator, memory->temporary, memory->temporary_size);
    ctx.elements_count_limit         = 50;
    ctx.elements.len                 = 0;
    ctx.elements.cap                 = ctx.elements_count_limit;
    ctx.elements.elements            = (Imgui_Element*) gmv_arena_alloc(&ctx.allocator, sizeof(Imgui_Element) * ctx.elements.cap);
    ctx.elements_open_stack.len      = 0;
    ctx.elements_open_stack.cap      = ctx.elements_count_limit;
    ctx.elements_open_stack.elements = (uint32_t*) gmv_arena_alloc(&ctx.allocator, sizeof(uint32_t) * ctx.elements_open_stack.cap);

    // Begin

    // Element creation / size definition phase
    imgui_element_open(
        &ctx,
        Vertical,
        0x2E2E2E,
        5,
        (Imgui_Padding) {5, 5, 5, 5},
        (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
        (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .size = offscreen_buffer->width, .type = Fixed }, .height = (Imgui_Sizing_Axis) { .size = offscreen_buffer->height, .type = Fixed } }
    );
        // Header
        imgui_element_open(
            &ctx,
            Horizontal,
            0xE8E8E8,
            5,
            (Imgui_Padding) {5, 5, 5, 5},
            (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
            (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Grow }, .height = (Imgui_Sizing_Axis) { .type = Fixed, .size = 100 } }
        );
        imgui_element_close(&ctx);

        // Main
        imgui_element_open(
            &ctx,
            Horizontal,
            0x2E2E2E,
            5,
            (Imgui_Padding) {0},
            (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
            (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Grow }, .height = (Imgui_Sizing_Axis) { .type = Grow } }
        );

            imgui_element_open(
                &ctx,
                Vertical,
                0xE8E8E8,
                5,
                (Imgui_Padding) {5, 5, 5, 5},
                (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
                (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Fixed, .size = 300, }, .height = (Imgui_Sizing_Axis) { .type = Grow } }
            );
            imgui_element_close(&ctx);

            imgui_element_open(
                &ctx,
                Horizontal,
                0x1C1C1C,
                0,
                (Imgui_Padding) {5, 5, 5, 5},
                (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
                (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Grow }, .height = (Imgui_Sizing_Axis) { .type = Grow } }
            );
            imgui_element_close(&ctx);

            imgui_element_open(
                &ctx,
                Vertical,
                0xC97B2C,
                5,
                (Imgui_Padding) {5, 5, 5, 5},
                (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
                (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Fixed, .size = 200 }, .height = (Imgui_Sizing_Axis) {  .type = Fit } }
            );

            for (int i = 0; i < 3; i += 1) {
                imgui_element_open(
                    &ctx,
                    Horizontal,
                    0x965c21,
                    0,
                    (Imgui_Padding) {5, 5, 5, 5},
                    (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
                    (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Grow }, .height = (Imgui_Sizing_Axis) { .type = Fixed, .size = 20 } }
                );
                imgui_element_close(&ctx);
            }

            imgui_element_close(&ctx);

        imgui_element_close(&ctx);

        // Footer
        imgui_element_open(
            &ctx,
            Horizontal,
            0xE8E8E8,
            5,
            (Imgui_Padding) {5, 5, 5, 5},
            (Imgui_Position) {.type = Relative, .x = 0, .y = 0},
            (Imgui_Sizing) { .width = (Imgui_Sizing_Axis) { .type = Grow }, .height = (Imgui_Sizing_Axis) { .type = Fixed, .size = 100 } }
        );
        imgui_element_close(&ctx);

    imgui_element_close(&ctx);

    // END

    ASSERT(ctx.elements_open_stack.len == 0, "All elements should be closed before drawing them on screen");

    // Flex sizing phase

    // TODO(AJA): see if the arena should be updated to free to a defined offset/savepoint (like temp alloc from ginger bill articles).
    //            The idea here is to avoid a big block allocation.
    Imgui_U32_Array flex_children_idx = (Imgui_U32_Array) {
        .cap = UINT8_MAX,
        .len = 0,
        .elements = (uint32_t*) gmv_arena_alloc(&ctx.allocator, sizeof(uint32_t) * UINT8_MAX)
    };
    uint32_t flex_children_with_width_grow_count;
    uint32_t flex_children_with_height_grow_count;

    for (uint32_t i = 0; i < ctx.elements.len; i += 1) {
        flex_children_idx.len                = 0; // RESET the flex child indexes array.
        flex_children_with_width_grow_count  = 0;
        flex_children_with_height_grow_count = 0;

        Imgui_Element* parent_element   = &ctx.elements.elements[i];
        uint32_t remaining_width        = parent_element->sizing.width.size - (parent_element->padding.left + parent_element->padding.right);
        uint32_t remaining_height       = parent_element->sizing.height.size - (parent_element->padding.top + parent_element->padding.bottom);

        // TODO(AJA): we can optimize this a bit if we can store those index at closing or opening of the flex child.
        for (uint32_t j = 0; j < parent_element->children.len; j += 1) {

            bool is_flex_child           = false;
            uint32_t child_idx           = parent_element->children.elements[j];
            Imgui_Element* child_element = &ctx.elements.elements[child_idx];

            if (child_element->sizing.width.type == Grow) {
                is_flex_child = true;
                flex_children_with_width_grow_count += 1;
            } else {
                if (parent_element->layout_direction == Horizontal && child_element->position.type == Relative) {
                    remaining_width  -= child_element->sizing.width.size;
                }
            }

            if(child_element->sizing.height.type == Grow) {
                is_flex_child = true;
                flex_children_with_height_grow_count += 1;
            } else {
                if (parent_element->layout_direction == Vertical && child_element->position.type == Relative) {
                    remaining_height  -= child_element->sizing.height.size;
                }
            }

            if (j > 0 && child_element->position.type == Relative) {
                if (parent_element->layout_direction == Horizontal) {
                    remaining_width -= parent_element->child_gap;
                } else {
                    remaining_height -= parent_element->child_gap;
                }
            }

            if (is_flex_child) {
                imgui_u32_array_push(&flex_children_idx, child_idx);
             }

         }

        if (flex_children_idx.len > 0) {
            uint32_t flex_children_width  = remaining_width;
            uint32_t flex_children_height = remaining_height;

            for (uint32_t j = 0; j < flex_children_idx.len; j += 1) {
                uint32_t child_idx           = flex_children_idx.elements[j];
                Imgui_Element* child_element = &ctx.elements.elements[child_idx];

                if (child_element->sizing.width.type == Grow) {
                    if (parent_element->layout_direction == Horizontal) {
                        flex_children_width = remaining_width / flex_children_with_width_grow_count;
                    }
                    child_element->sizing.width.size = flex_children_width;
                }

                if (child_element->sizing.height.type == Grow) {
                    if (parent_element->layout_direction == Vertical) {
                        flex_children_height = remaining_height / flex_children_with_height_grow_count;
                    }
                    child_element->sizing.height.size = flex_children_height;
                }
            }
        }
    }

    // Position computation phase
    for (uint32_t i = 0; i < ctx.elements.len; i += 1) {
        Imgui_Element* parent_element = &ctx.elements.elements[i];
        uint32_t child_offset_x = parent_element->padding.left;
        uint32_t child_offset_y = parent_element->padding.top;
        for (uint32_t j = 0; j < parent_element->children.len; j += 1) {
            uint32_t child_idx = parent_element->children.elements[j];
            Imgui_Element* child_element = &ctx.elements.elements[child_idx];

            if (child_element->position.type == Relative) {
                child_element->position.x += parent_element->position.x + child_offset_x;
                child_element->position.y += parent_element->position.y + child_offset_y;

                if (parent_element->layout_direction == Horizontal) {
                    child_offset_x += child_element->sizing.width.size + parent_element->child_gap; 
                } else {
                    child_offset_y += child_element->sizing.height.size + parent_element->child_gap; 
                }
            }
        }
    }

    // Draw phrase
    for (uint32_t i = 0; i < ctx.elements.len; i += 1) {
        Imgui_Element element = ctx.elements.elements[i];
        imgui_draw_rect(offscreen_buffer, element.position.x, element.position.y, element.sizing.width.size, element.sizing.height.size, element.bg_color);
    }
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
    ASSERT(ctx != NULL, "Context must not be NULL");
    ASSERT(ctx->elements.elements != NULL, "Elements must not be NULL");
    ASSERT(ctx->elements_open_stack.elements != NULL, "Open stack must not be NULL");

    if(ctx->elements.len >= ctx->elements_count_limit) {
        // log limit of element count reached.
        return;
    }

    uint32_t element_idx = ctx->elements.len;

    Imgui_Element element;
    element.layout_direction  = layout_direction;
    element.bg_color          = bg_color;
    element.child_gap         = child_gap;
    element.padding           = padding;
    element.position          = position;
    element.sizing            = sizing;
    element.children.cap      = UINT8_MAX;
    element.children.len      = 0;
    element.children.elements = (uint32_t*) gmv_arena_alloc(&ctx->allocator, sizeof(uint32_t) * element.children.cap);

    imgui_element_array_push(&ctx->elements, &element);
    imgui_u32_stack_push(&ctx->elements_open_stack, element_idx);
}

void imgui_element_close(Imgui_Context* ctx) {
    ASSERT(ctx != NULL, "Context must not be NULL.");

    // TODO(AJA): handle failure.
    uint32_t element_idx;
    (void) imgui_u32_stack_pop(&ctx->elements_open_stack, &element_idx);
    Imgui_Element* element = &ctx->elements.elements[element_idx];

    // TODO: Try to improve the handling of padding, because this should not influence the size of the element.
    //       Maybe reduce the size of the children ?
    if (element->children.len > 0 && element->sizing.width.type == Fit) {
        element->sizing.width.size  += element->padding.left + element->padding.right;
    }

    if (element->children.len > 0 && element->sizing.height.type == Fit) {
        element->sizing.height.size += element->padding.top + element->padding.bottom;
    }

    uint32_t* parent_idx = imgui_u32_stack_peek(&ctx->elements_open_stack);
    if (parent_idx == NULL) {
        return;
    }

    Imgui_Element* parent = &ctx->elements.elements[*parent_idx];
    imgui_u32_array_push(&parent->children, element_idx);
    if (parent->layout_direction == Horizontal) {
        if (parent->sizing.width.type == Fit) {
            if (parent->children.len > 1) {
                parent->sizing.width.size += parent->child_gap;
            }

            parent->sizing.width.size += element->sizing.width.size;
        }

        if (parent->sizing.height.type == Fit) {
            if (parent->sizing.height.size < element->sizing.height.size) {
                parent->sizing.height.size = element->sizing.height.size;
            }
        }
    } else {
        if (parent->sizing.height.type == Fit) {
            if (parent->children.len > 1) {
                parent->sizing.height.size += parent->child_gap;
            }
            parent->sizing.height.size += element->sizing.height.size;
        }

        if (parent->sizing.width.type == Fit) {
            if (parent->sizing.width.size < element->sizing.width.size) {
                parent->sizing.width.size = element->sizing.width.size;
            }
        }
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

bool imgui_element_array_push(Imgui_Element_Array* array, Imgui_Element* element) {
    ASSERT(array != NULL, "Array must not be null");

    if (array->len >= array->cap) {
        return false;
    }

    array->elements[array->len] = *element;
    array->len += 1;
    return true;
}

bool imgui_u32_array_push(Imgui_U32_Array* array, uint32_t element) {
    ASSERT(array != NULL, "Array must not be null");

    if (array->len >= array->cap) {
        return false;
    }

    array->elements[array->len] = element;
    array->len += 1;
    return true;
}

bool imgui_u32_stack_push(Imgui_U32_Stack* stack, uint32_t element) {
    ASSERT(stack != NULL, "Stack must not be null");

    if(stack->len >= stack->cap) {
        return false;
    }

    stack->elements[stack->len] = element;
    stack->len += 1;
    return true;
}

uint32_t* imgui_u32_stack_peek(Imgui_U32_Stack* stack) {
    ASSERT(stack != NULL, "Stack must not be null");

    if(stack->len == 0) {
        return NULL;
    }

    return &stack->elements[stack->len - 1];
}

bool imgui_u32_stack_pop(Imgui_U32_Stack* stack, uint32_t* poped_val) {
    ASSERT(stack != NULL, "Stack must not be null");

    if (stack->len == 0) {
        return false;
    }

    *poped_val = stack->elements[stack->len - 1];
    stack->len -= 1;
    return true;
}
