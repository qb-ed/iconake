// iconake.c - Desktop Snake (v3.4, CRT-free)
//
//   iconake.exe              play
//   iconake.exe --restore    restore the layout backed up before the last run
//   iconake.exe --no-hook    poll GetAsyncKeyState instead of the global hook
//   iconake.exe --no-park    leave unused icons where they are

typedef unsigned int       ULONG;
typedef int                LONG;
typedef int                BOOL;
typedef void*              HWND;
typedef void*              HANDLE;
typedef void*              HHOOK;
typedef void*              HINSTANCE;
typedef unsigned long long ULONG_PTR;
typedef long long          LONG_PTR;
typedef ULONG_PTR          WPARAM;
typedef long long          LPARAM;
typedef long long          LRESULT;
typedef unsigned short     wchar_t;
typedef unsigned int       UINT;
typedef unsigned long      DWORD;
typedef long               HRESULT;

#ifdef _WIN64
typedef unsigned long long SIZE_T_T;
#else
typedef unsigned int       SIZE_T_T;
#endif

#ifdef _WIN64
#define STDMETHODCALLTYPE
#define CALLBACK
#else
#define STDMETHODCALLTYPE __stdcall
#define CALLBACK __stdcall
#endif

#define WINAPI __stdcall

// ---------- constants ----------
#define VK_ESCAPE 0x1B
#define VK_LEFT   0x25
#define VK_UP     0x26
#define VK_RIGHT  0x27
#define VK_DOWN   0x28
#define VK_W      0x57
#define VK_A      0x41
#define VK_S      0x53
#define VK_D      0x44

#define MB_OK         0x00000000
#define MB_ICONERROR  0x00000010
#define MB_ICONINFO   0x00000040
#define MB_ICONWARN   0x00000030

#define COINIT_APARTMENTTHREADED 0x2
#define CLSCTX_ALL               0x17

#define S_OK          0
#define S_FALSE       1
#define FAILED(hr)    ((HRESULT)(hr) < 0)
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)

#define SWC_DESKTOP       0x00000008
#define SWFO_NEEDDISPATCH 0x00000001

#define SVGIO_ALLVIEW     0x00000002
#define SVSI_POSITIONITEM 0x00000080

#define FWF_SNAPTOGRID    0x00000004

#define CSIDL_DESKTOP     0x0000
#define SPI_GETWORKAREA   0x0030

#define SM_XVIRTUALSCREEN  76
#define SM_CXVIRTUALSCREEN 78

#define VT_EMPTY 0
#define VT_I4    3

#define MEM_COMMIT     0x1000
#define MEM_RESERVE    0x2000
#define MEM_RELEASE    0x8000
#define PAGE_READWRITE 0x04

#define MAX_SNAKE 256
#define MAX_ICONS 512

#define PM_REMOVE 0x0001

#define RDW_INVALIDATE  0x0001
#define RDW_ALLCHILDREN 0x0080
#define RDW_UPDATENOW   0x0100

#define WH_KEYBOARD_LL 13
#define WM_KEYDOWN     0x0100
#define WM_SYSKEYDOWN  0x0104

#define GENERIC_READ    0x80000000u
#define GENERIC_WRITE   0x40000000u
#define CREATE_ALWAYS   2
#define OPEN_EXISTING   3
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define INVALID_HANDLE_VALUE  ((HANDLE)(LONG_PTR)-1)
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFFu

#define BK_MAGIC 0x4B4E4349u
#define BK_VER   1u
#define BK_MAXPIDL 4096

// ---------- GUIDs ----------
typedef struct { unsigned long Data1; unsigned short Data2; unsigned short Data3; unsigned char Data4[8]; } GUID;
typedef GUID IID;
typedef GUID CLSID;

static const GUID IID_IShellWindows    = {0x85CB6900,0x4D95,0x11CF,{0x96,0x0C,0x00,0x80,0xC7,0xF4,0xEE,0x85}};
static const GUID CLSID_ShellWindows   = {0x9BA05972,0xF6A8,0x11CF,{0xA4,0x42,0x00,0xA0,0xC9,0x0A,0x8F,0x39}};
static const GUID IID_IServiceProvider = {0x6D5140C1,0x7436,0x11CE,{0x80,0x34,0x00,0xAA,0x00,0x60,0x09,0xFA}};
static const GUID IID_IShellBrowser    = {0x000214E2,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID SID_STopLevelBrowser = {0x4C96BE40,0x915C,0x11CF,{0x99,0xD3,0x00,0xAA,0x00,0x4A,0xE8,0x37}};
static const GUID IID_IFolderView      = {0xCDE725B0,0xCCC9,0x4519,{0x91,0x7E,0x32,0x5D,0x72,0xFA,0xB4,0xCE}};
static const GUID IID_IFolderView2     = {0x1AF3A467,0x214F,0x4298,{0x90,0x8E,0x06,0xB0,0x3E,0x0B,0x39,0xF9}};
static const GUID IID_IEnumIDList      = {0x000214F2,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID IID_IShellView       = {0x000214E3,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

// ---------- VARIANT ----------
typedef struct {
    unsigned short vt;
    unsigned short wReserved1;
    unsigned short wReserved2;
    unsigned short wReserved3;
    union {
        long      lVal;
        long long llVal;
        void*     pv[2];
    } data;
} VARIANT;

// ---------- COM vtables ----------
typedef struct IUnknownVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
} IUnknownVtbl;

typedef struct IShellWindowsVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(void*, UINT*);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(void*, UINT, ULONG, void**);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(void*, const IID*, wchar_t**, UINT, ULONG, long*);
    HRESULT (STDMETHODCALLTYPE *Invoke)(void*, long, const IID*, ULONG, unsigned short, void*, void*, void*, UINT*);
    HRESULT (STDMETHODCALLTYPE *get_Count)(void*, long*);
    HRESULT (STDMETHODCALLTYPE *Item)(void*, VARIANT, void**);
    HRESULT (STDMETHODCALLTYPE *_NewEnum)(void*, void**);
    HRESULT (STDMETHODCALLTYPE *Register)(void*, void*, long, int, long*);
    HRESULT (STDMETHODCALLTYPE *RegisterPending)(void*, long, VARIANT*, VARIANT*, int, long*);
    HRESULT (STDMETHODCALLTYPE *Revoke)(void*, long);
    HRESULT (STDMETHODCALLTYPE *OnNavigate)(void*, long, VARIANT*);
    HRESULT (STDMETHODCALLTYPE *OnActivated)(void*, long, VARIANT);
    HRESULT (STDMETHODCALLTYPE *FindWindowSW)(void*, VARIANT*, VARIANT*, int, long*, int, void**);
    HRESULT (STDMETHODCALLTYPE *OnCreated)(void*, long, void*);
    HRESULT (STDMETHODCALLTYPE *ProcessAttachDetach)(void*, BOOL);
} IShellWindowsVtbl;

typedef struct IServiceProviderVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *QueryService)(void*, const GUID*, const IID*, void**);
} IServiceProviderVtbl;

typedef struct IShellBrowserVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *GetWindow)(void*, HWND*);
    HRESULT (STDMETHODCALLTYPE *ContextSensitiveHelp)(void*, BOOL);
    HRESULT (STDMETHODCALLTYPE *InsertMenusSB)(void*, void*, void*);
    HRESULT (STDMETHODCALLTYPE *SetMenuSB)(void*, void*, void*, void*);
    HRESULT (STDMETHODCALLTYPE *RemoveMenusSB)(void*, void*);
    HRESULT (STDMETHODCALLTYPE *SetStatusTextSB)(void*, const wchar_t*);
    HRESULT (STDMETHODCALLTYPE *EnableModelessSB)(void*, BOOL);
    HRESULT (STDMETHODCALLTYPE *TranslateAcceleratorSB)(void*, void*, unsigned short);
    HRESULT (STDMETHODCALLTYPE *BrowseObject)(void*, void*, UINT);
    HRESULT (STDMETHODCALLTYPE *GetViewStateStream)(void*, DWORD, void**);
    HRESULT (STDMETHODCALLTYPE *GetControlWindow)(void*, UINT, HWND*);
    HRESULT (STDMETHODCALLTYPE *SendControlMsg)(void*, UINT, UINT, WPARAM, LPARAM, LRESULT*);
    HRESULT (STDMETHODCALLTYPE *QueryActiveShellView)(void*, void**);
    HRESULT (STDMETHODCALLTYPE *OnViewWindowActive)(void*, void*);
    HRESULT (STDMETHODCALLTYPE *SetToolbarItems)(void*, void*, UINT, UINT);
} IShellBrowserVtbl;

typedef struct IFolderViewVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *GetCurrentViewMode)(void*, UINT*);
    HRESULT (STDMETHODCALLTYPE *SetCurrentViewMode)(void*, UINT);
    HRESULT (STDMETHODCALLTYPE *GetFolder)(void*, const IID*, void**);
    HRESULT (STDMETHODCALLTYPE *Item)(void*, int, void**);
    HRESULT (STDMETHODCALLTYPE *ItemCount)(void*, UINT, int*);
    HRESULT (STDMETHODCALLTYPE *Items)(void*, UINT, const IID*, void**);
    HRESULT (STDMETHODCALLTYPE *GetSelectionMarkedItem)(void*, int*);
    HRESULT (STDMETHODCALLTYPE *GetFocusedItem)(void*, int*);
    HRESULT (STDMETHODCALLTYPE *GetItemPosition)(void*, const void*, long*);
    HRESULT (STDMETHODCALLTYPE *GetSpacing)(void*, long*);
    HRESULT (STDMETHODCALLTYPE *GetDefaultSpacing)(void*, long*);
    HRESULT (STDMETHODCALLTYPE *GetAutoArrange)(void*);
    HRESULT (STDMETHODCALLTYPE *SelectItem)(void*, int, DWORD);
    HRESULT (STDMETHODCALLTYPE *SelectAndPositionItems)(void*, UINT, const void**, long*, DWORD);
} IFolderViewVtbl;

// IFolderView2: SetRedraw is slot 39 (byte offset 0x138).
typedef struct IFolderView2Vtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *GetCurrentViewMode)(void*, UINT*);
    HRESULT (STDMETHODCALLTYPE *SetCurrentViewMode)(void*, UINT);
    HRESULT (STDMETHODCALLTYPE *GetFolder)(void*, const IID*, void**);
    HRESULT (STDMETHODCALLTYPE *Item)(void*, int, void**);
    HRESULT (STDMETHODCALLTYPE *ItemCount)(void*, UINT, int*);
    HRESULT (STDMETHODCALLTYPE *Items)(void*, UINT, const IID*, void**);
    HRESULT (STDMETHODCALLTYPE *GetSelectionMarkedItem)(void*, int*);
    HRESULT (STDMETHODCALLTYPE *GetFocusedItem)(void*, int*);
    HRESULT (STDMETHODCALLTYPE *GetItemPosition)(void*, const void*, long*);
    HRESULT (STDMETHODCALLTYPE *GetSpacing)(void*, long*);
    HRESULT (STDMETHODCALLTYPE *GetDefaultSpacing)(void*, long*);
    HRESULT (STDMETHODCALLTYPE *GetAutoArrange)(void*);
    HRESULT (STDMETHODCALLTYPE *SelectItem)(void*, int, DWORD);
    HRESULT (STDMETHODCALLTYPE *SelectAndPositionItems)(void*, UINT, const void**, long*, DWORD);
    HRESULT (STDMETHODCALLTYPE *SetGroupBy)(void*, const void*, BOOL);
    HRESULT (STDMETHODCALLTYPE *GetGroupBy)(void*, void*, BOOL*);
    HRESULT (STDMETHODCALLTYPE *SetViewProperty)(void*, const void*, const void*, const void*);
    HRESULT (STDMETHODCALLTYPE *GetViewProperty)(void*, const void*, const void*, void**);
    HRESULT (STDMETHODCALLTYPE *SetTileViewProperties)(void*, const void*, const wchar_t*);
    HRESULT (STDMETHODCALLTYPE *SetExtendedTileViewProperties)(void*, const void*, const wchar_t*);
    HRESULT (STDMETHODCALLTYPE *SetText)(void*, int, const wchar_t*);
    HRESULT (STDMETHODCALLTYPE *SetCurrentFolderFlags)(void*, DWORD, DWORD);
    HRESULT (STDMETHODCALLTYPE *GetCurrentFolderFlags)(void*, DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetSortColumnCount)(void*, int*);
    HRESULT (STDMETHODCALLTYPE *SetSortColumns)(void*, const void*, int);
    HRESULT (STDMETHODCALLTYPE *GetSortColumns)(void*, void*, int);
    HRESULT (STDMETHODCALLTYPE *GetItem)(void*, int, const IID*, void**);
    HRESULT (STDMETHODCALLTYPE *GetVisibleItem)(void*, int, BOOL, int*);
    HRESULT (STDMETHODCALLTYPE *GetSelectedItem)(void*, int, int*);
    HRESULT (STDMETHODCALLTYPE *GetSelection)(void*, BOOL, void**);
    HRESULT (STDMETHODCALLTYPE *GetSelectionState)(void*, const void*, DWORD*);
    HRESULT (STDMETHODCALLTYPE *InvokeVerbOnSelection)(void*, const char*);
    HRESULT (STDMETHODCALLTYPE *SetViewModeAndIconSize)(void*, int, int);
    HRESULT (STDMETHODCALLTYPE *GetViewModeAndIconSize)(void*, int*, int*);
    HRESULT (STDMETHODCALLTYPE *SetGroupSubsetCount)(void*, UINT);
    HRESULT (STDMETHODCALLTYPE *GetGroupSubsetCount)(void*, UINT*);
    HRESULT (STDMETHODCALLTYPE *SetRedraw)(void*, BOOL);
} IFolderView2Vtbl;

typedef struct IEnumIDListVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *Next)(void*, ULONG, void**, ULONG*);
    HRESULT (STDMETHODCALLTYPE *Skip)(void*, ULONG);
    HRESULT (STDMETHODCALLTYPE *Reset)(void*);
    HRESULT (STDMETHODCALLTYPE *Clone)(void*, void**);
} IEnumIDListVtbl;

typedef struct IShellViewVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void*, const IID*, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE *Release)(void*);
    HRESULT (STDMETHODCALLTYPE *GetWindow)(void*, HWND*);
    HRESULT (STDMETHODCALLTYPE *ContextSensitiveHelp)(void*, BOOL);
    HRESULT (STDMETHODCALLTYPE *TranslateAccelerator)(void*, void*);
    HRESULT (STDMETHODCALLTYPE *EnableModeless)(void*, BOOL);
    HRESULT (STDMETHODCALLTYPE *UIActivate)(void*, UINT);
    HRESULT (STDMETHODCALLTYPE *Refresh)(void*);
    HRESULT (STDMETHODCALLTYPE *CreateViewWindow)(void*, void*, void*, void*, void*, HWND*);
    HRESULT (STDMETHODCALLTYPE *DestroyViewWindow)(void*);
    HRESULT (STDMETHODCALLTYPE *GetCurrentInfo)(void*, void*);
    HRESULT (STDMETHODCALLTYPE *AddPropertySheetPages)(void*, DWORD, void*, LPARAM);
    HRESULT (STDMETHODCALLTYPE *SaveViewState)(void*);
    HRESULT (STDMETHODCALLTYPE *SelectItem)(void*, const void*, DWORD);
    HRESULT (STDMETHODCALLTYPE *GetItemObject)(void*, UINT, const IID*, void**);
} IShellViewVtbl;

typedef struct {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    long pt_x;
    long pt_y;
} MSG;

typedef struct { long left, top, right, bottom; } RECT;

typedef struct {
    DWORD vkCode;
    DWORD scanCode;
    DWORD flags;
    DWORD time;
    ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT;

typedef LRESULT (CALLBACK *HOOKPROC)(int, WPARAM, LPARAM);

// ---------- API imports ----------
__declspec(dllimport) HRESULT WINAPI CoInitializeEx(void*, DWORD);
__declspec(dllimport) HRESULT WINAPI CoCreateInstance(const CLSID*, void*, DWORD, const IID*, void**);
__declspec(dllimport) void    WINAPI CoTaskMemFree(void*);

__declspec(dllimport) int     WINAPI MessageBoxW(HWND, const wchar_t*, const wchar_t*, UINT);
__declspec(dllimport) short   WINAPI GetAsyncKeyState(int);
__declspec(dllimport) void    WINAPI Sleep(DWORD);
__declspec(dllimport) DWORD   WINAPI GetTickCount(void);
__declspec(dllimport) BOOL    WINAPI SetProcessDPIAware(void);
__declspec(dllimport) HWND    WINAPI GetShellWindow(void);
__declspec(dllimport) BOOL    WINAPI GetClientRect(HWND, RECT*);
__declspec(dllimport) BOOL    WINAPI SystemParametersInfoW(UINT, UINT, void*, UINT);
__declspec(dllimport) int     WINAPI GetSystemMetrics(int);
__declspec(dllimport) void*   WINAPI VirtualAlloc(void*, SIZE_T_T, DWORD, DWORD);
__declspec(dllimport) BOOL    WINAPI VirtualFree(void*, SIZE_T_T, DWORD);
__declspec(dllimport) __attribute__((noreturn)) void WINAPI ExitProcess(UINT);
__declspec(dllimport) BOOL    WINAPI PeekMessageW(MSG*, HWND, UINT, UINT, UINT);
__declspec(dllimport) BOOL    WINAPI TranslateMessage(const MSG*);
__declspec(dllimport) LRESULT WINAPI DispatchMessageW(const MSG*);

__declspec(dllimport) HWND    WINAPI FindWindowW(const wchar_t*, const wchar_t*);
__declspec(dllimport) HWND    WINAPI FindWindowExW(HWND, HWND, const wchar_t*, const wchar_t*);
__declspec(dllimport) BOOL    WINAPI RedrawWindow(HWND, const RECT*, void*, UINT);

__declspec(dllimport) HHOOK     WINAPI SetWindowsHookExW(int, HOOKPROC, HINSTANCE, DWORD);
__declspec(dllimport) LRESULT   WINAPI CallNextHookEx(HHOOK, int, WPARAM, LPARAM);
__declspec(dllimport) BOOL      WINAPI UnhookWindowsHookEx(HHOOK);
__declspec(dllimport) HINSTANCE WINAPI GetModuleHandleW(const wchar_t*);
__declspec(dllimport) wchar_t*  WINAPI GetCommandLineW(void);

__declspec(dllimport) HANDLE WINAPI CreateFileW(const wchar_t*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL   WINAPI WriteFile(HANDLE, const void*, DWORD, DWORD*, void*);
__declspec(dllimport) BOOL   WINAPI ReadFile(HANDLE, void*, DWORD, DWORD*, void*);
__declspec(dllimport) BOOL   WINAPI CloseHandle(HANDLE);
__declspec(dllimport) BOOL   WINAPI DeleteFileW(const wchar_t*);
__declspec(dllimport) DWORD  WINAPI GetFileAttributesW(const wchar_t*);
__declspec(dllimport) DWORD  WINAPI GetTempPathW(DWORD, wchar_t*);

// winmm: 1ms timer resolution while playing (restored on exit).
__declspec(dllimport) UINT WINAPI timeBeginPeriod(UINT);
__declspec(dllimport) UINT WINAPI timeEndPeriod(UINT);

// ---------- state ----------
static void* g_pShellWindows;
static void* g_pFolderView;
static void* g_pFolderView2;
static void* g_pShellView;

static void* g_savedPidl[MAX_ICONS];
static long  g_savedPos[MAX_ICONS][2];
static int   g_savedCount;

// Static on purpose: as locals they would blow the stack frame past 4 KB,
// clang would emit __chkstk, and -nostdlib does not provide it.
static const void* g_batchPidl[MAX_ICONS];
static long        g_batchPt[MAX_ICONS * 2];
static int         g_batchN;

static void* g_curPidl[MAX_ICONS];
static unsigned char g_bkBuf[BK_MAXPIDL];

static int g_snakeX[MAX_SNAKE], g_snakeY[MAX_SNAKE];
static int g_snakeLen;
static int g_dirX, g_dirY;
static int g_nextDirX, g_nextDirY;

static int g_cols, g_rows;
static unsigned char* g_occupied;

static int g_foodX, g_foodY;
static int g_gameOver, g_won, g_quit;

static int g_spacingX, g_spacingY;
static int g_originX, g_originY;

static unsigned int g_rng = 1;

static HWND  g_hwndView;
static HWND  g_hwndList;
static HHOOK g_hook;

static DWORD g_origFlags;
static int   g_flagsChanged;
static int   g_layoutDirty;
static int   g_backupWritten;
static int   g_restoreFailed;
static int   g_timerPeriod;

static int g_noHook, g_noPark;

static wchar_t g_backupPath[600];

// ---------- CRT replacements ----------
void* memset(void* d, int v, SIZE_T_T n) {
    unsigned char* p = (unsigned char*)d;
    while (n--) *p++ = (unsigned char)v;
    return d;
}
void* memcpy(void* d, const void* s, SIZE_T_T n) {
    unsigned char* dp = (unsigned char*)d;
    const unsigned char* sp = (const unsigned char*)s;
    while (n--) *dp++ = *sp++;
    return d;
}
void* memmove(void* d, const void* s, SIZE_T_T n) {
    unsigned char* dp = (unsigned char*)d;
    const unsigned char* sp = (const unsigned char*)s;
    if (dp == sp || n == 0) return d;
    if (dp < sp) { SIZE_T_T i; for (i = 0; i < n; i++) dp[i] = sp[i]; }
    else { SIZE_T_T i; for (i = n; i > 0; i--) dp[i-1] = sp[i-1]; }
    return d;
}
int memcmp(const void* a, const void* b, SIZE_T_T n) {
    const unsigned char* x = (const unsigned char*)a;
    const unsigned char* y = (const unsigned char*)b;
    while (n--) { if (*x != *y) return (int)*x - (int)*y; x++; y++; }
    return 0;
}

// ---------- tiny string helpers ----------
static int wlen(const wchar_t* s) { int n = 0; if (!s) return 0; while (s[n]) n++; return n; }

static wchar_t* wapp(wchar_t* d, const wchar_t* s) {
    while (*s) *d++ = *s++;
    *d = 0;
    return d;
}

static wchar_t* wappu(wchar_t* d, unsigned int v) {
    wchar_t t[16];
    int n = 0;
    do { t[n++] = (wchar_t)(L'0' + (v % 10u)); v /= 10u; } while (v);
    while (n) *d++ = t[--n];
    *d = 0;
    return d;
}

static int has_switch(const wchar_t* needle) {
    const wchar_t* cmd = GetCommandLineW();
    int nl = wlen(needle), i, j;
    if (!cmd) return 0;
    for (i = 0; cmd[i]; i++) {
        for (j = 0; j < nl && cmd[i+j] && cmd[i+j] == needle[j]; j++) { }
        if (j == nl) return 1;
    }
    return 0;
}

// ---------- PIDL helpers ----------
static unsigned int pidl_len(const void* pidl) {
    const unsigned char* p = (const unsigned char*)pidl;
    unsigned int total = 0, cb;
    if (!p) return 0;
    for (;;) {
        cb = (unsigned int)p[0] | ((unsigned int)p[1] << 8);
        if (cb == 0) return total + 2;
        if (cb < 2 || total > 60000u) return 0;
        total += cb;
        p += cb;
    }
}

static int pidl_eq(const void* a, unsigned int alen, const void* b) {
    unsigned int blen = pidl_len(b);
    return (alen != 0 && alen == blen && memcmp(a, b, alen) == 0);
}

// ---------- misc ----------
static int my_rand(void) {
    g_rng = g_rng * 1103515245u + 12345u;
    return (int)((g_rng >> 16) & 0x7FFF);
}

static int cell_index(int x, int y) { return y * g_cols + x; }

static int is_occupied(int x, int y) {
    if (x < 0 || x >= g_cols || y < 0 || y >= g_rows) return 1;
    return g_occupied[cell_index(x, y)];
}

static void pump_messages(void) {
    MSG msg;
    while (PeekMessageW(&msg, (HWND)0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

static void say(const wchar_t* msg, UINT kind) {
    MessageBoxW((HWND)0, msg, L"Desktop Snake", kind);
}

static void say_path(const wchar_t* pre, const wchar_t* post, UINT kind) {
    static wchar_t buf[900];
    wapp(wapp(wapp(buf, pre), g_backupPath), post);
    say(buf, kind);
}

// ---------- repaint ----------
static void set_desktop_redraw(BOOL on) {
    if (g_pFolderView2)
        ((IFolderView2Vtbl*)(*(void***)g_pFolderView2))->SetRedraw(g_pFolderView2, on);
}

static void refresh_shell_view(void) {
    if (g_pShellView)
        ((IShellViewVtbl*)(*(void***)g_pShellView))->Refresh(g_pShellView);
}

// Heavy path: forces the namespace to re-enumerate and commit. Used for the
// one-shot bulk operations (park / restore), never per frame.
// No RDW_ERASE: erasing the whole desktop background every time is wasteful.
static void force_repaint(void) {
    HWND h;
    refresh_shell_view();
    h = g_hwndList;
    if (h) {
        RedrawWindow(h, (const RECT*)0, (void*)0,
                     RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    } else {
        HWND prog = GetShellWindow();
        if (prog)
            RedrawWindow(prog, (const RECT*)0, (void*)0,
                         RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    }
}

// Light path for the game loop: SetRedraw(FALSE/TRUE) already commits the
// positions, so we only need to invalidate. Refresh() re-enumerates the whole
// folder and can block on shell extensions / OneDrive - that was the stutter.
// If icons ever freeze mid-game, uncomment the refresh line below.
static void repaint_icons(void) {
    HWND h;
    // refresh_shell_view();
    h = g_hwndList ? g_hwndList : GetShellWindow();
    if (h)
        RedrawWindow(h, (const RECT*)0, (void*)0,
                     RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

// ---------- snap-to-grid ----------
static void disable_snap_to_grid(void) {
    IFolderView2Vtbl* fv2;
    if (!g_pFolderView2) return;
    fv2 = (IFolderView2Vtbl*)(*(void***)g_pFolderView2);
    if (FAILED(fv2->GetCurrentFolderFlags(g_pFolderView2, &g_origFlags))) return;
    if (!(g_origFlags & FWF_SNAPTOGRID)) return;
    if (SUCCEEDED(fv2->SetCurrentFolderFlags(g_pFolderView2, FWF_SNAPTOGRID, 0)))
        g_flagsChanged = 1;
}

static void restore_folder_flags(void) {
    if (!g_flagsChanged || !g_pFolderView2) return;
    ((IFolderView2Vtbl*)(*(void***)g_pFolderView2))->SetCurrentFolderFlags(
        g_pFolderView2, FWF_SNAPTOGRID, g_origFlags & FWF_SNAPTOGRID);
    g_flagsChanged = 0;
}

// ---------- batched positioning ----------
static void batch_reset(void) { g_batchN = 0; }

static void batch_add(void* pidl, long x, long y) {
    if (!pidl || g_batchN >= MAX_ICONS) return;
    g_batchPidl[g_batchN]     = pidl;
    g_batchPt[2*g_batchN]     = x;
    g_batchPt[2*g_batchN + 1] = y;
    g_batchN++;
}

static HRESULT batch_flush(void) {
    IFolderViewVtbl* fv;
    HRESULT hr = S_OK;
    if (g_batchN == 0 || !g_pFolderView) { g_batchN = 0; return S_OK; }
    fv = (IFolderViewVtbl*)(*(void***)g_pFolderView);
    hr = fv->SelectAndPositionItems(g_pFolderView, (UINT)g_batchN,
                                    g_batchPidl, g_batchPt, SVSI_POSITIONITEM);
    g_batchN = 0;
    return hr;
}

// ---------- COM plumbing ----------
static void release_com(void) {
    if (g_pFolderView2) { ((IUnknownVtbl*)(*(void***)g_pFolderView2))->Release(g_pFolderView2); g_pFolderView2 = 0; }
    if (g_pShellView)   { ((IUnknownVtbl*)(*(void***)g_pShellView))->Release(g_pShellView);     g_pShellView = 0; }
    if (g_pFolderView)  { ((IUnknownVtbl*)(*(void***)g_pFolderView))->Release(g_pFolderView);   g_pFolderView = 0; }
    if (g_pShellWindows){ ((IUnknownVtbl*)(*(void***)g_pShellWindows))->Release(g_pShellWindows); g_pShellWindows = 0; }
}

static HRESULT get_desktop_folder_view(void** ppv) {
    HRESULT hr;
    IShellWindowsVtbl* sw;
    IServiceProviderVtbl* sp;
    IShellBrowserVtbl* sb;
    IUnknownVtbl* sv;
    VARIANT vtLoc, vtEmpty;
    long lhwnd = 0;
    void* pdisp = 0;
    void* pProvider = 0;
    void* pBrowser = 0;
    void* pView = 0;
    int attempt;

    pump_messages();
    hr = CoCreateInstance(&CLSID_ShellWindows, 0, CLSCTX_ALL, &IID_IShellWindows, &g_pShellWindows);
    if (FAILED(hr)) return hr;
    pump_messages();

    sw = (IShellWindowsVtbl*)(*(void***)g_pShellWindows);

    memset(&vtLoc, 0, sizeof(VARIANT));
    vtLoc.vt = VT_I4;
    vtLoc.data.lVal = CSIDL_DESKTOP;
    memset(&vtEmpty, 0, sizeof(VARIANT));
    vtEmpty.vt = VT_EMPTY;

    for (attempt = 0; attempt < 5; attempt++) {
        pump_messages();
        hr = sw->FindWindowSW(g_pShellWindows, &vtLoc, &vtEmpty,
                              SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &pdisp);
        pump_messages();
        if (SUCCEEDED(hr) && pdisp) break;
        if (pdisp) { ((IUnknownVtbl*)(*(void***)pdisp))->Release(pdisp); pdisp = 0; }
        Sleep(200);
    }
    if (!pdisp) return hr;

    g_hwndView = (HWND)(ULONG_PTR)lhwnd;

    hr = ((IUnknownVtbl*)(*(void***)pdisp))->QueryInterface(pdisp, &IID_IServiceProvider, &pProvider);
    ((IUnknownVtbl*)(*(void***)pdisp))->Release(pdisp);
    pump_messages();
    if (FAILED(hr) || !pProvider) return hr;

    sp = (IServiceProviderVtbl*)(*(void***)pProvider);
    hr = sp->QueryService(pProvider, &SID_STopLevelBrowser, &IID_IShellBrowser, &pBrowser);
    ((IUnknownVtbl*)(*(void***)pProvider))->Release(pProvider);
    pump_messages();
    if (FAILED(hr) || !pBrowser) return hr;

    sb = (IShellBrowserVtbl*)(*(void***)pBrowser);
    hr = sb->QueryActiveShellView(pBrowser, &pView);
    ((IUnknownVtbl*)(*(void***)pBrowser))->Release(pBrowser);
    pump_messages();
    if (FAILED(hr) || !pView) return hr;

    sv = (IUnknownVtbl*)(*(void***)pView);
    sv->QueryInterface(pView, &IID_IShellView, &g_pShellView);
    sv->QueryInterface(pView, &IID_IFolderView2, &g_pFolderView2);
    hr = sv->QueryInterface(pView, &IID_IFolderView, ppv);
    sv->Release(pView);
    pump_messages();
    return hr;
}

static void find_desktop_listview(void) {
    if (g_hwndView)
        g_hwndList = FindWindowExW(g_hwndView, 0, L"SysListView32", L"FolderView");
    if (!g_hwndList) {
        HWND prog = FindWindowW(L"Progman", 0);
        HWND dv = prog ? FindWindowExW(prog, 0, L"SHELLDLL_DefView", 0) : 0;
        if (!dv) {
            // Wallpaper slideshow / Win11: the view can live under a WorkerW.
            HWND w = 0;
            while ((w = FindWindowExW(0, w, L"WorkerW", 0)) != 0) {
                dv = FindWindowExW(w, 0, L"SHELLDLL_DefView", 0);
                if (dv) break;
            }
        }
        if (dv) {
            g_hwndList = FindWindowExW(dv, 0, L"SysListView32", L"FolderView");
            if (!g_hwndList) g_hwndList = FindWindowExW(dv, 0, L"SysListView32", 0);
        }
    }
}

// ---------- enumeration / save / restore ----------
static int enumerate_desktop(void** out, int max) {
    IFolderViewVtbl* fv;
    IEnumIDListVtbl* ev;
    void* en = 0;
    int n = 0;
    if (!g_pFolderView) return -1;
    fv = (IFolderViewVtbl*)(*(void***)g_pFolderView);
    if (FAILED(fv->Items(g_pFolderView, SVGIO_ALLVIEW, &IID_IEnumIDList, &en)) || !en) return -1;
    ev = (IEnumIDListVtbl*)(*(void***)en);
    while (n < max) {
        void* pidl = 0;
        ULONG got = 0;
        if (ev->Next(en, 1, &pidl, &got) != S_OK || got == 0 || !pidl) break;
        out[n++] = pidl;
    }
    ev->Release(en);
    return n;
}

static void free_pidls(void** arr, int n) {
    int i;
    for (i = 0; i < n; i++) if (arr[i]) { CoTaskMemFree(arr[i]); arr[i] = 0; }
}

static void free_saved(void) {
    free_pidls(g_savedPidl, g_savedCount);
    g_savedCount = 0;
}

static BOOL save_positions(void) {
    int total = 0, got, i;
    IFolderViewVtbl* fv = (IFolderViewVtbl*)(*(void***)g_pFolderView);

    got = enumerate_desktop(g_savedPidl, MAX_ICONS);
    if (got <= 0) return 0;

    for (i = 0; i < got; i++) {
        long pt[2];
        pt[0] = 0; pt[1] = 0;
        if (SUCCEEDED(fv->GetItemPosition(g_pFolderView, g_savedPidl[i], pt))) {
            g_savedPos[total][0] = pt[0];
            g_savedPos[total][1] = pt[1];
            g_savedPidl[total] = g_savedPidl[i];
            total++;
        } else {
            CoTaskMemFree(g_savedPidl[i]);
            g_savedPidl[i] = 0;
        }
    }
    g_savedCount = total;
    return total > 0;
}

static HRESULT restore_positions(void) {
    HRESULT hr;
    int i;
    if (!g_pFolderView || g_savedCount <= 0) return S_OK;
    set_desktop_redraw(0);
    batch_reset();
    for (i = 0; i < g_savedCount; i++)
        batch_add(g_savedPidl[i], g_savedPos[i][0], g_savedPos[i][1]);
    hr = batch_flush();
    set_desktop_redraw(1);
    force_repaint();
    if (SUCCEEDED(hr)) g_layoutDirty = 0;
    else g_restoreFailed = 1;
    return hr;
}

static void park_spares(void) {
    int i, k = 0, vx, vw, rows;
    long baseX;
    if (g_noPark) return;

    vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    if (vw <= 0) { vw = 1920; vx = 0; }
    rows = g_rows > 0 ? g_rows : 1;
    baseX = (long)(vx + vw + g_spacingX);   // just past the right edge of all monitors

    set_desktop_redraw(0);
    batch_reset();
    for (i = g_snakeLen + 1; i < g_savedCount; i++) {
        if (g_batchN >= MAX_ICONS) batch_flush();
        batch_add(g_savedPidl[i],
                  baseX + (long)(k / rows) * g_spacingX,
                  (long)(k % rows) * g_spacingY);
        k++;
    }
    batch_flush();
    set_desktop_redraw(1);

    // IMPORTANT: do NOT Refresh() here. Refresh re-enumerates and clamps
    // off-screen icons back to the visible edge. Just invalidate.
    repaint_icons();
}

// ---------- layout backup on disk ----------
static void build_backup_path(void) {
    wchar_t tmp[512];
    DWORD n = GetTempPathW(500, tmp);
    if (n == 0 || n > 480) { tmp[0] = L'.'; tmp[1] = L'\\'; tmp[2] = 0; n = 2; }
    tmp[n] = 0;
    wapp(wapp(g_backupPath, tmp), L"iconake.layout");
}

static int backup_exists(void) {
    return GetFileAttributesW(g_backupPath) != INVALID_FILE_ATTRIBUTES;
}

static int write_all(HANDLE h, const void* p, DWORD n) {
    DWORD wr = 0;
    return WriteFile(h, p, n, &wr, 0) && wr == n;
}

static int read_all(HANDLE h, void* p, DWORD n) {
    DWORD rd = 0;
    return ReadFile(h, p, n, &rd, 0) && rd == n;
}

static int backup_write(void) {
    HANDLE h;
    DWORD hdr[3];
    int i, ok = 1;

    h = CreateFileW(g_backupPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (h == INVALID_HANDLE_VALUE) return 0;

    hdr[0] = BK_MAGIC; hdr[1] = BK_VER; hdr[2] = (DWORD)g_savedCount;
    ok = write_all(h, hdr, (DWORD)sizeof(hdr));

    for (i = 0; ok && i < g_savedCount; i++) {
        unsigned int len = pidl_len(g_savedPidl[i]);
        unsigned short cb;
        long pt[2];
        if (len == 0 || len > 0xFFFFu) { ok = 0; break; }
        cb = (unsigned short)len;
        pt[0] = g_savedPos[i][0];
        pt[1] = g_savedPos[i][1];
        ok = write_all(h, &cb, 2) && write_all(h, g_savedPidl[i], cb) && write_all(h, pt, 8);
    }
    CloseHandle(h);
    if (!ok) DeleteFileW(g_backupPath);
    return ok;
}

static int backup_restore(void) {
    HANDLE h;
    DWORD hdr[3], i, ncur;
    int nfound = 0, nrec;

    h = CreateFileW(g_backupPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (h == INVALID_HANDLE_VALUE) return -1;
    if (!read_all(h, hdr, (DWORD)sizeof(hdr)) || hdr[0] != BK_MAGIC || hdr[1] != BK_VER) {
        CloseHandle(h);
        return -2;
    }
    nrec = (int)(hdr[2] > MAX_ICONS ? MAX_ICONS : hdr[2]);

    ncur = (DWORD)enumerate_desktop(g_curPidl, MAX_ICONS);
    if ((int)ncur <= 0) { CloseHandle(h); return -3; }

    set_desktop_redraw(0);
    batch_reset();
    for (i = 0; i < (DWORD)nrec; i++) {
        unsigned short cb = 0;
        long pt[2];
        int j;
        if (!read_all(h, &cb, 2) || cb < 2 || cb > BK_MAXPIDL) break;
        if (!read_all(h, g_bkBuf, cb)) break;
        if (!read_all(h, pt, 8)) break;
        for (j = 0; j < (int)ncur; j++) {
            if (g_curPidl[j] && pidl_eq(g_bkBuf, cb, g_curPidl[j])) {
                batch_add(g_curPidl[j], pt[0], pt[1]);
                nfound++;
                break;
            }
        }
        if (g_batchN >= MAX_ICONS) batch_flush();
    }
    batch_flush();
    set_desktop_redraw(1);
    force_repaint();

    CloseHandle(h);
    free_pidls(g_curPidl, (int)ncur);
    return nfound;
}

// ---------- grid ----------
static void compute_origin(void) {
    long minx = 0x7FFFFFFF, miny = 0x7FFFFFFF;
    int i;
    g_originX = g_originY = 0;
    for (i = 0; i < g_savedCount; i++) {
        long x = g_savedPos[i][0], y = g_savedPos[i][1];
        if (x >= 0 && x < minx) minx = x;
        if (y >= 0 && y < miny) miny = y;
    }
    if (minx != 0x7FFFFFFF) g_originX = (int)(minx % g_spacingX);
    if (miny != 0x7FFFFFFF) g_originY = (int)(miny % g_spacingY);
}

static BOOL compute_grid(void) {
    IFolderViewVtbl* fv = (IFolderViewVtbl*)(*(void***)g_pFolderView);
    RECT wa, r;
    long sp[2];
    HWND shell;
    int i;

    sp[0] = 0; sp[1] = 0;
    if (SUCCEEDED(fv->GetSpacing(g_pFolderView, sp)) && sp[0] >= 16 && sp[1] >= 16) {
        g_spacingX = (int)sp[0];
        g_spacingY = (int)sp[1];
    } else {
        g_spacingX = 75;
        g_spacingY = 90;
    }

    compute_origin();

    wa.left = wa.top = wa.right = wa.bottom = 0;
    if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0)) {
        shell = GetShellWindow();
        if (shell && GetClientRect(shell, &r)) { wa.right = r.right; wa.bottom = r.bottom; }
        else { wa.right = 1920; wa.bottom = 1080; }
    }

    g_cols = (int)((wa.right  - g_originX) / g_spacingX);
    g_rows = (int)((wa.bottom - g_originY) / g_spacingY);
    if (g_cols > 256) g_cols = 256;
    if (g_rows > 256) g_rows = 256;
    if (g_cols < 6 || g_rows < 4) return 0;

    if (g_occupied) VirtualFree(g_occupied, 0, MEM_RELEASE);
    g_occupied = (unsigned char*)VirtualAlloc(0, (SIZE_T_T)g_cols * g_rows,
                                              MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!g_occupied) return 0;
    for (i = 0; i < g_cols * g_rows; i++) g_occupied[i] = 0;
    return 1;
}

// ---------- game logic ----------
static void place_food(void) {
    int tries;
    for (tries = 0; tries < 100000; tries++) {
        g_foodX = my_rand() % g_cols;
        g_foodY = my_rand() % g_rows;
        if (!is_occupied(g_foodX, g_foodY)) return;
    }
    g_gameOver = 1; g_won = 1;
}

static void tick(void) {
    int nx, ny, tailX, tailY, willEat, i;

    if (g_nextDirX == -g_dirX && g_nextDirY == -g_dirY) {
        g_nextDirX = g_dirX;
        g_nextDirY = g_dirY;
    }
    g_dirX = g_nextDirX;
    g_dirY = g_nextDirY;

    nx = g_snakeX[0] + g_dirX;
    ny = g_snakeY[0] + g_dirY;
    if (nx < 0 || nx >= g_cols || ny < 0 || ny >= g_rows) { g_gameOver = 1; return; }

    tailX = g_snakeX[g_snakeLen - 1];
    tailY = g_snakeY[g_snakeLen - 1];
    willEat = (nx == g_foodX && ny == g_foodY);

    if (is_occupied(nx, ny) && !(nx == tailX && ny == tailY && !willEat)) { g_gameOver = 1; return; }

    if (willEat) {
        if (g_snakeLen + 1 > MAX_SNAKE) { g_gameOver = 1; g_won = 1; return; }
        for (i = g_snakeLen; i > 0; i--) { g_snakeX[i] = g_snakeX[i-1]; g_snakeY[i] = g_snakeY[i-1]; }
        g_snakeLen++;
    } else {
        g_occupied[cell_index(tailX, tailY)] = 0;
        for (i = g_snakeLen - 1; i > 0; i--) { g_snakeX[i] = g_snakeX[i-1]; g_snakeY[i] = g_snakeY[i-1]; }
    }

    g_snakeX[0] = nx; g_snakeY[0] = ny;
    g_occupied[cell_index(nx, ny)] = 1;

    if (willEat) {
        if (g_snakeLen >= g_savedCount) { g_gameOver = 1; g_won = 1; return; }
        place_food();
    }
}

// Win10 19H1+/Win11 needs the view frozen around the batch:
//   SetRedraw(0) -> move -> SetRedraw(1) -> invalidate.
static void render(void) {
    static int wob = 0;
    int i;

    set_desktop_redraw(0);
    batch_reset();
    for (i = 0; i < g_snakeLen && i < g_savedCount; i++)
        batch_add(g_savedPidl[i],
                  g_originX + g_snakeX[i] * g_spacingX,
                  g_originY + g_snakeY[i] * g_spacingY);

    if (!g_gameOver && g_snakeLen < g_savedCount) {
        wob ^= 1;
        batch_add(g_savedPidl[g_snakeLen],
                  g_originX + g_foodX * g_spacingX + (wob ? 8 : 0),
                  g_originY + g_foodY * g_spacingY);
    }
    if (FAILED(batch_flush())) g_gameOver = 1;   // e.g. Explorer restarted
    set_desktop_redraw(1);
    repaint_icons();
}

// ---------- input ----------
static void steer(int dx, int dy) {
    if (dx && g_dirX == -dx) return;
    if (dy && g_dirY == -dy) return;
    g_nextDirX = dx;
    g_nextDirY = dy;
}

static LRESULT CALLBACK kbd_hook(int code, WPARAM w, LPARAM l) {
    if (code >= 0 && (w == WM_KEYDOWN || w == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)l;
        switch (k->vkCode) {
            case VK_ESCAPE: g_quit = 1;      return 1;
            case VK_UP:     case VK_W: steer(0, -1); return 1;
            case VK_DOWN:   case VK_S: steer(0,  1); return 1;
            case VK_LEFT:   case VK_A: steer(-1, 0); return 1;
            case VK_RIGHT:  case VK_D: steer(1,  0); return 1;
            default: break;
        }
    }
    return CallNextHookEx(g_hook, code, w, l);
}

static void input_install(void) {
    if (g_noHook) return;
    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, kbd_hook, GetModuleHandleW(0), 0);
}

static void input_remove(void) {
    if (g_hook) { UnhookWindowsHookEx(g_hook); g_hook = 0; }
}

static void input_poll(void) {
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { g_quit = 1; return; }
    if ((GetAsyncKeyState(VK_UP)    & 0x8000) || (GetAsyncKeyState(VK_W) & 0x8000)) steer(0, -1);
    if ((GetAsyncKeyState(VK_DOWN)  & 0x8000) || (GetAsyncKeyState(VK_S) & 0x8000)) steer(0,  1);
    if ((GetAsyncKeyState(VK_LEFT)  & 0x8000) || (GetAsyncKeyState(VK_A) & 0x8000)) steer(-1, 0);
    if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState(VK_D) & 0x8000)) steer(1,  0);
}

// ---------- shutdown ----------
static void cleanup(void) {
    if (g_timerPeriod) { timeEndPeriod(1); g_timerPeriod = 0; }
    input_remove();
    if (g_layoutDirty) restore_positions();   // before re-enabling snap-to-grid
    restore_folder_flags();
    if (g_occupied) { VirtualFree(g_occupied, 0, MEM_RELEASE); g_occupied = 0; }
    free_saved();
    release_com();
    if (g_backupWritten && !g_layoutDirty) { DeleteFileW(g_backupPath); g_backupWritten = 0; }
}

static __attribute__((noreturn)) void fatal(const wchar_t* msg) {
    cleanup();
    say(msg, MB_ICONERROR);
    ExitProcess(1);
}

static __attribute__((noreturn)) void fatal_path(const wchar_t* pre, const wchar_t* post) {
    cleanup();
    say_path(pre, post, MB_ICONERROR);
    ExitProcess(1);
}

// ---------- modes ----------
static int acquire_view(void) {
    IFolderViewVtbl* fv;
    if (FAILED(get_desktop_folder_view(&g_pFolderView)) || !g_pFolderView) return 0;
    fv = (IFolderViewVtbl*)(*(void***)g_pFolderView);
    find_desktop_listview();
    if (fv->GetAutoArrange(g_pFolderView) == S_OK) return 2;
    return 1;
}

static __attribute__((noreturn)) void restore_mode(void) {
    wchar_t msg[160];
    int rc, n;
    rc = acquire_view();
    if (rc == 0) fatal(L"Could not access the desktop folder view.");
    if (rc == 2) fatal(L"'Auto arrange icons' is enabled; turn it off first.");

    n = backup_restore();
    if (n == -1) fatal_path(L"No layout backup found at\n", L"");
    if (n == -2) fatal(L"The layout backup is corrupt.");
    if (n == -3) fatal(L"Could not enumerate the desktop icons.");

    release_com();

    if (n > 0) {
        DeleteFileW(g_backupPath);
        wapp(wappu(wapp(msg, L"Restored "), (unsigned int)n), L" icon positions. Backup deleted.");
        say(msg, MB_ICONINFO);
    } else {
        wapp(msg, L"None of the backed-up icons are on the desktop any more. Backup kept.");
        say(msg, MB_ICONWARN);
    }
    ExitProcess(0);
}

__attribute__((noreturn))
void entry(void) {
    DWORD last_tick, now;
    const DWORD interval = 150;
    int i, rc;

    SetProcessDPIAware();

    g_noHook = has_switch(L"--no-hook");
    g_noPark = has_switch(L"--no-park");
    build_backup_path();

    if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED))) {
        say(L"COM initialization failed.", MB_ICONERROR);
        ExitProcess(1);
    }
    pump_messages();

    if (has_switch(L"--restore")) restore_mode();

    if (backup_exists())
        fatal(L"A previous run did not finish cleanly.\n"
              L"Run  iconake.exe --restore  to put your icons back, then start again.");

    rc = acquire_view();
    if (rc == 0) fatal(L"Could not access the desktop folder view.");
    if (rc == 2)
        fatal(L"'Auto arrange icons' is enabled on the desktop.\n"
              L"Right-click the desktop > View > uncheck it, then retry.");

    g_rng = GetTickCount(); if (!g_rng) g_rng = 1;

    if (!save_positions()) fatal(L"Failed to read icon positions. Refusing to run.");
    if (g_savedCount < 5)  fatal(L"Need at least 5 desktop icons.");
    if (!compute_grid())   fatal(L"Desktop is too small for the game grid.");

    if (!backup_write())
        fatal_path(L"Could not write the layout backup to\n", L"\nRefusing to run.");
    g_backupWritten = 1;

    disable_snap_to_grid();

    g_snakeLen = 3;
    g_snakeX[0] = g_cols / 2;       g_snakeY[0] = g_rows / 2;
    g_snakeX[1] = g_snakeX[0] - 1;  g_snakeY[1] = g_snakeY[0];
    g_snakeX[2] = g_snakeX[0] - 2;  g_snakeY[2] = g_snakeY[0];
    for (i = 0; i < g_snakeLen; i++)
        g_occupied[cell_index(g_snakeX[i], g_snakeY[i])] = 1;

    g_dirX = 1; g_dirY = 0; g_nextDirX = 1; g_nextDirY = 0;
    g_gameOver = 0; g_won = 0; g_quit = 0;

    place_food();

    g_layoutDirty = 1;
    park_spares();
    render();

    say_path(L"Desktop Snake\n\n"
             L"Arrows / WASD to move. Esc to quit.\n"
             L"Eat the wobbling icon to grow!\n\n"
             L"Layout backup (restored on exit):\n",
             L"\n\nIf this program is killed, run:  iconake.exe --restore", MB_ICONINFO);

    input_install();

    // 1ms timer resolution: Sleep(4) really sleeps ~4ms and GetTickCount
    // advances in ~1ms steps, so the 150ms cadence stops beating.
    if (timeBeginPeriod(1) == 0) g_timerPeriod = 1;
    last_tick = GetTickCount();

    while (!g_gameOver && !g_quit) {
        input_poll();
        now = GetTickCount();
        if (now - last_tick >= interval) {
            // Keep the phase, discard the overrun: after an Explorer stall we
            // resume on-beat instead of fast-forwarding (teleporting).
            last_tick = now - (now - last_tick) % interval;
            tick();
            render();
        }
        pump_messages();      // also keeps the low-level keyboard hook alive
        Sleep(4);
    }

    if (g_timerPeriod) { timeEndPeriod(1); g_timerPeriod = 0; }
    input_remove();

    pump_messages();
    cleanup();

    if (g_restoreFailed)
        say_path(L"Could not put the icons back (did Explorer restart?).\n"
                 L"Your backup is still at\n", L"\nRun:  iconake.exe --restore", MB_ICONWARN);
    else if (g_won)  say(L"You win! Desktop restored.", MB_OK);
    else if (g_quit) say(L"Quit. Desktop restored.", MB_OK);
    else             say(L"Game over! Desktop restored.", MB_OK);

    ExitProcess(0);
}