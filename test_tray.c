#define WINVER 0x0501
#define _WIN32_IE 0x0600

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <tchar.h>


#define MY_WNDCLASS TEXT("TrayTest")
#define MY_TRAYMESSAGE (WM_USER+100)
#define MY_TRAY_ICON_ID 1
#define IDC_ADD_ICON 101
#define IDC_DEL_ICON 102
#define IDC_POPUP 103

// wine headers fix
#define NOTIFYICON_VERSION NOTIFY_VERSION
#define NOTIFYICON_VERSION_4 NOTIFY_VERSION_4

HWND g_hWnd = NULL;
BOOL g_added = FALSE;


static void notifyicondata_init(NOTIFYICONDATA *pdata) {
	memset(pdata, 0, sizeof(NOTIFYICONDATA));
	
	pdata->cbSize = sizeof(NOTIFYICONDATA);
	pdata->hWnd = g_hWnd;
	pdata->uID = MY_TRAY_ICON_ID;
	pdata->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_STATE;
	pdata->uCallbackMessage = MY_TRAYMESSAGE;
	pdata->hIcon = LoadIcon(NULL, IDI_APPLICATION);
	strncpy(pdata->szTip, "Winelib test icon", sizeof(pdata->szTip)-1);
	pdata->dwState = 0;
	pdata->dwStateMask = NIS_HIDDEN | NIS_SHAREDICON;
	pdata->uVersion = NOTIFYICON_VERSION;
	// only set tese to display balloon
	// TCHAR szInfo[256];
	// TCHAR szInfoTitle[64];
	// DWORD dwInfoFlags;
	// GUID  guidItem; // don'use if use uID as icon ID
	// HICON hBalloonIcon; // already initialised to zero by memset
}


void tray_add_icon(HWND hWnd) {
	printf("adding icon\n");
	
	NOTIFYICONDATA data;
	notifyicondata_init(&data);
	
	BOOL res = Shell_NotifyIcon(NIM_ADD, &data);
	if( res ) {
		g_added = TRUE;
	} else {
		fprintf(stderr, "Shell_NotifyIcon() failed!\n");
	}
}


void tray_del_icon(HWND hWnd) {
	printf("hiding icon\n");
	
	NOTIFYICONDATA data;
	notifyicondata_init(&data);
	
	BOOL res = Shell_NotifyIcon(NIM_DELETE, &data);
	if( res ) {
		g_added = FALSE;
	} else {
		fprintf(stderr, "Shell_NotifyIcon() failed!\n");
	}
}


void tray_popup(HWND hWnd) {
	if( !g_added ) {
		fprintf(stderr, "tray icon not added - cannot show popup!\n");
		return;
	}
	printf("popup!\n");
	
	NOTIFYICONDATA data;
	notifyicondata_init(&data);
	
	// add balloon info
	data.uFlags |= NIF_INFO;
	strncpy(data.szInfo, "long popup description text", sizeof(data.szInfo)-1);
	strncpy(data.szInfoTitle, "popup title", sizeof(data.szInfoTitle)-1);
	data.dwInfoFlags = NIIF_INFO;
	
	BOOL res = Shell_NotifyIcon(NIM_MODIFY, &data);
	if( !res ) {
		fprintf(stderr, "Shell_NotifyIcon() failed!\n");
	}
}


void on_command(HWND hWnd, HWND hWndControl, int control_id, int notify_id) {
	switch(control_id) {
		case IDC_ADD_ICON:
			if( notify_id == BN_CLICKED)
				tray_add_icon(hWnd);
			break;
		case IDC_DEL_ICON:
			if( notify_id == BN_CLICKED)
				tray_del_icon(hWnd);
			break;
		case IDC_POPUP:
			if( notify_id == BN_CLICKED)
				tray_popup(hWnd);
			break;
	}
}


void on_tray(int icon_id, UINT uMsg) {
	switch(icon_id) {
		case MY_TRAY_ICON_ID: {
			switch(uMsg) {
				case WM_LBUTTONUP:
					printf("tray: left click\n");
					break;
				case WM_RBUTTONUP:
					printf("tray: right click\n");
					break;
			}
		} break;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
		case WM_CREATE:
			return 0;
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			return 0;
			break;
		case WM_COMMAND:
			// button clicks go here
			on_command(hWnd, (HWND)lParam, LOWORD(wParam), HIWORD(wParam));
			return 0;
			break;
		case MY_TRAYMESSAGE:
			on_tray((int)wParam, (UINT)lParam);
			return 0;
			break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpszCmdLine, int nShowCmd)
{
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
	wc.lpszClassName = MY_WNDCLASS;
	if( !RegisterClassEx(&wc) ) {
		fprintf(stderr, "RegisterClassEx() failed!\n");
		return EXIT_FAILURE;
	}
	
	int width = 400;
	int height = 300;
	int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	
	g_hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		MY_WNDCLASS, TEXT("Test tray (WineLib)"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		x, y, width, height,
		NULL, NULL, hInstance, NULL);
	if( !g_hWnd ) {
		fprintf(stderr, "CreateWindowEx() failed!\n");
		return EXIT_FAILURE;
	}
	
	int button_width = 90;
	int button_height = 30;
	int button_spacing = 5;
	x = 10;
	y = 10;
	
	CreateWindowEx(0,
		WC_BUTTON, TEXT("Add icon"),
		WS_CHILD | WS_VISIBLE,
		x, y, button_width, button_height,
		g_hWnd, (HMENU)IDC_ADD_ICON, hInstance, NULL);
	x += button_width + button_spacing;
	
	CreateWindowEx(0,
		WC_BUTTON, TEXT("Del icon"),
		WS_CHILD | WS_VISIBLE,
		x, y, button_width, button_height,
		g_hWnd, (HMENU)IDC_DEL_ICON, hInstance, NULL);
	x += button_width + button_spacing;
	
	CreateWindowEx(0,
		WC_BUTTON, TEXT("Show popup"),
		WS_CHILD | WS_VISIBLE,
		x, y, button_width, button_height,
		g_hWnd, (HMENU)IDC_POPUP, hInstance, NULL);
	x += button_width + button_spacing;
	
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}
