#pragma once

#include "stdafx.h"
#include "MemoryLayer.h"

class CGridLayer: public CMemoryLayer
{
public:
	virtual void Render();

	CRect GetCellRect(int c, int r);
	CSize GetGridSize();

	struct
	{
		int  count;
		int  size;
		int  spacing;
		bool half;
	} _row, _col;

	COLORREF _foreColor;
	COLORREF _backColor;

protected:
	CRect GetGridRect();
};


