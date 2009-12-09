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

#include <stdio.h>
#include <tchar.h>
#include "iig_settings.h"
#include "iig_gui.h"

//* \brief Do not check these processes for game modules
static const TCHAR* defaultBlackList[] = {
	_T("<unknown>"),
	_T("RGSC.EXE"), // rockstar social games club
	_T("WMPLAYER.EXE"),
	_T("ITUNES.EXE"),
	_T("VLC.EXE"), // videolan
	_T("BSPLAYER.EXE"),
	_T("IEXPLORE.EXE"),
	_T("FIREFOX.EXE"),
	_T("OPERA.EXE"),
	_T("WINAMP.EXE"),
	_T("MPLAYERC.EXE"), // media player classic
	_T("EXPLORER.EXE"),
	_T("STEAM.EXE"),
	_T("SMP.EXE"), // steam media player
	_T("msnmsgr.exe"), // msn/live messenger
	_T("nvCplUI.exe"), // nvidia control panel
	_T("mumble.exe"),
	_T("GameOverlayUI.exe") // steam in-game overlay
};

//* \brief Do not check these processes for game modules
static const TCHAR* defaultWhiteList[][2] = {
	{ _T("chuzzle.exe"), _T("Chuzzle") },
	{ _T("WinBej.exe"), _T("Bejeweled") },
};

static int bwListCompare(const void* elt1, const void* elt2) {
	return _tcsicmp(((struct bwListElt*)elt1)->procname, ((struct bwListElt*)elt2)->procname);
}

static int bwListCompareKey(const void* key, const void* elt) {
	return _tcsicmp((TCHAR*)key, ((struct bwListElt*)elt)->procname);
}

struct bwListElt* bwListSearch(const TCHAR* procname, const struct bwListElt list[], int listSize) {
	return (struct bwListElt*)bsearch(procname, list, listSize, sizeof(*list), bwListCompareKey);
}

static void RemoveDoublesFromBWList(struct bwListElt list[], UINT* pListSize) {
	UINT i = 0;
	for (i = 0; *pListSize > 1 && i < *pListSize - 1;) {
		if (_tcsicmp(list[i].procname, list[i+1].procname) == 0) {
			if (i < *pListSize - 2) {
				memmove(&list[i+1], &list[i+2], (*pListSize - (i+2)) * sizeof(*list));
			}
			(*pListSize)--;
		} else {
			++i;
		}
	}
}

void SaveBlackList(const SystemSettings* settings) {
	FILE *file = NULL;
	UINT i = 0;
	TCHAR filepath[_MAX_PATH];

	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\blist.txt"), settings->path);
	if ((file = _tfopen(filepath, _T("w"))) != NULL) {
		for (i = 0; i < settings->blackListSize; ++i) {
			_ftprintf(file, _T("%s\n"), settings->blackList[i].procname);
		}
		fclose(file);
	}
}

void SaveWhiteList(const SystemSettings* settings) {
	FILE *file = NULL;
	UINT i = 0;
	TCHAR filepath[_MAX_PATH];

	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\wlist.txt"), settings->path);
	if ((file = _tfopen(filepath, _T("w"))) != NULL) {
		for (i = 0; i < settings->whiteListSize; ++i) {
			_ftprintf(file, _T("%s|%s\n"), settings->whiteList[i].procname, settings->whiteList[i].windowName);
		}
		fclose(file);
	}
}

void LoadWhiteList(SystemSettings* settings) {
	FILE *file = NULL;
	TCHAR filepath[_MAX_PATH];

	// Read whitelist, restore default if missing
	settings->whiteListSize = 0;
	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\wlist.txt"), settings->path);
	if ((file = _tfopen(filepath, _T("r"))) != NULL) {
		while (settings->whiteListSize < sizeof(settings->whiteList)/sizeof(*settings->whiteList)) {
			if (_ftscanf(file, _T("%255[^|]|%255[^\n]\n"), settings->whiteList[settings->whiteListSize].procname, settings->whiteList[settings->whiteListSize].windowName) == 2) {
				++settings->whiteListSize;
			} else {
				break;
			}
		}
		fclose(file);	
	} else {
		while (settings->whiteListSize < sizeof(defaultWhiteList)/sizeof(*defaultWhiteList)
			&& settings->whiteListSize < sizeof(settings->whiteList)/sizeof(*settings->whiteList)) {
			_tcsncpy(settings->whiteList[settings->whiteListSize].procname, defaultWhiteList[settings->whiteListSize][0], 255);
			_tcsncpy(settings->whiteList[settings->whiteListSize].windowName, defaultWhiteList[settings->whiteListSize][1], 255);
			++settings->whiteListSize;
		}
	}
	qsort(settings->whiteList, settings->whiteListSize, sizeof(*settings->whiteList), bwListCompare);
	RemoveDoublesFromBWList(settings->whiteList, &settings->whiteListSize);
}

void LoadBlackList(SystemSettings* settings) {
	FILE *file = NULL;
	TCHAR filepath[_MAX_PATH];

	// Read blacklist, restore default if missing
	settings->blackListSize = 0;
	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\blist.txt"), settings->path);
	if ((file = _tfopen(filepath, _T("r"))) != NULL) {
		while (settings->blackListSize < sizeof(settings->blackList)/sizeof(*settings->blackList)) {
			if (_ftscanf(file, _T("%255[^\n]\n"), settings->blackList[settings->blackListSize].procname) == 1) {
				++settings->blackListSize;
			} else {
				break;
			}
		}
		fclose(file);
	}

	// Always add default blacklist in order to propagate updates
	while (settings->blackListSize < sizeof(defaultBlackList)/sizeof(*defaultBlackList)
		&& settings->blackListSize < sizeof(settings->blackList)/sizeof(*settings->blackList)) {
		_tcsncpy(settings->blackList[settings->blackListSize].procname, defaultBlackList[settings->blackListSize], 255);
		++settings->blackListSize;
	}

	qsort(settings->blackList, settings->blackListSize, sizeof(*settings->blackList), bwListCompare);
	RemoveDoublesFromBWList(settings->blackList, &settings->blackListSize);
}

static void AddToBWList(struct bwListElt list[], UINT listCapacity, UINT* pListSize, const TCHAR* procname, const TCHAR* windowName) {
	if (*pListSize < listCapacity) {
		_tcscpy(list[*pListSize].procname, procname);
		if (windowName) {
			_tcscpy(list[*pListSize].windowName, windowName);
		}
		++(*pListSize);
		qsort(list, *pListSize, sizeof(*list), bwListCompare);	
	}
}

static void RemoveFromBWList(struct bwListElt list[], UINT* pListSize, const TCHAR* procname) {
	struct bwListElt* elt = bwListSearch(procname, list, *pListSize);
	if (elt) {
		int len = &list[*pListSize] - elt;
		if (--len) {
			memmove(elt, elt + 1, len * sizeof(*list));
		}
		--(*pListSize);
	}
}

void AddToBlackList(SystemSettings* settings, const TCHAR* procname) {
	AddToBWList(settings->blackList, sizeof(settings->blackList)/sizeof(*settings->blackList), &settings->blackListSize, procname, NULL);
	SaveBlackList(settings);
}

void RemoveFromBlackList(SystemSettings* settings, const TCHAR* procname) {
	RemoveFromBWList(settings->blackList, &settings->blackListSize, procname);
	SaveBlackList(settings);
}

void AddToWhiteList(SystemSettings* settings, const TCHAR* procname, const TCHAR* windowName) {
	AddToBWList(settings->whiteList, sizeof(settings->whiteList)/sizeof(*settings->whiteList), &settings->whiteListSize, procname, windowName);
	SaveWhiteList(settings);
}

void RemoveFromWhiteList(SystemSettings* settings, const TCHAR* procname) {
	RemoveFromBWList(settings->whiteList, &settings->whiteListSize, procname);
	SaveWhiteList(settings);
}

void SaveSettings(const SystemSettings* settings)
{
	FILE *file = NULL;
	UINT i = 0;
	TCHAR filepath[_MAX_PATH];

	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\settings.ini"), settings->path);
	if ((file = _tfopen(filepath, _T("w"))) != NULL) {
		_ftprintf(file, _T("[general]\nuserMessage=%s\ninterval=%u\nasGame=%d\nlegacyTimer=%d\nlang=%u\n"),
			settings->userMessage, settings->interval, settings->asGame, settings->legacyTimer, settings->lang);
		fclose(file);
    }

	SaveWhiteList(settings);
	SaveBlackList(settings);
}


void LoadSettings(SystemSettings* settings)
{
	FILE *file = NULL;
	BOOL loadSuccess = FALSE;
	UINT lang = 0;
	TCHAR filepath[_MAX_PATH];
	DWORD pathlen = sizeof(settings->path);
	HKEY hkey = NULL;

	memset(settings->path, 0, pathlen);
	_sntprintf(settings->path, sizeof(settings->path)/sizeof(*settings->path), _T("."));

	// Try and read installation directory from registry, use current directory if it fails
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\IMinGame"), 0, KEY_READ, &hkey)) {
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\IMinGame"), 0, KEY_READ, &hkey)) {
			hkey = NULL;
		}
	}

	if (hkey) {
		if (ERROR_SUCCESS != RegQueryValueEx(hkey, _T("Path"), NULL, NULL, (LPBYTE)settings->path, &pathlen)) {
			_sntprintf(settings->path, sizeof(settings->path)/sizeof(*settings->path), _T("."));
		}
		RegCloseKey(hkey);
	}

	// Read settings file if present
	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\settings.ini"), settings->path);
	if ((file = _tfopen(filepath, _T("r"))) != NULL) {
		// This part could be replaced with GetPrivateProfileSection/GetPrivateProfileString
		loadSuccess = _ftscanf(file, _T("[general]\nuserMessage=%62[^\n]\ninterval=%u\nasGame=%d\nlegacyTimer=%d\nlang=%u\n"),
			&settings->userMessage, &settings->interval, &settings->asGame, &settings->legacyTimer, &settings->lang) == 5;
		fclose(file);
	} 

	// Fallback on pre-settings file (created by installer) if settings file is missing or corrupted
	_sntprintf(filepath, sizeof(filepath)/sizeof(*filepath), _T("%s\\presettings.ini"), settings->path);
	if (!loadSuccess && (file = _tfopen(filepath, _T("r"))) != NULL) {
		_ftscanf(file, _T("[general]\nlang=%u\n"), &lang);
		fclose(file);
	}

	// Set default settings
	if (!loadSuccess) {
        settings->interval = 25;
        settings->asGame = FALSE;
		settings->legacyTimer = FALSE;
		settings->lang = lang;
		_tcscpy(settings->userMessage, getLangString(settings->lang, IIG_LANGSTR_USERMSGDEF));
     }

	LoadWhiteList(settings);
	LoadBlackList(settings);
}
