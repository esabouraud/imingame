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
#include "iig_settings.h"

void SaveSettings(const SystemSettings& settings)
{
	FILE *file = NULL;
	if ((file = _tfopen(_T("settings.dat"), _T("w"))) != NULL) {
		_ftprintf(file, _T("userMessage=%s\ninterval=%u\nasGame=%d\nlegacyTimer=%d\n"),
			settings.userMessage, settings.interval, settings.asGame, settings.legacyTimer);
		fclose(file);
    }
}

void LoadSettings(SystemSettings& settings)
{
	FILE *file = NULL;
	bool loadSuccess = false;
	if ((file = _tfopen(_T("settings.dat"), _T("r"))) != NULL) {
		//* \todo This part could easily overflow, and should be improved
		loadSuccess = _ftscanf(file, _T("userMessage=%[^\n]s"),
			&settings.userMessage) == 1;
		if (loadSuccess) {
			loadSuccess = _ftscanf(file, _T("\ninterval=%u\nasGame=%d\nlegacyTimer=%d\n"),
				&settings.interval, &settings.asGame, &settings.legacyTimer) == 3;
		}
		fclose(file);
	} 
	
	if (!loadSuccess) {
        _tcscpy(settings.userMessage,_T("In-Game"));
        settings.interval = 25;
        settings.asGame = false;
		settings.legacyTimer = false;
     }
}
