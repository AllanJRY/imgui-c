#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> // TODO: remove.

#define BITS_PER_PIXEL  32
#define BYTES_PER_PIXEL 4

#define KEY_STATE_IS_DOWN_MASK 0x8000

#include "imgui.c"

#include <windows.h>

typedef struct W32_Rect {
    int x, y, width, height;
} W32_Rect;

typedef struct W32_Bmp { // TODO: Maybe rename to W32_Screen_buffer or something like that
    BITMAPINFO info;
    void*      mem; // TODO: rename to buffer ? rbg_buffer ?
    int        width;
    int        height;
} W32_Bmp;

static bool        running;
static W32_Bmp     screen_bmp; // TODO: Rename ?
static Imgui_Input old_input;
static Imgui_Input curr_input;

static W32_Rect w32_wnd_content_rect(HWND  wnd_handle) {
    RECT client_rect;
    GetClientRect(wnd_handle, &client_rect);
    return (W32_Rect) {
        .x      = client_rect.left,
        .y      = client_rect.top,
        .width  = client_rect.right - client_rect.left,
        .height = client_rect.bottom - client_rect.top
    };
}

static void w32_bmp_resize_dib_section(W32_Bmp* bmp, int width, int height) {
    if(bmp->mem) {
        VirtualFree(bmp->mem, 0, MEM_RELEASE);
    }

    bmp->width                          = width;
    bmp->height                         = height;
    bmp->info.bmiHeader.biSize          = sizeof(bmp->info.bmiHeader);
    bmp->info.bmiHeader.biWidth         = width;
    bmp->info.bmiHeader.biHeight        = -height; // negative=top-down, positive=bottom-up
    bmp->info.bmiHeader.biPlanes        = 1;
    bmp->info.bmiHeader.biBitCount      = BITS_PER_PIXEL;
    bmp->info.bmiHeader.biCompression   = BI_RGB;

    int bmp_mem_size = (width * height) * BYTES_PER_PIXEL; 
    bmp->mem = VirtualAlloc(0, bmp_mem_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

// TODO: Rename to w32_dc_apply_bmp ?
static void w32_dc_update_content(HDC dc_handle, int dest_width, int dest_height, W32_Bmp* src_bmp) {
    // TODO: correct the aspect ratio.
    (void) StretchDIBits(
        dc_handle,
        0, 0, dest_width, dest_height,
        0, 0, src_bmp->width, src_bmp->height,
        src_bmp->mem,
        &src_bmp->info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT w32_wnd_callback(HWND wnd_handle, UINT msg, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 0;

    switch(msg) {
        case WM_LBUTTONDOWN: {
            OutputDebugStringA("WM_LBUTTONDOWN");
        } break;
        case WM_LBUTTONUP: {
            OutputDebugStringA("WM_LBUTTONUP");
        } break;
        case WM_RBUTTONDOWN: {
            OutputDebugStringA("WM_LBUTTONDOWN");
        } break;
        case WM_RBUTTONUP: {
            OutputDebugStringA("WM_LBUTTONUP");
        } break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN: {
            uint32_t vk_code   = w_param;
            bool was_down = (l_param & (1 << 30)) != 0;
            bool is_down  = (l_param & (1 << 31)) == 0;
            bool is_shift = GetKeyState(VK_SHIFT) & KEY_STATE_IS_DOWN_MASK;
            (void) is_shift;
            
            if (was_down != is_down) {
                if (vk_code == 'H') {
                    curr_input.move_left = true;
                } else if(vk_code == 'J') {
                    curr_input.move_bottom = true;
                } else if(vk_code == 'K') {
                    curr_input.move_top = true;
                } else if(vk_code == 'L') {
                    curr_input.move_right = true;
                }
            }

            bool is_alt_down = (l_param & (1 << 29)) != 0;
            if (vk_code == VK_F4 && is_alt_down) {
                running = false;
            }
        } break;
        case WM_CLOSE: {
            // TODO: Here we can ask the user it is sure to close the app.
            running = false;
        } break;
        case WM_DESTROY: {
            // TODO: Here we can try to rebuild the window.
            running = false;
        } break;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc_handle = BeginPaint(wnd_handle, &paint);

            W32_Rect wnd_content_rect = w32_wnd_content_rect(wnd_handle);
            w32_dc_update_content(dc_handle, wnd_content_rect.width, wnd_content_rect.height, &screen_bmp);

            EndPaint(wnd_handle, &paint);
        } break;
        default: {
            result = DefWindowProc(wnd_handle, msg, w_param, l_param);
        } break;
    }

    return result;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_cmd) {
    (void) prev_instance;
    (void) cmd_line;
    (void) show_cmd;

    LARGE_INTEGER perf_count_frequency;
    (void) QueryPerformanceFrequency(&perf_count_frequency);

    WNDCLASS wnd_class = {0};
    wnd_class.hInstance     = instance;
    wnd_class.lpszClassName = "IMGUI";
    wnd_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc   = w32_wnd_callback;
    /*wnd_class.hIcon;*/
    /*wnd_class.hCursor;*/

    w32_bmp_resize_dib_section(&screen_bmp, 1280, 720);

    RegisterClass(&wnd_class);

    HWND wnd_handle = CreateWindowEx(
        0,
        wnd_class.lpszClassName,
        "IMGUI",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0,
        0,
        instance,
        0
    );

    if (!wnd_handle) {
        MessageBox(0, "Unable to create a window.", "Window creation error.", MB_ICONERROR);
        return 1;
    }

    LARGE_INTEGER last_counter;
    QueryPerformanceCounter(&last_counter);
    int64_t last_cycle_count = __rdtsc();

    running       = true;
    HDC dc_handle = GetDC(wnd_handle);
    while(running) {
        curr_input = (Imgui_Input){0};

        MSG msg = {0};
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Imgui_Offscreen_Buffer offscreen_buffer = {0};
        offscreen_buffer.mem    = screen_bmp.mem;
        offscreen_buffer.width  = screen_bmp.width;
        offscreen_buffer.height = screen_bmp.height;
        imgui_update_and_render(&offscreen_buffer, &curr_input);
        
        W32_Rect wnd_content_rect = w32_wnd_content_rect(wnd_handle);
        w32_dc_update_content(dc_handle, wnd_content_rect.width, wnd_content_rect.height, &screen_bmp);

        int64_t end_cycle_count = __rdtsc();
        LARGE_INTEGER end_counter;
        QueryPerformanceCounter(&end_counter);

        int64_t cycle_elapsed        = end_cycle_count - last_cycle_count;
        int64_t counter_elapsed      = end_counter.QuadPart - last_counter.QuadPart;
        int32_t ms_per_frame         = (int32_t) ((1000 * counter_elapsed) / perf_count_frequency.QuadPart); // NOTE: multiplying by 1000, help getting millisecond instead of seconds
        int32_t fps                  = perf_count_frequency.QuadPart / counter_elapsed;
        int32_t mega_cycle_per_frame = (int32_t) (cycle_elapsed / (1000 * 1000));
        
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%dms/f, %dfps, %dmc/f", ms_per_frame, fps, mega_cycle_per_frame);
        OutputDebugStringA(buffer);

        last_counter     = end_counter;
        last_cycle_count = end_cycle_count;
    }

    return 0;
}

