// ShellHookWindow.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ShellHookWindow.h"
#include "commctrl.h"
#include "OptionsWindow.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
const wchar_t* szTitle = L"SCT - File Explorer Hook";
const wchar_t* szWindowClass = L"SctFehHookWindow";
UINT ShellMessage;
HHOOK keyboardHook;

extern BOOL ClientEdge;
extern BOOL RebarBorders;
extern BOOL RebarFixedHeight;
extern BOOL HideNavigationBar;
extern BOOL RestoreAddressBar;
extern BOOL AddressBarFixHeight;
extern BOOL AddressBarRemoveRefresh;
extern BOOL AddressBarAddPath;

ATOM                RegisterWindowClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    HookWndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct EnumChildProcData
{
    HWND result;
    int index;
    int instancesFound;
    const wchar_t* className;
};
BOOL CALLBACK EnumChildProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
    wchar_t buffer[256];
    EnumChildProcData* data = (EnumChildProcData*)lParam;
    GetClassNameW(hwnd, buffer, 256);
    if (lstrcmpW(buffer, data->className) == 0)
    {
        if (data->instancesFound + 1 == data->index)
        {
            data->result = hwnd;
            return FALSE;
        }
        else
        {
            data->instancesFound++;
        }
    }
    return TRUE;
};
HWND GetChildWindow(HWND parent, const wchar_t* className, int index)
{
    EnumChildProcData data = { 0 };
    data.className = className;
    data.index = index;
    EnumChildWindows(parent, EnumChildProc, (LPARAM)&data);
    return data.result;
}

__declspec(dllexport) LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
    {
        KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
        int key = hooked_key.vkCode;

        BOOL SHIFT = GetKeyState(VK_SHIFT) & 1;
        BOOL CTRL = GetKeyState(VK_LCONTROL) & 1;

        if (CTRL && SHIFT && key == 'A')
        {
            wchar_t buffer[256];
            GetClassNameW(GetForegroundWindow(), buffer, 256);
            if (lstrcmpW(buffer, L"CabinetWClass") == 0)
                ShowOptionsDialog();
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadOptions();
    keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)KeyboardHook, hInstance, NULL);

    RegisterWindowClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    
    return (int) msg.wParam;
}

ATOM RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = HookWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = wcex.hIcon;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   RegisterShellHookWindow(hWnd);
   ShellMessage = RegisterWindowMessageW(L"SHELLHOOK");

   //ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    wchar_t classNameBuffer[256];
    wchar_t pathBuffer[MAX_PATH];
    HKEY key;
    RECT rect;
    if (message == ShellMessage)
    {
        GetClassNameW((HWND)lParam, classNameBuffer, 256);
        if (lstrcmpW(classNameBuffer, L"CabinetWClass") == 0)
        {
            HWND NavBarParent = GetChildWindow((HWND)lParam, L"WorkerW", 1);
            HWND NavBarAddressBandRoot = GetChildWindow(NavBarParent, L"Address Band Root", 1);
            HWND NavBarToolBar = GetChildWindow(NavBarAddressBandRoot, L"ToolbarWindow32", 2);
            HWND ClassicAddressBandRoot = GetChildWindow((HWND)lParam, L"Address Band Root", 2);
            HWND ClassicProgressControl = GetChildWindow(ClassicAddressBandRoot, L"msctls_progress32", 1);
            HWND ClassicComboBox = GetChildWindow(ClassicAddressBandRoot, L"ComboBoxEx32", 1);
            HWND ClassicRebar = GetChildWindow((HWND)lParam, L"ReBarWindow32", 2);
            
            // Add Client Edge to the folder view and the tree view (the navigation pane)
            if (ClientEdge)
            {
                HWND TreeView = GetChildWindow((HWND)lParam, L"SysTreeView32", 1);
                LONG TreeViewExtendedStyle = GetWindowLongPtrW(TreeView, GWL_EXSTYLE);
                TreeViewExtendedStyle |= WS_EX_CLIENTEDGE;
                SetWindowLongPtrW(TreeView, GWL_EXSTYLE, TreeViewExtendedStyle);

                HWND FolderView = GetChildWindow((HWND)lParam, L"FolderView", 1);
                LONG FolderViewExtendedStyle = GetWindowLongPtrW(FolderView, GWL_EXSTYLE);
                FolderViewExtendedStyle |= WS_EX_CLIENTEDGE;
                SetWindowLongPtrW(FolderView, GWL_EXSTYLE, FolderViewExtendedStyle);
            }

            // Add borders to the outside of the Rebar and in between the bands
            if (RebarBorders)
            {
                LONG ClassicRebarStyle = GetWindowLongPtrW(ClassicRebar, GWL_STYLE);
                ClassicRebarStyle |= RBS_BANDBORDERS;
                ClassicRebarStyle |= WS_BORDER;
                SetWindowLongPtrW(ClassicRebar, GWL_STYLE, ClassicRebarStyle);
            }

            // Make the size of all the Rebar bands equal to the size of the biggest band
            if (RebarFixedHeight)
            {
                LONG ClassicRebarStyle = GetWindowLongPtrW(ClassicRebar, GWL_STYLE);
                ClassicRebarStyle &= ~RBS_VARHEIGHT;
                SetWindowLongPtrW(ClassicRebar, GWL_STYLE, ClassicRebarStyle);
            }

            // Apply any changes made to the Rebar
            if (RebarBorders || RebarFixedHeight)
            {
                GetWindowRect(ClassicRebar, &rect);
                SetWindowPos(ClassicRebar, NULL, NULL, NULL, rect.right - rect.left + 1, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
                SetWindowPos(ClassicRebar, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
            }

            if (HideNavigationBar && wParam == 1)
            {
                for (int i = 0; i < 1; i++)
                {
                    // Save the location of the Rebar
                    HWND NavBarRebar = GetChildWindow((HWND)lParam, L"ReBarWindow32", 1);
                    GetWindowRect(NavBarRebar, &rect);
                    LONG xRebar = rect.left;
                    LONG yRebar = rect.top;
                    LONG cxRebar = rect.right - rect.left;
                    LONG cyRebar = rect.bottom - rect.top;

                    // Destroy the NC area of the rebar
                    SendMessageW(NavBarRebar, WM_NCDESTROY, 0, 0);

                    // Hide the WorkerW and the Rebar
                    ShowWindow(NavBarParent, SW_HIDE);
                    ShowWindow(NavBarRebar, SW_HIDE);

                    // Save the location of the ShellTabWindowClass
                    HWND ShellTabWnd = GetChildWindow((HWND)lParam, L"ShellTabWindowClass", 1);
                    GetWindowRect(NavBarRebar, &rect);
                    LONG xTabWnd = rect.left;
                    LONG yTabWnd = rect.top;
                    LONG cxTabWnd = rect.right - rect.left;
                    LONG cyTabWnd = rect.bottom - rect.top;

                    // Move ShellTabWindow to (*; yRebar; *; yTabWnd - yRebar + cyTabWnd)
                    SetWindowPos(ShellTabWnd, NULL, xTabWnd, yRebar, cxTabWnd, yTabWnd - yRebar + cyTabWnd, SWP_NOZORDER);

                    // Move Rebar to (*; *; *; 0)
                    SetWindowPos(NavBarRebar, NULL, xRebar, yRebar, cxRebar, 0, SWP_NOZORDER);

                    // Resize the window to apply shit
                    GetWindowRect((HWND)lParam, &rect);
                    SetWindowPos((HWND)lParam, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top + 1, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
                    SetWindowPos((HWND)lParam, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);

                    // Redraw the entire explorer window
                    RedrawWindow((HWND)lParam, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
                }
            }

            // Set Address bar text
            if (AddressBarAddPath)
            {
                // Determine CabinetState (whether to display full path or not)
                RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer", &key);
                DWORD CabinetState = 0;
                DWORD size = sizeof(DWORD);
                HRESULT ErrorLevel = RegQueryValueExW(key, L"CabinetState", NULL, NULL, (LPBYTE)&CabinetState, &size);
                RegCloseKey(key);

                // Generate text string for the address bar depending on CabinetState
                if (!ErrorLevel && CabinetState >= 1)
                {
                    HWND toolbarwindow = GetChildWindow(NavBarAddressBandRoot, L"ToolbarWindow32", 1);
                    GetWindowTextW(toolbarwindow, pathBuffer, MAX_PATH);
                    //TODO
                }
                else
                {
                    HWND shelltabwindow = GetChildWindow((HWND)lParam, L"ShellTabWindowClass", 1);
                    GetWindowTextW(shelltabwindow, pathBuffer, MAX_PATH);
                }

                HWND AddressBarEdit = GetChildWindow(ClassicAddressBandRoot, L"Edit", 1);
                for (int i = 0; i < 3; i++)
                    SendMessageW(AddressBarEdit, WM_SETTEXT, 0, (LPARAM)pathBuffer);
            }

            // Remove the toolbar with the Go button
            if (AddressBarRemoveRefresh)
            {
                HWND GoButtonToolbar = GetChildWindow(ClassicAddressBandRoot, L"ToolbarWindow32", 1);
                SendMessageW(GoButtonToolbar, WM_CLOSE, 0, 0);
            }

            // Set the Address bar height to 22px as oppossed to the default 39px height
            if (AddressBarFixHeight)
            {
                // Allocate memory inside Explorer
                DWORD count = SendMessage(GetParent(ClassicAddressBandRoot), RB_GETBANDCOUNT, 0, 0);
                SIZE_T bytesRead = 0;
                DWORD ExplorerPID = 0;
                GetWindowThreadProcessId(ClassicAddressBandRoot, &ExplorerPID);
                HANDLE ExplorerProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ExplorerPID);
                void* ExplorerMemoryRebar = VirtualAllocEx(ExplorerProcess, NULL, sizeof(REBARBANDINFO), MEM_COMMIT, PAGE_READWRITE);
                void* ExplorerMemoryComboBoxItem = VirtualAllocEx(ExplorerProcess, NULL, sizeof(COMBOBOXEXITEM), MEM_COMMIT, PAGE_READWRITE);
                void* ExplorerMemoryToolbarButton = VirtualAllocEx(ExplorerProcess, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);

                // Make the band that's 39 pixels high, 22 pixels high
                REBARBANDINFO bandInfo = { 0 };
                for (int i = 0; i < count; i++)
                {
                    bandInfo = { 0 };
                    bandInfo.cbSize = sizeof(REBARBANDINFO);
                    bandInfo.fMask = RBBIM_CHILDSIZE;
                    WriteProcessMemory(ExplorerProcess, ExplorerMemoryRebar, &bandInfo, sizeof(REBARBANDINFO), &bytesRead);
                    SendMessageW(GetParent(ClassicAddressBandRoot), RB_GETBANDINFO, i, (LPARAM)ExplorerMemoryRebar);
                    ReadProcessMemory(ExplorerProcess, ExplorerMemoryRebar, &bandInfo, sizeof(REBARBANDINFO), &bytesRead);
                    if (bandInfo.cyMinChild == 39)
                    {
                        bandInfo.cyMinChild = 22;
                        WriteProcessMemory(ExplorerProcess, ExplorerMemoryRebar, &bandInfo, sizeof(REBARBANDINFO), &bytesRead);
                        SendMessageW(GetParent(ClassicAddressBandRoot), RB_SETBANDINFO, i, (LPARAM)ExplorerMemoryRebar);
                    }
                }

                // Free Explorer Memory
                VirtualFreeEx(ExplorerProcess, ExplorerMemoryRebar, sizeof(REBARBANDINFO), MEM_DECOMMIT);
                VirtualFreeEx(ExplorerProcess, ExplorerMemoryToolbarButton, sizeof(REBARBANDINFO), MEM_DECOMMIT);
                CloseHandle(ExplorerProcess);
                
                // Set ComboBox height
                SendMessageW(ClassicComboBox, CB_SETITEMHEIGHT, -1, 22 - 6);
                SetParent(ClassicComboBox, ClassicAddressBandRoot);
                
                // Remove ProgressBsr
                SendMessageW(ClassicProgressControl, WM_CLOSE, 0, 0);

                // Fix ComboBox
                GetWindowRect(ClassicComboBox, &rect);
                SetWindowPos(ClassicComboBox, NULL, NULL, NULL, rect.right - rect.left + 1, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
                SetWindowPos(ClassicComboBox, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);

                // Redraw the ComboBox
                RedrawWindow(ClassicComboBox, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
            }

            // Set toolbar height?
            /*DWORD buttonSize = SendMessageW((HWND)0x001408F6, TB_GETBUTTONSIZE, 0, 0);
            if (HIWORD(buttonSize) != 22)
                SendMessageW((HWND)0x001408F6, TB_SETBUTTONSIZE, 0, MAKEWPARAM(LOWORD(buttonSize), 22));*/

            // Set ComboBox icon
            // Well at least, try to
            //HIMAGELIST imageList = (HIMAGELIST)SendMessageW(ClassicComboBox, CBEM_SETIMAGELIST, 0, 0);
            //SendMessageW(comboBox, CB_SETCURSEL, (WPARAM)itemIndex, 0);
            //HICON icon = (HICON)SendMessageW(GetAncestor(comboBox, GA_ROOT), WM_GETICON, 0, 0);
            //int imageIndex = ImageList_AddIcon(imageList, icon);
            //comboBoxItem.mask = CBEIF_IMAGE;
            //comboBoxItem.iItem = itemIndex;
            //comboBoxItem.iImage = imageIndex;
            //SendMessageW(comboBox, CBEM_SETITEM, 0, (LPARAM)&comboBoxItem);
            /*COMBOBOXEXITEM comboBoxItem = {0};
            TBBUTTON tbButton = { 0 };
            DWORD countButtosn = SendMessageW(NavBarToolBar, TB_BUTTONCOUNT, 0, 0);
            HIMAGELIST imageList = (HIMAGELIST)SendMessageW(NavBarToolBar, TB_GETIMAGELIST, 0, 0);
            SendMessageW(ClassicComboBox, CBEM_SETIMAGELIST, 0, (LPARAM)imageList);
            SendMessageW(NavBarToolBar, TB_GETBUTTON, 2, (LPARAM)ExplorerMemoryToolbarButton);
            ReadProcessMemory(ExplorerProcess, ExplorerMemoryToolbarButton, &tbButton, sizeof(TBBUTTON), &bytesRead);
            comboBoxItem.mask = CBEIF_IMAGE;
            comboBoxItem.iItem = -1;
            comboBoxItem.iImage = tbButton.iBitmap;
            WriteProcessMemory(ExplorerProcess, ExplorerMemoryToolbarButton, &comboBoxItem, sizeof(COMBOBOXEXITEM), &bytesRead);
            SendMessageW(ClassicComboBox, CBEM_SETITEM, 0, (LPARAM)ExplorerMemoryToolbarButton);*/
        }
    }
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
            ShowOptionsDialog();
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
