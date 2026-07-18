#ifndef _MINIFB_H_
#define _MINIFB_H_
#include <stdlib.h>
#include <stdint.h>
// Header-only usage:
//   In exactly one .c file, define MINIFB_IMPLEMENTATION before including this header.
//   In all other files, include this header without MINIFB_IMPLEMENTATION.
//
// Optional input callback:
//   #define MFB_HANDLE_INPUT(wParam, isKeyDown) your_input_handler(wParam, isKeyDown)

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public API

#define MFB_RGB(r, g, b) ((((unsigned int)r) << 16) | (((unsigned int)g) << 8) | b)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create a window with a frame buffer of the given pixel depth (1, 8, 15, 16, or 32 bpp).
// Returns 1 on success and 0 on error.
int mfb_open(const char *title, int width, int height, int scale, int bpp);

// Blit the buffer to the window and process pending messages.
// Returns true to continue, false if the window was closed.
bool mfb_update(const void *buffer, int fps_limit);

// Set a range of palette entries (8-bit mode only).
void mfb_set_palette_array(const uint32_t *new_palette, uint8_t start, uint8_t count);

// Set a single palette entry (8-bit mode only).
void mfb_set_palette(uint8_t color_index, uint32_t color);

// Close the window and release all resources.
void mfb_close();

// Returns a pointer to the 512-byte key state array (nonzero = held).
const char *mfb_keystatus();

// Consume one queued text input character from WM_CHAR. Returns false when no
// character is pending. Enter is '\r', Backspace is '\b', Escape is 27.
bool mfb_poll_char(uint32_t *ch);

// Consume the most recent right-click. Returns true once per click and writes
// the click position in FRAMEBUFFER pixels (already divided by the window
// scale). Returns false when no unconsumed right-click is pending.
bool mfb_poll_rclick(int *x, int *y);

// Consume the most recent left-button press / double-click. Same FRAMEBUFFER-
// pixel convention as mfb_poll_rclick. The window class uses CS_DBLCLKS, so the
// second press of a double-click arrives as a DBLCLK (poll_ldblclick), not a
// plain lclick — the caller can defer the single-click action until the
// double-click window (mfb_double_click_ms) elapses, like the UO client.
bool mfb_poll_lclick(int *x, int *y);
bool mfb_poll_ldblclick(int *x, int *y);

// System double-click interval in milliseconds (GetDoubleClickTime()).
unsigned mfb_double_click_ms(void);

// Current mouse position in FRAMEBUFFER pixels. Returns true while the cursor
// is inside the window's client area, false when it has left.
bool mfb_mousepos(int *x, int *y);

// True while the right mouse button is held down. Used for Transport Tycoon
// style right-drag map panning. The window captures the mouse on right-press so
// the held state (and motion) keep reporting even if the drag leaves the client.
bool mfb_mouse_right_down(void);

// Capture/release the mouse for first-person controls. While captured, the
// cursor is hidden and constrained to the client area, and raw relative motion
// is accumulated for mfb_poll_mouse_delta().
void mfb_set_mouse_capture(int capture);
bool mfb_mouse_captured(void);
bool mfb_poll_mouse_delta(int *dx, int *dy);

// Hide the OS cursor over the client area (so the caller can draw its own).
// Off by default — only enable once you actually have a cursor to draw, or the
// pointer would simply vanish over the window.
void mfb_set_hide_cursor(int hide);

// Update the window title bar.
void mfb_set_title(const char *title);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#ifdef MINIFB_IMPLEMENTATION
#ifdef MINIFB_IMPLEMENTATION_INCLUDED
#error "MINIFB_IMPLEMENTATION must be defined before including MiniFB.h in only one translation unit."
#endif
#define MINIFB_IMPLEMENTATION_INCLUDED

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifndef MFB_HANDLE_INPUT
#define MFB_HANDLE_INPUT(wParam, isKeyDown) ((void)(wParam), (void)(isKeyDown))
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

typedef struct {
    HWND window;
    int close_requested;
    int buffer_width;
    int buffer_height;
    int scale;
    int bits_per_pixel;
    HDC device_context;
    const void *pixel_buffer;
    BITMAPINFO *bitmap_info;
    char keys[512];
    uint32_t text_chars[64];
    int text_read;
    int text_write;
    int rclick_pending;   // a right-click is waiting to be consumed
    int rclick_x;         // framebuffer-pixel click position
    int rclick_y;
    int lclick_pending;   // a left-button press is waiting to be consumed
    int lclick_x;
    int lclick_y;
    int ldblclick_pending; // a left double-click is waiting to be consumed
    int ldblclick_x;
    int ldblclick_y;
    int mouse_x;          // framebuffer-pixel cursor position
    int mouse_y;
    int mouse_inside;     // cursor currently within the client area
    int mouse_right_down; // right button held (for right-drag map panning)
    int hide_cursor;      // hide the OS cursor over the client area
    int mouse_captured;   // first-person raw-input mode
    LONG mouse_delta_x;   // accumulated raw relative motion
    LONG mouse_delta_y;
} MfbState;

static MfbState *mfb_state;

static void mfb_push_char(uint32_t ch) {
    const int next = (mfb_state->text_write + 1) % 64;
    if (next == mfb_state->text_read)
        return;
    mfb_state->text_chars[mfb_state->text_write] = ch;
    mfb_state->text_write = next;
}

// Sets up bitmap_info header and color table for the given bpp.
static void setup_bitmap(const int width, const int height, const int bpp) {
    mfb_state->bitmap_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    mfb_state->bitmap_info->bmiHeader.biPlanes = 1;
    mfb_state->bitmap_info->bmiHeader.biWidth = width;
    mfb_state->bitmap_info->bmiHeader.biHeight = -height;

    DWORD *const masks = (DWORD *) &mfb_state->bitmap_info->bmiColors[0];

    switch (bpp) {
        case 1:
            mfb_state->bitmap_info->bmiHeader.biBitCount = 1;
            mfb_state->bitmap_info->bmiHeader.biCompression = BI_RGB;
            mfb_state->bitmap_info->bmiHeader.biClrUsed = 2;
            mfb_state->bitmap_info->bmiHeader.biClrImportant = 2;
            masks[0] = 0x000000;
            masks[1] = 0xFFFFFF;
            break;
        case 8:
            mfb_state->bitmap_info->bmiHeader.biBitCount = 8;
            mfb_state->bitmap_info->bmiHeader.biCompression = BI_RGB;
            mfb_state->bitmap_info->bmiHeader.biClrUsed = 256;
            mfb_state->bitmap_info->bmiHeader.biClrImportant = 256;
            break;
        case 15:
            mfb_state->bitmap_info->bmiHeader.biBitCount = 16;
            mfb_state->bitmap_info->bmiHeader.biCompression = BI_BITFIELDS;
            masks[0] = 0x7C00;
            masks[1] = 0x03E0;
            masks[2] = 0x001F;
            break;
        case 16:
            mfb_state->bitmap_info->bmiHeader.biBitCount = 16;
            mfb_state->bitmap_info->bmiHeader.biCompression = BI_BITFIELDS;
            masks[0] = 0xF800;
            masks[1] = 0x07E0;
            masks[2] = 0x001F;
            break;
        case 32:
        default:
            mfb_state->bitmap_info->bmiHeader.biBitCount = 32;
            mfb_state->bitmap_info->bmiHeader.biCompression = BI_RGB;
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Handles window messages: paint, keyboard input, and close.

static LRESULT CALLBACK WndProc(HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
    LRESULT result = 0;

    switch (message) {
        case WM_PAINT: {
            if (mfb_state->pixel_buffer) {
                StretchDIBits(mfb_state->device_context,
                              0, 0, mfb_state->buffer_width * mfb_state->scale, mfb_state->buffer_height * mfb_state->scale,
                              0, 0, mfb_state->buffer_width, mfb_state->buffer_height,
                              mfb_state->pixel_buffer,
                              mfb_state->bitmap_info, DIB_RGB_COLORS, SRCCOPY);
                ValidateRect(hWnd, nullptr);
            }
            break;
        }

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP: {
            const BOOL down = !(lParam >> 31 & 1);
            MFB_HANDLE_INPUT(wParam, down);
            if (wParam < sizeof(mfb_state->keys)) {
                mfb_state->keys[wParam] = down;
            }
            break;
        }

        case WM_CHAR:
            mfb_push_char((uint32_t) wParam);
            break;

        case WM_INPUT: {
            if (mfb_state->mouse_captured) {
                RAWINPUT raw;
                UINT size = sizeof(raw);
                if (GetRawInputData((HRAWINPUT) lParam, RID_INPUT, &raw, &size,
                                    sizeof(RAWINPUTHEADER)) == size &&
                    raw.header.dwType == RIM_TYPEMOUSE &&
                    !(raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)) {
                    mfb_state->mouse_delta_x += raw.data.mouse.lLastX;
                    mfb_state->mouse_delta_y += raw.data.mouse.lLastY;
                }
            }
            // Required by the raw-input contract for foreground (RIM_INPUT) data.
            result = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

        case WM_KILLFOCUS:
            mfb_set_mouse_capture(0);
            mfb_state->mouse_right_down = 0;  // capture is lost with focus
            for (size_t i = 0; i < sizeof(mfb_state->keys); ++i)
                mfb_state->keys[i] = 0;
            break;

        case WM_MOUSEMOVE: {
            const int scale = mfb_state->scale > 0 ? mfb_state->scale : 1;
            mfb_state->mouse_x = ((int) (short) LOWORD(lParam)) / scale;
            mfb_state->mouse_y = ((int) (short) HIWORD(lParam)) / scale;
            if (!mfb_state->mouse_inside) {
                mfb_state->mouse_inside = 1;
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                tme.dwHoverTime = 0;
                TrackMouseEvent(&tme);   // so we get a WM_MOUSELEAVE
            }
            break;
        }

        case WM_MOUSELEAVE:
            mfb_state->mouse_inside = 0;
            break;

        case WM_SETCURSOR:
            // Hide the OS cursor over the client area only when enabled; we
            // draw our own there. Otherwise keep the normal arrow.
            if (mfb_state->hide_cursor && LOWORD(lParam) == HTCLIENT) {
                SetCursor(nullptr);
                result = TRUE;
                break;
            }
            result = DefWindowProc(hWnd, message, wParam, lParam);
            break;

        case WM_RBUTTONDOWN: {
            // lParam packs the cursor position in client (scaled) pixels.
            const int cx = (int) (short) LOWORD(lParam);
            const int cy = (int) (short) HIWORD(lParam);
            const int scale = mfb_state->scale > 0 ? mfb_state->scale : 1;
            int fx = cx / scale, fy = cy / scale;
            if (fx < 0) fx = 0;
            if (fy < 0) fy = 0;
            if (fx >= mfb_state->buffer_width)  fx = mfb_state->buffer_width - 1;
            if (fy >= mfb_state->buffer_height) fy = mfb_state->buffer_height - 1;
            mfb_state->rclick_x = fx;
            mfb_state->rclick_y = fy;
            mfb_state->rclick_pending = 1;
            mfb_state->mouse_right_down = 1;
            SetCapture(hWnd);  // keep the drag alive if it leaves the client
            break;
        }

        case WM_RBUTTONUP:
            mfb_state->mouse_right_down = 0;
            ReleaseCapture();
            break;

        case WM_LBUTTONDOWN: {
            const int cx = (int) (short) LOWORD(lParam);
            const int cy = (int) (short) HIWORD(lParam);
            const int scale = mfb_state->scale > 0 ? mfb_state->scale : 1;
            int fx = cx / scale, fy = cy / scale;
            if (fx < 0) fx = 0;
            if (fy < 0) fy = 0;
            if (fx >= mfb_state->buffer_width)  fx = mfb_state->buffer_width - 1;
            if (fy >= mfb_state->buffer_height) fy = mfb_state->buffer_height - 1;
            mfb_state->lclick_x = fx;
            mfb_state->lclick_y = fy;
            mfb_state->lclick_pending = 1;
            break;
        }

        case WM_LBUTTONDBLCLK: {
            // With CS_DBLCLKS the second press of a double-click arrives here
            // (sequence: DOWN, UP, DBLCLK, UP). The first DOWN already queued an
            // lclick; the caller treats a pending dblclick as the use/open gesture.
            const int cx = (int) (short) LOWORD(lParam);
            const int cy = (int) (short) HIWORD(lParam);
            const int scale = mfb_state->scale > 0 ? mfb_state->scale : 1;
            int fx = cx / scale, fy = cy / scale;
            if (fx < 0) fx = 0;
            if (fy < 0) fy = 0;
            if (fx >= mfb_state->buffer_width)  fx = mfb_state->buffer_width - 1;
            if (fy >= mfb_state->buffer_height) fy = mfb_state->buffer_height - 1;
            mfb_state->ldblclick_x = fx;
            mfb_state->ldblclick_y = fy;
            mfb_state->ldblclick_pending = 1;
            break;
        }

        case WM_CLOSE:
            mfb_state->close_requested = 1;
            break;

        default:
            result = DefWindowProc(hWnd, message, wParam, lParam);
    }

    return result;
}

// Creates a window with a frame buffer of the given pixel depth (1, 8, 15, 16, or 32 bpp).
int mfb_open(const char *title, const int width, const int height, const int scale, const int bpp) {
    WNDCLASS wc = {0};
    wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = title;

    if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return 0;
    }

    RECT rect = {0};
    rect.right = width * scale;
    rect.bottom = height * scale;

    AdjustWindowRect(&rect, WS_POPUP | WS_SYSMENU | WS_CAPTION, 0);
    rect.right -= rect.left;
    rect.bottom -= rect.top;

    const int screen_width = GetSystemMetrics(SM_CXSCREEN);
    const int screen_height = GetSystemMetrics(SM_CYSCREEN);

    const int x_offset = GetSystemMetrics(SM_XVIRTUALSCREEN) < 0 ? -2120 : 0;
    const int y_offset = GetSystemMetrics(SM_XVIRTUALSCREEN) < 0 ? height : 0;

    mfb_state = (MfbState *) calloc(1, sizeof(MfbState));
    if (!mfb_state) {
        return 0;
    }
    mfb_state->buffer_width = width;
    mfb_state->buffer_height = height;
    mfb_state->scale = scale;
    mfb_state->bits_per_pixel = bpp;

    mfb_state->window = CreateWindowEx(0,
                                       title, title,
                                       WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
                                       x_offset + (screen_width - rect.right) / 2,
                                       y_offset + (screen_height - rect.bottom + rect.top) / 2,
                                       rect.right, rect.bottom,
                                       nullptr, nullptr, nullptr, nullptr);

    if (!mfb_state->window) {
        free(mfb_state);
        mfb_state = nullptr;
        return 0;
    }

    mfb_state->bitmap_info = (BITMAPINFO *) calloc(1, sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256);
    if (!mfb_state->bitmap_info) {
        DestroyWindow(mfb_state->window);
        free(mfb_state);
        mfb_state = nullptr;
        return 0;
    }

    ShowWindow(mfb_state->window, SW_NORMAL);

    const RAWINPUTDEVICE raw_mouse = {
        .usUsagePage = 0x01,
        .usUsage = 0x02,
        .dwFlags = 0,
        .hwndTarget = mfb_state->window
    };
    if (!RegisterRawInputDevices(&raw_mouse, 1, sizeof(raw_mouse))) {
        DestroyWindow(mfb_state->window);
        free(mfb_state);
        mfb_state = nullptr;
        return 0;
    }

    setup_bitmap(width, height, bpp);

    mfb_state->device_context = GetDC(mfb_state->window);
    if (!mfb_state->device_context) {
        free(mfb_state->bitmap_info);
        DestroyWindow(mfb_state->window);
        free(mfb_state);
        mfb_state = nullptr;
        return 0;
    }

    return 1;
}

// Sets a range of palette entries (8-bit mode only).
void mfb_set_palette_array(const uint32_t *const new_palette, const uint8_t start, const uint8_t count) {
    uint32_t *const palette = (uint32_t *) &mfb_state->bitmap_info->bmiColors[0];
    for (int i = start; i < start + count; i++) {
        palette[i] = new_palette[i - start];
    }
}

// Sets a single palette entry (8-bit mode only).
void mfb_set_palette(const uint8_t color_index, const uint32_t color) {
    uint32_t *const palette = (uint32_t *) &mfb_state->bitmap_info->bmiColors[0];
    palette[color_index] = color;
}

// Blits the buffer to the window and processes pending messages. Returns false if window was closed.
bool mfb_update(const void *buffer, const int fps_limit) {
    static DWORD previous_frame_time = 0;
    MSG msg;

    mfb_state->pixel_buffer = buffer;

    InvalidateRect(mfb_state->window, nullptr, TRUE);
    SendMessage(mfb_state->window, WM_PAINT, 0, 0);

    while (PeekMessage(&msg, mfb_state->window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (mfb_state->close_requested)
        return false;

    if (fps_limit) {
        const DWORD target = 1000 / fps_limit;
        const DWORD elapsed = GetTickCount() - previous_frame_time;
        if (elapsed < target)
            Sleep(target - elapsed);
        previous_frame_time = GetTickCount();
    }

    return true;
}

// Releases all resources and destroys the window.
void mfb_close() {
    mfb_set_mouse_capture(0);
    mfb_state->pixel_buffer = nullptr;
    free(mfb_state->bitmap_info);
    ReleaseDC(mfb_state->window, mfb_state->device_context);
    DestroyWindow(mfb_state->window);
    free(mfb_state);
    mfb_state = nullptr;
}

// Returns a pointer to the 512-byte key state array (nonzero = held).
const char *mfb_keystatus() {
    return mfb_state->keys;
}

bool mfb_poll_char(uint32_t *ch) {
    if (!mfb_state || mfb_state->text_read == mfb_state->text_write)
        return false;
    if (ch) *ch = mfb_state->text_chars[mfb_state->text_read];
    mfb_state->text_read = (mfb_state->text_read + 1) % 64;
    return true;
}

// Consume the most recent right-click (framebuffer pixels). One true per click.
bool mfb_poll_rclick(int *x, int *y) {
    if (!mfb_state || !mfb_state->rclick_pending)
        return false;
    if (x) *x = mfb_state->rclick_x;
    if (y) *y = mfb_state->rclick_y;
    mfb_state->rclick_pending = 0;
    return true;
}

// Consume the most recent left-button press (framebuffer pixels).
bool mfb_poll_lclick(int *x, int *y) {
    if (!mfb_state || !mfb_state->lclick_pending)
        return false;
    if (x) *x = mfb_state->lclick_x;
    if (y) *y = mfb_state->lclick_y;
    mfb_state->lclick_pending = 0;
    return true;
}

// Consume the most recent left double-click (framebuffer pixels).
bool mfb_poll_ldblclick(int *x, int *y) {
    if (!mfb_state || !mfb_state->ldblclick_pending)
        return false;
    if (x) *x = mfb_state->ldblclick_x;
    if (y) *y = mfb_state->ldblclick_y;
    mfb_state->ldblclick_pending = 0;
    return true;
}

unsigned mfb_double_click_ms(void) {
    return GetDoubleClickTime();
}

// Current mouse position in framebuffer pixels; true while inside the client.
bool mfb_mousepos(int *x, int *y) {
    if (!mfb_state) return false;
    if (x) *x = mfb_state->mouse_x;
    if (y) *y = mfb_state->mouse_y;
    return mfb_state->mouse_inside != 0;
}

bool mfb_mouse_right_down(void) {
    return mfb_state && mfb_state->mouse_right_down != 0;
}

void mfb_set_mouse_capture(const int capture) {
    if (!mfb_state) return;

    const int enabled = capture != 0;
    if (enabled == mfb_state->mouse_captured) return;

    mfb_state->mouse_captured = enabled;
    mfb_state->mouse_delta_x = 0;
    mfb_state->mouse_delta_y = 0;

    if (enabled) {
        SetFocus(mfb_state->window);
        SetCapture(mfb_state->window);

        RECT rect;
        GetClientRect(mfb_state->window, &rect);
        MapWindowPoints(mfb_state->window, nullptr, (POINT *) &rect, 2);
        ClipCursor(&rect);

        mfb_state->hide_cursor = 1;
        const int center_x = (rect.left + rect.right) / 2;
        const int center_y = (rect.top + rect.bottom) / 2;
        SetCursorPos(center_x, center_y);
    } else {
        ClipCursor(nullptr);
        if (GetCapture() == mfb_state->window) ReleaseCapture();
        mfb_state->hide_cursor = 0;
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
    }
}

bool mfb_mouse_captured(void) {
    return mfb_state && mfb_state->mouse_captured;
}

bool mfb_poll_mouse_delta(int *dx, int *dy) {
    if (!mfb_state || !mfb_state->mouse_captured) return false;

    const LONG x = InterlockedExchange(&mfb_state->mouse_delta_x, 0);
    const LONG y = InterlockedExchange(&mfb_state->mouse_delta_y, 0);
    if (dx) *dx = (int) x;
    if (dy) *dy = (int) y;
    return x != 0 || y != 0;
}

void mfb_set_hide_cursor(int hide) {
    if (mfb_state) mfb_state->hide_cursor = hide;
}

// Update the window title bar.
void mfb_set_title(const char *title) {
    if (mfb_state && mfb_state->window)
        SetWindowText(mfb_state->window, title);
}

#endif

#endif
