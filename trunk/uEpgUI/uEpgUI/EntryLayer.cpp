// EntryLayer.cpp : implementation file
//

#include "stdafx.h"
#include "EntryLayer.h"
#include "SaveDC.h"

CEntryLayer::CEntryLayer()
{
	_fontSmall.CreatePointFont(60, L"Verdana");
	_fontBig.CreatePointFont(70, L"Verdana");
	_minHeight = 20;
	_minWidth  = 60;
}

CEntryLayer::~CEntryLayer()
{
}

void CEntryLayer::SetPosition(CDC* pParentDC, CRect r)
{
	SetPosition(r.TopLeft());
	Resize(pParentDC, r.Width()+1, r.Height()+1);
}

void CEntryLayer::RenderRect(const CRect& rect, enum Direction underflow)
{
	CPen   pen(PS_SOLID, 1, 0xFF0000);
	CPen   lightPen(PS_SOLID, 1, 0xFFAAAA);
	CBrush brush(0xFFCCCC);

	CRect rectDims = rect;
	rectDims.right++;
	rectDims.bottom++;

	CSaveDC save(GetMemDC());
	
	GetMemDC().FillSolidRect(&rectDims, 0xFFFFFF);
	GetMemDC().SelectObject(&pen);
	GetMemDC().SelectObject(&brush);
	
	CPoint round(9,9);
	//GetMemDC().RoundRect(&rectDims, round);
	GetMemDC().FillRect(&rectDims, &brush);


	// starting point
	CPoint orig(rect.left, rect.top);

	// draw the title
	GetMemDC().SelectObject(&_fontBig);
	CString title(_title.c_str());
	CSize   titleExtent = GetMemDC().GetTextExtent(title);

	CRect titleRect(orig, CSize(rect.Width(), titleExtent.cy));
	orig.y += titleExtent.cy;

	GetMemDC().SetBkMode(TRANSPARENT);
	GetMemDC().DrawText(title, &titleRect, DT_CENTER);	

	// draw a fancy bar
	orig.y += 2;
	GetMemDC().SelectObject(&lightPen);
	GetMemDC().MoveTo(orig + CPoint(2,0));
	GetMemDC().LineTo(orig + CPoint(rect.Width()-2, 0));

	// draw the description
	orig.y += 2;
	CRect descRect(orig.x+2, orig.y, rect.right-2, rect.bottom-2);
	GetMemDC().SelectObject(&_fontSmall);
	CString desc(_description.c_str());
	GetMemDC().DrawText(desc, &descRect, DT_LEFT|DT_WORDBREAK|DT_PATH_ELLIPSIS);	


	if (underflow & DirectionTop)
		DrawArrow(rect.right-10, rect.top+5, 2, DirectionTop);

	if (underflow & DirectionBottom)
		DrawArrow(rect.right-10, rect.bottom-5, 2, DirectionBottom);

	if (underflow & DirectionLeft)
		DrawArrow(rect.left+5, (rect.top+rect.bottom)/2, 3, DirectionLeft);

	if (underflow & DirectionRight)
		DrawArrow(rect.right-5, (rect.top+rect.bottom)/2, 3, DirectionRight);
}

void CEntryLayer::RenderVisible(const CRect& parentRect,
							   CPoint* parentPosition)
{
	enum Direction underflow;
	CRect r = ComputeVisibleRect(parentRect, parentPosition, &underflow);

	if (r != _visibleRect)
	{
		RenderRect(r, underflow);
		_visibleRect = r;
	}
}

void CEntryLayer::DrawArrow(int dstX, int dstY, int w, enum Direction d)
{
	bool v = (d == DirectionTop) || (d == DirectionBottom);
	bool p = (d == DirectionRight) || (d == DirectionBottom);

	for (int y=0; w>=0; w--, y++)
		for (int x=-w; x<=w; x++)
		{
			int dx = (v)?(x):(y);
			if (!p) dx = -dx;

			int dy = (v)?(y):(x);
			if (!p) dy = -dy;

			GetMemDC().SetPixel(dstX+dx, dstY+dy, 0xFF0000);
		}
}

CRect CEntryLayer::ComputeVisibleRect(const CRect& parentRect,
									 CPoint* parentPosition,
									 enum Direction* pUnderflow)
{
	CRect r = GetRect();
	CPoint p = GetPosition();
	CRect v = r;

	v.OffsetRect(p);
	v.IntersectRect(&v, &parentRect);
	int underflow = DirectionNone;

	if (v.Height() < _minHeight)
	{
		if (v.top != p.y)
			v.top = v.bottom - _minHeight;
		else
			v.bottom = v.top + _minHeight;
	}

	if (v.Width() < _minWidth)
	{
		if (v.left != p.x)
			v.left = v.right - _minWidth;
		else
			v.right = v.left + _minWidth;
	}

	if (v.Height() < r.Height())
	{
		if (v.top != p.y)
			underflow |= DirectionTop;
		else
			underflow |= DirectionBottom;
	}

	if (v.Width() < r.Width())
	{
		if (v.left != p.x)
			underflow |= DirectionLeft;
		else
			underflow |= DirectionRight;
	}

	if (parentPosition)
	{
		parentPosition->x = v.left;
		parentPosition->y = v.top;
	}

	if (pUnderflow) 
		*pUnderflow = static_cast<enum Direction>(underflow);

	v.OffsetRect(-p);

	return v;
}