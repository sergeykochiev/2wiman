#include "windows.h"
#include <minwindef.h>
#include <windef.h>
#include "C:\Users\dupa\gcc\x86_64-w64-mingw32\include\wingdi.h"
#include <wingdi.h>
#include <winuser.h>
#include <commctrl.h>
#include <windowsx.h>
#include <stdio.h>
#include <wchar.h>
#include <math.h>

static const COLORREF BG_COLOR = 0x00444444;
static const COLORREF DESK_COLOR = 0x00878787;
static const COLORREF DESK_BG_COLOR = 0x006B979E;
static const COLORREF ACTIVE_DESK_COLOR = 0x00BC750B;
static const COLORREF ACTIVE_DESK_BG_COLOR = 0x00FFDA0A;
static const COLORREF DESK_COLORS[2] = { [FALSE] = DESK_COLOR, [TRUE] = ACTIVE_DESK_COLOR };
static const COLORREF DESK_BG_COLORS[2] = { [FALSE] = DESK_BG_COLOR, [TRUE] = ACTIVE_DESK_BG_COLOR };
static const POINT DESK_TILE_SIZE = { 40, 40 };
static const char WINDOW_CLASSNAME[] = "2wiman";
static const char BUTTON_CLASSNAME[] = "2wiman-control";
#define DESK_COUNT 9
#define TILE_OFFSET 3
#define MODIFIER_KEY VK_CAPITAL
#define order_wnd_z(hwnd, pos) SetWindowPos(hwnd, pos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)
#define is_param_in_xor(xor, param) ((xor | param) == xor)
#define is_wh_equal_rect(a, b) (((a.bottom - a.top) == (b.bottom - b.top)) && ((a.right - a.left) == (b.right - b.left)))
#define is_pressed(key) HIWORD(GetKeyState(key)) != 0
#define is_toolwindow(dxExStyle) is_param_in_xor(dxExStyle, WS_EX_TOOLWINDOW)
#define is_resizable(dxStyle) is_param_in_xor(dxStyle, WS_THICKFRAME)
#define get_curr_hwnd(wmds) (wmds).wnd_list[(wmds).curr_wnd].hwnd
#define g_curr_desk wms.desk_list[wms.curr_desk]
#define print_rect(rect) printf("left %ld, top %ld, right %ld, bottom %ld", rect.left , rect.top, rect.right, rect.bottom);
#define is_point_in_rect(point, rect) (point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
#define ONLY_HOVER TME_HOVER
#define ONLY_LEAVE TME_LEAVE
#define HOVER_AND_LEAVE TME_HOVER | TME_LEAVE

typedef struct {
    int stack_mode_infinite_scroll;
    int default_mode;
} WIMAN_CONFIG;

typedef struct {
    HWND hwnd;
    RECT last_set_pos;
    int is_freeroam;
    int is_unresizable;
} WIMAN_WINDOW;

typedef struct {
    HMONITOR hmonitor;
    int h;
    int w;
    RECT pos;
    int front_desk;
    HWND child_hwnd;
    size_t desk_count;
} WIN_MONITOR;

typedef enum {
    WMM_STACK,
    WMM_TILING_V,
    WMM_TILING_H,
    WMM_COUNT,
} WIMAN_MODE;

typedef struct {
    HWND hwnd;
    POINT last_set_left_top;
    int is_hovered;
    int is_tracking;
} WIMAN_DESK_BUTTON;

typedef struct {
    WIMAN_WINDOW *wnd_list;
    WIMAN_DESK_BUTTON button;
    size_t wnd_count;
    size_t tiling_count;
    int curr_wnd;
    WIMAN_MODE mode;
    int changed;
    int on_monitor;
} WIMAN_DESKTOP_STATE;

typedef struct {
    WIMAN_DESKTOP_STATE desk_list[DESK_COUNT];
    UINT dpi;
    int curr_desk;
    WIN_MONITOR *monitors;
    // int curr_monitor;
    size_t monitor_count;
    HWND main_hwnd;
    HINSTANCE *h_instance;
} WIMAN_STATE;

WIMAN_STATE wms = {};

WIMAN_CONFIG wiman_config = {
    .stack_mode_infinite_scroll = FALSE,
    .default_mode = WMM_STACK,
};

const char* const WMM_NAMES[WMM_COUNT] = {
    [WMM_TILING_V] = "vertical tiling",
    [WMM_TILING_H] = "horizontal tiling",
    [WMM_STACK] = "stack",
};
UINT WM_SHELLHOOKMESSAGE;

LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GuiWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void wnd_msg_proc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);

void print_ex_styles(long ex_styles) {
    if(is_param_in_xor(ex_styles, WS_EX_ACCEPTFILES)) printf("WS_EX_ACCEPTFILES, "); if(is_param_in_xor(ex_styles, WS_EX_APPWINDOW)) printf("WS_EX_APPWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_CLIENTEDGE)) printf("WS_EX_CLIENTEDGE, "); if(is_param_in_xor(ex_styles, WS_EX_COMPOSITED)) printf("WS_EX_COMPOSITED, "); if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("WS_EX_CONTEXTHELP, "); if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("WS_EX_CONTEXTHELP, "); if(is_param_in_xor(ex_styles, WS_EX_CONTROLPARENT)) printf("WS_EX_CONTROLPARENT, "); if(is_param_in_xor(ex_styles, WS_EX_DLGMODALFRAME)) printf("WS_EX_DLGMODALFRAME, "); if(is_param_in_xor(ex_styles, WS_EX_LAYERED)) printf("WS_EX_LAYERED, "); if(is_param_in_xor(ex_styles, WS_EX_LAYOUTRTL)) printf("WS_EX_LAYOUTRTL, "); if(is_param_in_xor(ex_styles, WS_EX_LEFT)) printf("WS_EX_LEFT, "); if(is_param_in_xor(ex_styles, WS_EX_LEFTSCROLLBAR)) printf("WS_EX_LEFTSCROLLBAR, "); if(is_param_in_xor(ex_styles, WS_EX_LTRREADING)) printf("WS_EX_LTRREADING, "); if(is_param_in_xor(ex_styles, WS_EX_MDICHILD)) printf("WS_EX_MDICHILD, "); if(is_param_in_xor(ex_styles, WS_EX_NOACTIVATE)) printf("WS_EX_NOACTIVATE, "); if(is_param_in_xor(ex_styles, WS_EX_NOINHERITLAYOUT)) printf("WS_EX_NOINHERITLAYOUT, "); if(is_param_in_xor(ex_styles, WS_EX_NOPARENTNOTIFY)) printf("WS_EX_NOPARENTNOTIFY, "); if(is_param_in_xor(ex_styles, WS_EX_NOREDIRECTIONBITMAP)) printf("WS_EX_NOREDIRECTIONBITMAP, "); if(is_param_in_xor(ex_styles, WS_EX_OVERLAPPEDWINDOW)) printf("WS_EX_OVERLAPPEDWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_PALETTEWINDOW)) printf("WS_EX_PALETTEWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_RIGHT)) printf("WS_EX_RIGHT, "); if(is_param_in_xor(ex_styles, WS_EX_RIGHTSCROLLBAR)) printf("WS_EX_RIGHTSCROLLBAR, "); if(is_param_in_xor(ex_styles, WS_EX_RTLREADING)) printf("WS_EX_RTLREADING, "); if(is_param_in_xor(ex_styles, WS_EX_STATICEDGE)) printf("WS_EX_STATICEDGE, "); if(is_param_in_xor(ex_styles, WS_EX_TOOLWINDOW)) printf("WS_EX_TOOLWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_TOPMOST)) printf("WS_EX_TOPMOST, "); if(is_param_in_xor(ex_styles, WS_EX_TRANSPARENT)) printf("WS_EX_TRANSPARENT, "); if(is_param_in_xor(ex_styles, WS_EX_WINDOWEDGE)) printf("WS_EX_WINDOWEDGE, ");
}

void print_styles(long styles) {
    if(is_param_in_xor(styles, WS_BORDER)) printf("WS_BORDER, "); if(is_param_in_xor(styles, WS_CAPTION)) printf("WS_CAPTION, "); if(is_param_in_xor(styles, WS_CHILD)) printf("WS_CHILD, "); if(is_param_in_xor(styles, WS_CHILDWINDOW)) printf("WS_CHILDWINDOW, "); if(is_param_in_xor(styles, WS_CLIPCHILDREN)) printf("WS_CLIPCHILDREN, "); if(is_param_in_xor(styles, WS_CLIPSIBLINGS)) printf("WS_CLIPSIBLINGS, "); if(is_param_in_xor(styles, WS_DISABLED)) printf("WS_DISABLED, "); if(is_param_in_xor(styles, WS_DLGFRAME)) printf("WS_DLGFRAME, "); if(is_param_in_xor(styles, WS_GROUP)) printf("WS_GROUP, "); if(is_param_in_xor(styles, WS_HSCROLL)) printf("WS_HSCROLL, "); if(is_param_in_xor(styles, WS_ICONIC)) printf("WS_ICONIC, "); if(is_param_in_xor(styles, WS_MAXIMIZE)) printf("WS_MAXIMIZE, "); if(is_param_in_xor(styles, WS_MAXIMIZEBOX)) printf("WS_MAXIMIZEBOX, "); if(is_param_in_xor(styles, WS_MINIMIZE)) printf("WS_MINIMIZE, "); if(is_param_in_xor(styles, WS_MINIMIZEBOX)) printf("WS_MINIMIZEBOX, "); if(is_param_in_xor(styles, WS_OVERLAPPED)) printf("WS_OVERLAPPED, "); if(is_param_in_xor(styles, WS_OVERLAPPEDWINDOW)) printf("WS_OVERLAPPEDWINDOW, "); if(is_param_in_xor(styles, WS_POPUPWINDOW)) printf("WS_POPUPWINDOW, "); if(is_param_in_xor(styles, WS_SIZEBOX)) printf("WS_SIZEBOX, "); if(is_param_in_xor(styles, WS_SYSMENU)) printf("WS_SYSMENU, "); if(is_param_in_xor(styles, WS_TABSTOP)) printf("WS_TABSTOP, "); if(is_param_in_xor(styles, WS_THICKFRAME)) printf("WS_THICKFRAME, "); if(is_param_in_xor(styles, WS_TILED)) printf("WS_TILED, "); if(is_param_in_xor(styles, WS_TILEDWINDOW)) printf("WS_TILEDWINDOW, "); if(is_param_in_xor(styles, WS_VISIBLE)) printf("WS_VISIBLE, "); if(is_param_in_xor(styles, WS_VSCROLL)) printf("WS_VSCROLL, ");
}

void print_windowinfo(WINDOWINFO *info, const char prefix[]) {
    printf("%scdSize: %lu\n", prefix, info->cbSize);
    printf("%srcWindow: ", prefix);
    print_rect(info->rcWindow);
    printf("\n");
    printf("%srcClient: ", prefix);
    print_rect(info->rcClient);
    printf("\n");
    printf("%sdwStyle: ", prefix);
    print_styles(info->dwStyle);
    printf("\n");
    printf("%sdwExStyle: ", prefix);
    print_ex_styles(info->dwExStyle);
    printf("\n");
    printf("%sdwWindowStatus: %lu\n", prefix, info->dwWindowStatus);
    printf("%scxWindowBorders: %u\n", prefix, info->cxWindowBorders);
    printf("%scyWindowBorders: %u\n", prefix, info->cyWindowBorders);
    printf("%satomWindowType: %hu\n", prefix, info->atomWindowType);
    printf("%swCreatorVersion: %hu\n", prefix, info->wCreatorVersion);
}

int print_windows_list(WIMAN_WINDOW **wndl, size_t len, BOOL verbose) {
    char *title = malloc(0);
    int length;
    WINDOWINFO wi;
    for(int i = 0; i < len; i++) {
        #define wnd (*wndl)[i]
        length = GetWindowTextLengthA(wnd.hwnd);
        char *new_title = realloc(title, length + 1);
        if(new_title == NULL) return 1;
        title = new_title;
        GetWindowTextA(wnd.hwnd, title, length + 1);
        printf("   - Window %d%s: \"%s\"\n", i + 1, wnd.is_freeroam ? wnd.is_unresizable ? " (freeroam, unresizable)" : " (freeroam)" : "", title);
        GetWindowInfo(wnd.hwnd, &wi);
        if(verbose) print_windowinfo(&wi, "       ");
    }
    free(title);
    #undef wnd
    return 0;
}

void print_debug_info(WIMAN_STATE *wms, BOOL verbose) {
    #define wmds wms->desk_list[i]
    #define mn wms->monitors[i]
    printf("\n============DEBUG-INFO============\n");
    printf("%lld monitors:\n", wms->monitor_count);
    WINDOWINFO pwi = {
        .cbSize = sizeof(WINDOWINFO)
    };
    for(int i = 0; i < wms->monitor_count; i++) {
        printf("               Monitor %d: %dx%d%s - showing desk %d (%lld total)\n", i + 1, mn.w, mn.h, i == 0 ? " (primary)" : "", mn.front_desk + 1, mn.desk_count);
        if(verbose) {
            GetWindowInfo(mn.child_hwnd, &pwi);
            print_windowinfo(&pwi, "               ");
        }
    }
    printf("\n");
    for(int i = 0; i < DESK_COUNT; i++) {
        printf("DESK %d (monitor %d): %lld wnds (%lld tiling) - %s%s\n", i + 1, wmds.on_monitor + 1, wmds.wnd_count, wmds.tiling_count, WMM_NAMES[wmds.mode], i == wms->curr_desk ? " ACTIVE" : "");
        if(verbose) {
            GetWindowInfo(wmds.button.hwnd, &pwi);
            print_windowinfo(&pwi, "               ");
        }
        print_windows_list(&wmds.wnd_list, wmds.wnd_count, verbose);
    }
    printf("==========END-DEBUG-INFO==========\n\n");
    #undef wmds
    #undef mn
}

int append_new_wnd(WIMAN_WINDOW **wndl, size_t *len, WIMAN_WINDOW w) {
    (*len)++;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, sizeof(WIMAN_WINDOW) * *len);
    if(new_wndl == NULL) return 1;
    *wndl = new_wndl;
    (*wndl)[(*len) - 1] = w;
    return 0;
}

int insert_new_wnd(WIMAN_WINDOW **wndl, size_t *len, WIMAN_WINDOW w, int after) {
    printf("Inserting after %d\n", after);
    (*len)++;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, sizeof(WIMAN_WINDOW) * *len);
    if(new_wndl == NULL) return 1;
    *wndl = new_wndl;
    if(after < *len - 1) memmove_s(*wndl + after + 2, (*len - after - 2) * sizeof(WIMAN_WINDOW), *wndl + after + 1, (*len - after - 2) * sizeof(WIMAN_WINDOW));
    (*wndl)[after + 1] = w;
    return 0;
}

int remove_wnd_by_idx(WIMAN_WINDOW **wndl, size_t *len, int idx) {
    if(*len == 1) {
        free(*wndl);
        *wndl = NULL;
        *len = 0;
        return 0;
    }
    if(idx < *len - 1) memmove_s(*wndl + idx, (*len - idx) * sizeof(WIMAN_WINDOW), *wndl + idx + 1, (*len - idx - 1) * sizeof(WIMAN_WINDOW));
    (*len)--;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, *len * sizeof(WIMAN_WINDOW));
    if(new_wndl == NULL) return 1;
    *wndl = new_wndl;
    return 0;
}

// TODO tracking only one of the events doesn't work
void track_mouse_for(HWND hwnd, int what_to_track) {
    TRACKMOUSEEVENT te = {.cbSize = sizeof(TRACKMOUSEEVENT),.dwFlags = what_to_track,.hwndTrack = hwnd,.dwHoverTime = 10};
    TrackMouseEvent(&te);
    return;
}

int position_window(HWND hwnd, WINDOWPLACEMENT *wp, RECT *wr) {
    if(!SetWindowPlacement(hwnd, wp)) return 1;
    GetWindowRect(hwnd, wr);
    if(wr->bottom > wp->rcNormalPosition.bottom || wr->right > wp->rcNormalPosition.right) {
        return !SetWindowPos(hwnd, NULL, wp->rcNormalPosition.left, wp->rcNormalPosition.top, wp->rcNormalPosition.right - wp->rcNormalPosition.left, wp->rcNormalPosition.bottom - wp->rcNormalPosition.top, SWP_DEFERERASE | SWP_NOSENDCHANGING | SWP_NOOWNERZORDER);
        printf("Window: %ld, %ld, %ld, %ld\n", wr->left, wr->top, wr->right, wr->bottom);
    }
    return 0;
}

void switch_wnds(WIMAN_WINDOW **wndl, int first, int second) {
    if(first == second) return;
    WIMAN_WINDOW buffer = (*wndl)[first];
    (*wndl)[first] = (*wndl)[second];
    (*wndl)[second] = buffer;
    printf("Switched windows %d and %d\n", first + 1, second + 1);
}

void switch_hwnds_pos(WIMAN_WINDOW **wndl, int first, int second, WINDOWPLACEMENT *wp, RECT *wr) {
    HWND hwnd = (*wndl)[first].hwnd;
    (*wndl)[first].hwnd = (*wndl)[second].hwnd;
    (*wndl)[second].hwnd = hwnd;
    wp->rcNormalPosition = (*wndl)[first].last_set_pos;
    position_window((*wndl)[first].hwnd, wp, wr);
    wp->rcNormalPosition = (*wndl)[second].last_set_pos;
    position_window((*wndl)[second].hwnd, wp, wr);
    printf("Switched HWNDs of windows %d and %d and repositioned them\n", first + 1, second + 1);
}

int get_wnd_id_by_hwnd(WIMAN_DESKTOP_STATE *wmds, HWND hwnd) {
    for(int i = 0; i < wmds->wnd_count; i++) {
        if(wmds->wnd_list[i].hwnd == hwnd) return i;
    }
    return -1;
}

int get_monitor_id_by_hmonitor(WIMAN_STATE *wms, HMONITOR hmonitor) {
    for(int i = 0; i < wms->monitor_count; i++) {
        if(wms->monitors[i].hmonitor == hmonitor) return i;
    }
    return -1;
}

int get_monitor_id_by_cursor(WIMAN_STATE *wms, POINT curr) {
    for(int i = 0; i < wms->monitor_count; i++) if(is_point_in_rect(curr, wms->monitors[i].pos)) return i;
    return -1;
}

int is_actual_window(HWND hwnd) {
    if(GetWindowTextLengthA(hwnd) == 0 || !IsWindowVisible(hwnd)) return FALSE;
    long dxExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    return !is_param_in_xor(dxExStyle, WS_EX_NOREDIRECTIONBITMAP) && !is_toolwindow(dxExStyle);
}

HWND create_window(HINSTANCE *hInstance, HWND hwnd_parent, int offset_left) {
    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        WINDOW_CLASSNAME,
        "2wiman",
        0,
        offset_left, 0, DESK_TILE_SIZE.x * DESK_COUNT, DESK_TILE_SIZE.y,
        hwnd_parent,
        NULL,
        *hInstance,
        NULL
    );
    SetWindowLongPtrA(hwnd, GWL_STYLE, (hwnd_parent != NULL) << 30 & WS_CHILDWINDOW);
    return hwnd;
}

HWND create_button(HINSTANCE *hInstance, HWND hwnd_parent, int offset_left, int desk) {
    if(hwnd_parent == NULL) return NULL;
    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        BUTTON_CLASSNAME,
        "2wmctrl",
        0,
        offset_left, 0, DESK_TILE_SIZE.x, DESK_TILE_SIZE.y,
        hwnd_parent,
        NULL,
        *hInstance,
        NULL
    );
    SetWindowLongPtrA(hwnd, GWL_STYLE, WS_CHILD | BS_OWNERDRAW);
    SetWindowLongPtrA(hwnd, -21, desk);
    track_mouse_for(hwnd, ONLY_LEAVE);
    return hwnd;
}

BOOL enum_monitors(HMONITOR hmonitor, HDC hdc, LPRECT lpRect, LPARAM lParam) {
    #define wms ((WIMAN_STATE*)lParam)
    wms->monitor_count++;
    WIN_MONITOR *monitors = realloc(wms->monitors, sizeof(WIN_MONITOR) * wms->monitor_count);
    if(monitors == NULL) return FALSE;
    wms->monitors = monitors;
    MONITORINFO lpmi = { .cbSize = sizeof(MONITORINFO) };
    GetMonitorInfoA(hmonitor, &lpmi);
    WIN_MONITOR wm = { .hmonitor = hmonitor, .front_desk = wms->monitor_count - 1, .desk_count = 1 };
    wm.h = lpmi.rcWork.bottom - lpmi.rcWork.top;
    wm.w = lpmi.rcWork.right - lpmi.rcWork.left;
    wm.pos = lpmi.rcWork;
    wms->desk_list[wms->monitor_count - 1].on_monitor = wms->monitor_count - 1;
    HWND button_hwnd = create_button(wms->h_instance, wms->main_hwnd, 0, wms->monitor_count - 1);
    wms->desk_list[wms->monitor_count - 1].button.hwnd = button_hwnd;
    wms->desk_list[wms->monitor_count - 1].button.is_tracking = TRUE;
    SetWindowPos(button_hwnd, 0, lpmi.rcWork.left, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(button_hwnd, SW_SHOW);
    if(is_param_in_xor(lpmi.dwFlags, MONITORINFOF_PRIMARY)) {
        printf("Found primary monitor (%lld)\n", wms->monitor_count);
        wms->monitors[wms->monitor_count - 1] = wms->monitors[0];
        wms->monitors[wms->monitor_count - 1].front_desk = wms->monitor_count - 1;
        wm.front_desk = 0;
        wms->monitors[0] = wm;
    } else {
        wms->monitors[wms->monitor_count - 1] = wm;
    }
    printf("Monitor %lld: W = %d, H = %d\n", wms->monitor_count, wm.w, wm.h);
    #undef wms
}

BOOL CALLBACK enum_wnd(HWND hwnd, LPARAM lParam) {
    #define wms ((WIMAN_STATE*)lParam)
    #define wmds wms->desk_list[0]
    if(!is_actual_window(hwnd)) return TRUE;
    DWORD dxStyle = GetWindowLongPtrA(hwnd, GWL_STYLE);
    int is_resizable = is_resizable(dxStyle);
    WIMAN_WINDOW new_wnd = { .is_unresizable = !is_resizable, .is_freeroam = !is_resizable, .hwnd = hwnd };
    if(!is_resizable) order_wnd_z(hwnd, HWND_TOPMOST);
    if(!is_resizable) return !append_new_wnd(&wmds.wnd_list, &wmds.wnd_count, new_wnd);
    return !insert_new_wnd(
        &wmds.wnd_list, &wmds.wnd_count, new_wnd,
        wmds.tiling_count++ - 1
        // wmds.tiling_count + (wmds.wnd_count - wmds.tiling_count) * !is_resizable - 1
    );
    #undef wmds
    #undef wms
}

int move_wnd_to_desk(WIMAN_DESKTOP_STATE *desk_from, int wnd, WIMAN_DESKTOP_STATE *desk_to) {
    if(desk_from->on_monitor == desk_to->on_monitor) ShowWindow(desk_from->wnd_list[wnd].hwnd, SW_HIDE);
    insert_new_wnd(&desk_to->wnd_list, &desk_to->wnd_count, desk_from->wnd_list[wnd], desk_to->tiling_count - 1);
    desk_to->tiling_count += !desk_from->wnd_list[wnd].is_freeroam;
    desk_from->tiling_count -= !desk_from->wnd_list[wnd].is_freeroam;
    remove_wnd_by_idx(&desk_from->wnd_list, &desk_from->wnd_count, wnd);
    printf("  Moved window %d to another desktop\n", wnd + 1);
    return desk_to->tiling_count - 1;
}

// Snippet of code that uses inbuilt tiling system to tile windows. I still haven't figured out how to make it tile more than 2 on screen
    // int window_w = state->monitor_size.w / state->windows_list.len;
    // RECT window_pos = {0, 0, window_w, state->monitor_size.h - 20};
    // for(int i = 0; i < state->windows_list.len; i++) {
    //     TileWindows(NULL, MDITILE_VERTICAL, &window_pos, 1, &(state->windows_list.list[i].hwnd));
    //     window_pos.left += window_w - 1;
    //     window_pos.right += window_w - 1;
    // }
    // return 0;


int toggle_wnd_freeroam(WIMAN_DESKTOP_STATE *wmds, int idx, WIN_MONITOR *wm) {
    #define wnd wmds->wnd_list[idx]
    if(wnd.is_unresizable) return 2;
    int offset = (wmds->wnd_count - wmds->tiling_count) * 32;
    if(!SetWindowPos(wnd.hwnd, HWND_TOPMOST - wnd.is_freeroam, wm->w / 6 + offset, wm->h / 6 + offset, wm->w / 3 * 2, wm->h / 3 * 2, wnd.is_freeroam * (SWP_NOMOVE | SWP_NOSIZE))) return 1;
    wmds->tiling_count += !wnd.is_freeroam * -2 + 1;
    wnd.is_freeroam = !wnd.is_freeroam;
    int new_id = wmds->tiling_count + !wnd.is_freeroam;
    switch_wnds(&wmds->wnd_list, idx, new_id);
    wmds->curr_wnd = new_id;
    printf("Toggled window %d freeroam %s\n", idx + 1, wnd.is_freeroam ? "ON" : "OFF");
    #undef wnd
    // SetActiveWindow(curr_wnd.hwnd);
    return 0;
}

int tile_windows_vert(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    printf("Tiling windows (%lld in total) vertically\n", wmds->tiling_count);
    int window_w = wm->w / wmds->tiling_count;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){wm->pos.left, 0, window_w + wm->pos.left, wm->h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp;
    for(int i = 0; i < wmds->tiling_count; i++) {
        printf("  Setting window %d position\n", i);
        HWND curr_hwnd = wmds->wnd_list[i].hwnd;
        if(!SetWindowPlacement(curr_hwnd, &p)) return 1;
        GetWindowRect(curr_hwnd, &wp);
        if(wp.right > p.rcNormalPosition.right) {
            if(!SetWindowPos(curr_hwnd, HWND_BOTTOM, p.rcNormalPosition.left, p.rcNormalPosition.top, window_w, wm->h, SWP_DEFERERASE | SWP_NOSENDCHANGING)) return 1;
        }
        wmds->wnd_list[i].last_set_pos = p.rcNormalPosition;
        p.rcNormalPosition.left += window_w;
        p.rcNormalPosition.right += window_w;
    }
    return 0;
}

int tile_windows_horiz(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    int window_h = wm->h / wmds->tiling_count;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){wm->pos.left, 0, wm->w + wm->pos.left, window_h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp = {};
    for(int i = 0; i < wmds->tiling_count; i++) {
        HWND curr_hwnd = wmds->wnd_list[i].hwnd;
        if(!SetWindowPlacement(curr_hwnd, &p)) return 1;
        GetWindowRect(curr_hwnd, &wp);
        if(wp.bottom > window_h * (i + 1)) {
            if(!SetWindowPos(curr_hwnd, HWND_BOTTOM, p.rcNormalPosition.left, p.rcNormalPosition.top, wm->w, window_h, SWP_DEFERERASE | SWP_NOSENDCHANGING)) return 1;
        }
        wmds->wnd_list[i].last_set_pos = p.rcNormalPosition;
        // TODO i want the same border that window snapping uses around windows
        // long style = GetWindowLongPtrA(curr_hwnd, GWL_STYLE);
        // SetWindowLongPtrA(curr_hwnd, GWL_STYLE, style ^ !WS_SYSMENU);
        p.rcNormalPosition.top += window_h;
        p.rcNormalPosition.bottom += window_h;
    }
    printf("Tiled windows horizontally\n");
    return 0;
}

// Used when in stack mode by refocus_window().
int set_window_ontop(HWND curr) {
    order_wnd_z(curr, HWND_TOPMOST);
    order_wnd_z(curr, HWND_NOTOPMOST);
    SetForegroundWindow(curr);
    return 0;
}

// Basically switches to WMM_STACK
int reset_all_to_fullsize(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    RECT p = {wm->pos.left, 0, wm->w + wm->pos.left, wm->h};
    for(int i = 0; i < wmds->tiling_count; i++) {
        if(!SetWindowPos(wmds->wnd_list[i].hwnd, HWND_NOTOPMOST, p.left, p.top, p.right, p.bottom, 0)) return 1;
        wmds->wnd_list[i].last_set_pos = p;
    }
    printf("Made all windows fullsize\n");
    return 0;
}

int focus_act_window(WIMAN_DESKTOP_STATE *wmds, int prev_act_idx) {
    switch(wmds->mode) {
        case WMM_STACK: {
            return set_window_ontop(get_curr_hwnd(*wmds));
        }
        default:
            return 0;
    }
    return 0;
}

// Repositions all windows based on the current mode in state.
int init_curr_mode_reposition(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    printf("Initializing %s mode on desktop\n", WMM_NAMES[wmds->mode]);
    if(wmds->tiling_count == 0) return 0;
    switch(wmds->mode) {
        case WMM_STACK: {
            if(reset_all_to_fullsize(wmds, wm)) return 1;
            return set_window_ontop(get_curr_hwnd(*wmds));
        }
        case WMM_TILING_V: {
            return tile_windows_vert(wmds, wm);
        }
        case WMM_TILING_H: {
            return tile_windows_horiz(wmds, wm);
        }
        default:
            return 0;
    }
}

void reposition_buttons(WIMAN_STATE *wms) {
    printf("Repositioning buttons:\n");
    long *offsets = calloc(wms->monitor_count, sizeof(int));
    for(int i = 0; i < DESK_COUNT; i++) {
        ShowWindow(wms->desk_list[i].button.hwnd, SW_HIDE + (wms->desk_list[i].on_monitor != -1) * 5);
        if(wms->desk_list[i].on_monitor == -1) {
            printf("  Skipping and hiding desktop button %d (it isn't active)\n", i);
            continue;
        }
        int offset = offsets[wms->desk_list[i].on_monitor];
        if(offset == wms->desk_list[i].button.last_set_left_top.x && wms->monitors[wms->desk_list[i].on_monitor].pos.top == wms->desk_list[i].button.last_set_left_top.y) {
            printf("  Skipping desktop button %d (position didn't change)\n", i);
            offsets[wms->desk_list[i].on_monitor] += DESK_TILE_SIZE.x;
            continue;
        }
        printf("  Setting desktop button %d position\n", i);
        SetWindowPos(wms->desk_list[i].button.hwnd, 0, offset, wms->monitors[wms->desk_list[i].on_monitor].pos.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        wms->desk_list[i].button.last_set_left_top.x = offset;
        wms->desk_list[i].button.last_set_left_top.y = wms->monitors[wms->desk_list[i].on_monitor].pos.top;
        offsets[wms->desk_list[i].on_monitor] += DESK_TILE_SIZE.x;
        // InvalidateRect(wms->desk_list[i].button.hwnd, 0, TRUE);
    }
    free(offsets);
    return;
}

int switch_desc_to(WIMAN_STATE *wms, int target) {
    printf("Switching desktop to %d\n", target);
    if(target == wms->curr_desk) return wms->curr_desk;
    WIMAN_DESKTOP_STATE *curr_desk = &wms->desk_list[wms->curr_desk];
    WIMAN_DESKTOP_STATE *target_desk = &wms->desk_list[target];
    int is_desk_create = !target_desk->wnd_count && (target_desk->on_monitor == -1 || wms->monitors[target_desk->on_monitor].desk_count > 1);
    if(is_desk_create) {
        printf("  Target desktop to be \"created\"\n");
        target_desk->on_monitor = curr_desk->on_monitor;
        wms->monitors[curr_desk->on_monitor].desk_count++;
    }
    int is_same_monitor = curr_desk->on_monitor == target_desk->on_monitor;
    is_same_monitor ? printf("  Monitor is the same (%d)\n", curr_desk->on_monitor) : printf("  Monitors are different (%d and %d)\n", curr_desk->on_monitor, target_desk->on_monitor);
    int is_desk_destroy = !curr_desk->wnd_count && wms->monitors[curr_desk->on_monitor].desk_count > 1;
    if(is_desk_destroy) {
        printf("  Previous current desktop to be \"destroyed\"\n");
    }
    int on_monitor = curr_desk->on_monitor;
    curr_desk->on_monitor = ((curr_desk->on_monitor + 1) * !is_desk_destroy) - 1;
    wms->monitors[on_monitor].desk_count -= is_desk_destroy;
    InvalidateRect(curr_desk->button.hwnd, 0, TRUE);
    // reposition_buttons_for_monitor(wms, wms->curr_desk);
    int i;
    if(is_same_monitor || target_desk->on_monitor == -1) for(i = 0; i < curr_desk->wnd_count; i++) ShowWindow(curr_desk->wnd_list[i].hwnd, SW_HIDE);
    wms->curr_desk = target;
    curr_desk = &wms->desk_list[wms->curr_desk];
    InvalidateRect(curr_desk->button.hwnd, 0, TRUE);
    // reposition_buttons_for_monitor(wms, wms->curr_desk);
    if(curr_desk->changed || !is_same_monitor) {
        init_curr_mode_reposition(curr_desk, &wms->monitors[curr_desk->on_monitor]);
        curr_desk->changed = FALSE;
    }
    wms->monitors[target_desk->on_monitor].front_desk = target;
    if(is_same_monitor) for(i = 0; i < curr_desk->wnd_count; i++) ShowWindow(curr_desk->wnd_list[i].hwnd, SW_SHOW);
    printf("  Switched to desktop %d\n", target + 1);
    if(is_desk_create || is_desk_destroy) reposition_buttons(wms);
    return 0;
}

int send_desk_to_monitor(WIMAN_STATE *wms, int desk, int monitor) {
    printf("Sending desktop %d to monitor %d\n", desk, monitor);
    WIMAN_DESKTOP_STATE *desk_s = &wms->desk_list[desk];
    if(wms->monitors[desk_s->on_monitor].desk_count <= 1 || monitor == wms->desk_list[desk].on_monitor || monitor >= wms->monitor_count) {
        printf("  Cannot send desktop: invalid conditions\n");
        return 2;
    }
    int prev_monitor = desk_s->on_monitor;
    desk_s->on_monitor = monitor;
    wms->monitors[prev_monitor].front_desk = -1;
    wms->monitors[monitor].front_desk = desk;
    init_curr_mode_reposition(desk_s, &wms->monitors[desk_s->on_monitor]);
    reposition_buttons(wms);
    return 0;
}

int send_wnd_to_desk(WIMAN_STATE *wms, int wnd, int from, int desk) {
    printf("Moving window from desktop %d to %d:\n", from, desk);
    if(desk == from) {
        printf("  Desktops are the same, aborting\n");
        return 2;
    }
    WIMAN_DESKTOP_STATE *target_desk = &wms->desk_list[desk];
    WIMAN_DESKTOP_STATE *desk_from = &wms->desk_list[from];
    int is_desk_create = !target_desk->wnd_count && (target_desk->on_monitor == -1 || wms->monitors[target_desk->on_monitor].desk_count > 1);
    int is_same_monitor = desk_from->on_monitor == target_desk->on_monitor;
    is_same_monitor ? printf("  Monitor is the same (%d)\n", desk_from->on_monitor) : printf("  Monitors are different (%d and %d)\n", desk_from->on_monitor, target_desk->on_monitor);
    if(is_desk_create) {
        printf("  Target desktop to be \"created\"\n");
        target_desk->on_monitor = desk_from->on_monitor;
        wms->monitors[desk_from->on_monitor].desk_count++;
    }
    int new_wnd_idx = move_wnd_to_desk(desk_from, desk_from->curr_wnd, target_desk);
    printf("  Moved window, new idx is %d\n", new_wnd_idx);
    target_desk->curr_wnd = new_wnd_idx;
    // desk_from->curr_wnd--;
    int is_desk_destroy = !desk_from->wnd_count && wms->monitors[desk_from->on_monitor].desk_count > 1;
    // TODO optimize
    if(is_desk_destroy) {
        printf("  Desktop from which window was moved to be \"destroyed\"\n");
        switch_desc_to(wms, desk);
        return 0;
    }
    if(!target_desk->wnd_list[new_wnd_idx].is_freeroam) init_curr_mode_reposition(desk_from, &wms->monitors[desk_from->on_monitor]);
    target_desk->changed = is_same_monitor;
    if(!is_same_monitor) init_curr_mode_reposition(target_desk, &wms->monitors[target_desk->on_monitor]);
    reposition_buttons(wms);
    return 0;
}

// TODO add plus button on every monitor
// TODO fetch hidden windows too
// (scuffed fix has been added) TODO can't catch when a single window is closing
// TODO .IDEA handle movesizestart event and assign window id to dragging_this and resizing_this flags in state
// TODO somehow handle monitor changed event, ideally on cursor switch to it.
// TODO FIX sometimes app crashes on window open
// TODO FIX SOME (NOT ALL) fullscreen apps (games) doesn't allow to capture key input and doesn't hide/show properly
// (works, needs further testing) TODO .IDEA if cannot set the position make it freeroam and add some flag like is_permanenty_freeroam (is_unresizable)
// TODO FIX spotify rerenders ui on every reposition
// (currently and probably at all undoable) TODO FIX weird margins around some windows
// TODO .IDEA handle WM_DPICHANGED
// TODO .IDEA use deferwindowpos and enddeferwindowpos for tiling windows
// TODO FIX sometimes weird things happen when opening a window or switching windows while there are freeroam ones
// TODO closing windows shortcut
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    // MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY)
    wms.h_instance = &hInstance;
    wms.dpi = GetDpiForSystem();
    printf("DPI is %d\n", wms.dpi);

    WNDCLASS main_wc = { .lpfnWndProc = MainWindowProc, .hInstance = hInstance, .lpszClassName = WINDOW_CLASSNAME };
    RegisterClass(&main_wc);
    wms.main_hwnd = CreateWindowExA(0, WINDOW_CLASSNAME, "mywindow", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    WNDCLASS button_wc = { .lpfnWndProc = ButtonWindowProc, .hInstance = hInstance, .lpszClassName = BUTTON_CLASSNAME, .cbWndExtra = sizeof(int) };
    RegisterClass(&button_wc);

    HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_proc, NULL, 0);
    HWINEVENTHOOK msg_hook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_OBJECT_STATECHANGE, NULL, wnd_msg_proc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    RegisterShellHookWindow(wms.main_hwnd);

    int icon_uid = 1;
    NOTIFYICONDATAA icon_data = {
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = wms.main_hwnd,
        .uID = icon_uid,
        .uFlags = 0,
    };
    Shell_NotifyIconA(NIM_ADD, &icon_data);

    wms.curr_desk = 0;
    g_curr_desk = (WIMAN_DESKTOP_STATE){ .curr_wnd = -1 };
    EnumWindows(enum_wnd, (LPARAM)&wms);
    printf("Fetched %lld windows, %lld tiling\n", g_curr_desk.wnd_count, g_curr_desk.tiling_count);
    if(g_curr_desk.wnd_count > 0) g_curr_desk.curr_wnd = 0;

    wms.monitors = calloc(0, sizeof(WIN_MONITOR));
    EnumDisplayMonitors(NULL, NULL, enum_monitors, (LPARAM)&wms);
    printf("Fetched %lld monitors\n", wms.monitor_count);
    for(int i = wms.monitor_count; i < DESK_COUNT; i++) {
        wms.desk_list[i].on_monitor = -1;
        wms.desk_list[i].button.is_tracking = TRUE;
        wms.desk_list[i].button.hwnd = create_button(&hInstance, wms.main_hwnd, 0, i);
    }

    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(TEXT("SHELLHOOK"));

    init_curr_mode_reposition(&wms.desk_list[wms.curr_desk], &wms.monitors[wms.desk_list[wms.curr_desk].on_monitor]);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    int prev_act_idx = g_curr_desk.curr_wnd;
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION && is_pressed(MODIFIER_KEY))
    {
        switch (key->vkCode)
        {
        case VK_LEFT:
        case VK_UP:
            if(g_curr_desk.curr_wnd <= 0 && g_curr_desk.tiling_count >= 0) {
                if(!wiman_config.stack_mode_infinite_scroll) break;
                g_curr_desk.curr_wnd = g_curr_desk.tiling_count;
            }
            g_curr_desk.curr_wnd--;
            goto switch_wnd;
            break;
        case VK_RIGHT:
        case VK_DOWN:
            if(g_curr_desk.curr_wnd >= g_curr_desk.tiling_count - 1) {
                if(!wiman_config.stack_mode_infinite_scroll) break;
                g_curr_desk.curr_wnd = -1;
            }
            g_curr_desk.curr_wnd++;
            switch_wnd:
            WINDOWPLACEMENT wp = {
                .showCmd = SC_RESTORE,
                .length = sizeof(WINDOWPLACEMENT)
            };
            RECT wr;
            if(is_pressed(VK_SHIFT) && g_curr_desk.mode != WMM_STACK) {
                switch_hwnds_pos(&g_curr_desk.wnd_list, prev_act_idx, g_curr_desk.curr_wnd, &wp, &wr);
                focus_act_window(&g_curr_desk, g_curr_desk.curr_wnd);
                break;
            }
            focus_act_window(&g_curr_desk, prev_act_idx);
            break;
        case 'W':
            if(g_curr_desk.mode == WMM_STACK) break;
            g_curr_desk.mode = WMM_STACK;
            init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            break;
        case 'E':
            g_curr_desk.mode = (g_curr_desk.mode == WMM_TILING_V) + 1;
            // if(g_curr_desk.mode == WMM_TILING_V) {
            //     g_curr_desk.mode = WMM_TILING_H;
            // } else {
            //     g_curr_desk.mode = WMM_TILING_V;
            // }
            init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            break;
        case VK_SPACE:
            if(toggle_wnd_freeroam(&g_curr_desk, g_curr_desk.curr_wnd, &wms.monitors[g_curr_desk.on_monitor]) == 2) break;
            init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            break;
        case 'R':
            free(g_curr_desk.wnd_list);
            g_curr_desk.wnd_list = calloc(0, sizeof(WIMAN_WINDOW));
            // g_curr_desk.wnd_count = 0;
            EnumWindows(enum_wnd, (LPARAM)&wms);
            g_curr_desk.tiling_count = g_curr_desk.wnd_count;
            init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            break;
        case 'D':
            print_debug_info(&wms, FALSE);
            break;
        case 49 ... 57:
            int target = key->vkCode - 49;
            WIMAN_DESKTOP_STATE *target_desk = &wms.desk_list[target];
            if(is_pressed(VK_SHIFT)) {
                send_wnd_to_desk(&wms, g_curr_desk.curr_wnd, wms.curr_desk, target);
                break;
            }
            if(is_pressed(VK_CONTROL)) {
                send_desk_to_monitor(&wms, wms.curr_desk, target);
                break;
            }
            switch_desc_to(&wms, target);
            break;
        }
        return 2;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK ButtonWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            int desk = GetWindowLongPtrA(hwnd, -21);
            printf("GUI: pressed button %d\n", desk);
            wms.desk_list[desk].button.is_hovered = FALSE;
            switch_desc_to(&wms, desk);
            break;
        }
        // for moving window from any place in its client area. TODO figure out how this works
        // case WM_NCHITTEST: {
        //     LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
        //     if (hit == HTCLIENT) hit = HTCAPTION;
        //     return hit;
        // }
        case WM_MOUSEHOVER: {
            int desk = GetWindowLongPtrA(hwnd, -21);
            printf("GUI: hovered button %d\n", desk);
            if(wms.desk_list[desk].button.is_hovered) return 0;
            wms.desk_list[desk].button.is_hovered = TRUE;
            wms.desk_list[desk].button.is_tracking = FALSE;
            InvalidateRect(hwnd, 0, TRUE);
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            RECT r;
            GetWindowRect(hwnd, &r);
            WIMAN_DESKTOP_STATE *desk = &wms.desk_list[GetWindowLongPtrA(hwnd, -21)];
            POINT p;
            GetCursorPos(&p);
            for(int i = 0; i < wms.monitor_count; i++) {
                if(i == desk->on_monitor) {
                    set_def_pos:
                    SetWindowPos(hwnd, 0, desk->button.last_set_left_top.x, desk->button.last_set_left_top.y, 0, 0,  SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);
                    return 0;
                }
                if(is_point_in_rect(p, r)) {
                    if(send_desk_to_monitor(&wms, wms.curr_desk, i) == 2) goto set_def_pos;
                    return 0;
                }
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            int desk = GetWindowLongPtrA(hwnd, -21);
            printf("GUI: mouse left button %d\n", desk);
            wms.desk_list[desk].button.is_hovered = FALSE;
            wms.desk_list[desk].button.is_tracking = FALSE;
            InvalidateRect(hwnd, 0, TRUE);
            return 0;
        }
        case WM_MOUSEMOVE: {
            int desk = GetWindowLongPtrA(hwnd, -21);
            if(wms.desk_list[desk].button.is_tracking) return 0;
            track_mouse_for(hwnd, HOVER_AND_LEAVE);
            wms.desk_list[desk].button.is_tracking = TRUE;
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CLOSE:
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            int desk = GetWindowLongPtrA(hwnd, -21);
            HDC hdc = BeginPaint(hwnd, &ps);
            printf("GUI: repainting button %d\n", desk);
            WIN_MONITOR *target_monitor = &wms.monitors[wms.desk_list[desk].on_monitor];
            int is_desk_last = desk >= target_monitor->desk_count - 1;
            int is_active = desk == wms.curr_desk;
            int is_hovered = wms.desk_list[desk].button.is_hovered && !is_active;
            char buffer[2];
            RECT pos_r = { ps.rcPaint.left, 0, ps.rcPaint.left + DESK_TILE_SIZE.x, 0 + DESK_TILE_SIZE.y };
            RECT inner_tile = { pos_r.left + TILE_OFFSET, pos_r.top + TILE_OFFSET, pos_r.right - TILE_OFFSET, pos_r.bottom - TILE_OFFSET };
            FillRect(hdc, &pos_r, (HBRUSH)(CreateSolidBrush(BG_COLOR * !is_hovered + DESK_BG_COLORS[TRUE] * is_hovered)));
            FillRect(hdc, &inner_tile, (HBRUSH)(CreateSolidBrush(DESK_BG_COLORS[is_active])));
            FrameRect(hdc, &pos_r, (HBRUSH)(CreateSolidBrush(DESK_COLORS[is_active] * !is_hovered + DESK_BG_COLORS[TRUE] * is_hovered)));
            sprintf(buffer, "%d", desk + 1);
            SetBkMode(hdc, TRANSPARENT);
            // SetTextColor(hdc, ~DESK_BG_COLORS[is_active] - 0xff000000);
            DrawTextA(hdc, buffer, -1, &pos_r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if(wms.desk_list[desk].wnd_count > 0) {
                sprintf(buffer, "%lld", wms.desk_list[desk].wnd_count);
                SetBkMode(hdc, OPAQUE);
                DrawTextA(hdc, buffer, -1, &inner_tile, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
            }
            EndPaint(hwnd, &ps);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_SHELLHOOKMESSAGE) {
        #define chwnd (HWND)lParam
        // if(wParam == HSHELL_MONITORCHANGED) {
        //     printf("Monitor has changed (event)\n");
        // }
        switch(wParam) {
            case HSHELL_WINDOWACTIVATED: {
                long curr_wnd_style = GetWindowLongPtrA(get_curr_hwnd(g_curr_desk), GWL_STYLE);
                // if(!is_param_in_xor(curr_wnd_style, WS_VISIBLE) || (is_param_in_xor(curr_wnd_style, WS_MINIMIZE) && !g_curr_desk.wnd_list[g_curr_desk.curr_wnd].is_freeroam)) {
                //     printf("Assuming that window %d is closing...\n", g_curr_desk.curr_wnd);
                //     g_curr_desk.tiling_count -= !g_curr_desk.wnd_list[g_curr_desk.curr_wnd].is_freeroam;
                //     remove_wnd_by_idx(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, g_curr_desk.curr_wnd);
                //     init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                // }
                if(!is_actual_window(chwnd)) return DefWindowProc(hwnd, uMsg, wParam, lParam);
                int monitor_id = get_monitor_id_by_hmonitor(&wms, MonitorFromWindow(chwnd, MONITOR_DEFAULTTONEAREST));
                if(monitor_id != g_curr_desk.on_monitor) {
                    printf("Monitor has changed\n");
                }
                wms.curr_desk = wms.monitors[monitor_id].front_desk;
                int wnd_id = get_wnd_id_by_hwnd(&g_curr_desk, chwnd);
                if(wnd_id == -1) goto add_window;
                printf("Window %d activated\n", wnd_id);
                g_curr_desk.curr_wnd = wnd_id;
                break;
            }
            case HSHELL_WINDOWCREATED: {
                if(!is_actual_window(chwnd)) return DefWindowProc(hwnd, uMsg, wParam, lParam);
                // int monitor_id = get_monitor_id_by_hmonitor(&wms, MonitorFromWindow(chwnd, MONITOR_DEFAULTTONEAREST));
                // if(monitor_id != g_curr_desk.on_monitor) {
                //     printf("Monitor has changed\n");
                // }
                // wms.curr_desk = wms.monitors[monitor_id].front_desk;
                int wnd_id = get_wnd_id_by_hwnd(&g_curr_desk, chwnd);
                if(wnd_id != -1) break;
                add_window:
                printf("New window is opened\n");
                long wnd_style = GetWindowLongPtrA(chwnd, GWL_STYLE);
                if(!is_resizable(wnd_style)) {
                    order_wnd_z(chwnd, HWND_TOPMOST);
                    append_new_wnd(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, (WIMAN_WINDOW){ .hwnd = chwnd, .is_unresizable = TRUE, .is_freeroam = TRUE });
                    break;
                }
                insert_new_wnd(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, (WIMAN_WINDOW){ .hwnd = chwnd }, g_curr_desk.tiling_count++ - 1);
                // print_windows_list(&wiman_state.windows_list, " - ");
                switch(g_curr_desk.mode) {
                    case WMM_STACK:
                        SetWindowPos(chwnd, HWND_TOPMOST, 0, 0, wms.monitors[g_curr_desk.on_monitor].w, wms.monitors[g_curr_desk.on_monitor].h, 0);
                        order_wnd_z(chwnd, HWND_NOTOPMOST);
                        SetForegroundWindow(chwnd);
                        break;
                    case WMM_TILING_H:
                    case WMM_TILING_V:
                        order_wnd_z(chwnd, HWND_NOTOPMOST);
                        init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                    default:
                        break;
                }
            }
        }
        #undef hwnd
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            free(g_curr_desk.wnd_list);
            break;
        case WM_CLOSE:
            free(g_curr_desk.wnd_list);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void wnd_msg_proc(
  HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG idObject,
  LONG idChild,
  DWORD idEventThread,
  DWORD dwmsEventTime
) {
    if(!is_actual_window(hwnd)) return;
    int wnd_id = get_wnd_id_by_hwnd(&g_curr_desk, hwnd);
    if(wnd_id == -1) return;
    switch(event) {
        case EVENT_SYSTEM_MOVESIZESTART: {
            printf("Movesize start\n");
            break;
        }
        case EVENT_SYSTEM_MOVESIZEEND:
            #define w g_curr_desk.wnd_list[wnd_id]
            printf("Window %d is moved or resized: ", wnd_id);
            WINDOWPLACEMENT wp = {
                .length = sizeof(WINDOWPLACEMENT),
                .showCmd = SW_RESTORE,
                .rcNormalPosition = w.last_set_pos
            };
            RECT wr;
            GetWindowRect(hwnd, &wr);
            if(!is_wh_equal_rect(w.last_set_pos, wr)) {
                printf("Window is resized\n");
                if(w.is_freeroam) break;
                position_window(hwnd, &wp, &wr);
                break;
            }
            POINT clp;
            GetCursorPos(&clp);
            printf("Window is moved, cursor was on %ld %ld\n", clp.x, clp.y);
            int monitor_id = get_monitor_id_by_cursor(&wms, clp);
            if(monitor_id != g_curr_desk.on_monitor) {
                printf("Window is moved from desktop %d (monitor %d) to desktop %d (monitor %d)\n", wms.curr_desk, wms.desk_list[wms.curr_desk].on_monitor, wms.monitors[monitor_id].front_desk, monitor_id);
                // move_wnd_to_desk(&g_curr_desk, wnd_id, &wms.desk_list[wms.monitors[monitor_id].front_desk]);
                // // if(w.is_freeroam) {
                // //     wms.curr_desk = wms.monitors[monitor_id].front_desk;
                // //     break;
                // // }
                // if(g_curr_desk.wnd_count == 0) {
                //     g_curr_desk.on_monitor = 0;
                // }
                // init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                // wms.curr_desk = wms.monitors[monitor_id].front_desk;
                // init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                send_wnd_to_desk(&wms, wnd_id, wms.curr_desk, wms.monitors[monitor_id].front_desk);
                break;
            }
            #undef w
            int new_place;
            switch(g_curr_desk.mode) {
                case WMM_STACK:
                    position_window(hwnd, &wp, &wr);
                    break;
                case WMM_TILING_V:
                    new_place = g_curr_desk.tiling_count * clp.x / wms.monitors[g_curr_desk.on_monitor].w;
                    goto place;
                case WMM_TILING_H:
                    new_place = g_curr_desk.tiling_count * clp.y / wms.monitors[g_curr_desk.on_monitor].h;
                    place:if(new_place == wnd_id) {
                        position_window(hwnd, &wp, &wr);
                        break;
                    }
                    switch_hwnds_pos(&g_curr_desk.wnd_list, wnd_id, new_place, &wp, &wr);
                    break;
                default:
                    break;
            }
            g_curr_desk.curr_wnd = new_place;
            SetActiveWindow(hwnd);
            break;
    }
}