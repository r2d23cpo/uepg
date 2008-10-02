// uEpgUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CuEpgUIApp:
// See uEpgUI.cpp for the implementation of this class
//

class CuEpgUIApp : public CWinApp
{
public:
	CuEpgUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

};

extern CuEpgUIApp theApp;