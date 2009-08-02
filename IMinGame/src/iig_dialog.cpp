/*
Copyright (C) 2009, Eric Sabouraud <eric.sabouraud@gmail.com>
Copyright (C) 2008-2009, Pier Castonguay <dunge2@yahoo.com>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- Neither the name of the Mumble Developers nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "iig_msg.h"
#include "iig_gui.h"
#include "iig_settings.h"
#include "iig_status.h"
#include "iig_detect.h"


extern HWND gHwnd;
extern SystemSettings gSystemSettings;
extern DWORD gGameProcessId;

static HBITMAP g_hbmBkgnd = NULL;
static BOOL gHidden = TRUE;

/**
	\brief Set captions for all elements of settings dialog
	\param [in] : settings dialog handle
*/
void initSettingsDialogText(HWND hwnd) {
	SetDlgItemText(hwnd, ID_GROUP_EMUMODE, getLangString(gSystemSettings.lang, IIG_LANGSTR_EMUMODELBL));
	SetDlgItemText(hwnd, ID_RADIO_EMUMUSIC, getLangString(gSystemSettings.lang, IIG_LANGSTR_ACTMUSICLBL));
	SetDlgItemText(hwnd, ID_RADIO_EMUGAME, getLangString(gSystemSettings.lang, IIG_LANGSTR_ACTGAMELBL));
	SetDlgItemText(hwnd, ID_COMBO_LANGUAGE, getLangString(gSystemSettings.lang, IIG_LANGSTR_LANGLBL));
	SetDlgItemText(hwnd, ID_BUTTON_OK, getLangString(gSystemSettings.lang, IIG_LANGSTR_OKLBL));
	SetDlgItemText(hwnd, ID_BUTTON_CANCEL, getLangString(gSystemSettings.lang, IIG_LANGSTR_CANCELLBL));
	SetDlgItemText(hwnd, ID_BUTTON_APPLY, getLangString(gSystemSettings.lang, IIG_LANGSTR_APPLYLBL));
	SetDlgItemText(hwnd, ID_GROUP_LISTS, getLangString(gSystemSettings.lang, IIG_LANGSTR_PROCLISTLBL));
	SetDlgItemText(hwnd, ID_BUTTON_BLIST, getLangString(gSystemSettings.lang, IIG_LANGSTR_EDITBLISTLBL));
	SetDlgItemText(hwnd, ID_BUTTON_WLIST, getLangString(gSystemSettings.lang, IIG_LANGSTR_EDITWLISTLBL));
}

/**
	\brief Settings dialog message processing
*/
BOOL CALLBACK SettingsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message) {
		case WM_INITDIALOG: {
			HWND hwndCombo = NULL;
			initSettingsDialogText(hwnd);
			hwndCombo = GetDlgItem(hwnd, IDC_COMBO_LANGUAGE);
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)getLangString(0, IIG_LANGSTR_LANG));
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)getLangString(1, IIG_LANGSTR_LANG)); 
			SendMessage(hwndCombo, CB_SETCURSEL, gSystemSettings.lang, 0);
			SendDlgItemMessage(hwnd, gSystemSettings.asGame ? ID_RADIO_EMUGAME : ID_RADIO_EMUMUSIC, BM_SETCHECK, BST_CHECKED, 0);
			return TRUE;
		}
		
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case ID_BUTTON_CANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
				case ID_BUTTON_OK:
				case ID_BUTTON_APPLY:
					// Reload whitelist and black list
					LoadSettings(&gSystemSettings);

					gSystemSettings.asGame = (BST_CHECKED == SendDlgItemMessage(hwnd, ID_RADIO_EMUGAME, BM_GETCHECK, 0, 0));
					gSystemSettings.lang = SendDlgItemMessage(hwnd, IDC_COMBO_LANGUAGE, CB_GETCURSEL, 0, 0);
					
					initSettingsDialogText(hwnd);
					resetWindowLabels(&gSystemSettings);

					SaveSettings(&gSystemSettings);
					if (LOWORD(wParam) == ID_BUTTON_OK) {
						EndDialog(hwnd, IDOK);
					}
					break;
				case ID_BUTTON_BLIST:
					ShellExecute(NULL, _T("open"), _T("blist.txt"), NULL, NULL, SW_SHOW);
					break;
				case ID_BUTTON_WLIST:
					ShellExecute(NULL, _T("open"), _T("wlist.txt"), NULL, NULL, SW_SHOW);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

/**
	\brief Main message processing dispatch function
*/
LRESULT WINAPI IMinGameProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	HDC hdc;

    switch (msg) {
	case WM_CREATE:
		g_hbmBkgnd = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_LOGO));
		if(g_hbmBkgnd == NULL)
			MessageBox(hWnd, _T("Could not load IDB_LOGO!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		break;
	case WM_PAINT:
		{
			BITMAP bm;
			PAINTSTRUCT ps;

			hdc = BeginPaint(hWnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmBkgnd);

			GetObject(g_hbmBkgnd, sizeof(bm), &bm);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			EndPaint(hWnd, &ps);
			break;
		}
    case WM_CLOSE:
		if(MessageBox(gHwnd, getLangString(gSystemSettings.lang, IIG_LANGSTR_EXITMBOX), getLangString(gSystemSettings.lang, IIG_LANGSTR_EXITLBL), MB_YESNO | MB_ICONQUESTION) == IDYES) {
			DestroyWindow(hWnd);
		}
        break;
	case WM_DESTROY:
			DeleteObject(g_hbmBkgnd);
			PostQuitMessage(0);
		break;
    case WM_CTLCOLORSTATIC:
        {
            if((HWND)lParam != txtUserMessage && (HWND)lParam != txtInterval)
            {
                hdc = (HDC)wParam;
                SetBkMode(hdc, TRANSPARENT);
                return (LPARAM)GetStockObject(NULL_BRUSH);
            }
        }
    case WM_TIMER: 
        {
			if (gSystemSettings.legacyTimer) {
				PoolProcesses();
			} else {
				if (gGameProcessId) {
					EnumWindows(EnumWindowsProc, gGameProcessId);
				} else {
					KillTimer(gHwnd, 0);
				}
			}
            return 0;
        }
	case WM_IIG_PROCSTART:
		{
			DWORD processID = lParam;
			TryProcess(processID);
			return 0;
        }
	case WM_IIG_PROCSTOP: 
        {	
			DWORD processID = lParam;
			if (gGameProcessId == processID) {
				ClearMessage();
			}
			return 0;
        }
    case WM_TRAYMESSAGE:
        {
            if(lParam == WM_LBUTTONDOWN || lParam == WM_LBUTTONDBLCLK || (lParam == WM_MOUSEMOVE && wParam == MK_LBUTTON) )
            {
                gHidden = !gHidden;
                if(gHidden)
                    ShowWindow( gHwnd, SW_SHOWDEFAULT );
                else
                    ShowWindow( gHwnd, SW_HIDE );
            }
            return 0;
        }
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
			case ID_BUTTON_SETTINGS:
				{
					int ret = DialogBox(GetModuleHandle(NULL), 
					MAKEINTRESOURCE(IDD_OPTIONS), hWnd, SettingsDlgProc);
					if(ret == IDOK){
						//* \bug Makes the main window flicker
						PoolProcesses();
					}
				}
				break;

			case ID_BUTTON_REFRESH:
				PoolProcesses();
				break;

				break;
			case ID_EDIT_TITLE:
				switch(HIWORD(wParam))
				{
				case EN_CHANGE:
					{
						TCHAR text[63];
						GetWindowText((HWND)lParam, text, 63);
						_tcscpy(gSystemSettings.userMessage, text);
						//SaveSettings(&gSystemSettings);
					}
				}
				break;
        }
        return 0;

    case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_MINIMIZE:
                {
                    gHidden = TRUE;
                    ShowWindow( gHwnd, SW_HIDE );
                    return TRUE;
                }
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}
