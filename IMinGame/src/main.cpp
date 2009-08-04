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
#include "iig_detect.h"


typedef void (*HookInstaller)(HWND);
typedef void (*HookUninstaller)();


HWND gHwnd = NULL;
DWORD gGameProcessId = 0;
SystemSettings gSystemSettings;




int APIENTRY _tWinMain(
	HINSTANCE hInst,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
    HANDLE hMutex;
	NOTIFYICONDATA tnd, tnid;
	HookInstaller installHook = NULL;
	HookUninstaller uninstallHook = NULL;

    hMutex = CreateMutex(NULL, TRUE, _T("NOWPLAYINGMUTEX"));
    if(GetLastError() == ERROR_ALREADY_EXISTS)
        return 0;

	LoadSettings(&gSystemSettings);
	SaveSettings(&gSystemSettings);
	BuildGUI(hInst, &gSystemSettings);
    ShowWindow(gHwnd, SW_SHOWDEFAULT );

    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = gHwnd;
    tnd.uID = 0;
    tnd.uFlags = NIF_INFO;
    tnd.dwInfoFlags = NIIF_INFO;
    _tcscpy(tnd.szInfoTitle, _T("IMinGame"));
    _tcscpy(tnd.szInfo, getLangString(gSystemSettings.lang, IIG_LANGSTR_HELPTXT));
    Shell_NotifyIcon(NIM_MODIFY,&tnd);

	DragAcceptFiles(gHwnd, TRUE);
    
	if (gSystemSettings.legacyTimer) {
		SetTimer( gHwnd, 0, gSystemSettings.interval * 1000, (TIMERPROC)NULL );
	} else {
		HINSTANCE hinst = LoadLibrary(_T("IMinGameHook.dll")); 
		if (hinst == NULL) 
		{
			MessageBox(gHwnd, _T("IMinGameHook.dll load failed !"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
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
	{
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
	}

    // Quit
	if (gSystemSettings.legacyTimer) {
	    KillTimer(gHwnd, 0); 
	} else {
		uninstallHook();
	}


	ClearMessage();
    SaveSettings(&gSystemSettings);

	tnid.cbSize=sizeof(NOTIFYICONDATA);
	tnid.hWnd=gHwnd;
	tnid.uID=0;
	Shell_NotifyIcon(NIM_DELETE,&tnid);

    DestroyWindow(gHwnd);
    UnregisterClass( _T("IMinGame"), hInst );
    ReleaseMutex(hMutex);
    return 0;
}
