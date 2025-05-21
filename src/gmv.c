#include <string.h>

#include "gmv.h"

uintptr_t gmv_align_forward(uintptr_t ptr, size_t align) {
    ASSERT((align & (align-1)) == 0, "alignement should be a power of 2");

    // = ptr % align
    uintptr_t modulo = ptr & (align - 1);
    
    uintptr_t ptr_align = ptr;
    if (modulo != 0) {
        ptr_align += align - modulo;
    }

    return ptr_align;
}

void gmv_arena_init(Gmv_Arena* arena, void* backing_buf, size_t backing_buf_len) {
    arena->buf               = backing_buf;
    arena->buf_len           = backing_buf_len;
    arena->prev_offset       = 0;
    arena->curr_offset       = 0;
}

void* gmv_arena_alloc_align(Gmv_Arena* arena, size_t size, size_t align) {
    uintptr_t curr_offset_ptr = (uintptr_t) arena->buf + (uintptr_t) arena->curr_offset;
    curr_offset_ptr = gmv_align_forward(curr_offset_ptr, align);
    size_t curr_offset_aligned = (size_t) curr_offset_ptr - (size_t) arena->buf;

    if(curr_offset_aligned + size > arena->buf_len) {
        return NULL;
    }

    arena->prev_offset  = curr_offset_aligned;
    arena->curr_offset  = curr_offset_aligned + size;
    void* new_alloc_ptr = ((uint8_t*) arena->buf) + curr_offset_aligned;
    memset(new_alloc_ptr, 0, size);
    return new_alloc_ptr;
}

void* gmv_arena_alloc(Gmv_Arena* arena, size_t size) {
    // Default to 16 bytes alignement on 64 bits machine. Recommended for more efficient reads.
    return gmv_arena_alloc_align(arena, size, (2 * sizeof(void*)));
}

void gmv_arena_reset(Gmv_Arena* arena) {
    arena->prev_offset = 0;
    arena->curr_offset = 0;
}
