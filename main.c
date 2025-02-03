#include "windows.h"
#include <minwindef.h>
#include <shellapi.h>
#include "wingdi.h"
#include <stdlib.h>
#include <winnt.h>
#include <winuser.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <winternl.h>
#include <winnt.h>
#include <shellscalingapi.h>

#define WINDOW_TITLE_BUFFER_SIZE 64
char WINDOW_TITLE_BUFFER[WINDOW_TITLE_BUFFER_SIZE];
static const int DEFAULT_DPI = 96;
static const COLORREF BG_COLOR = 0x00444444;
static const COLORREF DESK_COLOR = 0x00878787;
static const COLORREF DESK_BG_COLOR = 0x006B979E;
static const COLORREF ACTIVE_DESK_COLOR = 0x00BC750B;
static const COLORREF ACTIVE_DESK_BG_COLOR = 0x00FFDA0A;
static const COLORREF DESK_COLORS[2] = { [FALSE] = DESK_COLOR, [TRUE] = ACTIVE_DESK_COLOR };
static const COLORREF DESK_BG_COLORS[2] = { [FALSE] = DESK_BG_COLOR, [TRUE] = ACTIVE_DESK_BG_COLOR };
static const POINT DESK_TILE_SIZE = { 32, 32 };
static const char WINDOW_CLASSNAME[] = "2wiman";
static const char BUTTON_CLASSNAME[] = "2wiman-control";
static const char SETTINGS_CLASSNAME[] = "2wiman-settings";
#define WM_NOTIFYICON WM_APP + 1
#define DESK_COUNT 9
#define TILE_OFFSET 3
#define LOGFILE_BUFFER_SIZE 128
#define MODIFIER_KEY VK_CAPITAL
#define scale_to_dpi(thing, dpi) ((thing) * dpi / DEFAULT_DPI)
#define apply_dpi_factor(thing, factor) ((thing) * (factor * wiman_config.is_scale_with_dpi + 1 * !wiman_config.is_scale_with_dpi))
#define order_wnd_z(hwnd, pos) SetWindowPos(hwnd, pos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)
#define is_param_in_xor(xor, param) ((xor | param) == xor)
#define is_wh_equal_rect(a, b) (((a.bottom - a.top) == (b.bottom - b.top)) && ((a.right - a.left) == (b.right - b.left)))
#define is_pressed(key) (HIWORD(GetKeyState(key)))
#define is_toolwindow(dxExStyle) is_param_in_xor(dxExStyle, WS_EX_TOOLWINDOW)
#define is_resizable(dxStyle) is_param_in_xor(dxStyle, WS_THICKFRAME)
#define get_curr_hwnd(wmds) (wmds).wnd_list[(wmds).curr_wnd].hwnd
#define g_curr_desk wms.desk_list[wms.curr_desk]
#define curr_from_lp(lp) ((POINT){ GET_X_LPARAM(lp), GET_Y_LPARAM(lp) })
#define print_rect(rect) printf("RECT: left%ld top%ld right%ld bottom%ld\n", rect.left , rect.top, rect.right, rect.bottom);
#define is_point_in_rect(point, rect) (point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
#define ONLY_HOVER TME_HOVER
#define ONLY_LEAVE TME_LEAVE
#define HOVER_AND_LEAVE ONLY_HOVER | ONLY_LEAVE
#define hover_or_leave(hover) (TME_HOVER + !hover)
#define dpi_awareness() GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext())

typedef struct {
    HWND hwnd;
    RECT last_set_pos;
    RECT initial_pos;
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
    unsigned int dpi;
    float dpi_factor;
} WIN_MONITOR;

typedef enum {
    WMM_STACK,
    WMM_TILING_V,
    WMM_TILING_H,
    WMM_COUNT,
} WIMAN_MODE;

typedef enum {
    OPTION_EXIT,
    OPTION_SETTINGS,
    OPTION_COUNT,
} WIMAN_ICON_OPTION;

typedef struct {
    WIMAN_WINDOW *wnd_list;
    struct {
        HWND hwnd;
        POINT last_set_left_top;
        int is_hovered;
        int is_tracking;
    } button;
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
    char time_buffer[32];
    FILE *logfile;
    HINSTANCE *h_instance;
    union {
        WNDCLASS button;
        WNDCLASS main;
        WNDCLASS settings;
    } wc;
} WIMAN_STATE;

WIMAN_STATE wms = {};

typedef struct {
    BOOL is_stack_mode_infinite_scroll;
    WIMAN_MODE default_mode;
    POINT button_size;
    BOOL is_scale_with_dpi;
} WIMAN_CONFIG;

WIMAN_CONFIG wiman_config = {
    .is_stack_mode_infinite_scroll = FALSE,
    .is_scale_with_dpi = TRUE,
    .button_size = DESK_TILE_SIZE,
    .default_mode = WMM_STACK,
};

const char* const WMM_NAMES[WMM_COUNT] = {
    [WMM_TILING_V] = "vertical tiling",
    [WMM_TILING_H] = "horizontal tiling",
    [WMM_STACK] = "stack",
};
UINT WM_SHELLHOOKMESSAGE;

struct {
    size_t offset;
    const char name[48];
} config_names_map[] = {
    {
        .name = "is_stack_mode_infinite_scroll",
        .offset = offsetof(WIMAN_CONFIG, is_stack_mode_infinite_scroll)
    },
    {
        .name = "is_scale_with_dpi",
        .offset = offsetof(WIMAN_CONFIG, is_scale_with_dpi)
    },
    {
        .name = "button_size_x",
        .offset = offsetof(WIMAN_CONFIG, button_size)
    },
    {
        .name = "button_size_y",
        .offset = offsetof(WIMAN_CONFIG, button_size) + offsetof(POINT, y)
    },
    {
        .name = "default_mode",
        .offset = offsetof(WIMAN_CONFIG, default_mode)
    }
};

const char CONFIG_BUFFER[] = "is_stack_mode_infinite_scroll=1\ndefault_mode=1\nbutton_size_x=32\nbutton_size_y=32\nis_scale_with_dpi=1\n";

int parse_config_file(WIMAN_CONFIG *wmcfg, const char buffer[], size_t len) {
    char c;
    char name_buf[48];
    long long offset = 1;
    int cur = 0;
    for(int i = 0; i < len; i++) {
        c = buffer[i];
        if(c == '=') {
            cur = 0;
            offset = -1;
            printf("searhing for %s\n", name_buf);
            for(int j = 0; j < 5; j++) {
                if(strncmp(config_names_map[j].name, name_buf, strlen(config_names_map[j].name)) == 0) {
                    printf("found\n");
                    offset = config_names_map[j].offset;
                }
            }
            if(offset == -1) {
                printf("Invalid config entry\n");
                return 1;
            }
        } else if(c == '\n') {
            name_buf[cur] = '\0';
            printf("assigning %s\n", name_buf);
            cur = 0;
            *(long*)((char*)wmcfg + offset) = atol(name_buf);
        } else {
            name_buf[cur] = c;
            cur++;
        }
    }
}

void get_time(char* output) {
    time_t tt = time(0);
    struct tm t;
    localtime_s(&t, &tt);
    sprintf(output, "[%d.%d.%d %d:%d:%d] ", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
}

void scale_rect_to_dpi(RECT *r, int dpi) {
    r->bottom = scale_to_dpi(r->bottom, dpi);
    r->top = scale_to_dpi(r->top, dpi);
    r->left = scale_to_dpi(r->left, dpi);
    r->right = scale_to_dpi(r->right, dpi);
    return;
}

void log_to_file(WIMAN_STATE *wms, const char *pattern, ...) {
    va_list args;
    va_start(args, pattern);
    get_time(wms->time_buffer);
    fprintf_s(wms->logfile, wms->time_buffer);
    vfprintf_s(wms->logfile, pattern, args);
    return;
}

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
}

int print_windows_list(WIMAN_WINDOW **wndl, size_t len, BOOL verbose) {
    int length;
    WINDOWINFO wi;
    for(int i = 0; i < len; i++) {
        #define wnd (*wndl)[i]
        GetWindowTextA(wnd.hwnd, WINDOW_TITLE_BUFFER, WINDOW_TITLE_BUFFER_SIZE);
        printf("   - Window %d%s: \"%s\"\n", i + 1, wnd.is_freeroam ? wnd.is_unresizable ? " (freeroam, unresizable)" : " (freeroam)" : "", WINDOW_TITLE_BUFFER);
        GetWindowInfo(wnd.hwnd, &wi);
        if(verbose) print_windowinfo(&wi, "       ");
    }
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
    log_to_file(&wms, "Inserting new window after %d\n", after);
    (*len)++;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, sizeof(WIMAN_WINDOW) * *len);
    if(new_wndl == NULL) return 1;
    *wndl = new_wndl;
    if(after < *len - 1) memmove_s(*wndl + after + 2, (*len - after - 2) * sizeof(WIMAN_WINDOW), *wndl + after + 1, (*len - after - 2) * sizeof(WIMAN_WINDOW));
    (*wndl)[after + 1] = w;
    return 0;
}

int remove_wnd_by_idx(WIMAN_WINDOW **wndl, size_t *len, int idx) {
    log_to_file(&wms, "Removing window with index %d\n", idx);
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

void track_mouse_for(HWND hwnd, int event) {
    TRACKMOUSEEVENT te = {.cbSize = sizeof(TRACKMOUSEEVENT),.dwFlags = event,.hwndTrack = hwnd,.dwHoverTime = 10};
    TrackMouseEvent(&te);
    return;
}

int position_window(HWND hwnd, RECT *r) {
    log_to_file(&wms, "Positioning window at %ld, %ld, %ld, %ld\n", r->left, r->top, r->right, r->bottom);
    WINDOWPLACEMENT wp = {
        .rcNormalPosition = *r,
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT gwr;
    BOOL result = SetWindowPlacement(hwnd, &wp);
    if(!result) {
        log_to_file(&wms, "Positioning window: SetWindowPlacement failed with code 0x%x\n", GetLastError());
        goto set_window_pos;
    }
    GetWindowRect(hwnd, &gwr);
    if(gwr.bottom > r->bottom || gwr.right > r->right) {
        set_window_pos:
        result = SetWindowPos(hwnd, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, SWP_DEFERERASE | SWP_NOSENDCHANGING | SWP_NOOWNERZORDER);
    }
    if(!result) {
        log_to_file(&wms, "Positioning window: SetWindowPos failed with code 0x%x\n", GetLastError());
    }
    return result;
}

void switch_wnds(WIMAN_WINDOW **wndl, int first, int second) {
    log_to_file(&wms, "Switching windows %d and %d\n", first + 1, second + 1);
    if(first == second) return;
    WIMAN_WINDOW buffer = (*wndl)[first];
    (*wndl)[first] = (*wndl)[second];
    (*wndl)[second] = buffer;
}

void switch_hwnds_pos(WIMAN_WINDOW **wndl, int first, int second) {
    log_to_file(&wms, "Switching HWNDs of windows %d and %d and repositioned them\n", first + 1, second + 1);
    HWND hwnd = (*wndl)[first].hwnd;
    (*wndl)[first].hwnd = (*wndl)[second].hwnd;
    (*wndl)[second].hwnd = hwnd;
    position_window((*wndl)[first].hwnd, &(*wndl)[first].last_set_pos);
    position_window((*wndl)[second].hwnd, &(*wndl)[second].last_set_pos);
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

// "heavily inspired" by https://stackoverflow.com/a/22949726
int is_window_thread_suspended(HWND hwnd) {
    DWORD pid;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if(!tid) return -1;
    ULONG needed = 0;
    BYTE *d = malloc(0);
    NTSTATUS s32_Status;
    do {
        int ds = needed + 1;
        needed = 0;
        BYTE *nd = realloc(d, ds);
        if(nd == NULL) {
            free(d);
            return -1;
        }
        d = nd;
        s32_Status = NtQuerySystemInformation(SystemProcessInformation, d, ds, &needed);
    } while(s32_Status == 0xC0000004);
    SYSTEM_PROCESS_INFORMATION *spi = (SYSTEM_PROCESS_INFORMATION*)d;
    while(spi->NextEntryOffset) {
        if((DWORD)(DWORD_PTR)spi->UniqueProcessId == pid) break;
        spi = (SYSTEM_PROCESS_INFORMATION*)((BYTE*)spi + spi->NextEntryOffset);
    }
    if((DWORD)(DWORD_PTR)spi->UniqueProcessId != pid) {
        free(d);
        return -1;
    }
    SYSTEM_THREAD_INFORMATION *sti = (SYSTEM_THREAD_INFORMATION*)((BYTE*)spi + sizeof(SYSTEM_PROCESS_INFORMATION));
    for(int i = 0; i < spi->NumberOfThreads; i++) {
        if(sti->ClientId.UniqueThread == (HANDLE)(DWORD_PTR)tid) return sti->ThreadState == 5 && sti->WaitReason == 13;
        // sti = (SYSTEM_THREAD_INFORMATION*)((BYTE*)(sti + sizeof(SYSTEM_THREAD_INFORMATION)));
        sti++;
    }
    free(d);
    return -1;
}

int is_actual_window(HWND hwnd) {
    long dxExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    long dxStyle = GetWindowLongA(hwnd, GWL_STYLE);
    return is_param_in_xor(dxStyle, WS_CAPTION) && IsWindowVisible(hwnd) && GetWindowTextLengthA(hwnd) && !is_toolwindow(dxExStyle);
}

HWND create_window(HINSTANCE *hInstance, HWND hwnd_parent, int offset_left) {
    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        WINDOW_CLASSNAME,
        "2wiman",
        0,
        0, 0, 0, 0,
        hwnd_parent,
        NULL,
        *hInstance,
        NULL
    );
    SetWindowLongPtrA(hwnd, GWL_STYLE, (hwnd_parent != NULL) << 30 & WS_CHILDWINDOW);
    return hwnd;
}

HWND create_button(HINSTANCE *hInstance, HWND hwnd_parent, int desk) {
    if(hwnd_parent == NULL) return NULL;
    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        BUTTON_CLASSNAME,
        "2wmctrl",
        0,
        0, 0, 0, 0,
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
    if(GetDpiForMonitor(hmonitor, MDT_EFFECTIVE_DPI, &wm.dpi, &wm.dpi) != S_OK) wm.dpi = DEFAULT_DPI;
    wm.dpi_factor = (float)wm.dpi / DEFAULT_DPI;
    wm.h = lpmi.rcWork.bottom - lpmi.rcWork.top;
    wm.w = lpmi.rcWork.right - lpmi.rcWork.left;
    wm.pos = lpmi.rcWork;
    wms->desk_list[wms->monitor_count - 1].on_monitor = wms->monitor_count - 1;
    HWND button_hwnd = create_button(wms->h_instance, wms->main_hwnd, wms->monitor_count - 1);
    wms->desk_list[wms->monitor_count - 1].button.hwnd = button_hwnd;
    wms->desk_list[wms->monitor_count - 1].button.is_tracking = TRUE;
    SetWindowPos(button_hwnd, 0, lpmi.rcWork.left, lpmi.rcWork.top, lpmi.rcWork.left + apply_dpi_factor(wiman_config.button_size.x, wm.dpi_factor), lpmi.rcWork.top + apply_dpi_factor(wiman_config.button_size.y, wm.dpi_factor), SWP_NOZORDER);
    wms->desk_list[wms->monitor_count - 1].button.last_set_left_top = (POINT){ .x = lpmi.rcWork.left, .y = lpmi.rcWork.top };
    // SetWindowPos(button_hwnd, 0, lpmi.rcWork.left, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(button_hwnd, SW_SHOW);
    if(is_param_in_xor(lpmi.dwFlags, MONITORINFOF_PRIMARY)) {
        log_to_file(wms, "Found primary monitor (%lld)\n", wms->monitor_count);
        wms->monitors[wms->monitor_count - 1] = wms->monitors[0];
        wms->monitors[wms->monitor_count - 1].front_desk = wms->monitor_count - 1;
        wm.front_desk = 0;
        wms->monitors[0] = wm;
    } else {
        wms->monitors[wms->monitor_count - 1] = wm;
    }
    log_to_file(wms, "Found monitor %lld: W = %d, H = %d, DPI = %d\n", wms->monitor_count, wm.w, wm.h, wm.dpi);
    #undef wms
}

BOOL CALLBACK enum_wnd(HWND hwnd, LPARAM lParam) {
    #define wms ((WIMAN_STATE*)lParam)
    #define wmds wms->desk_list[0]
    if(!is_actual_window(hwnd)) return TRUE;
    if(is_window_thread_suspended(hwnd)) return TRUE;
    GetWindowTextA(hwnd, (char*)WINDOW_TITLE_BUFFER, WINDOW_TITLE_BUFFER_SIZE);
    log_to_file(wms, "Window \"%s\" found\n", WINDOW_TITLE_BUFFER);
    DWORD dxStyle = GetWindowLongPtrA(hwnd, GWL_STYLE);
    int is_resizable = is_resizable(dxStyle);
    RECT init_pos;
    GetWindowRect(hwnd, &init_pos);
    WIMAN_WINDOW new_wnd = { .is_unresizable = !is_resizable, .is_freeroam = !is_resizable, .hwnd = hwnd, .initial_pos = init_pos };
    order_wnd_z(hwnd, HWND_TOPMOST - is_resizable);
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
    log_to_file(&wms, "Moving window %d to another desktop\n", wnd + 1);
    if(desk_from->on_monitor == desk_to->on_monitor) ShowWindow(desk_from->wnd_list[wnd].hwnd, SW_HIDE);
    insert_new_wnd(&desk_to->wnd_list, &desk_to->wnd_count, desk_from->wnd_list[wnd], desk_to->tiling_count - 1);
    desk_to->tiling_count += !desk_from->wnd_list[wnd].is_freeroam;
    desk_from->tiling_count -= !desk_from->wnd_list[wnd].is_freeroam;
    remove_wnd_by_idx(&desk_from->wnd_list, &desk_from->wnd_count, wnd);
    return desk_to->tiling_count - 1;
}

// tile_windows_vert that uses inbuilt tiling system to tile windows
// int tile_windows_vert(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
//     log_to_file(&wms, "Tiling windows (%lld in total) vertically using Windows inbuilt system\n", wmds->tiling_count);
//     HWND *lpKids = calloc(wmds->tiling_count, sizeof(HWND));
//     for(int i = 0; i < wmds->tiling_count; i++) {
//         lpKids[i] = wmds->wnd_list[i].hwnd;
//     }
//     TileWindows(NULL, MDITILE_VERTICAL, NULL, wmds->tiling_count, lpKids);
//     free(lpKids);
//     return 0;
// }


int toggle_wnd_freeroam(WIMAN_DESKTOP_STATE *wmds, int idx, WIN_MONITOR *wm) {
    #define wnd wmds->wnd_list[idx]
    log_to_file(&wms, "Toggling window %d freeroam (currently %s)\n", idx + 1, wnd.is_freeroam ? "ON" : "OFF");
    if(wnd.is_unresizable) {
        log_to_file(&wms, "Toggling freeroam: window is unresizable, returning");
        return 2;
    }
    int offset = (wmds->wnd_count - wmds->tiling_count) * 32;
    if(!SetWindowPos(wnd.hwnd, HWND_TOPMOST - wnd.is_freeroam, wm->w / 6 + offset, wm->h / 6 + offset, wm->w / 3 * 2, wm->h / 3 * 2, wnd.is_freeroam * (SWP_NOMOVE | SWP_NOSIZE))) return 1;
    wmds->tiling_count += !wnd.is_freeroam * -2 + 1;
    wnd.is_freeroam = !wnd.is_freeroam;
    int new_id = wmds->tiling_count + !wnd.is_freeroam;
    switch_wnds(&wmds->wnd_list, idx, new_id);
    wmds->curr_wnd = new_id;
    #undef wnd
    // SetActiveWindow(curr_wnd.hwnd);
    return 0;
}

int tile_windows_vert(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    log_to_file(&wms, "Tiling windows (%lld in total) vertically\n", wmds->tiling_count);
    int window_w = wm->w / wmds->tiling_count;
    RECT pos = { wm->pos.left, wm->pos.top, wm->pos.left + window_w, wm->pos.bottom };
    for(int i = 0; i < wmds->tiling_count; i++) {
        log_to_file(&wms, "Tiling vertically: Setting window %d position\n", i);
        position_window(wmds->wnd_list[i].hwnd, &pos);
        wmds->wnd_list[i].last_set_pos = pos;
        pos.left += window_w;
        pos.right += window_w;
    }
    return 0;
}

int tile_windows_horiz(WIMAN_DESKTOP_STATE *wmds, WIN_MONITOR *wm) {
    log_to_file(&wms, "Tiling windows (%lld in total) horizontally\n", wmds->tiling_count);
    int window_h = wm->h / wmds->tiling_count;
    RECT pos = { wm->pos.left, wm->pos.top, wm->w, wm->pos.top + window_h };
    for(int i = 0; i < wmds->tiling_count; i++) {
        log_to_file(&wms, "Tiling horizontally: Setting window %d position\n", i);
        position_window(wmds->wnd_list[i].hwnd, &pos);
        wmds->wnd_list[i].last_set_pos = pos;
        // TODO i want the same border that window snapping uses around windows
        // long style = GetWindowLongPtrA(curr_hwnd, GWL_STYLE);
        // SetWindowLongPtrA(curr_hwnd, GWL_STYLE, style ^ !WS_SYSMENU);
        pos.top += window_h;
        pos.bottom += window_h;
    }
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
    log_to_file(&wms, "Maximizing all windows (switching to stack mode)\n");
    // RECT p = {wm->pos.left, wm->pos.top, wm->w + wm->pos.left, wm->h};
    // for(int i = 0; i < wmds->tiling_count; i++) {
    //     if(!SetWindowPos(wmds->wnd_list[i].hwnd, HWND_NOTOPMOST, p.left, p.top, p.right, p.bottom, 0)) return 1;
    //     wmds->wnd_list[i].last_set_pos = p;
    // }
    for(int i = 0; i < wmds->tiling_count; i++) {
        ShowWindow(wmds->wnd_list[i].hwnd, SW_MAXIMIZE);
        // SetWindowLongPtrA(wmds->wnd_list[i].hwnd, GWL_EXSTYLE, GetWindowLongPtrA(wmds->wnd_list[i].hwnd, GWL_EXSTYLE) | WS_MAXIMIZE);
    }
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
    log_to_file(&wms, "Initializing %s mode on desktop\n", WMM_NAMES[wmds->mode]);
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
    log_to_file(wms, "Buttons repositioning\n");
    long *offsets = calloc(wms->monitor_count, sizeof(int));
    int monitor, offset;
    for(int i = 0; i < DESK_COUNT; i++) {
        monitor = wms->desk_list[i].on_monitor;
        ShowWindow(wms->desk_list[i].button.hwnd, SW_HIDE + (monitor != -1) * 5);
        if(monitor == -1) {
            log_to_file(wms, "Buttons: Skipping and hiding desktop button %d (it isn't active or is current)\n", i);
            continue;
        }
        offset = offsets[monitor];
        if(offset == wms->desk_list[i].button.last_set_left_top.x && wms->monitors[monitor].pos.top == wms->desk_list[i].button.last_set_left_top.y) {
            log_to_file(wms, "Buttons: Skipping desktop button %d (position didn't change)\n", i);
            offsets[monitor] += wiman_config.button_size.x;
            continue;
        }
        log_to_file(wms, "Buttons: Setting desktop button %d position\n", i);
        SetWindowPos(wms->desk_list[i].button.hwnd, 0, wms->monitors[monitor].pos.left + apply_dpi_factor(offset, wms->monitors[monitor].dpi_factor), wms->monitors[monitor].pos.top, wms->monitors[monitor].pos.left + apply_dpi_factor(offset + wiman_config.button_size.x, wms->monitors[monitor].dpi_factor), wms->monitors[monitor].pos.top + apply_dpi_factor(wiman_config.button_size.y, wms->monitors[monitor].dpi_factor), SWP_NOZORDER);
        wms->desk_list[i].button.last_set_left_top.x = offset;
        wms->desk_list[i].button.last_set_left_top.y = wms->monitors[monitor].pos.top;
        offsets[monitor] += wiman_config.button_size.x;
        // InvalidateRect(wms->desk_list[i].button.hwnd, 0, TRUE);
    }
    free(offsets);
    return;
}

int switch_desc_to(WIMAN_STATE *wms, int target) {
    log_to_file(wms, "Switching desktop to %d\n", target);
    if(target == wms->curr_desk) return wms->curr_desk;
    WIMAN_DESKTOP_STATE *curr_desk = &wms->desk_list[wms->curr_desk];
    WIMAN_DESKTOP_STATE *target_desk = &wms->desk_list[target];
    int is_desk_create = !target_desk->wnd_count && (target_desk->on_monitor == -1 || wms->monitors[target_desk->on_monitor].desk_count > 1);
    if(is_desk_create) {
        log_to_file(wms, "Switching desktop: Target desktop to be \"created\"\n");
        target_desk->on_monitor = curr_desk->on_monitor;
        wms->monitors[curr_desk->on_monitor].desk_count++;
    }
    int is_same_monitor = curr_desk->on_monitor == target_desk->on_monitor;
    is_same_monitor ? log_to_file(wms, "Switching desktop: Monitor is the same (%d)\n", curr_desk->on_monitor) : log_to_file(wms, "Switching desktop: Monitors are different (%d and %d)\n", curr_desk->on_monitor, target_desk->on_monitor);
    int is_desk_destroy = !curr_desk->wnd_count && wms->monitors[curr_desk->on_monitor].desk_count > 1;
    if(is_desk_destroy) {
        log_to_file(wms, "Switching desktop: Previous current desktop to be \"destroyed\"\n");
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
    if(curr_desk->changed) {
        init_curr_mode_reposition(curr_desk, &wms->monitors[curr_desk->on_monitor]);
        curr_desk->changed = FALSE;
    }
    wms->monitors[target_desk->on_monitor].front_desk = target;
    if(is_same_monitor) for(i = 0; i < curr_desk->wnd_count; i++) ShowWindow(curr_desk->wnd_list[i].hwnd, SW_SHOW);
    if(is_desk_create || is_desk_destroy) reposition_buttons(wms);
    return 0;
}

int send_desk_to_monitor(WIMAN_STATE *wms, int desk, int monitor) {
    log_to_file(wms, "Sending desktop %d to monitor %d\n", desk, monitor);
    WIMAN_DESKTOP_STATE *desk_s = &wms->desk_list[desk];
    if(wms->monitors[desk_s->on_monitor].desk_count <= 1 || monitor == wms->desk_list[desk].on_monitor || monitor >= wms->monitor_count) {
        log_to_file(wms, "Sending desktop: Cannot send desktop: invalid conditions\n");
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
    log_to_file(wms, "Moving window %d from desktop %d to %d:\n", wnd, from, desk);
    if(desk == from) {
        log_to_file(wms, "Moving window: Desktops are the same, aborting\n");
        return 2;
    }
    WIMAN_DESKTOP_STATE *target_desk = &wms->desk_list[desk];
    WIMAN_DESKTOP_STATE *desk_from = &wms->desk_list[from];
    int is_desk_create = !target_desk->wnd_count && (target_desk->on_monitor == -1 || wms->monitors[target_desk->on_monitor].desk_count > 1);
    int is_same_monitor = desk_from->on_monitor == target_desk->on_monitor;
    is_same_monitor ? log_to_file(wms, "Moving window: Monitor is the same (%d)\n", desk_from->on_monitor) : log_to_file(wms, "Moving window: Monitors are different (%d and %d)\n", desk_from->on_monitor, target_desk->on_monitor);
    if(is_desk_create) {
        log_to_file(wms, "Moving window: Target desktop to be \"created\"\n");
        target_desk->on_monitor = desk_from->on_monitor;
        wms->monitors[target_desk->on_monitor].desk_count++;
    }
    int new_wnd_idx = move_wnd_to_desk(desk_from, desk_from->curr_wnd, target_desk);
    log_to_file(wms, "Moving window: Moved window, new idx is %d\n", new_wnd_idx);
    target_desk->curr_wnd = new_wnd_idx;
    // desk_from->curr_wnd--;
    int is_desk_destroy = !desk_from->wnd_count && wms->monitors[desk_from->on_monitor].desk_count > 1;
    // TODO optimize
    InvalidateRect(desk_from->button.hwnd, 0, TRUE);
    InvalidateRect(target_desk->button.hwnd, 0, TRUE);
    if(is_desk_destroy) {
        log_to_file(wms, "Moving window: Desktop from which window was moved to be \"destroyed\"\n");
        switch_desc_to(wms, desk);
        reposition_buttons(wms);
        return 0;
    }
    if(!target_desk->wnd_list[new_wnd_idx].is_freeroam) init_curr_mode_reposition(desk_from, &wms->monitors[desk_from->on_monitor]);
    target_desk->changed = is_same_monitor;
    if(!is_same_monitor) init_curr_mode_reposition(target_desk, &wms->monitors[target_desk->on_monitor]);
    reposition_buttons(wms);
    return 0;
}

int handle_keydown(int keyCode, int shift, int ctrl) {
    int prev_act_idx = g_curr_desk.curr_wnd;
    switch (keyCode)
        {
        case VK_LEFT:
        case VK_UP:
            if(g_curr_desk.curr_wnd <= 0 && g_curr_desk.tiling_count >= 0) {
                if(!wiman_config.is_stack_mode_infinite_scroll) break;
                g_curr_desk.curr_wnd = g_curr_desk.tiling_count;
            }
            g_curr_desk.curr_wnd--;
            goto switch_wnd;
        case VK_RIGHT:
        case VK_DOWN:
            if(g_curr_desk.curr_wnd >= g_curr_desk.tiling_count - 1) {
                if(!wiman_config.is_stack_mode_infinite_scroll) break;
                g_curr_desk.curr_wnd = -1;
            }
            g_curr_desk.curr_wnd++;
            switch_wnd:
            WINDOWPLACEMENT wp = {
                .showCmd = SC_RESTORE,
                .length = sizeof(WINDOWPLACEMENT)
            };
            RECT wr;
            if(shift && g_curr_desk.mode != WMM_STACK) {
                switch_hwnds_pos(&g_curr_desk.wnd_list, prev_act_idx, g_curr_desk.curr_wnd);
                return 2 - focus_act_window(&g_curr_desk, g_curr_desk.curr_wnd);
            }
            return 2 - focus_act_window(&g_curr_desk, prev_act_idx);
        case 'W':
            if(g_curr_desk.mode == WMM_STACK) break;
            g_curr_desk.mode = WMM_STACK;
            return 2 - init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
        case 'E':
            g_curr_desk.mode = (g_curr_desk.mode == WMM_TILING_V) + 1;
            return 2 - init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
        case VK_SPACE:
            if(toggle_wnd_freeroam(&g_curr_desk, g_curr_desk.curr_wnd, &wms.monitors[g_curr_desk.on_monitor]) == 2) break;
            init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            break;
        case 'D':
            print_debug_info(&wms, shift);
            return 2;
        case 49 ... 57:
            int target = keyCode - 49;
            WIMAN_DESKTOP_STATE *target_desk = &wms.desk_list[target];
            if(shift) return 2 - send_wnd_to_desk(&wms, g_curr_desk.curr_wnd, wms.curr_desk, target);
            if(ctrl) return 2 - send_desk_to_monitor(&wms, wms.curr_desk, target);
            return 2 - switch_desc_to(&wms, target);
        // case 'R':
        //     free(g_curr_desk.wnd_list);
        //     g_curr_desk.wnd_list = calloc(0, sizeof(WIMAN_WINDOW));
        //     // g_curr_desk.wnd_count = 0;
        //     EnumWindows(enum_wnd, (LPARAM)&wms);
        //     g_curr_desk.tiling_count = g_curr_desk.wnd_count;
        //     init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
        //     break;
    }
    return 2;
}

LRESULT CALLBACK main_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_SHELLHOOKMESSAGE) {
        #define chwnd (HWND)lParam
        // if(wParam == HSHELL_MONITORCHANGED) {
        //     printf("Monitor has changed (event)\n");
        // }
        switch(wParam) {
            case HSHELL_WINDOWACTIVATED: {
                long curr_wnd_style = GetWindowLongPtrA(get_curr_hwnd(g_curr_desk), GWL_STYLE);
                if(!is_actual_window(chwnd)) return DefWindowProc(hwnd, uMsg, wParam, lParam);
                int monitor_id = get_monitor_id_by_hmonitor(&wms, MonitorFromWindow(chwnd, MONITOR_DEFAULTTONEAREST));
                if(monitor_id != g_curr_desk.on_monitor) {
                    log_to_file(&wms, "Monitor has changed to %d\n", monitor_id);
                    InvalidateRect(g_curr_desk.button.hwnd, 0, TRUE);
                    wms.curr_desk = wms.monitors[monitor_id].front_desk;
                    InvalidateRect(g_curr_desk.button.hwnd, 0, TRUE);
                }
                int wnd_id = get_wnd_id_by_hwnd(&g_curr_desk, chwnd);
                if(wnd_id == -1) goto add_window;
                log_to_file(&wms, "Window %d activated\n", wnd_id);
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
                log_to_file(&wms, "New window is opened\n");
                InvalidateRect(g_curr_desk.button.hwnd, 0, TRUE);
                long wnd_style = GetWindowLongPtrA(chwnd, GWL_STYLE);
                if(!is_resizable(wnd_style)) {
                    order_wnd_z(chwnd, HWND_TOPMOST);
                    append_new_wnd(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, (WIMAN_WINDOW){ .hwnd = chwnd, .is_unresizable = TRUE, .is_freeroam = TRUE });
                    break;
                }
                insert_new_wnd(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, (WIMAN_WINDOW){ .hwnd = chwnd }, g_curr_desk.tiling_count++ - 1);
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
        case WM_NOTIFYICON: {
            POINT cur = curr_from_lp(wParam);
            printf("cursor on %ld %ld\n", cur.x, cur.y);
            switch(LOWORD(lParam)) {
                case WM_LBUTTONUP:
                case WM_RBUTTONUP: {
                    HMENU hm = CreatePopupMenu();
                    InsertMenu(hm, 0, MF_BYPOSITION | MF_STRING, OPTION_EXIT, "Exit");
                    InsertMenu(hm, 0, MF_BYPOSITION | MF_STRING, OPTION_SETTINGS, "Settings");
                    SetForegroundWindow(wms.main_hwnd);
                    switch(TrackPopupMenu(hm, TPM_VERNEGANIMATION | TPM_RETURNCMD | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_LEFTALIGN, cur.x, cur.y, 0, wms.main_hwnd, NULL)) {
                        PostMessage(wms.main_hwnd, WM_NULL, 0, 0);
                        case OPTION_EXIT: {
                            SendMessage(wms.main_hwnd, WM_CLOSE, 0, 0);
                            return 0;
                        }
                    }
                }
            }
            return 0;
        }
        case WM_KEYDOWN: {
            return handle_keydown((int)wParam, HIWORD(lParam), LOWORD(lParam));
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            UnregisterClassA(WINDOW_CLASSNAME, *wms.h_instance);
            break;
        case WM_CLOSE:
            log_to_file(&wms, "Exiting 2wiman\n");
            log_to_file(&wms, "Returning windows to their original positions\n");
            for(int i = 0; i < DESK_COUNT; i++) {
                for(int j = 0; j < wms.desk_list[i].wnd_count; j++) {
                    WIMAN_WINDOW *wnd = &wms.desk_list[i].wnd_list[j];
                    ShowWindow(wnd->hwnd, SW_SHOW);
                    SetWindowPos(wnd->hwnd, HWND_TOPMOST, wnd->initial_pos.left, wnd->initial_pos.top, wnd->initial_pos.right - wnd->initial_pos.left, wnd->initial_pos.bottom - wnd->initial_pos.top, 0);
                }
                free(wms.desk_list[i].wnd_list);
            }
            free(wms.monitors);
            fclose(wms.logfile);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION && is_pressed(MODIFIER_KEY))
    {
        PostMessageA(wms.main_hwnd, WM_KEYDOWN, key->vkCode, is_pressed(VK_SHIFT) << 16 | is_pressed(VK_CONTROL));
        return 2;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK button_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            int desk = GetWindowLongPtrA(hwnd, -21);
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
                    SetWindowPos(hwnd, 0, desk->button.last_set_left_top.x, desk->button.last_set_left_top.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);
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
            wms.desk_list[desk].button.is_hovered = FALSE;
            wms.desk_list[desk].button.is_tracking = FALSE;
            InvalidateRect(hwnd, 0, TRUE);
            return 0;
        }
        case WM_MOUSEMOVE: {
            int desk = GetWindowLongPtrA(hwnd, -21);
            if(wms.desk_list[desk].button.is_tracking) return 0;
            track_mouse_for(hwnd, hover_or_leave(!wms.desk_list[desk].button.is_hovered));
            wms.desk_list[desk].button.is_tracking = TRUE;
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            UnregisterClassA(BUTTON_CLASSNAME, *wms.h_instance);
            break;
        case WM_CLOSE:
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            int desk = GetWindowLongPtrA(hwnd, -21);
            HDC hdc = BeginPaint(hwnd, &ps);
            log_to_file(&wms, "GUI: Repainting button %d\n", desk);
            WIN_MONITOR *target_monitor = &wms.monitors[wms.desk_list[desk].on_monitor];
            int is_desk_last = desk >= target_monitor->desk_count - 1;
            int is_active = desk == wms.curr_desk;
            int is_hovered = wms.desk_list[desk].button.is_hovered && !is_active;
            char buffer[2];
            RECT pos_r = { ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.left + apply_dpi_factor(wiman_config.button_size.x, target_monitor->dpi_factor), ps.rcPaint.top + apply_dpi_factor(wiman_config.button_size.y, target_monitor->dpi_factor) };
            int scaled_tile_offset = apply_dpi_factor(TILE_OFFSET, target_monitor->dpi_factor);
            RECT inner_tile = { pos_r.left + scaled_tile_offset, pos_r.top + scaled_tile_offset, pos_r.right - scaled_tile_offset, pos_r.bottom - scaled_tile_offset };
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

void winevent_proc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime
) {
    if(!is_actual_window(hwnd)) return;
    int wnd_id = get_wnd_id_by_hwnd(&g_curr_desk, hwnd);
    if(wnd_id == -1) return;
    switch(event) {
        // statechange works for indentifying the closing of a window somehow
        case EVENT_OBJECT_STATECHANGE:
        case EVENT_SYSTEM_MINIMIZESTART: {
            log_to_file(&wms, "Assuming that window %d is closing...\n", wnd_id);
            g_curr_desk.tiling_count -= !g_curr_desk.wnd_list[wnd_id].is_freeroam;
            remove_wnd_by_idx(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, wnd_id);
            if(g_curr_desk.wnd_count) init_curr_mode_reposition(&g_curr_desk, &wms.monitors[g_curr_desk.on_monitor]);
            InvalidateRect(g_curr_desk.button.hwnd, 0, TRUE);
            break;
        }
        case EVENT_SYSTEM_MOVESIZEEND:
            RECT *lsp = &g_curr_desk.wnd_list[wnd_id].last_set_pos;
            RECT gwr;
            GetWindowRect(hwnd, &gwr);
            if(!is_wh_equal_rect((*lsp), gwr)) {
                log_to_file(&wms, "Window %d is resized\n", wnd_id);
                if(g_curr_desk.wnd_list[wnd_id].is_freeroam) break;
                position_window(hwnd, lsp);
                break;
            }
            POINT clp;
            GetCursorPos(&clp);
            log_to_file(&wms, "Window %d is moved, cursor was on %ld %ld\n", clp.x, clp.y);
            int monitor_id = get_monitor_id_by_cursor(&wms, clp);
            if(monitor_id != g_curr_desk.on_monitor) {
                log_to_file(&wms, "Window %d is moved: from desktop %d (monitor %d) to desktop %d (monitor %d)\n", wnd_id, wms.curr_desk, wms.desk_list[wms.curr_desk].on_monitor, wms.monitors[monitor_id].front_desk, monitor_id);
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
            int new_place;
            switch(g_curr_desk.mode) {
                case WMM_STACK:
                    position_window(hwnd, lsp);
                    ShowWindow(hwnd, SW_MAXIMIZE);
                    break;
                case WMM_TILING_V:
                    new_place = g_curr_desk.tiling_count * clp.x / wms.monitors[g_curr_desk.on_monitor].w;
                    goto place;
                case WMM_TILING_H:
                    new_place = g_curr_desk.tiling_count * clp.y / wms.monitors[g_curr_desk.on_monitor].h;
                    place:if(new_place == wnd_id) {
                        position_window(hwnd, lsp);
                        break;
                    }
                    switch_hwnds_pos(&g_curr_desk.wnd_list, wnd_id, new_place);
                    break;
                default:
                    break;
            }
            g_curr_desk.curr_wnd = new_place;
            SetActiveWindow(hwnd);
            break;
    }
}

// TODO FIX sending desktops to different monitors doesn't work properly
// (fixed, the solution is questionable) TODO find a way to detect when a single window on a desktop is closing or minimizing (previous solution expectidly didn't work fully)
// TODO add plus button on every monitor
// (idk how honestly) TODO fetch hidden windows too
// TODO .IDEA handle movesizestart event and assign window id to dragging_this and resizing_this flags in state
// TODO somehow handle monitor changed event, ideally on cursor switch to it.
// TODO FIX sometimes app crashes on window open
// TODO FIX some fullscreen apps (games) doesn't allow to capture key input and doesn't hide/show properly
// (works, needs further testing) TODO .IDEA if cannot set the position make it freeroam and add some flag like is_permanenty_freeroam (is_unresizable)
// TODO FIX spotify rerenders ui on every reposition
// (currently and probably at all undoable) TODO FIX weird margins around some windows
// TODO .IDEA handle WM_DPICHANGED
// TODO .IDEA use deferwindowpos and enddeferwindowpos for tiling windows
// TODO FIX sometimes weird things happen when opening a window or switching windows while there are freeroam ones
// TODO closing windows shortcut
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    fopen_s(&wms.logfile, "2wiman.log", "w");
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    wms.h_instance = &hInstance;

    WIMAN_CONFIG test_cfg = {};
    printf("asdhuasuhd %d %d %ld %ld %d\n", test_cfg.is_stack_mode_infinite_scroll, test_cfg.default_mode, test_cfg.button_size.x, test_cfg.button_size.y, test_cfg.is_scale_with_dpi);
    parse_config_file(&test_cfg, CONFIG_BUFFER, strlen(CONFIG_BUFFER));
    printf("asdhuasuhd %d %d %ld %ld %d\n", test_cfg.is_stack_mode_infinite_scroll, test_cfg.default_mode, test_cfg.button_size.x, test_cfg.button_size.y, test_cfg.is_scale_with_dpi);

    log_to_file(&wms, "Registering window classes\n");
    wms.wc.main = (WNDCLASS){ .lpfnWndProc = main_window_proc, .hInstance = hInstance, .lpszClassName = WINDOW_CLASSNAME };
    RegisterClass(&wms.wc.main);
    wms.wc.button = (WNDCLASS){ .lpfnWndProc = button_window_proc, .hInstance = hInstance, .lpszClassName = BUTTON_CLASSNAME, .cbWndExtra = sizeof(int) };
    RegisterClass(&wms.wc.button);
    wms.wc.settings = (WNDCLASS){ .lpfnWndProc = button_window_proc, .hInstance = hInstance, .lpszClassName = BUTTON_CLASSNAME, .cbWndExtra = sizeof(int) };
    RegisterClass(&wms.wc.settings);

    log_to_file(&wms, "Creating main window\n");
    wms.main_hwnd = CreateWindowExA(0, WINDOW_CLASSNAME, "mywindow", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    log_to_file(&wms, "Initializing hooks\n");
    HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_proc, NULL, 0);
    HWINEVENTHOOK msg_hook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_OBJECT_STATECHANGE, NULL, winevent_proc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    RegisterShellHookWindow(wms.main_hwnd);
    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(TEXT("SHELLHOOK"));

    log_to_file(&wms, "Setting up tray icon\n");
    int icon_uid = 1;
    NOTIFYICONDATAA icon_data = {
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = wms.main_hwnd,
        .uID = icon_uid,
        .uFlags = NIF_MESSAGE,
        .uVersion = NOTIFYICON_VERSION_4,
        .uCallbackMessage = WM_NOTIFYICON,
    };
    Shell_NotifyIconA(NIM_ADD, &icon_data);
    Shell_NotifyIconA(NIM_SETVERSION, &icon_data);
    
    log_to_file(&wms, "Fetching windows\n");
    wms.curr_desk = 0;
    g_curr_desk = (WIMAN_DESKTOP_STATE){ .curr_wnd = -1 };
    EnumWindows(enum_wnd, (LPARAM)&wms);
    log_to_file(&wms, "Fetched %lld windows, %lld tiling\n", g_curr_desk.wnd_count, g_curr_desk.tiling_count);
    if(g_curr_desk.wnd_count > 0) g_curr_desk.curr_wnd = 0;

    log_to_file(&wms, "Fetching monitors\n");
    wms.monitors = calloc(0, sizeof(WIN_MONITOR));
    EnumDisplayMonitors(NULL, NULL, enum_monitors, (LPARAM)&wms);
    log_to_file(&wms, "Fetched %lld monitors\n", wms.monitor_count);
    for(int i = wms.monitor_count; i < DESK_COUNT; i++) {
        wms.desk_list[i].on_monitor = -1;
        wms.desk_list[i].button.is_tracking = TRUE;
        wms.desk_list[i].button.hwnd = create_button(&hInstance, wms.main_hwnd, i);
    }

    init_curr_mode_reposition(&wms.desk_list[wms.curr_desk], &wms.monitors[wms.desk_list[wms.curr_desk].on_monitor]);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}