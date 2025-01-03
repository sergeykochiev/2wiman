#ifndef UNICODE
#define UNICODE
#endif 
#include "C:\\Users\\dupa\\gcc\\x86_64-w64-mingw32\\include\\windows.h"
#include <stdio.h>
#include <wchar.h>
#include <math.h>

#define MODIFIER_KEY VK_CAPITAL
#define MAGIC_NUMBER WS_EX_NOREDIRECTIONBITMAP
#define is_param_in_xor(xor, param) ((xor | param) == xor)
#define is_wh_equal_rect(a, b) (((a.bottom - a.top) == (b.bottom - b.top)) && ((a.right - a.left) == (b.right - b.left)))

typedef struct {
    int stack_mode_infinite_scroll;
} WIMAN_CONFIG;

typedef struct {
    HWND hwnd;
    RECT last_set_pos;
    int is_freeroam;
    int is_unresizable;
} WIMAN_WINDOW;

typedef enum {
    WMM_TILING_V,
    WMM_TILING_H,
    WMM_STACK,
    WMM_COUNT,
} WIMAN_MODE;

typedef struct {
    int w;
    int h;
} MONITOR_SIZE;

typedef struct {
    WIMAN_WINDOW *wnd_list;
    size_t wnd_count;
    size_t tiling_count;
    int curr_wnd;
    WIMAN_MODE mode;
} WIMAN_DESKTOP_STATE;

typedef struct {
    WIMAN_DESKTOP_STATE *desk_list;
    size_t desk_count;
    MONITOR_SIZE monitor_size;
    size_t curr_desk;
} WIMAN_STATE;

WIMAN_STATE wms = {};

WIMAN_CONFIG wiman_config = {
    .stack_mode_infinite_scroll = FALSE,
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

void print_ex_styles(long ex_styles) {
    if(is_param_in_xor(ex_styles, WS_EX_ACCEPTFILES)) printf("WS_EX_ACCEPTFILES, ");
    if(is_param_in_xor(ex_styles, WS_EX_APPWINDOW)) printf("WS_EX_APPWINDOW, ");
    if(is_param_in_xor(ex_styles, WS_EX_CLIENTEDGE)) printf("WS_EX_CLIENTEDGE, ");
    if(is_param_in_xor(ex_styles, WS_EX_COMPOSITED)) printf("WS_EX_COMPOSITED, ");
    if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("WS_EX_CONTEXTHELP, ");
    if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("WS_EX_CONTEXTHELP, ");
    if(is_param_in_xor(ex_styles, WS_EX_CONTROLPARENT)) printf("WS_EX_CONTROLPARENT, ");
    if(is_param_in_xor(ex_styles, WS_EX_DLGMODALFRAME)) printf("WS_EX_DLGMODALFRAME, ");
    if(is_param_in_xor(ex_styles, WS_EX_LAYERED)) printf("WS_EX_LAYERED, ");
    if(is_param_in_xor(ex_styles, WS_EX_LAYOUTRTL)) printf("WS_EX_LAYOUTRTL, ");
    if(is_param_in_xor(ex_styles, WS_EX_LEFT)) printf("WS_EX_LEFT, ");
    if(is_param_in_xor(ex_styles, WS_EX_LEFTSCROLLBAR)) printf("WS_EX_LEFTSCROLLBAR, ");
    if(is_param_in_xor(ex_styles, WS_EX_LTRREADING)) printf("WS_EX_LTRREADING, ");
    if(is_param_in_xor(ex_styles, WS_EX_MDICHILD)) printf("WS_EX_MDICHILD, ");
    if(is_param_in_xor(ex_styles, WS_EX_NOACTIVATE)) printf("WS_EX_NOACTIVATE, ");
    if(is_param_in_xor(ex_styles, WS_EX_NOINHERITLAYOUT)) printf("WS_EX_NOINHERITLAYOUT, ");
    if(is_param_in_xor(ex_styles, WS_EX_NOPARENTNOTIFY)) printf("WS_EX_NOPARENTNOTIFY, ");
    if(is_param_in_xor(ex_styles, WS_EX_NOREDIRECTIONBITMAP)) printf("WS_EX_NOREDIRECTIONBITMAP, ");
    if(is_param_in_xor(ex_styles, WS_EX_OVERLAPPEDWINDOW)) printf("WS_EX_OVERLAPPEDWINDOW, ");
    if(is_param_in_xor(ex_styles, WS_EX_PALETTEWINDOW)) printf("WS_EX_PALETTEWINDOW, ");
    if(is_param_in_xor(ex_styles, WS_EX_RIGHT)) printf("WS_EX_RIGHT, ");
    if(is_param_in_xor(ex_styles, WS_EX_RIGHTSCROLLBAR)) printf("WS_EX_RIGHTSCROLLBAR, ");
    if(is_param_in_xor(ex_styles, WS_EX_RTLREADING)) printf("WS_EX_RTLREADING, ");
    if(is_param_in_xor(ex_styles, WS_EX_STATICEDGE)) printf("WS_EX_STATICEDGE, ");
    if(is_param_in_xor(ex_styles, WS_EX_TOOLWINDOW)) printf("WS_EX_TOOLWINDOW, ");
    if(is_param_in_xor(ex_styles, WS_EX_TOPMOST)) printf("WS_EX_TOPMOST, ");
    if(is_param_in_xor(ex_styles, WS_EX_TRANSPARENT)) printf("WS_EX_TRANSPARENT, ");
    if(is_param_in_xor(ex_styles, WS_EX_WINDOWEDGE)) printf("WS_EX_WINDOWEDGE, ");
}

void print_styles(long styles) {
    if(is_param_in_xor(styles, WS_BORDER)) printf("WS_BORDER, ");
    if(is_param_in_xor(styles, WS_CAPTION)) printf("WS_CAPTION, ");
    if(is_param_in_xor(styles, WS_CHILD)) printf("WS_CHILD, ");
    if(is_param_in_xor(styles, WS_CHILDWINDOW)) printf("WS_CHILDWINDOW, ");
    if(is_param_in_xor(styles, WS_CLIPCHILDREN)) printf("WS_CLIPCHILDREN, ");
    if(is_param_in_xor(styles, WS_CLIPSIBLINGS)) printf("WS_CLIPSIBLINGS, ");
    if(is_param_in_xor(styles, WS_DISABLED)) printf("WS_DISABLED, ");
    if(is_param_in_xor(styles, WS_DLGFRAME)) printf("WS_DLGFRAME, ");
    if(is_param_in_xor(styles, WS_GROUP)) printf("WS_GROUP, ");
    if(is_param_in_xor(styles, WS_HSCROLL)) printf("WS_HSCROLL, ");
    if(is_param_in_xor(styles, WS_ICONIC)) printf("WS_ICONIC, ");
    if(is_param_in_xor(styles, WS_MAXIMIZE)) printf("WS_MAXIMIZE, ");
    if(is_param_in_xor(styles, WS_MAXIMIZEBOX)) printf("WS_MAXIMIZEBOX, ");
    if(is_param_in_xor(styles, WS_MINIMIZE)) printf("WS_MINIMIZE, ");
    if(is_param_in_xor(styles, WS_MINIMIZEBOX)) printf("WS_MINIMIZEBOX, ");
    if(is_param_in_xor(styles, WS_OVERLAPPED)) printf("WS_OVERLAPPED, ");
    if(is_param_in_xor(styles, WS_OVERLAPPEDWINDOW)) printf("WS_OVERLAPPEDWINDOW, ");
    if(is_param_in_xor(styles, WS_POPUPWINDOW)) printf("WS_POPUPWINDOW, ");
    if(is_param_in_xor(styles, WS_SIZEBOX)) printf("WS_SIZEBOX, ");
    if(is_param_in_xor(styles, WS_SYSMENU)) printf("WS_SYSMENU, ");
    if(is_param_in_xor(styles, WS_TABSTOP)) printf("WS_TABSTOP, ");
    if(is_param_in_xor(styles, WS_THICKFRAME)) printf("WS_THICKFRAME, ");
    if(is_param_in_xor(styles, WS_TILED)) printf("WS_TILED, ");
    if(is_param_in_xor(styles, WS_TILEDWINDOW)) printf("WS_TILEDWINDOW, ");
    if(is_param_in_xor(styles, WS_VISIBLE)) printf("WS_VISIBLE, ");
    if(is_param_in_xor(styles, WS_VSCROLL)) printf("WS_VSCROLL, ");
}

void print_rect(RECT *rect) {
    printf("left %ld, top %ld, right %ld, bottom %ld", rect->left , rect->top, rect->right, rect->bottom);
}

void print_windowinfo(WINDOWINFO *info) {
    printf("      cdSize: %lu\n", info->cbSize);
    printf("      rcWindow: ");
    print_rect(&info->rcWindow);
    printf("\n");
    printf("      rcClient: ");
    print_rect(&info->rcClient);
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

#define get_curr_hwnd(wmds) (wmds).wnd_list[(wmds).curr_wnd].hwnd
#define g_curr_desk wms.desk_list[wms.curr_desk]

void switch_windows_in_list(WIMAN_WINDOW **wndl, int first, int second) {
    if(first == second) return;
    printf("Switching %d and %d...\n", first, second);
    WIMAN_WINDOW buffer = (*wndl)[first];
    (*wndl)[first] = (*wndl)[second];
    (*wndl)[second] = buffer;
}

int append_new_wnd(WIMAN_WINDOW **wndl, size_t *len, HWND new_hwnd) {
    (*len)++;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, sizeof(WIMAN_WINDOW) * *len);
    if(new_wndl == NULL) {
        return 1;
    }
    *wndl = new_wndl;
    (*wndl)[(*len) - 1] = (WIMAN_WINDOW){
        .hwnd = new_hwnd,
        .is_freeroam = FALSE
    };
    return 0;
}

int insert_new_window(WIMAN_WINDOW **wndl, size_t *len, HWND new_hwnd, int after) {
    (*len)++;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, sizeof(WIMAN_WINDOW) * *len);
    if(new_wndl == NULL) return 1;
    *wndl = new_wndl;
    if(after < *len - 1) memmove_s(*wndl + after + 2, (*len - after - 2) * sizeof(WIMAN_WINDOW), *wndl + after + 1, (*len - after - 2) * sizeof(WIMAN_WINDOW));
    (*wndl)[after + 1] = (WIMAN_WINDOW){
        .hwnd = new_hwnd,
        .is_freeroam = FALSE
    };
    return 0;
}

int print_windows_list(WIMAN_WINDOW **wndl, size_t len) {
    printf("* Windows list:\n");
    char *title = malloc(0);
    int length;
    WINDOWINFO wi;
    for(int i = 0; i < len; i++) {
        #define wnd (*wndl)[i]
        length = GetWindowTextLengthA(wnd.hwnd);
        char *new_title = realloc(title, length + 1);
        if(new_title == NULL) {
            printf("Error printing windows list, returning...\n");
            return 1;
        }
        title = new_title;
        GetWindowTextA(wnd.hwnd, title, length + 1);
        printf("  - Window %d%s: \"%s\"\n", i + 1, wnd.is_freeroam ? " (freeroam)" : "", title);
        GetWindowInfo(wnd.hwnd, &wi);
        print_windowinfo(&wi);
    }
    printf("* Windows count: %lld\n", len);
    free(title);
    #undef wnd
    return 0;
}

// TODO write new to use WIMAN_STATE
void print_debug_info(WIMAN_DESKTOP_STATE *wmds) {
    printf("============DEBUG-INFO============\n");
    print_windows_list(&wmds->wnd_list, wmds->wnd_count);
    printf("* Current window: %d\n", wmds->curr_wnd);
    printf("* Number of tiling windows: %lld\n", wmds->tiling_count);
    printf("==========END-DEBUG-INFO==========\n");
}

// TODO doesn't work
int remove_window_by_idx(WIMAN_WINDOW **wndl, size_t *len, int idx) {
    if(idx < *len - 1) memmove_s(*wndl + idx, (*len - idx) * sizeof(WIMAN_WINDOW), *wndl + idx + 1, (*len - idx - 1) * sizeof(WIMAN_WINDOW));
    (*len)--;
    WIMAN_WINDOW *new_wndl = realloc(*wndl, *len * sizeof(WIMAN_WINDOW));
    if(new_wndl == NULL) {
        return 1;
    }
    *wndl = new_wndl;
    return 0;
}

int get_window_id_by_hwnd(WIMAN_DESKTOP_STATE *wmds, HWND hwnd) {
    for(int i = 0; i < wmds->wnd_count; i++) {
        if(wmds->wnd_list[i].hwnd == hwnd) return i;
    }
    return -1;
}

int is_actual_window(HWND hwnd) {
    if(GetWindowTextLengthA(hwnd) == 0 || !IsWindowVisible(hwnd)) return FALSE;
    long dxExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    if(is_param_in_xor(dxExStyle, MAGIC_NUMBER)) return FALSE;
    return TRUE;
}

HWND create_main_window(HINSTANCE *hInstance) {
    const wchar_t ClassName[] = L"mywindow";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = *hInstance;
    wc.lpszClassName = ClassName;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        0,
        ClassName,
        L"mywindow",
        WS_EX_TOPMOST,
        0, 0, wms.monitor_size.w, 80,
        NULL,
        NULL,
        *hInstance,
        NULL
    );
    SetWindowLong(hwnd, GWL_STYLE, 0);
    return hwnd;
}

BOOL CALLBACK fetch_wnds(HWND hwnd, LPARAM lParam) {
    if(!is_actual_window(hwnd)) return TRUE;
    #define wmds ((WIMAN_DESKTOP_STATE*)lParam)
    return !append_new_wnd(&wmds->wnd_list, &wmds->wnd_count, hwnd);
    #undef wmds
}

int position_window(HWND hwnd, WINDOWPLACEMENT *wp, RECT *wr) {
    if(!SetWindowPlacement(hwnd, wp)) return 1;
    GetWindowRect(hwnd, wr);
    printf("Window: %ld, %ld, %ld, %ld\n", wr->left, wr->top, wr->right, wr->bottom);
    if(wr->bottom > wp->rcNormalPosition.bottom || wr->right > wp->rcNormalPosition.right) {
        return !SetWindowPos(hwnd, NULL, wp->rcNormalPosition.left, wp->rcNormalPosition.top, wp->rcNormalPosition.right - wp->rcNormalPosition.left, wp->rcNormalPosition.top - wp->rcNormalPosition.bottom, SWP_DEFERERASE | SWP_NOSENDCHANGING);
    }
    return 0;
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


int toggle_window_freeroam(WIMAN_DESKTOP_STATE *wmds, int idx, MONITOR_SIZE *ms) {
    #define wnd wmds->wnd_list[idx]
    int i = wmds->tiling_count - 1;
    if(!wnd.is_freeroam) {
        int offset = (wmds->wnd_count - wmds->tiling_count) * 32;
        if(!SetWindowPos(wnd.hwnd, HWND_TOPMOST, ms->w / 6 + offset, ms->h / 6 + offset, ms->w / 3 * 2, ms->h / 3 * 2, 0)) return 1;
        wmds->tiling_count--;
    } else {
        if(!SetWindowPos(wnd.hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) return 1;
        i++;
        wmds->tiling_count++;
    }
    switch_windows_in_list(&wmds->wnd_list, idx, i);
    wnd.is_freeroam = !wnd.is_freeroam;
    wmds->curr_wnd = i;
    // SetActiveWindow(curr_wnd.hwnd);
    return 0;
}

// Basically switches to WMM_TILE
int tile_windows_vert(WIMAN_DESKTOP_STATE *wmds, MONITOR_SIZE *ms) {
    printf("Tiling windows vertically...\n");
    int window_w = ms->w / wmds->tiling_count;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){0, 0, window_w, ms->h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp = {};
    for(int i = 0; i < wmds->tiling_count; i++) {
        HWND curr_hwnd = wmds->wnd_list[i].hwnd;
        // TODO firefox doesn't get resized properly
        if(!SetWindowPlacement(curr_hwnd, &p)) return 1;
        GetWindowRect(curr_hwnd, &wp);
        if(wp.right > p.rcNormalPosition.right) {
            if(!SetWindowPos(curr_hwnd, HWND_BOTTOM, p.rcNormalPosition.left, p.rcNormalPosition.top, window_w, ms->h, SWP_DEFERERASE | SWP_NOSENDCHANGING)) return 1;
        }
        wmds->wnd_list[i].last_set_pos = p.rcNormalPosition;
        p.rcNormalPosition.left += window_w;
        p.rcNormalPosition.right += window_w;
    }
    return 0;
}

int tile_windows_horiz(WIMAN_DESKTOP_STATE *wmds, MONITOR_SIZE *ms) {
    printf("Tiling windows...\n");
    int window_h = ms->h / wmds->tiling_count;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){0, 0, ms->w, window_h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp = {};
    for(int i = 0; i < wmds->tiling_count; i++) {
        HWND curr_hwnd = wmds->wnd_list[i].hwnd;
        if(!SetWindowPlacement(curr_hwnd, &p)) return 1;
        GetWindowRect(curr_hwnd, &wp);
        if(wp.bottom > window_h * (i + 1)) {
            if(!SetWindowPos(curr_hwnd, HWND_BOTTOM, p.rcNormalPosition.left, p.rcNormalPosition.top, ms->w, window_h, SWP_DEFERERASE | SWP_NOSENDCHANGING)) return 1;
        }
        wmds->wnd_list[i].last_set_pos = p.rcNormalPosition;
        // TODO i want the same border that window snapping uses around windows
        // long style = GetWindowLongPtrA(curr_hwnd, GWL_STYLE);
        // SetWindowLongPtrA(curr_hwnd, GWL_STYLE, style ^ !WS_SYSMENU);
        p.rcNormalPosition.top += window_h;
        p.rcNormalPosition.bottom += window_h;
    }
    return 0;
}

// Used when in stack mode by refocus_window().
int set_window_ontop(HWND curr) {
    SetWindowPos(curr, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetWindowPos(curr, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetForegroundWindow(curr);
    return 0;
}

// Basically switches to WMM_STACK
int reset_all_to_fullsize(WIMAN_DESKTOP_STATE *wmds, MONITOR_SIZE *ms) {
    printf("Fullsizing windows...\n");
    WINDOWPLACEMENT p = {
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
        .rcNormalPosition = (RECT){0, 0, ms->w, ms->h}
    };
    for(int i = 0; i < wmds->tiling_count; i++) {
        // if(!SetWindowPlacement(state->windows_list.list[i].hwnd, &p)) return 1;
        if(!SetWindowPos(wmds->wnd_list[i].hwnd, HWND_NOTOPMOST, p.rcNormalPosition.left, p.rcNormalPosition.top, p.rcNormalPosition.right, p.rcNormalPosition.bottom, 0)) return 1;
        wmds->wnd_list[i].last_set_pos = p.rcNormalPosition;
    }
    return 0;
}

int focus_act_window(WIMAN_DESKTOP_STATE *wmds, int prev_act_idx) {
    printf("Getting ready to focus active window...\n");
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
int init_curr_mode_reposition(WIMAN_DESKTOP_STATE *wmds, MONITOR_SIZE *ms) {
    printf("Initializing %s mode...\n", WMM_NAMES[wmds->mode]);
    if(wmds->tiling_count == 0) return 0;
    switch(wmds->mode) {
        case WMM_STACK: {
            if(reset_all_to_fullsize(wmds, ms)) return 1;
            return set_window_ontop(get_curr_hwnd(*wmds));
        }
        case WMM_TILING_V: {
            return tile_windows_vert(wmds, ms);
        }
        case WMM_TILING_H: {
            return tile_windows_horiz(wmds, ms);
        }
        default:
            return 0;
    }
}

// TODO FIX sometimes weird things happen when opening a window or switching windows while there are freeroam ones
// (works, needs further testing) TODO FIX when a freeroam window is present and a new window is opened (appended), freeroam window becomes non freeroam and get placed at the same place as the newly opened window
// (currently undoable) TODO .IDEA if cannot set the position make it freeroam and add some flag like is_permanenty_freeroam
// (works, needs further testing) TODO refetch windows on new window open
// (works, needs further testing) TODO refetch windows on window close
// (works, needs further testing) TODO set active_window on actual active window anytime
// TODO virtual desktops
// TODO closing windows shortcut
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    wms.monitor_size.w = GetSystemMetrics(SM_CXSCREEN);
    wms.monitor_size.h = GetSystemMetrics(SM_CYFULLSCREEN);
    printf("Monitor sizes are: %d for width and %d for height\n", wms.monitor_size.w, wms.monitor_size.h);

    HWND main_hwnd = create_main_window(&hInstance);
    // ShowWindow(main_hwnd, nShowCmd);

    HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_proc, NULL, 0);
    HWINEVENTHOOK msg_hook = SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, NULL, wnd_msg_proc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    RegisterShellHookWindow(main_hwnd);

    int icon_uid = 1;
    NOTIFYICONDATAA icon_data = {
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = main_hwnd,
        .uID = icon_uid,
        .uFlags = 0,
    };
    Shell_NotifyIconA(NIM_ADD, &icon_data);
    
    wms.desk_list = calloc(1, sizeof(WIMAN_DESKTOP_STATE));
    wms.curr_desk = 0;
    g_curr_desk = (WIMAN_DESKTOP_STATE){
        .curr_wnd = -1,
    };
    EnumWindows(fetch_wnds, (LPARAM)&g_curr_desk);
    g_curr_desk.tiling_count = g_curr_desk.wnd_count;
    printf("Fetched %lld windows\n", g_curr_desk.wnd_count);
    if(g_curr_desk.wnd_count > 0) g_curr_desk.curr_wnd = 0;

    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(TEXT("SHELLHOOK"));

    init_curr_mode_reposition(&wms.desk_list[wms.curr_desk], &wms.monitor_size);
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
    int modifier_is_pressed = HIWORD(GetKeyState(MODIFIER_KEY)) != 0;
    int prev_act_idx = g_curr_desk.curr_wnd;
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION && modifier_is_pressed)
    {
        switch (key->vkCode)
        {
        case VK_LEFT:
        case VK_UP:
            if(g_curr_desk.curr_wnd<= 0 && g_curr_desk.tiling_count >= 0) {
                if(!wiman_config.stack_mode_infinite_scroll) break;
                g_curr_desk.curr_wnd = g_curr_desk.tiling_count;
            }
            g_curr_desk.curr_wnd--;
            focus_act_window(&g_curr_desk, prev_act_idx);
            break;
        case VK_RIGHT:
        case VK_DOWN:
            if(g_curr_desk.curr_wnd >= g_curr_desk.tiling_count - 1) {
                if(!wiman_config.stack_mode_infinite_scroll) break;
                g_curr_desk.curr_wnd = -1;
            }
            g_curr_desk.curr_wnd++;
            focus_act_window(&g_curr_desk, prev_act_idx);
            break;
        case 'W':
            if(g_curr_desk.mode != WMM_STACK) {
                g_curr_desk.mode = WMM_STACK;
                init_curr_mode_reposition(&g_curr_desk, &wms.monitor_size);
            }
            break;
        case 'E':
            if(g_curr_desk.mode == WMM_TILING_V) {
                g_curr_desk.mode = WMM_TILING_H;
            } else {
                g_curr_desk.mode = WMM_TILING_V;
            }
            init_curr_mode_reposition(&g_curr_desk, &wms.monitor_size);
            break;
        case VK_SPACE:
            toggle_window_freeroam(&g_curr_desk, g_curr_desk.curr_wnd, &wms.monitor_size);
            init_curr_mode_reposition(&g_curr_desk, &wms.monitor_size);
            break;
        case 'R':
            free(g_curr_desk.wnd_list);
            g_curr_desk.wnd_list = calloc(0, sizeof(WIMAN_WINDOW));
            g_curr_desk.wnd_count = 0;
            EnumWindows(fetch_wnds, (LPARAM)&g_curr_desk);
            g_curr_desk.tiling_count = g_curr_desk.wnd_count;
            init_curr_mode_reposition(&g_curr_desk, &wms.monitor_size);
            break;
        case 'D':
            print_debug_info(&g_curr_desk);
        }
        return 1;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_SHELLHOOKMESSAGE) {
        int id;
        #define chwnd (HWND)lParam        
        switch(wParam) {
            case HSHELL_WINDOWACTIVATED:
            case HSHELL_MONITORCHANGED:
                if(!is_actual_window(chwnd)) break;
                id = get_window_id_by_hwnd(&g_curr_desk, chwnd);
                if(id == -1) {
                    goto add_window;
                }
                long curr_wnd_style = GetWindowLongPtrA(get_curr_hwnd(g_curr_desk), GWL_STYLE);
                if(!is_param_in_xor(curr_wnd_style, WS_VISIBLE) || (is_param_in_xor(curr_wnd_style, WS_MINIMIZE) && !g_curr_desk.wnd_list[g_curr_desk.curr_wnd].is_freeroam)) {
                    printf("Assuming that window %d is closing...\n", g_curr_desk.curr_wnd);
                    if(!g_curr_desk.wnd_list[g_curr_desk.curr_wnd].is_freeroam) {
                        g_curr_desk.tiling_count--;
                    }
                    remove_window_by_idx(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, g_curr_desk.curr_wnd);
                    init_curr_mode_reposition(&g_curr_desk, &wms.monitor_size);
                }
                printf("Window %d activated\n", id);
                g_curr_desk.curr_wnd = id;
                break;
            case HSHELL_WINDOWCREATED:
                if(!is_actual_window(chwnd)) break;
                add_window:
                printf("New window is opened\n");
                insert_new_window(&g_curr_desk.wnd_list, &g_curr_desk.wnd_count, chwnd, g_curr_desk.tiling_count - 1);
                g_curr_desk.tiling_count++;
                // print_windows_list(&wiman_state.windows_list, " - ");
                switch(g_curr_desk.mode) {
                    case WMM_STACK:
                        SetWindowPos(chwnd, HWND_TOPMOST, 0, 0, wms.monitor_size.w, wms.monitor_size.h, 0);
                        SetWindowPos(chwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        SetForegroundWindow(chwnd);
                        break;
                    case WMM_TILING_H:
                    case WMM_TILING_V:
                        SetWindowPos(chwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        init_curr_mode_reposition(&g_curr_desk, &wms.monitor_size);
                    default:
                        break;
                }
        }
        #undef hwnd
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        free(g_curr_desk.wnd_list);
        break;
    case WM_CLOSE:
        free(g_curr_desk.wnd_list);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

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
    if(!is_actual_window(hwnd)) return;
    int id = get_window_id_by_hwnd(&g_curr_desk, hwnd);
    if(id == -1) return;
    switch(event) {
        case EVENT_SYSTEM_MOVESIZEEND:
            #define w g_curr_desk.wnd_list[id]
            if(w.is_freeroam) return;
            printf("Window %d moved or/and resized\n", id);
            WINDOWPLACEMENT wp = {
                .length = sizeof(WINDOWPLACEMENT),
                .rcNormalPosition = w.last_set_pos,
                .showCmd = SW_RESTORE
            };
            RECT wr;
            GetWindowRect(hwnd, &wr);
            if(!is_wh_equal_rect(w.last_set_pos, wr)) {
                printf("Windows is actually resized\n");
                position_window(hwnd, &wp, &wr);
                break;
            }
            #undef w
            LPPOINT clp = malloc(sizeof(struct tagPOINT));
            GetCursorPos(clp);
            printf("cursor was on %ld %ld\n", clp->x, clp->y);
            int new_place;
            switch(g_curr_desk.mode) {
                case WMM_STACK:
                    position_window(hwnd, &wp, &wr);
                    break;
                case WMM_TILING_V:
                    new_place = g_curr_desk.tiling_count * clp->x / wms.monitor_size.w;
                    goto place;
                case WMM_TILING_H:
                    new_place = g_curr_desk.tiling_count * clp->y / wms.monitor_size.h;
                    place:if(new_place == id) {
                        position_window(hwnd, &wp, &wr);
                        break;
                    }
                    g_curr_desk.wnd_list[id].hwnd = g_curr_desk.wnd_list[new_place].hwnd;
                    g_curr_desk.wnd_list[new_place].hwnd = hwnd;
                    position_window(g_curr_desk.wnd_list[id].hwnd, &wp, &wr);
                    wp.rcNormalPosition = g_curr_desk.wnd_list[new_place].last_set_pos;
                    position_window(hwnd, &wp, &wr);
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