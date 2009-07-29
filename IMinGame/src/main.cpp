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

/**
	\file
	\brief IMinGame core
*/

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <Psapi.h>

#include "iig_msg.h"
#include "iig_gui.h"
#include "iig_settings.h"
#include "iig_status.h"


typedef void (*HookInstaller)(HWND);
typedef void (*HookUninstaller)();


HWND gHwnd = NULL;


static bool gHidden = true;

static DWORD gGameProcessId = 0;

static SystemSettings gSystemSettings;


//* \brief Do not check these processes for game modules
static const TCHAR* tabBlackList[] = {
	_T("<unknown>"),
	_T("RGSC.EXE"),
	_T("WMPLAYER.EXE"),
	_T("ITUNES.EXE"),
	_T("VLC.EXE"),
	_T("BSPLAYER.EXE"),
	_T("IEXPLORE.EXE"),
	_T("FIREFOX.EXE"),
	_T("OPERA.EXE"),
	_T("WINAMP.EXE"),
	NULL
};

//* \brief Processes using one of these modules are considered as games
static const TCHAR* tabModulesList[] = {
	_T("D3D8.DLL"),
	_T("D3D9.DLL"),
	_T("D3D10.DLL"),
	_T("D3D11.DLL"),
	_T("OPENGL32.DLL"),
	NULL
};




/**
	\brief Callback function to find name of game process window and update IM status
	\param [in] hwnd : handle of window to check
	\param [in] lParam : PID of game process
	\return TRUE
*/
static BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    TCHAR szWindowName[255] = _T("<unknown>");
    TCHAR szWindowNameMod[255] = _T(" - ");
    DWORD processID;

    GetWindowThreadProcessId(hwnd, &processID);
    GetWindowText(hwnd, szWindowName, 255);

    if(processID == (DWORD)lParam && 
       _tcscmp(szWindowName, _T("")) != 0 &&
       IsWindowVisible(hwnd))
    {
        GetWindowText(hwnd, szWindowName, 255);
		setMsnNowPlaying(gSystemSettings.userMessage, szWindowName, gSystemSettings.asGame, gHwnd);
		_tcscat(szWindowNameMod, szWindowName);
		updateWindowText(szWindowNameMod);

		if (!gSystemSettings.legacyTimer) {
			KillTimer(gHwnd, 0);
		}
		return FALSE;
    }
    return TRUE;
}

/**
	\brief Check if a process is a game, trigger status update if it is
	\param [in] processId : PID of the process to check
	\return true if the process was indeed a game
*/
static bool tryProcess(DWORD processId) {
	bool isPlaying = false;
	DWORD cbNeeded, cProcessesModules;
    HMODULE hMod[1024];
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	TCHAR szModuleName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );

	if (NULL != hProcess) {
		if ( EnumProcessModules( hProcess, hMod, sizeof(hMod), &cbNeeded) ) {
			TCHAR szProcessName[1024+64];
			// Get the process name.
			GetModuleBaseName( hProcess, hMod[0], szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
			bool isBlacklisted = false;
			for (unsigned int i = 0; tabBlackList[i] != NULL; ++i) {
				if (_tcscmp(_tcsupr(szProcessName), tabBlackList[i]) == 0) {
					isBlacklisted = true;
					break;
				}
			}
			
			if(!isBlacklisted) {
				cProcessesModules = cbNeeded / sizeof(HMODULE);
				for (unsigned int k = 1; k < cProcessesModules; ++k) {
					GetModuleBaseName( hProcess, hMod[k], szModuleName, sizeof(szModuleName)/sizeof(TCHAR) );
					bool hasModuleLoaded = false;
					for (unsigned int i = 0; tabModulesList[i] != NULL; ++i) {
						if (_tcscmp(_tcsupr(szModuleName), tabModulesList[i]) == 0) {
							hasModuleLoaded = true;break;
						}
					}
					if(hasModuleLoaded) {
						if (gSystemSettings.legacyTimer) {
							EnumWindows(EnumWindowsProc, processId);
						} else {
							SetTimer(gHwnd, 0, 5000, NULL);
						}
						gGameProcessId = processId;
						isPlaying = true;
						break;
					}
				}
			}
		}
	}

	CloseHandle( hProcess );

	return isPlaying;
}

/**
	\brief Reset name of game in IM status and application window
*/
static void ClearMessage()
{
	setMsnNowPlaying(_T(""), _T(""), false, gHwnd);
	updateWindowText(_T(" - Game Title"));

	gGameProcessId = 0;
}

/**
	\brief Used for in legacy process pooling mode
*/
static void PoolProcesses()
{
	DWORD newGameProcessId = 0;

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    // Get the list of process identifiers.
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return;

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

	// Try and find a game process
    for ( i = 0; i < cProcesses; i++ ) {
        if( aProcesses[i] != 0 && tryProcess(aProcesses[i])) {
			newGameProcessId = aProcesses[i];
			break;
        }
    }

	// A game has stopped, clear message
	if (gGameProcessId != 0 && newGameProcessId == 0) {
        ClearMessage();
	}

    gGameProcessId = newGameProcessId;
}



/**
	\brief Main message processing dispatch function
*/
LRESULT WINAPI IMinGameProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg) 
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_CLOSE:
        if(MessageBox(gHwnd, _T("This will stop showing games in your MSN status, are you sure?"), _T("Exit"), MB_YESNO | MB_ICONQUESTION) == IDYES)
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
				EnumWindows(EnumWindowsProc, gGameProcessId);
			}
            return 0;
        }
	case WM_IIG_PROCSTART:
		{
			DWORD processID = lParam;
			tryProcess(processID);
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
        switch(LOWORD(wParam))
        {
		case ID_BUTTON_REFRESH:
			PoolProcesses();
			break;
        case ID_BUTTON_MINIMIZE:
            gHidden = true;
            ShowWindow( gHwnd, SW_HIDE );
            break;
        case ID_BUTTON_EXIT:
            if(MessageBox(gHwnd, _T("This will stop showing games in your MSN status, are you sure?"), _T("Exit"), MB_YESNO | MB_ICONQUESTION) == IDYES)
                PostQuitMessage(0);
            break;
        case ID_EDIT_TITLE:
            switch(HIWORD(wParam))
            {
            case EN_CHANGE:
                {
                    TCHAR text[63];
                    GetWindowText((HWND)lParam, text, 63);
                    _tcscpy(gSystemSettings.userMessage, text);
                    SaveSettings(gSystemSettings);
                }
            }
            break;
        case ID_EDIT_INTERVAL:
            switch(HIWORD(wParam))
            {
            case EN_CHANGE:
                {
                    TCHAR text[10];
                    GetWindowText((HWND)lParam, text, 10);
                    gSystemSettings.interval = _ttoi(text);
                    SaveSettings(gSystemSettings);
					if (gSystemSettings.legacyTimer) {
						SetTimer( gHwnd, 0, gSystemSettings.interval * 1000, (TIMERPROC)NULL );
					}
                }
            }
            break;
        case ID_BUTTON_MUSIC:
            SendMessage(optAsMusic,BM_SETCHECK,BST_CHECKED,0);
            SendMessage(optAsGame,BM_SETCHECK,BST_UNCHECKED,0);
            gSystemSettings.asGame = false;
            SaveSettings(gSystemSettings);
            break;
        case ID_BUTTON_GAME:
            SendMessage(optAsMusic,BM_SETCHECK,BST_UNCHECKED,0);
            SendMessage(optAsGame,BM_SETCHECK,BST_CHECKED,0);
            gSystemSettings.asGame = true;
            SaveSettings(gSystemSettings);
            break;
        }
        return 0;

    case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_MINIMIZE:
                {
                    gHidden = true;
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


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
    HANDLE hMutex;
    hMutex = CreateMutex(NULL, TRUE, _T("NOWPLAYINGMUTEX"));
    if(GetLastError() == ERROR_ALREADY_EXISTS)
        return 0;

	LoadSettings(gSystemSettings);

	BuildGUI(hInst, gSystemSettings);

    ShowWindow(gHwnd, SW_SHOWDEFAULT );

    NOTIFYICONDATA tnd;
    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = gHwnd;
    tnd.uID = 0;
    tnd.uFlags = NIF_INFO;
    tnd.dwInfoFlags = NIIF_INFO;
    _tcscpy(tnd.szInfoTitle, _T("IMinGame"));
    _tcscpy(tnd.szInfo, _T("The application stay active in the system tray.\nYou can now start a game of your choice and it will appear in MSN Messenger."));
    Shell_NotifyIcon(NIM_MODIFY,&tnd);

    
	HookInstaller installHook = NULL;
	HookUninstaller uninstallHook = NULL;

	if (gSystemSettings.legacyTimer) {
		SetTimer( gHwnd, 0, gSystemSettings.interval * 1000, (TIMERPROC)NULL );
	} else {
		HINSTANCE hinst = LoadLibrary(_T("IMinGameHook.dll")); 
		if (hinst == NULL) 
		{
			_tprintf(_T("null hinst"));
			return -1;
		} 

		installHook = (HookInstaller) GetProcAddress(hinst, "install");
		uninstallHook = (HookUninstaller) GetProcAddress(hinst, "uninstall");

		if (!installHook || !uninstallHook) {
			return -1;
		}

		installHook(gHwnd);
	}


	// Catch game running before program launch
	PoolProcesses();	

    // Message Loop
    MSG msg={0};
    BOOL bRet;
    while( (bRet = GetMessage( &msg, 0, 0, 0 )) != 0)
    { 
        if (bRet == -1)
            break;
        else
        {
            if(!IsDialogMessage(gHwnd, &msg))
            {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            }
        }
    }

    // Quit
	if (gSystemSettings.legacyTimer) {
	    KillTimer(gHwnd, 0); 
	} else {
		uninstallHook();
	}


	ClearMessage();

    SaveSettings(gSystemSettings);

    NOTIFYICONDATA tnid;
    tnid.cbSize=sizeof(NOTIFYICONDATA);
    tnid.hWnd=gHwnd;
    tnid.uID=0;
    Shell_NotifyIcon(NIM_DELETE,&tnid);

    DestroyWindow(gHwnd);
    UnregisterClass( _T("IMinGame"), hInst );
    ReleaseMutex(hMutex);
    return 0;
}
