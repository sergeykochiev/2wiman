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

typedef struct {
    int stack_mode_infinite_scroll;
} WIMAN_CONFIG;

typedef struct {
    HWND hwnd;
    int is_freeroam;
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
    WIMAN_WINDOW *list;
    size_t len;
} HWND_LIST;

typedef struct {
    HWND_LIST windows_list;
    int curr_act_window;
    WIMAN_MODE mode;
    MONITOR_SIZE monitor_size;
    long border_width;
} WIMAN_STATE;

WIMAN_STATE wiman_state = {
    .windows_list = {},
    .curr_act_window = -1,
    .mode = WMM_TILING_V,
};

WIMAN_CONFIG wiman_config = {
    .stack_mode_infinite_scroll = 1,
};

const char* const WMM_NAMES[WMM_COUNT] = {
    [WMM_TILING_V] = "vertical tiling",
    [WMM_TILING_H] = "horizontal tiling",
    [WMM_STACK] = "stack",
};
UINT WM_SHELLHOOKMESSAGE;

LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK call_wnd_proc(int nCode, WPARAM wParam, LPARAM lParam);

void print_ex_styles(long ex_styles, const char* prefix) {
    if(is_param_in_xor(ex_styles, WS_EX_ACCEPTFILES)) printf("%sWS_EX_ACCEPTFILES\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_APPWINDOW)) printf("%sWS_EX_APPWINDOW\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_CLIENTEDGE)) printf("%sWS_EX_CLIENTEDGE\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_COMPOSITED)) printf("%sWS_EX_COMPOSITED\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("%sWS_EX_CONTEXTHELP\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_CONTEXTHELP)) printf("%sWS_EX_CONTEXTHELP\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_CONTROLPARENT)) printf("%sWS_EX_CONTROLPARENT\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_DLGMODALFRAME)) printf("%sWS_EX_DLGMODALFRAME\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_LAYERED)) printf("%sWS_EX_LAYERED\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_LAYOUTRTL)) printf("%sWS_EX_LAYOUTRTL\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_LEFT)) printf("%sWS_EX_LEFT\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_LEFTSCROLLBAR)) printf("%sWS_EX_LEFTSCROLLBAR\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_LTRREADING)) printf("%sWS_EX_LTRREADING\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_MDICHILD)) printf("%sWS_EX_MDICHILD\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_NOACTIVATE)) printf("%sWS_EX_NOACTIVATE\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_NOINHERITLAYOUT)) printf("%sWS_EX_NOINHERITLAYOUT\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_NOPARENTNOTIFY)) printf("%sWS_EX_NOPARENTNOTIFY\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_NOREDIRECTIONBITMAP)) printf("%sWS_EX_NOREDIRECTIONBITMAP\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_OVERLAPPEDWINDOW)) printf("%sWS_EX_OVERLAPPEDWINDOW\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_PALETTEWINDOW)) printf("%sWS_EX_PALETTEWINDOW\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_RIGHT)) printf("%sWS_EX_RIGHT\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_RIGHTSCROLLBAR)) printf("%sWS_EX_RIGHTSCROLLBAR\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_RTLREADING)) printf("%sWS_EX_RTLREADING\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_STATICEDGE)) printf("%sWS_EX_STATICEDGE\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_TOOLWINDOW)) printf("%sWS_EX_TOOLWINDOW\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_TOPMOST)) printf("%sWS_EX_TOPMOST\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_TRANSPARENT)) printf("%sWS_EX_TRANSPARENT\n", prefix);
    if(is_param_in_xor(ex_styles, WS_EX_WINDOWEDGE)) printf("%sWS_EX_WINDOWEDGE\n", prefix);
}

void print_styles(long styles, const char* prefix) {
    if(is_param_in_xor(styles, WS_BORDER)) printf("%sWS_BORDER\n", prefix);
    if(is_param_in_xor(styles, WS_CAPTION)) printf("%sWS_CAPTION\n", prefix);
    if(is_param_in_xor(styles, WS_CHILD)) printf("%sWS_CHILD\n", prefix);
    if(is_param_in_xor(styles, WS_CHILDWINDOW)) printf("%sWS_CHILDWINDOW\n", prefix);
    if(is_param_in_xor(styles, WS_CLIPCHILDREN)) printf("%sWS_CLIPCHILDREN\n", prefix);
    if(is_param_in_xor(styles, WS_CLIPSIBLINGS)) printf("%sWS_CLIPSIBLINGS\n", prefix);
    if(is_param_in_xor(styles, WS_DISABLED)) printf("%sWS_DISABLED\n", prefix);
    if(is_param_in_xor(styles, WS_DLGFRAME)) printf("%sWS_DLGFRAME\n", prefix);
    if(is_param_in_xor(styles, WS_GROUP)) printf("%sWS_GROUP\n", prefix);
    if(is_param_in_xor(styles, WS_HSCROLL)) printf("%sWS_HSCROLL\n", prefix);
    if(is_param_in_xor(styles, WS_ICONIC)) printf("%sWS_ICONIC\n", prefix);
    if(is_param_in_xor(styles, WS_MAXIMIZE)) printf("%sWS_MAXIMIZE\n", prefix);
    if(is_param_in_xor(styles, WS_MAXIMIZEBOX)) printf("%sWS_MAXIMIZEBOX\n", prefix);
    if(is_param_in_xor(styles, WS_MINIMIZE)) printf("%sWS_MINIMIZE\n", prefix);
    if(is_param_in_xor(styles, WS_MINIMIZEBOX)) printf("%sWS_MINIMIZEBOX\n", prefix);
    if(is_param_in_xor(styles, WS_OVERLAPPED)) printf("%sWS_OVERLAPPED\n", prefix);
    if(is_param_in_xor(styles, WS_OVERLAPPEDWINDOW)) printf("%sWS_OVERLAPPEDWINDOW\n", prefix);
    if(is_param_in_xor(styles, WS_POPUPWINDOW)) printf("%sWS_POPUPWINDOW\n", prefix);
    if(is_param_in_xor(styles, WS_SIZEBOX)) printf("%sWS_SIZEBOX\n", prefix);
    if(is_param_in_xor(styles, WS_SYSMENU)) printf("%sWS_SYSMENU\n", prefix);
    if(is_param_in_xor(styles, WS_TABSTOP)) printf("%sWS_TABSTOP\n", prefix);
    if(is_param_in_xor(styles, WS_THICKFRAME)) printf("%sWS_THICKFRAME\n", prefix);
    if(is_param_in_xor(styles, WS_TILED)) printf("%sWS_TILED\n", prefix);
    if(is_param_in_xor(styles, WS_TILEDWINDOW)) printf("%sWS_TILEDWINDOW\n", prefix);
    if(is_param_in_xor(styles, WS_VISIBLE)) printf("%sWS_VISIBLE\n", prefix);
    if(is_param_in_xor(styles, WS_VSCROLL)) printf("%sWS_VSCROLL\n", prefix);
}

void print_windowinfo(WINDOWINFO *info, const char *prefix) {
    printf("WINDOWINFO\n%scdSize: %lu\n", prefix, info->cbSize);
    printf("%srcWindow: left: %lu, top: %lu, right: %lu, bottom: %lu\n", prefix, info->rcWindow.left, info->rcWindow.top,info->rcWindow.right, info->rcWindow.bottom);
    printf("%srcClient: left: %lu, top: %lu, right: %lu, bottom: %lu\n", prefix, info->rcClient.left, info->rcClient.top,info->rcClient.right, info->rcClient.bottom);
    printf("%sdwStyle:\n", prefix);
    print_styles(info->dwStyle, prefix);
    printf("%sdwExStyle:\n", prefix);
    print_styles(info->dwExStyle, prefix);
    printf("%sdwWindowStatus: %lu\n", prefix, info->dwWindowStatus);
    printf("%scxWindowBorders: %u\n", prefix, info->cxWindowBorders);
    printf("%scyWindowBorders: %u\n", prefix, info->cyWindowBorders);
    printf("%satomWindowType: %hu\n", prefix, info->atomWindowType);
    printf("%swCreatorVersion: %hu\n", prefix, info->wCreatorVersion);
}

HWND get_curr_window_hwnd(WIMAN_STATE *state) {
    return state->windows_list.list[state->curr_act_window].hwnd;
}

int append_new_window(HWND_LIST *windows_list, HWND new_hwnd) {
    windows_list->len++;
    WIMAN_WINDOW *new_windows_list = realloc(windows_list->list, sizeof(WIMAN_WINDOW) * windows_list->len);
    if(new_windows_list == NULL) {
        return 1;
    }
    windows_list->list = new_windows_list;
    windows_list->list[windows_list->len - 1] = (WIMAN_WINDOW){
        .hwnd = new_hwnd,
        .is_freeroam = FALSE
    };
    return 0;
}

int print_windows_list(HWND_LIST *windows_list, const char *prefix) {
    printf("Printing windows list...\n");
    char *title = malloc(0);
    int length;
    for(int i = 0; i < windows_list->len; i++) {
        length = GetWindowTextLengthA(windows_list->list[i].hwnd);
        char *new_title = realloc(title, length);
        if(new_title == NULL) {
            printf("Error printing windows list, returning...\n");
            return 1;
        }
        title = new_title;
        GetWindowTextA(windows_list->list[i].hwnd, title, length);
        printf("%s Window %d: %s\n", prefix, i + 1, title);
    }
    free(title);
    return 0;
}

// TODO doesn't work
int remove_window_by_idx(HWND_LIST *windows_list, int idx) {
    // for(int i = idx; i < windows_list->len - 1; i++) {
    //     windows_list->list[i] = windows_list->list[i + 1];
    // }
    memmove_s(windows_list->list + idx, windows_list->len - idx, windows_list->list + idx + 1, windows_list->len - idx - 1);
    windows_list->len--;
    WIMAN_WINDOW *new_windows_list = realloc(windows_list->list, sizeof(WIMAN_WINDOW) * windows_list->len);
    if(new_windows_list == NULL) {
        return 1;
    }
    windows_list->list = new_windows_list;
    print_windows_list(windows_list, "    - ");
    return 0;
}

int get_window_id_by_hwnd(WIMAN_STATE *state, HWND hwnd) {
    for(int i = 0; i < state->windows_list.len; i++) {
        if(state->windows_list.list[i].hwnd == hwnd) return i;
    }
    return -1;
}

int is_actual_window(HWND hwnd) {
    if(GetWindowTextLengthA(hwnd) == 0 || !IsWindowVisible(hwnd)) return FALSE;
    long dxExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    if(is_param_in_xor(dxExStyle, MAGIC_NUMBER)) return FALSE;
    return TRUE;
}

int get_visible_windows(HWND hwnd, LPARAM windows_list) {
    int appendResult;
    if(!is_actual_window(hwnd)) return TRUE;
    appendResult = append_new_window((HWND_LIST*)windows_list, hwnd);
    return !appendResult;
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
        0, 0, wiman_state.monitor_size.w, 80,
        NULL,
        NULL,
        *hInstance,
        NULL
    );
    SetWindowLong(hwnd, GWL_STYLE, 0);
    return hwnd;
}

void fetch_windows_list(HWND_LIST *windows_list) {
    windows_list->list = calloc(0, sizeof(HWND));
    EnumWindows(get_visible_windows, (LPARAM)windows_list);
    printf("Fetched %llu windows\n", windows_list->len);
    return;
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

// Basically switches to WMM_TILE
int tile_windows_vert(WIMAN_STATE *state) {
    printf("Tiling windows...\n");
    int window_w = state->monitor_size.w / state->windows_list.len;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){0, 0, window_w, state->monitor_size.h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp = {};
    for(int i = 0; i < state->windows_list.len; i++) {
        if(state->windows_list.list[i].is_freeroam) continue;
        HWND curr_hwnd = state->windows_list.list[i].hwnd;
        // TODO firefox doesn't get resized properly
        if(!SetWindowPlacement(curr_hwnd, &p)) return 1;
        GetWindowRect(curr_hwnd, &wp);
        printf("Window %d: %ld, %ld, %ld, %ld\n", i + 1, wp.left, wp.top, wp.right, wp.bottom);
        if(wp.right > window_w * (i + 1)) {
            if(!SetWindowPos(curr_hwnd, NULL, p.rcNormalPosition.left, p.rcNormalPosition.top, window_w, state->monitor_size.h, SWP_DEFERERASE | SWP_NOSENDCHANGING)) return 1;
        }
        // TODO arbitrary fix. need to figure out what is actually happenning
        // long style = GetWindowLongPtrA(curr_hwnd, GWL_STYLE);
        // SetWindowLongPtrA(curr_hwnd, GWL_STYLE, style ^ !WS_SYSMENU);
        p.rcNormalPosition.left += window_w;
        p.rcNormalPosition.right += window_w;
    }
    return 0;
}

int tile_windows_horiz(WIMAN_STATE *state) {
    printf("Tiling windows...\n");
    int window_h = state->monitor_size.h / state->windows_list.len;
    WINDOWPLACEMENT p = {
        .rcNormalPosition = (RECT){0, 0, state->monitor_size.w, window_h},
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
    };
    RECT wp = {};
    for(int i = 0; i < state->windows_list.len; i++) {
        if(state->windows_list.list[i].is_freeroam) continue;
        HWND curr_hwnd = state->windows_list.list[i].hwnd;
        // TODO firefox doesn't get resized properly
        if(!SetWindowPlacement(curr_hwnd, &p)) return 1;
        GetWindowRect(curr_hwnd, &wp);
        printf("Window %d: %ld, %ld, %ld, %ld\n", i + 1, wp.left, wp.top, wp.right, wp.bottom);
        if(wp.bottom > window_h * (i + 1)) {
            if(!SetWindowPos(curr_hwnd, NULL, p.rcNormalPosition.left, p.rcNormalPosition.top, state->monitor_size.w, window_h, SWP_DEFERERASE | SWP_NOSENDCHANGING)) return 1;
        }
        // TODO i want the same border that window snapping uses around windows
        // long style = GetWindowLongPtrA(curr_hwnd, GWL_STYLE);
        // SetWindowLongPtrA(curr_hwnd, GWL_STYLE, style ^ !WS_SYSMENU);
        p.rcNormalPosition.top += window_h;
        p.rcNormalPosition.bottom += window_h;
    }
    return 0;
}

// Basically switches to WMM_STACK
int reset_all_to_fullsize(WIMAN_STATE *state) {
    printf("Fullsizing windows...\n");
    WINDOWPLACEMENT p = {
        .length = sizeof(WINDOWPLACEMENT),
        .showCmd = SW_RESTORE,
        .rcNormalPosition = (RECT){0, 0, state->monitor_size.w, state->monitor_size.h}
    };
    for(int i = 0; i < state->windows_list.len; i++) {
        if(!SetWindowPlacement(state->windows_list.list[i].hwnd, &p)) return 1;
    }
    return 0;
}

// Used when in stack mode by refocus_window().
int set_window_ontop(WIMAN_STATE *state, int index) {
    printf("Setting window %d on top...\n", index + 1);
    if(index >= state->windows_list.len) return 1;
    for(int i = 0; i < state->windows_list.len; i++) {
        if(i == index) continue;
        if(!SetWindowPos(state->windows_list.list[i].hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) return 1;
    }
    if(!SetWindowPos(state->windows_list.list[index].hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) return 1;
    // SwitchToThisWindow(state->windows_list.list[index].hwnd, FALSE);
    return !SetForegroundWindow(state->windows_list.list[index].hwnd);
}

int toggle_window_freeroam(WIMAN_STATE *state, int idx) {
    WIMAN_WINDOW *curr_wnd = &state->windows_list.list[idx];
    if(!curr_wnd->is_freeroam) {
        if(!SetWindowPos(curr_wnd->hwnd, HWND_TOPMOST, state->monitor_size.w / 4, state->monitor_size.h / 4, state->monitor_size.w / 2, state->monitor_size.h / 2, 0)) return 1;
    } else {
        if(!SetWindowPos(curr_wnd->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) return 1;
    }
    curr_wnd->is_freeroam = !curr_wnd->is_freeroam;
    return 0;
}

int make_windows_notopmost(WIMAN_STATE *state) {
    printf("Removing topmost from all windows...\n");
    for(int i = 0; i < state->windows_list.len; i++) {
        if(!SetWindowPos(state->windows_list.list[i].hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) return 1;
    }
    return 0;
}

int focus_active_window(WIMAN_STATE *state) {
    printf("Getting ready to focus active window...\n");
    switch(state->mode) {
        case WMM_STACK: {
            return set_window_ontop(state, state->curr_act_window);
        }
        case WMM_TILING_V: {
            return !SetForegroundWindow(get_curr_window_hwnd(state));
        }
        case WMM_COUNT: {
            return 1;
        }
    }
    return 0;
}

// Repositions all windows based on the current mode in state.
int init_curr_mode_reposition(WIMAN_STATE *state) {
    printf("Initializing %s mode...\n", WMM_NAMES[state->mode]);
    if(wiman_state.windows_list.len == 0) return 0;
    if(wiman_state.curr_act_window == -1) {
        wiman_state.curr_act_window = 0;
    }
    switch(wiman_state.mode) {
        case WMM_STACK: {
            if(reset_all_to_fullsize(state)) return 1;
            return set_window_ontop(state, state->curr_act_window);
        }
        case WMM_TILING_V: {
            return tile_windows_vert(state);
        }
        case WMM_TILING_H: {
            return tile_windows_horiz(state);
        }
        case WMM_COUNT: {
            return 1;
        }
    }
}

// TODO refetch windows on new window open (works, needs further testing)
// TODO refetch windows on window close (works, needs further testing)
// TODO virtual desktops
// TODO closing windows shortcut
// TODO set active_window on actual active window anytime (works, needs further testing)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    wiman_state.monitor_size.w = GetSystemMetrics(SM_CXSCREEN);
    wiman_state.monitor_size.h = GetSystemMetrics(SM_CYFULLSCREEN);

    HWND main_hwnd = create_main_window(&hInstance);
    // ShowWindow(main_hwnd, nShowCmd);

    HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_proc, NULL, 0);
    RegisterShellHookWindow(main_hwnd);

    int icon_uid = 1;
    NOTIFYICONDATAA icon_data = {
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = main_hwnd,
        .uID = icon_uid,
        .uFlags = 0,
    };
    Shell_NotifyIconA(NIM_ADD, &icon_data);
    RECT client_rest = {};
    GetClientRect(NULL, &client_rest);
    printf("ClientRect %ld %ld %ld %ld", client_rest.left, client_rest.top, client_rest.right, client_rest.bottom);
    printf("Monitor sizes are: %d for width and %d for height\n", wiman_state.monitor_size.w, wiman_state.monitor_size.h);

    fetch_windows_list(&wiman_state.windows_list);
    if(wiman_state.windows_list.len > 0) {
        wiman_state.curr_act_window = 0;
    }

    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(TEXT("SHELLHOOK"));

    init_curr_mode_reposition(&wiman_state);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK call_wnd_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    #define cwpstruct ((CWPSTRUCT*)lParam)
    printf("WINDOW MESSAGE\n");
    #undef cwpstruct
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    int modifier_is_pressed = HIWORD(GetKeyState(MODIFIER_KEY)) != 0;
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION && modifier_is_pressed)
    {
        switch (key->vkCode)
        {
        case VK_LEFT:
        case VK_UP:
            if(wiman_state.curr_act_window <= 0) {
                if(!wiman_config.stack_mode_infinite_scroll) break;
                wiman_state.curr_act_window = wiman_state.windows_list.len;
            }
            wiman_state.curr_act_window--;
            focus_active_window(&wiman_state);
            return 0;
        case VK_RIGHT:
        case VK_DOWN:
            if(wiman_state.curr_act_window >= wiman_state.windows_list.len - 1) {
                if(!wiman_config.stack_mode_infinite_scroll) break;
                wiman_state.curr_act_window = -1;
            }
            wiman_state.curr_act_window++;
            focus_active_window(&wiman_state);
            return 0;
        case 'W':
            if(wiman_state.mode != WMM_STACK) {
                wiman_state.mode = WMM_STACK;
                init_curr_mode_reposition(&wiman_state);
            }
            return 0;
        case 'E':
            if(wiman_state.mode == WMM_TILING_V) {
                wiman_state.mode = WMM_TILING_H;
            } else {
                wiman_state.mode = WMM_TILING_V;
            }
            init_curr_mode_reposition(&wiman_state);
            return 0;
        case 'F':
            toggle_window_freeroam(&wiman_state, wiman_state.curr_act_window);
            init_curr_mode_reposition(&wiman_state);
            return 0;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_SHELLHOOKMESSAGE) {
        int id;
        #define hwnd (HWND)lParam        
        switch(wParam) {
            case HSHELL_WINDOWACTIVATED:
            case HSHELL_MONITORCHANGED:
                if(!is_actual_window(hwnd)) break;
                id = get_window_id_by_hwnd(&wiman_state, hwnd);
                if(id == -1) break;
                long curr_wnd_style = GetWindowLongPtrA(get_curr_window_hwnd(&wiman_state), GWL_STYLE);
                // TODO this condition should be something different
                if(is_param_in_xor(curr_wnd_style, WS_VISIBLE)) {
                    printf("Window %d activated\n", id);
                    wiman_state.curr_act_window = id;
                } else {
                    printf("Assuming that window %d is closing...\n", wiman_state.curr_act_window);
                    WINDOWINFO wi = {};
                    GetWindowInfo(get_curr_window_hwnd(&wiman_state), &wi);
                    // print_windowinfo(&wi, "    - ");
                    remove_window_by_idx(&wiman_state.windows_list, wiman_state.curr_act_window);
                    init_curr_mode_reposition(&wiman_state);
                    wiman_state.curr_act_window--;
                }
                break;
            case HSHELL_WINDOWCREATED:
                if(!is_actual_window(hwnd)) break;
                printf("New window is opened\n");
                append_new_window(&wiman_state.windows_list, hwnd);
                init_curr_mode_reposition(&wiman_state);
            case HSHELL_REDRAW:
                if(!is_actual_window(hwnd)) break;
                printf("Window needs to redraw\n");
        }
        #undef hwnd
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        make_windows_notopmost(&wiman_state);
        free(wiman_state.windows_list.list);
        break;
    case WM_CLOSE:
        make_windows_notopmost(&wiman_state);
        free(wiman_state.windows_list.list);
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