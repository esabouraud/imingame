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

/**
	\file
	\brief IMinGame steam web integration

	The code in this file uses functions contained in the libcurl and expat libraries.
*/
#include <stdio.h>
#include "curl/curl.h"
#include "expat.h"

#include "iig_steam.h"

/* Data structure fed to curl write callback */
typedef struct _SteamXmlHandler {
	XML_Parser parser;
} SteamXmlHandler;

/* Basic parser state machine steps */
typedef enum {
	ROOT, 
	PROFILE,
	ONLINE_STATE,
	INGAME_SERVER_IP,
	INGAME_INFO,
	GAME_NAME
} SteamXmlState;

/* Data structure fed to expat parse callbacks */
typedef struct _SteamXmlParseData {
	XML_Parser parser;
	SteamXmlState xmlState;
	SteamInGameInfo steaminfo;
} SteamXmlParseData;

static size_t iigCurlWriteCallback(
	char *buffer,
	size_t size,
	size_t nitems,
	void *outstream
) {
	SteamXmlHandler* handler = (SteamXmlHandler*)outstream;
	SteamXmlParseData* parseData = NULL;

	//printf("iigCurlWriteCallback %d %d\n", size, nitems);
	XML_Parse(handler->parser, buffer, size * nitems, 0);

	parseData = (SteamXmlParseData*)XML_GetUserData(handler->parser);
	if (parseData->steaminfo.onlineState.found && parseData->steaminfo.gameName.found) {
		// inGameServerIP is optional
		return 0;
	}

	return size * nitems;
}

/*
	The expected xml format is
	<profile>
	  ...
	  <onlineState>CDATA</onlineState>
	  ...
	  <inGameServerIP>CDATA</inGameServerIP>
	  <inGameInfo>
	  ...
	    <gameName>CDATA</gameName>
	Parsing stops as soon as these tags are parsed, or an unexpected element is found.
*/
static void SteamStartXmlElementHandler(
	void *userData,
	const XML_Char *name,
	const XML_Char **atts
) {
	SteamXmlParseData* parseData = (SteamXmlParseData*)userData;
	switch (parseData->xmlState) {
		case ROOT:
			if (strcmp(STEAM_XMLTAG_PROFILE, name) == 0) {
				parseData->xmlState = PROFILE;
			} else {
				/* Unexpected xml document format, stop parsing */
				XML_StopParser(parseData->parser, FALSE);
			}
			break;
		case PROFILE:
			if (strcmp(STEAM_XMLTAG_ONLINE_STATE, name) == 0) {
				parseData->xmlState = ONLINE_STATE;
				parseData->steaminfo.onlineState.found = TRUE;
			} else if (strcmp(STEAM_XMLTAG_INGAME_SERVER_IP, name) == 0) {
				parseData->xmlState = INGAME_SERVER_IP;
				parseData->steaminfo.inGameServerIP.found = TRUE;
			} else if (strcmp(STEAM_XMLTAG_INGAME_INFO, name) == 0) {
				parseData->xmlState = INGAME_INFO;
			}
			break;
		case INGAME_INFO:
			if (strcmp(STEAM_XMLTAG_GAME_NAME, name) == 0) {
				parseData->xmlState = GAME_NAME;
				parseData->steaminfo.gameName.found = TRUE;
			}
			break;
	}
}

static void SteamEndXmlElementHandler(
	void *userData,
	const XML_Char *name
) {
	SteamXmlParseData* parseData = (SteamXmlParseData*)userData;
	switch (parseData->xmlState) {	
		case PROFILE:
			if (strcmp(STEAM_XMLTAG_PROFILE, name) == 0) {
				parseData->xmlState = ROOT;
			}
			break;
		case ONLINE_STATE:
			if (strcmp(STEAM_XMLTAG_ONLINE_STATE, name) == 0) {
				parseData->xmlState = PROFILE;
			} else {
				/* Unexpected xml format, stop parsing */
				XML_StopParser(parseData->parser, FALSE);
			}
			break;
		case INGAME_SERVER_IP:
			if (strcmp(STEAM_XMLTAG_INGAME_SERVER_IP, name) == 0) {
				parseData->xmlState = PROFILE;
				if (_tcslen(parseData->steaminfo.inGameServerIP.string) == 0) {
					parseData->steaminfo.inGameServerIP.found = FALSE;
				}
			} else {
				/* Unexpected xml format, stop parsing */
				XML_StopParser(parseData->parser, FALSE);
			}
			break;
		case INGAME_INFO:
			if (strcmp(STEAM_XMLTAG_INGAME_INFO, name) == 0) {
				parseData->xmlState = PROFILE;
			}
			break;
		case GAME_NAME:
			if (strcmp(STEAM_XMLTAG_GAME_NAME, name) == 0) {
				parseData->xmlState = INGAME_INFO;
			} else {
				/* Unexpected xml format, stop parsing */
				XML_StopParser(parseData->parser, FALSE);
			}
			break;
	}
}

static int copyStringCharToTchar(const char * in, size_t inlen, TCHAR * out, size_t outlen) {
	int res = 0;

#ifdef UNICODE
	res = MultiByteToWideChar(CP_ACP, 0, in, inlen, out, outlen);
#else
	if (outlen > inlen) {
		memcpy(out, in, inlen);
		out[inlen] = 0;
		res = inlen;
	}
#endif
	return res;
}

static void SteamCharXmlElementHandler(
	void *userData,
	const XML_Char *s,
	int len
) {
	SteamXmlParseData* parseData = (SteamXmlParseData*)userData;
	switch (parseData->xmlState) {
		case ONLINE_STATE:
			//TODO handle errors
			copyStringCharToTchar(s, len, parseData->steaminfo.onlineState.string, sizeof(parseData->steaminfo.onlineState.string)/sizeof(parseData->steaminfo.onlineState.string[0]));
			if (_tcscmp(_T("in-game"), parseData->steaminfo.onlineState.string) != 0) {
				/* steam user not in-game, stop parsing */
				XML_StopParser(parseData->parser, FALSE);
			} else {
				parseData->steaminfo.onlineState.isInGame = TRUE;
			}
			break;
		case INGAME_SERVER_IP:
			copyStringCharToTchar(s, len, parseData->steaminfo.inGameServerIP.string, sizeof(parseData->steaminfo.inGameServerIP.string)/sizeof(parseData->steaminfo.inGameServerIP.string[0]));
			break;
		case GAME_NAME:
			copyStringCharToTchar(s, len, parseData->steaminfo.gameName.string, sizeof(parseData->steaminfo.gameName.string)/sizeof(parseData->steaminfo.gameName.string[0]));
			break;
	}
}


int getSteamProfileInfo(const TCHAR * url, const TCHAR * lang, SteamInGameInfo * steamInfo) {
	CURL *curl = NULL;
	int res = -1;
	SteamXmlHandler handler = {NULL};
	SteamXmlParseData parseData = { NULL, ROOT , { { FALSE, FALSE, _T("") }, { FALSE, _T("") }, { FALSE, _T("")} } };
	char wurl[512];
	char wcookie[128] = "Steam_Language=";

#ifdef UNICODE
	if (0 == WideCharToMultiByte(CP_ACP, 0, url, -1, wurl, sizeof(wurl), NULL, NULL)) {
		fprintf(stderr, "url unicode conversion failed\n");
		goto cleanup;
	}
	if (0 == WideCharToMultiByte(CP_ACP, 0, lang, -1, wcookie + strlen(wcookie), sizeof(wcookie) - strlen(wcookie), NULL, NULL)) {
		fprintf(stderr, "cookie unicode conversion failed\n");
		goto cleanup;
	}
#endif

	handler.parser = XML_ParserCreate(NULL);
	if (!handler.parser) {
		fprintf(stderr, "eXpat parser init failed\n");
		goto cleanup;
	}

	parseData.parser = handler.parser;
	XML_SetElementHandler(handler.parser, SteamStartXmlElementHandler, SteamEndXmlElementHandler);
	XML_SetCharacterDataHandler(handler.parser, SteamCharXmlElementHandler);
	XML_SetUserData(handler.parser, &parseData);

	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "cUrl easy init failed\n");
		goto cleanup;
	}

	curl_easy_setopt(curl, CURLOPT_URL, wurl);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, iigCurlWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &handler);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_COOKIE, wcookie);

	res = curl_easy_perform(curl);

	if (steamInfo) {
		memcpy(steamInfo, &parseData.steaminfo, sizeof(*steamInfo));
	}

cleanup:
	if (curl) {
		curl_easy_cleanup(curl);
	}

	if (handler.parser) {
		XML_ParserFree(handler.parser);
	}

	return res;
}
