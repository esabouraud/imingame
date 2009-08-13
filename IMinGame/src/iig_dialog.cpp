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
#include "iig_shlnk.h"


extern HWND gHwnd;
extern SystemSettings gSystemSettings;
extern DWORD gGameProcessId;

static HBITMAP g_hbmBkgnd = NULL;
static BOOL gHidden = TRUE;

// Used to exchange data between white list dialog and main dialog
static TCHAR procname[IIG_PROCNAME_MAXLEN];
static TCHAR appname[IIG_APPNAME_MAXLEN];

/**
	\brief Set captions for all elements of settings dialog
	\param [in] : settings dialog handle
*/
void initSettingsDialogText(HWND hwnd) {
	SetWindowText(hwnd, getLangString(gSystemSettings.lang, IIG_LANGSTR_SETTINGSLBL));
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
					LoadBlackList(&gSystemSettings);
					LoadWhiteList(&gSystemSettings);

					gSystemSettings.asGame = (BST_CHECKED == SendDlgItemMessage(hwnd, ID_RADIO_EMUGAME, BM_GETCHECK, 0, 0));
					gSystemSettings.lang = SendDlgItemMessage(hwnd, IDC_COMBO_LANGUAGE, CB_GETCURSEL, 0, 0);
					
					initSettingsDialogText(hwnd);
					resetWindowLabels(&gSystemSettings);

					SaveSettings(&gSystemSettings);
					PoolProcesses();
					if (LOWORD(wParam) == ID_BUTTON_OK) {
						EndDialog(hwnd, IDOK);
					}
					break;
				case ID_BUTTON_BLIST:
					{
						TCHAR filepath[_MAX_PATH];
						_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\blist.txt"), gSystemSettings.path);
						ShellExecute(NULL, _T("open"), filepath, NULL, NULL, SW_SHOW);
					}
					break;
				case ID_BUTTON_WLIST:
					{
						TCHAR filepath[_MAX_PATH];
						_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\wlist.txt"), gSystemSettings.path);
						ShellExecute(NULL, _T("open"), filepath, NULL, NULL, SW_SHOW);
					}
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
	\brief Settings dialog message processing
*/
BOOL CALLBACK WhiteListDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message) {
		case WM_INITDIALOG:
			{
				SetWindowText(hwnd, getLangString(gSystemSettings.lang, IIG_LANGSTR_WHITELISTDLG));
				SetDlgItemText(hwnd, IDOK, getLangString(gSystemSettings.lang, IIG_LANGSTR_OKLBL));
				SetDlgItemText(hwnd, IDCANCEL, getLangString(gSystemSettings.lang, IIG_LANGSTR_CANCELLBL));
				SetDlgItemText(hwnd, ID_GROUP_WLNAME, getLangString(gSystemSettings.lang, IIG_LANGSTR_WHITELISTDLGGRPBX));
				SetDlgItemText(hwnd, ID_STATIC_WLNAME, procname);
				SetDlgItemText(hwnd, ID_EDIT_WLNAME, appname);
				return TRUE;
			}
		
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDCANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
				case IDOK:
					GetDlgItemText(hwnd, ID_EDIT_WLNAME, appname, sizeof(appname)/sizeof(*appname));
					EndDialog(hwnd, IDOK);
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
			RECT rect;

			hdc = BeginPaint(hWnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmBkgnd);

			GetObject(g_hbmBkgnd, sizeof(bm), &bm);

			GetClientRect(hWnd, &rect);
			SetStretchBltMode(hdc, HALFTONE);
			StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

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
            if((HWND)lParam != txtUserMessage /*&& (HWND)lParam != txtInterval*/)
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
						//* \bug Makes the main window flickering obvious -> done in settings dialog instead
						//PoolProcesses();
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
			case ID_BUTTON_BLACKLIST:
				{
					if (gGameProcessId) {
						if (GetProcessName(procname, sizeof(procname), gGameProcessId)) {
							struct bwListElt* whiteElt = bwListSearch(procname, gSystemSettings.whiteList, gSystemSettings.whiteListSize);
							const TCHAR* question = NULL;
							if (whiteElt) {
								question = getLangString(gSystemSettings.lang, IIG_LANGSTR_BLACKLISTWHITECONFIRM);
							} else {
								question = getLangString(gSystemSettings.lang, IIG_LANGSTR_BLACKLISTCONFIRM);
							}

							if(MessageBox(gHwnd, question, getLangString(gSystemSettings.lang, IIG_LANGSTR_BLACKLISTBTNLBL), MB_YESNO | MB_ICONQUESTION) == IDYES) {
								if (whiteElt) {
									RemoveFromWhiteList(&gSystemSettings, procname);
								}
								RemoveFromBlackList(&gSystemSettings, procname);
								AddToBlackList(&gSystemSettings, procname);
								PoolProcesses();
							}
						}
					}
				}
				break;
        }
        return 0;

	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			TCHAR filepath[_MAX_PATH];
			TCHAR filepath2[_MAX_PATH];
			BOOL addToList = FALSE;
			if (DragQueryFile(hDrop, 0, filepath, sizeof(filepath)/sizeof(*filepath))) {
				TCHAR fileext[_MAX_EXT];
				_tsplitpath(filepath, NULL, NULL, procname, fileext);
				_tcscpy(appname, procname);
				if (_tcsicmp(fileext, _T(".lnk")) == 0) {
					if (SUCCEEDED(ResolveIt(hWnd, filepath, filepath2, _MAX_PATH))) {
						_tcscpy(appname, procname);
						_tsplitpath(filepath2, NULL, NULL, procname, fileext);
					}
				} 
				if(_tcsicmp(fileext, _T(".exe")) == 0) {
					if (_tcslen(procname) + _tcslen(fileext) < _MAX_FNAME) {
						_tcscat(procname, fileext);
						addToList = TRUE;
					}
				}

				if (addToList) {
					if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_WHITELIST), hWnd, WhiteListDlgProc) == IDOK) {
						RemoveFromBlackList(&gSystemSettings, procname);
						RemoveFromWhiteList(&gSystemSettings, procname);
						if (_tcscmp(appname, _T("")) == 0) {
							AddToWhiteList(&gSystemSettings, procname, procname);
						} else {
							AddToWhiteList(&gSystemSettings, procname, appname);
						}
						PoolProcesses();
					}
				}
			}
			DragFinish(hDrop);
		}
		break;
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
