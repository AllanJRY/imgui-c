#ifndef GMV_H

#include "debug.h"

typedef struct Gmv_Arena Gmv_Arena;
typedef struct Gmv_Block Gmv_Block;

uintptr_t gmv_align_forward(uintptr_t ptr, size_t align);

struct Gmv_Arena {
    void*  buf;
    size_t buf_len;
    size_t prev_offset;
    size_t curr_offset;
};

void  gmv_arena_init(Gmv_Arena* arena, void* backing_buf, size_t backing_buf_len);
void* gmv_arena_alloc_align(Gmv_Arena* arena, size_t size, size_t align);
void* gmv_arena_alloc(Gmv_Arena* arena, size_t size);
void  gmv_arena_reset(Gmv_Arena* arena);

#define GMV_H
#endif
