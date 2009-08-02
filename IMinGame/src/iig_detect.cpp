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

extern HWND gHwnd;
extern SystemSettings gSystemSettings;
extern DWORD gGameProcessId;


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
BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    TCHAR szWindowName[255] = _T("<unknown>");
    DWORD processID;

    GetWindowThreadProcessId(hwnd, &processID);
    GetWindowText(hwnd, szWindowName, 255);

    if(processID == (DWORD)lParam && 
       _tcscmp(szWindowName, _T("")) != 0 &&
       IsWindowVisible(hwnd))
    {
        GetWindowText(hwnd, szWindowName, 255);
		setMsnNowPlaying(gSystemSettings.userMessage, szWindowName, gSystemSettings.asGame, gHwnd);
		updateWindowText(szWindowName);

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
	\return TRUE if the process was indeed a game
*/
BOOL TryProcess(DWORD processId) {
	BOOL isPlaying = FALSE;
	DWORD cbNeeded, cProcessesModules;
    HMODULE hMod[1024];
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	TCHAR szModuleName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );

	if (NULL != hProcess) {
		if ( EnumProcessModules( hProcess, hMod, sizeof(hMod), &cbNeeded) ) {
			TCHAR szProcessName[1024+64];
			int i = 0;
			// Get the process name.
			GetModuleBaseName( hProcess, hMod[0], szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
			
			i = bwListSearch(szProcessName, gSystemSettings.whiteList, 0, gSystemSettings.whiteListSize);
			if (i >= 0) {
				// Process is whitelisted, update status immediately
				setMsnNowPlaying(gSystemSettings.userMessage, gSystemSettings.whiteList[i].windowName, gSystemSettings.asGame, gHwnd);
				updateWindowText(gSystemSettings.whiteList[i].windowName);
				gGameProcessId = processId;
				isPlaying = TRUE;
			} else {
				if(!isInBWList(szProcessName, gSystemSettings.blackList, gSystemSettings.blackListSize)) {
					// Process is not blacklisted, check its loaded modules
					BOOL hasModuleLoaded = FALSE;
					UINT j = 0, k = 0;
					cProcessesModules = cbNeeded / sizeof(HMODULE);
					for (k = 1; k < cProcessesModules && !hasModuleLoaded; ++k) {
						GetModuleBaseName( hProcess, hMod[k], szModuleName, sizeof(szModuleName)/sizeof(TCHAR) );
						for (j = 0; tabModulesList[j] != NULL; ++j) {
							if (_tcsicmp(szModuleName, tabModulesList[j]) == 0) {
								hasModuleLoaded = TRUE;
								break;
							}
						}
						if(hasModuleLoaded) {
							if (gSystemSettings.legacyTimer) {
								EnumWindows(EnumWindowsProc, processId);
							} else {
								SetTimer(gHwnd, 0, 5000, NULL);
							}
							gGameProcessId = processId;
							isPlaying = TRUE;
							break;
						}
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
void ClearMessage(void)
{
	setMsnNowPlaying(_T(""), _T(""), FALSE, gHwnd);
	updateWindowText(getLangString(gSystemSettings.lang, IIG_LANGSTR_GAMENAMEDEF));

	gGameProcessId = 0;
}

/**
	\brief Pool the list of process currently running
*/
void PoolProcesses(void)
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
        if( aProcesses[i] != 0 && TryProcess(aProcesses[i])) {
			newGameProcessId = aProcesses[i];
			break;
        }
    }

	// A game has stopped, clear message
	if (gGameProcessId != 0 && newGameProcessId == 0) {
        ClearMessage();
	}

    //gGameProcessId = newGameProcessId;
}
