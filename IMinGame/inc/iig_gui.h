/*
Copyright (C) 2009-2010, Eric Sabouraud <eric.sabouraud@gmail.com>
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
- Neither the name of the IMinGame Developers nor the names of its
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

#ifndef _IIG_GUI_H_
#define _IIG_GUI_H_

#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "iig_settings.h"



extern HWND txtUserMessage;
extern HWND txtInterval;
extern HWND txtSteamProfile;
extern HWND optAsMusic;
extern HWND optAsGame;
extern HWND lblGame;

extern void updateWindowText(const TCHAR* gameName);

extern void resetWindowLabels(const SystemSettings* settings);

extern void BuildGUI(HINSTANCE hInst, const SystemSettings* settings);

#define IIG_LANGSTR_MACRO( id, eng, fre ) IIG_LANGSTR_##id,
typedef enum _IIG_LANGSTR_IDX {
#include "iig_txt-inl.h"
}_IIG_LANGSTR_IDX;
#undef IIG_LANGSTR_MACRO

extern const _TCHAR* getLangString(UINT lang, UINT idx);


#endif
