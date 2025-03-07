#ifndef UNICODE
#define UNICODE
#endif

#include <stdbool.h>
#include <stdint.h>
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

static bool    running;
static W32_Bmp screen_bmp; // TODO: Rename ?

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

static void w32_resize_dib_section(int width, int height) {
    if(screen_bmp.mem) {
        VirtualFree(screen_bmp.mem, 0, MEM_RELEASE);
    }

    screen_bmp.width                          = width;
    screen_bmp.height                         = height;
    screen_bmp.info.bmiHeader.biSize          = sizeof(screen_bmp.info.bmiHeader);
    screen_bmp.info.bmiHeader.biWidth         = width;
    screen_bmp.info.bmiHeader.biHeight        = -height; // negative=top-down, positive=bottom-up
    screen_bmp.info.bmiHeader.biPlanes        = 1;
    screen_bmp.info.bmiHeader.biBitCount      = 32;
    screen_bmp.info.bmiHeader.biCompression   = BI_RGB;

    int bytes_per_pixel = 4;
    int bmp_mem_size = (width * height) * bytes_per_pixel; 
    screen_bmp.mem = VirtualAlloc(0, bmp_mem_size, MEM_COMMIT, PAGE_READWRITE);


    uint8_t* row = (uint8_t*) screen_bmp.mem;
    for (int y = 0; y < height; y += 1) {
        uint32_t* pixel = (uint32_t*) row;
        for (int x = 0; x < width; x += 1) {
            uint8_t blue  = (uint8_t) x;
            uint8_t green = (uint8_t) y;
            *pixel = (green << 8) | blue;
            pixel += 1;
        }

        row += bytes_per_pixel * width;
    }
}

static void w32_dc_update_content(HDC dc_handle, W32_Rect content_rect) {
    (void) StretchDIBits(
        dc_handle,
        0, 0, content_rect.width, content_rect.height,
        0, 0, screen_bmp.width, screen_bmp.height,
        screen_bmp.mem,
        &screen_bmp.info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT w32_wnd_callback(HWND wnd_handle, UINT msg, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 0;

    switch(msg) {
        case WM_SIZE: {
            W32_Rect wnd_content = w32_wnd_content_rect(wnd_handle);
            w32_resize_dib_section(wnd_content.width, wnd_content.height);
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
            w32_dc_update_content(dc_handle, wnd_content_rect);

            EndPaint(wnd_handle, &paint);
        } break;
        default: {
            result = DefWindowProc(wnd_handle, msg, w_param, l_param);
        } break;
    }

    return result;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int show_cmd) {
    (void) prev_instance;
    (void) cmd_line;
    (void) show_cmd;

    WNDCLASS wnd_class = {0};
    wnd_class.hInstance     = instance;
    wnd_class.lpszClassName = L"IMGUI";
    wnd_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc   = w32_wnd_callback;
    /*wnd_class.hIcon;*/
    /*wnd_class.hCursor;*/

    RegisterClass(&wnd_class);

    HWND wnd_handle = CreateWindowEx(
        0,
        wnd_class.lpszClassName,
        L"INGUI",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0,
        0,
        instance,
        0
    );

    if (!wnd_handle) {
        MessageBox(0, L"Unable to create a window.", L"Window creation error.", MB_ICONERROR);

        return 1;
    }

    MSG msg = {0};
    running = true;
    while(running) {
        if(GetMessage(&msg, 0, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Rest of the main loop.
    }

    return 0;
}

