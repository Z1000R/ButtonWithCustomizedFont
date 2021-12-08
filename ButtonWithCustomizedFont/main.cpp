#include <SDKDDKVer.h>
#include <windows.h>
#include "resource.h"

#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")

using namespace std;

INT_PTR CALLBACK    DialogProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DialogBoxW(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DialogProc);

	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	static HFONT hFont{ nullptr };

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		int iCtrlId{ LOWORD(wParam) };
		switch (iCtrlId)
		{
		case IDOK:
			EndDialog(hDlg, iCtrlId);
			return (INT_PTR)TRUE;
		}
		break;
	}   // end of WM_COMMAND
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pDis = (LPDRAWITEMSTRUCT)lParam;
		if (pDis->CtlID == IDC_BUTTON1)
		{
			HDC hdc = pDis->hDC;
			RECT r = pDis->rcItem;

			// After the focus is removed, DrawFocusRect will leave a mark, which should be filled in.
			HBRUSH hBrush = GetSysColorBrush(COLOR_BTNFACE);
			FillRect(hdc, &r, hBrush);

			// Draw all edges of the button.
			if (pDis->itemAction & ODA_SELECT && pDis->itemState & ODS_SELECTED)
				DrawEdge(hdc, &r, EDGE_SUNKEN, BF_RECT);
			else
				DrawEdge(hdc, &r, EDGE_RAISED, BF_RECT);

			// Focus
			if (GetFocus() == pDis->hwndItem)
			{
				// Draw a frame when the button is focused.
				DWORD dwFrameColor = GetSysColor(COLOR_WINDOWFRAME);
				HBRUSH hBrushFrame = CreateSolidBrush(dwFrameColor);
				FrameRect(hdc, &r, hBrushFrame);
				DeleteObject(hBrushFrame);
				InflateRect(&r, -1, -1);

				// Draws a rectangle in the style used to indicate that the rectangle has the focus.
				RECT rFocus{ pDis->rcItem };
				InflateRect(&rFocus, -4, -4);
				DrawFocusRect(hdc, &rFocus);
			}

			// Font
			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(0xFF, 0x00, 0xFF));

			// Caption
			wchar_t wszCaption[64];
			int iLength = GetWindowTextW(pDis->hwndItem, wszCaption, sizeof(wszCaption) / sizeof(wszCaption[0]));
			int prevBkMode = SetBkMode(hdc, TRANSPARENT);
			DrawTextW(hdc, wszCaption, iLength, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(hdc, hOldFont);
			SetBkMode(hdc, prevBkMode);
		}

		return TRUE;
	}	// end of case WM_DRAWITEM:
	case WM_INITDIALOG:
	{
		// Do not display the close button.
		LONG_PTR lpStyle = GetWindowLongPtrW(hDlg, GWL_STYLE);
		lpStyle &= ~WS_SYSMENU;
		SetWindowLongPtrW(hDlg, GWL_STYLE, lpStyle);

		// Init Common Controls
		INITCOMMONCONTROLSEX ic;
		ic.dwICC = ICC_COOL_CLASSES;
		ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCommonControlsEx(&ic);

		// Button1 : set ownerdraw flag
		HWND hBtn = GetDlgItem(hDlg, IDC_BUTTON1);
		lpStyle = GetWindowLongPtrW(hBtn, GWL_STYLE);
		lpStyle |= BS_OWNERDRAW;
		SetWindowLongPtrW(hBtn, GWL_STYLE, lpStyle);

		// Fonts to use for button
		hFont = CreateFontW(-13, 0, 0, 0,
			FW_NORMAL, FALSE, FALSE, 0,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg 2");

		return (INT_PTR)TRUE;
	}   // end of WM_INITDIALOG
	case WM_DESTROY:
	{
		if (hFont)
			DeleteObject(hFont);
		break;
	}	// end of case WM_DESTROY:

	}	// end of switch (uMsg)

	return (INT_PTR)FALSE;
}