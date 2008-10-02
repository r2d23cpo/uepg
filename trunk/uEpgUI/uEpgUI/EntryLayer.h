#pragma once
#include "stdafx.h"
#include "MemoryLayer.h"
#include <string>

// EntryLayer

class CEntryLayer: public CMemoryLayer
{
public:
	CEntryLayer();
	~CEntryLayer();

	enum Direction
	{
		DirectionNone=0,
		DirectionLeft=1,
		DirectionRight=2,
		DirectionTop=4,
		DirectionBottom=8
	};

	void RenderRect(const CRect& rect, enum Direction underflow);
	void RenderVisible(const CRect& parentRect, CPoint* parentPosition=0);

	void SetPosition(CDC* pParentDC, CRect r);
	void SetPosition(CPoint p)     { _position = p; }
	const CPoint& GetPosition()    { return _position; }
	const CRect&  GetVisibleRect() { return _visibleRect; }

	void setTitle(std::string title)   { _title = title; }
	void setDescription(std::string d) { _description = d; }

protected:
	void DrawArrow(int dstX, int dstY, int w, enum Direction d);
	CRect ComputeVisibleRect(const CRect& parentRect, CPoint* parentPosition=0, enum Direction* pUnderflow=0);

	CPoint _position;
	CRect  _visibleRect;
	CFont  _fontSmall;
	CFont  _fontBig;

	int    _minHeight;
	int    _minWidth;

	std::string _title;
	std::string _description;
};


