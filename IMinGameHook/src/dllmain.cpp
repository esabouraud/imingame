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
#include <tchar.h>
#include <stdio.h>

#include "iig_msg.h"

HINSTANCE hinst;
HHOOK hhk;
#pragma comment(linker, "/section:HOOKDAT,RWS") 
#pragma data_seg( "HOOKDAT" )
HWND cbHwnd = NULL;
#pragma data_seg()


LRESULT CALLBACK CallWndProc(int code,WPARAM wParam,LPARAM lParam) {  
	return CallNextHookEx(hhk,code,wParam,lParam);
}

extern "C" __declspec(dllexport) void install(HWND hWnd) {
	cbHwnd = hWnd;
	hhk = SetWindowsHookEx(WH_CBT, CallWndProc, hinst, NULL);
}
extern "C" __declspec(dllexport) void uninstall() {
	UnhookWindowsHookEx(hhk); 
}

BOOL WINAPI DllMain(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
  ) {
	hinst = hinstDLL;

	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			PostMessage(cbHwnd, WM_IIG_PROCSTART, NULL, GetCurrentProcessId());
			break;
		case DLL_PROCESS_DETACH:
			PostMessage(cbHwnd, WM_IIG_PROCSTOP, NULL, GetCurrentProcessId());
			break;
	}

	return TRUE;
}
