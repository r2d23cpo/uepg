#pragma once
#include "afxwin.h"

class CScrollableWnd: public CWnd
{
public:
	virtual void Scroll(int dx, int dy) = 0;
	CScrollableWnd(void);
	~CScrollableWnd(void);
};
