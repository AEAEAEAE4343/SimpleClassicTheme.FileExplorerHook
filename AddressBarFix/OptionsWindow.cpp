// OptionsWindow.cpp : Contains code for the configuration dialog
//

#include <windows.h>
#include "OptionsWindow.h"

INT_PTR OptionsDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL ClientEdge = 0;
BOOL RebarBorders = 0;
BOOL RebarFixedHeight = 0;
BOOL HideNavigationBar = 0;
BOOL RestoreAddressBar = 0;
BOOL AddressBarFixHeight = 0;
BOOL AddressBarRemoveRefresh = 0;
BOOL AddressBarAddPath = 0;

void ShowOptionsDialog()
{
	DialogBoxW(NULL, MAKEINTRESOURCEW(IDD_OPTIONS), NULL, OptionsDlgProc);
}

void LoadOptions()
{
	HKEY hkey = 0; 
	DWORD value = 0;
	RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", NULL, KEY_READ, &hkey);
	
	DWORD bytesRead = sizeof(DWORD); value = TRUE;
	RegQueryValueExW(hkey, L"ClientEdge", NULL, NULL, (LPBYTE)&value, &bytesRead);
	ClientEdge = value;

	bytesRead = sizeof(DWORD); value = TRUE;
	RegQueryValueExW(hkey, L"RebarBorders", NULL, NULL, (LPBYTE)&value, &bytesRead);
	RebarBorders = value;

	bytesRead = sizeof(DWORD); value = FALSE;
	RegQueryValueExW(hkey, L"RebarFixedHeight", NULL, NULL, (LPBYTE)&value, &bytesRead);
	RebarFixedHeight = value;

	bytesRead = sizeof(DWORD); value = TRUE;
	RegQueryValueExW(hkey, L"HideNavigationBar", NULL, NULL, (LPBYTE)&value, &bytesRead);
	HideNavigationBar = value;

	bytesRead = sizeof(DWORD); value = FALSE;
	RegQueryValueExW(hkey, L"RestoreAddressBar", NULL, NULL, (LPBYTE)&value, &bytesRead);
	RestoreAddressBar = value;

	bytesRead = sizeof(DWORD); value = FALSE;
	RegQueryValueExW(hkey, L"AddressBarFixHeight", NULL, NULL, (LPBYTE)&value, &bytesRead);
	AddressBarFixHeight = value;

	bytesRead = sizeof(DWORD); value = FALSE;
	RegQueryValueExW(hkey, L"AddressBarRemoveRefresh", NULL, NULL, (LPBYTE)&value, &bytesRead);
	AddressBarRemoveRefresh = value;

	bytesRead = sizeof(DWORD); value = FALSE;
	RegQueryValueExW(hkey, L"AddressBarAddPath", NULL, NULL, (LPBYTE)&value, &bytesRead);
	AddressBarAddPath = value;

	RegCloseKey(hkey);
}

void SaveOptions(HWND hWndDlg)
{
	ClientEdge = SendDlgItemMessageW(hWndDlg, IDC_CHECK_CLIENTEDGE, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"ClientEdge", REG_DWORD, &ClientEdge, sizeof(DWORD));

	RebarBorders = SendDlgItemMessageW(hWndDlg, IDC_CHECK_REBARBORDERS, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"RebarBorders", REG_DWORD, &RebarBorders, sizeof(DWORD));

	RebarFixedHeight = SendDlgItemMessageW(hWndDlg, IDC_CHECK_REBARFIXEDHEIGHT, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"RebarFixedHeight", REG_DWORD, &RebarFixedHeight, sizeof(DWORD));

	HideNavigationBar = SendDlgItemMessageW(hWndDlg, IDC_CHECK_HIDENAVBAR, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"HideNavigationBar", REG_DWORD, &HideNavigationBar, sizeof(DWORD));

	RestoreAddressBar = SendDlgItemMessageW(hWndDlg, IDC_CHECK_RESTOREADDBAR, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"RestoreAddressBar", REG_DWORD, &RestoreAddressBar, sizeof(DWORD));

	AddressBarFixHeight = SendDlgItemMessageW(hWndDlg, IDC_CHECK_ADDBAR_HEIGHT, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"AddressBarFixHeight", REG_DWORD, &AddressBarFixHeight, sizeof(DWORD));

	AddressBarRemoveRefresh = SendDlgItemMessageW(hWndDlg, IDC_CHECK_ADDBAR_REM_REFRESH, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"AddressBarRemoveRefresh", REG_DWORD, &AddressBarRemoveRefresh, sizeof(DWORD));

	AddressBarAddPath = SendDlgItemMessageW(hWndDlg, IDC_CHECK_ADDBAR_PATH, BM_GETCHECK, 0, 0);
	RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\1337ftw\\Simple Classic Theme\\FileExplorerHook", L"AddressBarAddPath", REG_DWORD, &AddressBarAddPath, sizeof(DWORD));
}

void LoadOptionsDlg(HWND hWndDlg)
{
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_CLIENTEDGE, BM_SETCHECK, ClientEdge, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_REBARBORDERS, BM_SETCHECK, RebarBorders, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_REBARFIXEDHEIGHT, BM_SETCHECK, RebarFixedHeight, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_HIDENAVBAR, BM_SETCHECK, HideNavigationBar, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_RESTOREADDBAR, BM_SETCHECK, RestoreAddressBar, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_ADDBAR_HEIGHT, BM_SETCHECK, AddressBarFixHeight, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_ADDBAR_REM_REFRESH, BM_SETCHECK, AddressBarRemoveRefresh, 0);
	SendDlgItemMessageW(hWndDlg, IDC_CHECK_ADDBAR_PATH, BM_SETCHECK, AddressBarAddPath, 0);
}

INT_PTR OptionsDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hWnd, IDABORT);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDAPPLY:
			SaveOptions(hWnd);
			return TRUE;
		case IDOK:
			SaveOptions(hWnd);
		case IDABORT:
			EndDialog(hWnd, wParam);
			return TRUE;
		}
		break;
	case WM_INITDIALOG:
		SendMessageW(hWnd, WM_SETICON, 0, (LPARAM)LoadImage(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_SCT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
		SendMessageW(hWnd, WM_SETICON, 1, (LPARAM)LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_SCT)));
		LoadOptionsDlg(hWnd);
		break;
	}
	return FALSE;
}