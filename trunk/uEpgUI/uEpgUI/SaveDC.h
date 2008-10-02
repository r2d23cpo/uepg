#pragma once
#include "stdafx.h"

class CSaveDC
{
public:
	CSaveDC(CDC& dc): _dc(dc) { _s = _dc.SaveDC(); };
	~CSaveDC() { _dc.RestoreDC(_s); };

private:
	int  _s;
	CDC& _dc;
};

