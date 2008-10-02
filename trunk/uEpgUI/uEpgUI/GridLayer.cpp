#include "stdafx.h"
#include "SaveDC.h"
#include "GridLayer.h"

void CGridLayer::Render()
{
	CRect dimensions = GetDimensions();	

	// fill background
	GetMemDC().FillSolidRect(&dimensions, _backColor);

	// solid pen
	CPen solid(PS_SOLID, 1, _foreColor);

	// dashed pen
	DWORD dotStyle[] = {1, 2};
	LOGBRUSH lb;
	lb.lbColor = _foreColor;
	lb.lbStyle = BS_SOLID;
	lb.lbHatch = 0;

	CPen dashed;
	dashed.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1, &lb, 2, dotStyle);

	// save DC
	CSaveDC save(GetMemDC());

	// draw vertical lines
	GetMemDC().SelectObject(&solid);

	int x = 0;
	for (int c=0; c<_col.count; c++)
	{
		GetMemDC().MoveTo(x, 0);
		GetMemDC().LineTo(x, GetHeight());

		x += _col.size + 1;

 		GetMemDC().MoveTo(x, 0);
		GetMemDC().LineTo(x, GetHeight());

		x += _col.spacing;
	}

	// draw horizontal lines
	int y = 0;
	for (int c=0; c<_row.count; c++)
	{
		GetMemDC().MoveTo(0, y);
		GetMemDC().LineTo(GetWidth(), y);

		y += _row.size + 1;

		GetMemDC().MoveTo(0, y);
		GetMemDC().LineTo(GetWidth(), y);

		y += _row.spacing;
	}

	// and half horizontal lines
	if (_row.half)
	{
		GetMemDC().SelectObject(&dashed);
		y = _row.size/2;
		for (int c=0; c<_row.count; c++)
		{
			GetMemDC().MoveTo(0, y);
			GetMemDC().LineTo(GetWidth(), y);

			y += _row.size + 1 + _row.spacing;
		}
	}

	CRect rect = GetRect();
	//GetMemDC().DrawEdge(&rect, BDR_SUNKENINNER|BDR_SUNKENOUTER, BF_RECT);
}


CRect CGridLayer::GetGridRect()
{
	CRect lastRect = GetCellRect(_col.count-1, _row.count-1);
	return CRect(CPoint(0,0), lastRect.BottomRight() + CPoint(1,1));
}

CSize CGridLayer::GetGridSize()
{
	CRect rect = GetGridRect();
	return CSize(rect.right+1, rect.bottom+1);
}

CRect CGridLayer::GetCellRect(int col, int row)
{
	CRect rect;

	rect.left   = (1 + _col.size + _col.spacing) * col + 1;
	rect.top    = (1 + _row.size + _row.spacing) * row + 1;
	rect.right  = rect.left + _col.size - 1;
	rect.bottom = rect.top  + _row.size - 1; 

	return rect;
}

