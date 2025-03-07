#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

LRESULT w32_wnd_callback(HWND wnd_handle, UINT msg, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 0;

    switch(msg) {
        case WM_CLOSE: {
            PostQuitMessage(0);
        } break;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC device_ctx = BeginPaint(wnd_handle, &paint);

            PatBlt(
              device_ctx,
              paint.rcPaint.left,
              paint.rcPaint.top,
              paint.rcPaint.right - paint.rcPaint.left,
              paint.rcPaint.bottom - paint.rcPaint.top,
              BLACKNESS
            );


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
    while(GetMessage(&msg, 0, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

