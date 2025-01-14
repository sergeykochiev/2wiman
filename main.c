#include "windows.h"
#include <windef.h>
#include "C:\Users\dupa\gcc\x86_64-w64-mingw32\include\wingdi.h"
#include <winuser.h>
#include <stdio.h>
#include <wchar.h>
#include <math.h>

static const COLORREF BG_COLOR = 0x00686664;
static const COLORREF DESC_COLOR = 0x00878787;
static const COLORREF DESC_BG_COLOR = 0x000ADAFF;
static const COLORREF ACTIVE_DESC_COLOR = 0x00BC750B;
static const COLORREF ACTIVE_DESC_BG_COLOR = 0x00FFDA0A;
static const COLORREF DESC_COLORS[2] = { [FALSE] = DESC_COLOR, [TRUE] = ACTIVE_DESC_COLOR };
static const COLORREF DESC_BG_COLORS[2] = { [FALSE] = DESC_BG_COLOR, [TRUE] = ACTIVE_DESC_BG_COLOR };
static const POINT DESC_TILE_SIZE = { 50, 50 };
#define DESC_COUNT 9

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
void print_ex_styles(long ex_styles) {
    if(is_param_in_xor(ex_styles, WS_EX_ACCEPTFILES)) printf("WS_EX_ACCEPTFILES, "); if(is_param_in_xor(ex_styles, WS_EX_APPWINDOW)) printf("WS_EX_APPWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_CLIENTEDGE)) printf("WS_EX_CLIENTEDGE, "); if(is_param_in_xor(ex_styles, WS_EX_COMPOSITED)) printf("WS_EX_COMPOSITED, "); if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("WS_EX_CONTEXTHELP, "); if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("WS_EX_CONTEXTHELP, "); if(is_param_in_xor(ex_styles, WS_EX_CONTROLPARENT)) printf("WS_EX_CONTROLPARENT, "); if(is_param_in_xor(ex_styles, WS_EX_DLGMODALFRAME)) printf("WS_EX_DLGMODALFRAME, "); if(is_param_in_xor(ex_styles, WS_EX_LAYERED)) printf("WS_EX_LAYERED, "); if(is_param_in_xor(ex_styles, WS_EX_LAYOUTRTL)) printf("WS_EX_LAYOUTRTL, "); if(is_param_in_xor(ex_styles, WS_EX_LEFT)) printf("WS_EX_LEFT, "); if(is_param_in_xor(ex_styles, WS_EX_LEFTSCROLLBAR)) printf("WS_EX_LEFTSCROLLBAR, "); if(is_param_in_xor(ex_styles, WS_EX_LTRREADING)) printf("WS_EX_LTRREADING, "); if(is_param_in_xor(ex_styles, WS_EX_MDICHILD)) printf("WS_EX_MDICHILD, "); if(is_param_in_xor(ex_styles, WS_EX_NOACTIVATE)) printf("WS_EX_NOACTIVATE, "); if(is_param_in_xor(ex_styles, WS_EX_NOINHERITLAYOUT)) printf("WS_EX_NOINHERITLAYOUT, "); if(is_param_in_xor(ex_styles, WS_EX_NOPARENTNOTIFY)) printf("WS_EX_NOPARENTNOTIFY, "); if(is_param_in_xor(ex_styles, WS_EX_NOREDIRECTIONBITMAP)) printf("WS_EX_NOREDIRECTIONBITMAP, "); if(is_param_in_xor(ex_styles, WS_EX_OVERLAPPEDWINDOW)) printf("WS_EX_OVERLAPPEDWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_PALETTEWINDOW)) printf("WS_EX_PALETTEWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_RIGHT)) printf("WS_EX_RIGHT, "); if(is_param_in_xor(ex_styles, WS_EX_RIGHTSCROLLBAR)) printf("WS_EX_RIGHTSCROLLBAR, "); if(is_param_in_xor(ex_styles, WS_EX_RTLREADING)) printf("WS_EX_RTLREADING, "); if(is_param_in_xor(ex_styles, WS_EX_STATICEDGE)) printf("WS_EX_STATICEDGE, "); if(is_param_in_xor(ex_styles, WS_EX_TOOLWINDOW)) printf("WS_EX_TOOLWINDOW, "); if(is_param_in_xor(ex_styles, WS_EX_TOPMOST)) printf("WS_EX_TOPMOST, "); if(is_param_in_xor(ex_styles, WS_EX_TRANSPARENT)) printf("WS_EX_TRANSPARENT, "); if(is_param_in_xor(ex_styles, WS_EX_WINDOWEDGE)) printf("WS_EX_WINDOWEDGE, ");
}
void print_styles(long styles) {
    if(is_param_in_xor(styles, WS_BORDER)) printf("WS_BORDER, "); if(is_param_in_xor(styles, WS_CAPTION)) printf("WS_CAPTION, "); if(is_param_in_xor(styles, WS_CHILD)) printf("WS_CHILD, "); if(is_param_in_xor(styles, WS_CHILDWINDOW)) printf("WS_CHILDWINDOW, "); if(is_param_in_xor(styles, WS_CLIPCHILDREN)) printf("WS_CLIPCHILDREN, "); if(is_param_in_xor(styles, WS_CLIPSIBLINGS)) printf("WS_CLIPSIBLINGS, "); if(is_param_in_xor(styles, WS_DISABLED)) printf("WS_DISABLED, "); if(is_param_in_xor(styles, WS_DLGFRAME)) printf("WS_DLGFRAME, "); if(is_param_in_xor(styles, WS_GROUP)) printf("WS_GROUP, "); if(is_param_in_xor(styles, WS_HSCROLL)) printf("WS_HSCROLL, "); if(is_param_in_xor(styles, WS_ICONIC)) printf("WS_ICONIC, "); if(is_param_in_xor(styles, WS_MAXIMIZE)) printf("WS_MAXIMIZE, "); if(is_param_in_xor(styles, WS_MAXIMIZEBOX)) printf("WS_MAXIMIZEBOX, "); if(is_param_in_xor(styles, WS_MINIMIZE)) printf("WS_MINIMIZE, "); if(is_param_in_xor(styles, WS_MINIMIZEBOX)) printf("WS_MINIMIZEBOX, "); if(is_param_in_xor(styles, WS_OVERLAPPED)) printf("WS_OVERLAPPED, "); if(is_param_in_xor(styles, WS_OVERLAPPEDWINDOW)) printf("WS_OVERLAPPEDWINDOW, "); if(is_param_in_xor(styles, WS_POPUPWINDOW)) printf("WS_POPUPWINDOW, "); if(is_param_in_xor(styles, WS_SIZEBOX)) printf("WS_SIZEBOX, "); if(is_param_in_xor(styles, WS_SYSMENU)) printf("WS_SYSMENU, "); if(is_param_in_xor(styles, WS_TABSTOP)) printf("WS_TABSTOP, "); if(is_param_in_xor(styles, WS_THICKFRAME)) printf("WS_THICKFRAME, "); if(is_param_in_xor(styles, WS_TILED)) printf("WS_TILED, "); if(is_param_in_xor(styles, WS_TILEDWINDOW)) printf("WS_TILEDWINDOW, "); if(is_param_in_xor(styles, WS_VISIBLE)) printf("WS_VISIBLE, "); if(is_param_in_xor(styles, WS_VSCROLL)) printf("WS_VSCROLL, ");
}

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
} WIN_MONITOR;

typedef enum {
    WMM_STACK,
    WMM_TILING_V,
    WMM_TILING_H,
    WMM_COUNT,
} WIMAN_MODE;

typedef struct {
    WIMAN_WINDOW *wnd_list;
    size_t wnd_count;
    size_t tiling_count;
    int curr_wnd;
    WIMAN_MODE mode;
    int changed;
    int on_monitor;
    int is_main;
} WIMAN_DESKTOP_STATE;

typedef struct {
    WIMAN_DESKTOP_STATE desk_list[DESC_COUNT];
    UINT dpi;
    int curr_desk;
    WIN_MONITOR *monitors;
    // int curr_monitor;
    size_t monitor_count;
    HWND main_hwnd;
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
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void wnd_msg_proc(
  HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG idObject,
  LONG idChild,
  DWORD idEventThread,
  DWORD dwmsEventTime
);

void print_windowinfo(WINDOWINFO *info) {
    printf("      cdSize: %lu\n", info->cbSize);
    printf("      rcWindow: ");
    print_rect(info->rcWindow);
    printf("\n");
    printf("      rcClient: ");
    print_rect(info->rcClient);
    printf("\n");
    printf("      dwStyle: ");
    print_styles(info->dwStyle);
    printf("\n");
    printf("      dwExStyle: ");
    print_ex_styles(info->dwExStyle);
    printf("\n");
    printf("      dwWindowStatus: %lu\n", info->dwWindowStatus);
    printf("      cxWindowBorders: %u\n", info->cxWindowBorders);
    printf("      cyWindowBorders: %u\n", info->cyWindowBorders);
    printf("      atomWindowType: %hu\n", info->atomWindowType);
    printf("      wCreatorVersion: %hu\n", info->wCreatorVersion);
}

int append_new_wnd(WIMAN_WINDOW **wndl, size_t *len, WIMAN_WINDOW w) {
    (*len)++;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, sizeof(WIMAN_WINDOW) * *len);
    if(new_wndl == NULL) {
        return 1;
    }
    *wndl = new_wndl;
    (*wndl)[(*len) - 1] = w;
    return 0;
}

int insert_new_wnd(WIMAN_WINDOW **wndl, size_t *len, WIMAN_WINDOW w, int after) {
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
    if(new_wndl == NULL) {
        return 1;
    }
    *wndl = new_wndl;
    return 0;
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


int print_windows_list(WIMAN_WINDOW **wndl, size_t len) {
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
        print_windowinfo(&wi);
    }
    free(title);
    #undef wnd
    return 0;
}

void print_debug_info(WIMAN_STATE *wms) {
    #define wmds wms->desk_list[i]
    #define mn wms->monitors[i]
    printf("============DEBUG-INFO============\n\n");
    printf("%lld monitors:\n", wms->monitor_count);
    for(int i = 0; i < wms->monitor_count; i++) {
        printf("  Monitor %d: %dx%d%s - showing desk %d\n", i + 1, mn.w, mn.h, i == 0 ? " (primary)" : "", mn.front_desk + 1);
    }
    printf("\n");
    for(int i = 0; i < DESC_COUNT; i++) {
        printf("DESKTOP %d on monitor %d, with %lld windows (%lld tiling) - %s mode\n", i + 1, wmds.on_monitor + 1, wmds.wnd_count, wmds.tiling_count, WMM_NAMES[wmds.mode]);
        print_windows_list(&wmds.wnd_list, wmds.wnd_count);
        printf("\n\n");
    }
    printf("  Current desktop: %d\n", wms->curr_desk + 1);
    printf("\n==========END-DEBUG-INFO==========\n");
    #undef wmds
    #undef mn
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

int get_monitor_id_by_cursor(WIMAN_STATE *wms, long cur_x, long cur_y) {
    for(int i = 0; i < wms->monitor_count; i++) {
        if(wms->monitors[i].pos.left <= cur_x && wms->monitors[i].pos.right >= cur_x && wms->monitors[i].pos.top <= cur_y && wms->monitors[i].pos.bottom >= cur_y) return i;
    }
    return -1;
}

int is_actual_window(HWND hwnd) {
    if(GetWindowTextLengthA(hwnd) == 0 || !IsWindowVisible(hwnd)) return FALSE;
    long dxExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    if(is_param_in_xor(dxExStyle, WS_EX_NOREDIRECTIONBITMAP) || is_toolwindow(dxExStyle)) return FALSE;
    return TRUE;
}

HWND create_main_window(HINSTANCE *hInstance) {
    const char ClassName[] = "mywindow";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = *hInstance;
    wc.lpszClassName = ClassName;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        ClassName,
        "mywindow",
        0,
        0, 0, DESC_TILE_SIZE.x * DESC_COUNT, DESC_TILE_SIZE.y,
        NULL,
        NULL,
        *hInstance,
        NULL
    );
    SetWindowLong(hwnd, GWL_STYLE, 0);
    return hwnd;
}

BOOL enum_monitors(HMONITOR hmonitor, HDC hdc, LPRECT lpRect, LPARAM lParam) {
    #define wms ((WIMAN_STATE*)lParam)
    wms->monitor_count++;
    WIN_MONITOR *monitors = realloc(wms->monitors, sizeof(WIN_MONITOR) * wms->monitor_count);
    if(monitors == NULL) return FALSE;
    wms->monitors = monitors;
    WIN_MONITOR wm = { .hmonitor = hmonitor, .front_desk = wms->monitor_count - 1 };
    MONITORINFO lpmi = { .cbSize = sizeof(MONITORINFO) };
    GetMonitorInfoA(hmonitor, &lpmi);
    wm.h = lpmi.rcWork.bottom - lpmi.rcWork.top;
    wm.w = lpmi.rcWork.right - lpmi.rcWork.left;
    wm.pos = lpmi.rcWork;
    wms->desk_list[wms->monitor_count - 1].on_monitor = wms->monitor_count - 1;
    wms->desk_list[wms->monitor_count - 1].is_main = TRUE;
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
    if(!is_actual_window(hwnd) || hwnd == wms->main_hwnd) return TRUE;
    DWORD dxStyle = GetWindowLongPtrA(hwnd, GWL_STYLE);
    if(!is_resizable(dxStyle)) {
        order_wnd_z(hwnd, HWND_TOPMOST);
        return !append_new_wnd(&wmds.wnd_list, &wmds.wnd_count, (WIMAN_WINDOW){ .is_unresizable = TRUE, .is_freeroam = TRUE, .hwnd = hwnd });
    };
    return !insert_new_wnd(&wmds.wnd_list, &wmds.wnd_count, (WIMAN_WINDOW){ .hwnd = hwnd }, wmds.tiling_count++ - 1);
    #undef wmds
    #undef wms
}

int move_wnd_to_desk(WIMAN_DESKTOP_STATE *desk_from, int wnd, WIMAN_DESKTOP_STATE *desk_to) {
    if(desk_from->on_monitor == desk_to->on_monitor) ShowWindow(desk_from->wnd_list[wnd].hwnd, SW_HIDE);
    insert_new_wnd(&desk_to->wnd_list, &desk_to->wnd_count, desk_from->wnd_list[wnd], desk_to->tiling_count - 1);
    desk_to->tiling_count += !desk_from->wnd_list[wnd].is_freeroam;
    desk_from->tiling_count -= !desk_from->wnd_list[wnd].is_freeroam;
    remove_wnd_by_idx(&desk_from->wnd_list, &desk_from->wnd_count, wnd);
    printf("Moved window %d to another desktop\n", wnd + 1);
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
    int i = wmds->tiling_count - 1;
    if(!wnd.is_freeroam) {
        int offset = (wmds->wnd_count - wmds->tiling_count) * 32;
        if(!SetWindowPos(wnd.hwnd, HWND_TOPMOST, wm->w / 6 + offset, wm->h / 6 + offset, wm->w / 3 * 2, wm->h / 3 * 2, 0)) return 1;
        wmds->tiling_count--;
    } else {
        order_wnd_z(wnd.hwnd, HWND_NOTOPMOST);
        i++;
        wmds->tiling_count++;
    }
    wnd.is_freeroam = !wnd.is_freeroam;
    switch_wnds(&wmds->wnd_list, idx, i);
    wmds->curr_wnd = i;
    printf("Toggled window %d freeroam %s\n", idx + 1, wnd.is_freeroam ? "ON" : "OFF");
    #undef wnd
    // SetActiveWindow(curr_wnd.hwnd);
    return 0;
}

int tile_windows_vert(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    int window_w = wm->w / wmds->tiling_count;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){wm->pos.left, 0, window_w + wm->pos.left, wm->h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp;
    for(int i = 0; i < wmds->tiling_count; i++) {
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
    printf("Tiled windows vertically\n");
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
    printf("Initializing %s mode on desktop with %lld tiling windows...\n", WMM_NAMES[wmds->mode], wmds->tiling_count);
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

int switch_desc_to(WIMAN_STATE *wms, int new_desk_idx) {
    #define wnd wms->desk_list[wms->curr_desk].wnd_list[i]
    int i;
    int is_same_monitor = wms->desk_list[wms->curr_desk].on_monitor == wms->desk_list[new_desk_idx].on_monitor;
    if(is_same_monitor) for(i = 0; i < wms->desk_list[wms->curr_desk].wnd_count; i++) ShowWindow(wnd.hwnd, SW_HIDE);
    if(wms->desk_list[wms->curr_desk].wnd_count == 0) {
        wms->desk_list[wms->curr_desk].on_monitor = 0;
    }
    wms->curr_desk = new_desk_idx;
    if(wms->desk_list[wms->curr_desk].changed || !is_same_monitor) {
        init_curr_mode_reposition(&wms->desk_list[wms->curr_desk], &wms->monitors[wms->desk_list[wms->curr_desk].on_monitor]);
        wms->desk_list[wms->curr_desk].changed = FALSE;
    }
    wms->monitors[wms->desk_list[new_desk_idx].on_monitor].front_desk = new_desk_idx;
    if(is_same_monitor) for(i = 0; i < wms->desk_list[wms->curr_desk].wnd_count; i++) ShowWindow(wnd.hwnd, SW_SHOW);
    #undef wnd
    printf("Switched to desktop %d\n", new_desk_idx + 1);
    return 0;
}

// TODO FIX desktops don't switch properly when cycled with different monitors
// TODO .IDEA handle movesizestart event and assign window id to dragging_this and resizing_this flags in state
// TODO reset desktops to primary monitor when they are uninitialized
// TODO moving desktop to a window by drag and dropping and uninitializing it when there are no windows. have a single "main" desktop for every monitor, that changes everytime it is empty and abandoned and new dekstop is created. somehow handle monitor changed event, ideally on cursor switch to it.
// TODO FIX sometimes app crashes on window open
// TODO FIX SOME (NOT ALL) fullscreen apps (games) doesn't allow to capture key input and doesn't hide/show properly
// TODO implement desktops relation to monitors (them being on one monitor at a time) and keeping all minitor sizes. Also freeroam windows should be allowed to change monitors (active desktops by dragging and dropping)
// (works, needs further testing) TODO .IDEA if cannot set the position make it freeroam and add some flag like is_permanenty_freeroam (is_unresizable)
// (fixed) TODO FIX when moving last window off of virtual desk segfault occurs
// TODO FIX spotify rerenders ui on every reposition
// (currently undoable) TODO FIX weird margins around some windows
// TODO .IDEA handle WM_DPICHANGED
// TODO .IDEA use deferwindowpos and enddeferwindowpos for tiling windows
// TODO FIX sometimes weird things happen when opening a window or switching windows while there are freeroam ones
// (works, needs further testing) TODO FIX when a freeroam window is present and a new window is opened (appended), freeroam window becomes non freeroam and get placed at the same place as the newly opened window
// (works, needs further testing) TODO refetch windows on new window open
// (works, needs further testing) TODO refetch windows on window close
// (works, needs further testing) TODO set active_window on actual active window anytime
// (works, needs further testing) TODO virtual desktops
// TODO closing windows shortcut
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    // MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY)
    wms.dpi = GetDpiForSystem();
    printf("DPI is %d\n", wms.dpi);

    wms.main_hwnd = create_main_window(&hInstance);
    // order_wnd_z(wms.main_hwnd, HWND_TOPMOST);
    ShowWindow(wms.main_hwnd, nShowCmd);

    HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_proc, NULL, 0);
    HWINEVENTHOOK msg_hook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, NULL, wnd_msg_proc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
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
    g_curr_desk = (WIMAN_DESKTOP_STATE){
        .curr_wnd = -1,
    };
    EnumWindows(enum_wnd, (LPARAM)&wms);
    printf("Fetched %lld windows, %lld tiling\n", g_curr_desk.wnd_count, g_curr_desk.tiling_count);
    if(g_curr_desk.wnd_count > 0) g_curr_desk.curr_wnd = 0;

    wms.monitors = calloc(0, sizeof(WIN_MONITOR));
    EnumDisplayMonitors(NULL, NULL, enum_monitors, (LPARAM)&wms);
    printf("Fetched %lld monitors\n", wms.monitor_count);

    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(TEXT("SHELLHOOK"));

    init_curr_mode_reposition(&wms.desk_list[wms.curr_desk],  &wms.monitors[wms.desk_list[wms.curr_desk].on_monitor]);
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
            if(g_curr_desk.mode != WMM_STACK) {
                g_curr_desk.mode = WMM_STACK;
                init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            }
            break;
        case 'E':
            if(g_curr_desk.mode == WMM_TILING_V) {
                g_curr_desk.mode = WMM_TILING_H;
            } else {
                g_curr_desk.mode = WMM_TILING_V;
            }
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
            print_debug_info(&wms);
            break;
        case 49 ... 57:
            int target = key->vkCode - 49;
            if(is_pressed(VK_SHIFT)) {
                if(target == wms.curr_desk) break;
                int new_wnd_idx = move_wnd_to_desk(&g_curr_desk, g_curr_desk.curr_wnd, &wms.desk_list[target]);
                wms.desk_list[target].changed = TRUE;
                if(g_curr_desk.wnd_count == 0) {
                    g_curr_desk.on_monitor = 0;
                }
                // TODO do only when window that moved was not freeroaming
                init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                if(g_curr_desk.on_monitor != wms.desk_list[target].on_monitor) {
                    wms.curr_desk = target;
                    init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                    g_curr_desk.curr_wnd = new_wnd_idx;
                }
                break;
            }
            if(is_pressed(VK_CONTROL) && !g_curr_desk.is_main) {
                if(target == g_curr_desk.on_monitor || target >= wms.monitor_count) break;
                g_curr_desk.on_monitor = target;
                wms.monitors[target].front_desk = wms.curr_desk;
                init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                break;
            }
            if(target == wms.curr_desk) break;
            // if((wms.desk_list[target].wnd_count && g_curr_desk.on_monitor == wms.desk_list[target].on_monitor || !wms.desk_list[target].wnd_count) && g_curr_desk.is_main && !g_curr_desk.wnd_count) {
            //     g_curr_desk.is_main = FALSE;
            //     wms.desk_list[target].is_main = TRUE;
            //     wms.desk_list[target].on_monitor = g_curr_desk.on_monitor;
            // }
            if(!wms.desk_list[target].wnd_count) {
                wms.desk_list[target].on_monitor = g_curr_desk.on_monitor;
            }
            switch_desc_to(&wms, target);
            InvalidateRect(wms.main_hwnd, 0, TRUE);
        }
        return 2;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_SHELLHOOKMESSAGE) {
        #define chwnd (HWND)lParam
        // if(wParam == HSHELL_MONITORCHANGED) {
        //     printf("Monitor has changed (event)\n");
        // }
        if(!is_actual_window(chwnd) || hwnd == wms.main_hwnd) return DefWindowProc(hwnd, uMsg, wParam, lParam);
        switch(wParam) {
            case HSHELL_WINDOWACTIVATED: {
                int monitor_id = get_monitor_id_by_hmonitor(&wms, MonitorFromWindow(chwnd, MONITOR_DEFAULTTONEAREST));
                if(monitor_id != g_curr_desk.on_monitor) {
                    printf("Monitor has changed\n");
                }
                wms.curr_desk = wms.monitors[monitor_id].front_desk;
                int wnd_id = get_wnd_id_by_hwnd(&g_curr_desk, chwnd);
                if(wnd_id == -1) goto add_window;
                long curr_wnd_style = GetWindowLongPtrA(get_curr_hwnd(g_curr_desk), GWL_STYLE);
                if(!is_param_in_xor(curr_wnd_style, WS_VISIBLE) || (is_param_in_xor(curr_wnd_style, WS_MINIMIZE) && !g_curr_desk.wnd_list[g_curr_desk.curr_wnd].is_freeroam)) {
                    printf("Assuming that window %d is closing...\n", g_curr_desk.curr_wnd);
                    if(!g_curr_desk.wnd_list[g_curr_desk.curr_wnd].is_freeroam) {
                        g_curr_desk.tiling_count--;
                    }
                    remove_wnd_by_idx(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, g_curr_desk.curr_wnd);
                    init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                }
                printf("Window %d activated\n", wnd_id);
                g_curr_desk.curr_wnd = wnd_id;
                break;
            }
            case HSHELL_WINDOWCREATED: {
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
        case WM_PAINT: {
                PAINTSTRUCT ps;
                print_rect(ps.rcPaint);
                HDC hdc = BeginPaint(hwnd, &ps);
                int offset = 0;
                char buffer[2];
                for(int i = 0; i < DESC_COUNT; i++) {
                    if(!wms.desk_list[i].wnd_count && wms.curr_desk != i) continue;
                    RECT pos_r = { ps.rcPaint.left + offset, 0, ps.rcPaint.left + offset + DESC_TILE_SIZE.x, 0 + DESC_TILE_SIZE.y };
                    FillRect(hdc, &pos_r, (HBRUSH)(CreateSolidBrush(DESC_BG_COLORS[i == wms.curr_desk])));
                    FrameRect(hdc, &pos_r, (HBRUSH)(CreateSolidBrush(DESC_COLORS[i == wms.curr_desk])));
                    sprintf(buffer, "%d", i + 1);
                    offset += DESC_TILE_SIZE.x;
                    DrawTextA(hdc, buffer, -1, &pos_r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                EndPaint(hwnd, &ps);
            }
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
    if(!is_actual_window(hwnd) || hwnd == wms.main_hwnd) return;
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
            LPPOINT clp = malloc(sizeof(struct tagPOINT));
            GetCursorPos(clp);
            printf("Window is moved, cursor was on %ld %ld\n", clp->x, clp->y);
            int monitor_id = get_monitor_id_by_cursor(&wms, clp->x, clp->y);
            if(monitor_id != g_curr_desk.on_monitor) {
                printf("Window is moved from desktop %d (monitor %d) to desktop %d (monitor %d)\n", wms.curr_desk, wms.desk_list[wms.curr_desk].on_monitor, wms.monitors[monitor_id].front_desk, monitor_id);
                move_wnd_to_desk(&g_curr_desk, wnd_id, &wms.desk_list[wms.monitors[monitor_id].front_desk]);
                // if(w.is_freeroam) {
                //     wms.curr_desk = wms.monitors[monitor_id].front_desk;
                //     break;
                // }
                if(g_curr_desk.wnd_count == 0) {
                    g_curr_desk.on_monitor = 0;
                }
                init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                wms.curr_desk = wms.monitors[monitor_id].front_desk;
                init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
                break;
            }
            #undef w
            int new_place;
            switch(g_curr_desk.mode) {
                case WMM_STACK:
                    position_window(hwnd, &wp, &wr);
                    break;
                case WMM_TILING_V:
                    new_place = g_curr_desk.tiling_count * clp->x / wms.monitors[g_curr_desk.on_monitor].w;
                    goto place;
                case WMM_TILING_H:
                    new_place = g_curr_desk.tiling_count * clp->y / wms.monitors[g_curr_desk.on_monitor].h;
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
            free(clp);
    }
}
