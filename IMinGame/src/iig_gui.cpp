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


#include "iig_gui.h"
#include "resource.h"
#include "iig_msg.h"
#include "iig_settings.h"

HWND txtUserMessage;
HWND txtInterval;
HWND txtSteamProfile;
HWND optAsMusic;
HWND optAsGame;
HWND lblGame;

extern HWND gHwnd;
extern LRESULT WINAPI IMinGameProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define APP_NAME    _T("IMinGame")
#define APP_VERSION _T("0.1.0")

void updateWindowText(const TCHAR* gameName) {
	RECT rect;
    SendMessage(lblGame, WM_SETTEXT, 0, (LPARAM)gameName);
    rect.left = 0; rect.top = 0; rect.right = 400; rect.bottom = 140;
    InvalidateRect(gHwnd, &rect, true);
}

void BuildGUI(HINSTANCE hInst, const SystemSettings& settings)
{
	WNDCLASSEX m_wc;

    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.style = CS_HREDRAW | CS_VREDRAW;
    m_wc.lpfnWndProc = IMinGameProc;
    m_wc.cbClsExtra = 0L;
    m_wc.cbWndExtra = 0L;
    m_wc.hInstance = hInst;
    m_wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1));
    m_wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    m_wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    m_wc.lpszMenuName = NULL;
    m_wc.lpszClassName = _T("IMinGame");
    m_wc.hIconSm = NULL;
    RegisterClassEx( &m_wc );

    // Windows
    RECT rect;
    rect.left=0;
    rect.top=0;
    rect.right=400;
    rect.bottom=140;
    gHwnd = CreateWindow(_T("IMinGame"), APP_NAME _T(" v") APP_VERSION, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom, NULL, NULL, hInst, NULL );
    AdjustWindowRect( &rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false);	

    CreateWindow(_T("STATIC"), _T("Message:"),  WS_CHILD | WS_VISIBLE | SS_SIMPLE, 5, 5, 70, 20, gHwnd, NULL, hInst, NULL);
    txtUserMessage = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),  WS_CHILD | WS_VISIBLE | ES_RIGHT, 75, 3, 120, 20, gHwnd, (HMENU)ID_EDIT_TITLE, hInst, NULL);
    lblGame = CreateWindow(_T("STATIC"), _T(" - Game Title"),  WS_CHILD | WS_VISIBLE | SS_SIMPLE, 195, 5, 500, 20, gHwnd, NULL, hInst, NULL);

	if (settings.legacyTimer) {
		CreateWindow(_T("STATIC"), _T("Refresh Interval:"),  WS_CHILD | WS_VISIBLE | SS_SIMPLE, 5, 28, 125, 20, gHwnd, NULL, hInst, NULL);
		txtInterval = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),  WS_CHILD | WS_VISIBLE | ES_NUMBER, 120, 26, 23, 20, gHwnd, (HMENU)ID_EDIT_INTERVAL, hInst, NULL);
		CreateWindow(_T("STATIC"), _T("secs"),  WS_CHILD | WS_VISIBLE | SS_SIMPLE, 150, 28, 30, 20, gHwnd, NULL, hInst, NULL);
	}

	//CreateWindow(_T("STATIC"), _T("Steam Profile:"),  WS_CHILD | WS_VISIBLE | SS_SIMPLE, 200, 28, 125, 20, gHwnd, NULL, hInst, NULL);
    //txtSteamProfile = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),  WS_CHILD | WS_VISIBLE | ES_NUMBER, 300, 26, 80, 20, gHwnd, (HMENU)ID_EDIT_STEAM, hInst, NULL);

    optAsMusic = CreateWindow(_T("BUTTON"), _T("Act as music"),  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON, 5, 50, 300, 20, gHwnd, (HMENU)ID_BUTTON_MUSIC, hInst, NULL);
    optAsGame = CreateWindow(_T("BUTTON"), _T("Act as game (don't show in contact list)"),  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON, 5, 67, 300, 20, gHwnd, (HMENU)ID_BUTTON_GAME, hInst, NULL);

    CreateWindow(_T("BUTTON"), _T("Refresh"),  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 108, 90, 98, 20, gHwnd, (HMENU)ID_BUTTON_REFRESH, hInst, NULL);
    CreateWindow(_T("BUTTON"), _T("Minimize to tray"),  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 208, 90, 120, 20, gHwnd, (HMENU)ID_BUTTON_MINIMIZE, hInst, NULL);
    CreateWindow(_T("BUTTON"), _T("Exit"),  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 330, 90, 60, 20, gHwnd, (HMENU)ID_BUTTON_EXIT, hInst, NULL);

    // Icon
    NOTIFYICONDATA tnd;
    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = gHwnd;
    tnd.uID = 0;
    tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
    tnd.uCallbackMessage = WM_TRAYMESSAGE;
    tnd.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1));
    wcscpy(tnd.szTip,_T("IMinGame"));
    Shell_NotifyIcon(NIM_ADD,&tnd);

    // Settings

    TCHAR strInterval[10];
    wsprintf(strInterval, _T("%u"), settings.interval);
    SendMessage(txtUserMessage, WM_SETTEXT, NULL, (LPARAM)settings.userMessage);
    SendMessage(txtInterval, WM_SETTEXT, NULL, (LPARAM)strInterval);
    if(settings.asGame)
        SendMessage(optAsGame,BM_SETCHECK,BST_CHECKED,0);
    else
        SendMessage(optAsMusic,BM_SETCHECK,BST_CHECKED,0);

}
