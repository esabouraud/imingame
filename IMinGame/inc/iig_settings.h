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

#ifndef _IIG_SETTINGS_H_
#define _IIG_SETTINGS_H_

#include <windows.h>
#include <tchar.h>

#define IIG_PROCNAME_MAXLEN _MAX_FNAME
#define IIG_APPNAME_MAXLEN _MAX_FNAME

struct bwListElt {
	TCHAR procname[IIG_PROCNAME_MAXLEN];
	TCHAR windowName[IIG_APPNAME_MAXLEN];
};

typedef struct _SystemSettings
{
	TCHAR userMessage[63];
	UINT interval;
	BOOL asGame;
	BOOL legacyTimer;
	UINT lang;

	struct bwListElt whiteList[50];
	UINT whiteListSize;

	struct bwListElt blackList[100];
	UINT blackListSize;
} SystemSettings; 

extern void SaveSettings(const SystemSettings* settings);

extern void LoadSettings(SystemSettings* settings);

extern void SaveBlackList(const SystemSettings* settings);

extern void LoadBlackList(SystemSettings* settings);

extern void SaveWhiteList(const SystemSettings* settings);

extern void LoadWhiteList(SystemSettings* settings);

/**
	\brief Binary search for process name in sorted black/white list
	\param [in] procname: process name to look for
	\param [in] list: list to search
	\param [in] listSize : number of elements in list
	\return pointer on element in list if found, else NULL
*/
extern struct bwListElt* bwListSearch(const TCHAR* procname, const struct bwListElt list[], int listSize);

extern void AddToBlackList(SystemSettings* settings, const TCHAR* procname);

extern void RemoveFromBlackList(SystemSettings* settings, const TCHAR* procname);

extern void AddToWhiteList(SystemSettings* settings, const TCHAR* procname, const TCHAR* windowName);

extern void RemoveFromWhiteList(SystemSettings* settings, const TCHAR* procname);

#endif
