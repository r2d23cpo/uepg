#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "moduleAPI.h"

static const char* moduleName = "uEPG";
static const char* moduleAuthor = "mrblack1134";
static const char* moduleEmail = "jonathan.fillion@gmail.com";

static HWND hDDMainWnd = 0;

static const char* outputFilename = "c:\\temp\\epg.txt";
static const char* outputBinname  = "c:\\temp\\epg.bin";

static FILE* outputFile = 0;
static FILE* outputBin = 0;

static void UpdateChannelInfo()
{
	TCHL* pChlData = 0;
	SendMessage(hDDMainWnd, WM_MODULE_MSG, DDMODAPI_CHL_GET_POINTER, (LPARAM)&pChlData);

	if (pChlData)
	{
		fwrite(pChlData, sizeof(TCHL), 1, outputBin);
	}
}

int __stdcall EpgCallback(TEPGInfo* pEpgInfo)
{
	pEpgInfo->EventName[256]=0;
	pEpgInfo->ShortDescr[1024]=0;
	pEpgInfo->ExtendedDescr[1024]=0;

	fprintf(outputFile, 
		"wSat: %d\n"
		"wNetId: %d\n"
		"wSId: %d\n"
		"wEventId: %d\n"
		"dwStartTime: %d\n"
		"dwDuration: %d\n"
		"EventName: %s\n"
		"ShortDescr: %s\n"
		"ExtendedDescr: %s\n"
		"\n",
		pEpgInfo->wSat,
		pEpgInfo->wNetID,
		pEpgInfo->wSID,
		pEpgInfo->wEventID,
		pEpgInfo->dwStartTime,
		pEpgInfo->dwDuration,
		pEpgInfo->EventName,
		pEpgInfo->ShortDescr,
		pEpgInfo->ExtendedDescr);

	fwrite(pEpgInfo, sizeof(TEPGInfo), 1, outputBin);
	return 0;
}

int __stdcall OnGetModuleInfo (TModuleInfo *pModuleInfo)
{
	pModuleInfo->dwModuleAPI = MODULE_API_CURRENT;

	strcpy_s(pModuleInfo->szModuleName, sizeof(pModuleInfo->szModuleName), moduleName);
	strcpy_s(pModuleInfo->szModuleEmail, sizeof(pModuleInfo->szModuleEmail), moduleEmail);
	strcpy_s(pModuleInfo->szModuleAuthor, sizeof(pModuleInfo->szModuleAuthor), moduleAuthor);

	pModuleInfo->dwModuleVer = 0x0001;
	pModuleInfo->dwModuleType = MODTYPE_EPG;
	//pModuleInfo->wShorCut = VK_F10;
	pModuleInfo->wShorCut = 0;
	pModuleInfo->dwModuleType = MODTYPE_EPG;
	pModuleInfo->dwRequiredDDver = 0x01040000;
	return 0;
}

int __stdcall OnInit (TInitInfo *pInitInfo)
{
	hDDMainWnd = (HWND)pInitInfo->hMainWnd;

	fopen_s(&outputFile, outputFilename, "wb");
	fopen_s(&outputBin,  outputBinname,  "wb");

	UpdateChannelInfo();
	return 0;
}

int __stdcall OnClose()
{
	fprintf(outputFile, "OnClose()\n");
	fclose(outputFile);
	fclose(outputBin);
	return 0;
}

int __stdcall OnShow()
{
	fprintf(outputFile, "OnShow()\n");
	return 0;
}

int __stdcall BeforeChannelChange(TChannelInfo *pChannelInfo)
{ 
	fprintf(outputFile, "BeforeChannelChange()\n");
	return 0;
}

int __stdcall AfterChannelChange(TChannelInfo *pChannelInfo)
{
	fprintf(outputFile, "AfterChannelChange()\n");
	SendMessage(hDDMainWnd, WM_MODULE_MSG, DDMODAPI_START_EPG, (LPARAM)&EpgCallback);
	return 0;
}

int __stdcall OnDDEvent(DWORD EventType, void *EventParams)
{
	switch (EventType)
	{
	case DDEVENT_ON_CH_LIST_UPDATE:
		UpdateChannelInfo();
		break;
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

