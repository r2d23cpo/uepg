#pragma once

#include "ScrollHelper.h"
#include "ScrollableWnd.h"
#include "EntryLayer.h"
#include "MemoryLayer.h"
#include "GridLayer.h"
#include <string>
#include <list>

class CGridControl;
class CGridControlListener
{
public:
	virtual void OnGridScroll(CGridControl* src, int dx, int dy) = 0;
};

class CGridControl: public CScrollableWnd, public CMemoryLayer
{
	DECLARE_DYNAMIC(CGridControl)

public:
	CGridControl(bool hasScrollbars = true);
	virtual ~CGridControl();

	virtual void Scroll(int dx, int dy);
	virtual void Render();

	void SetCount(int cols, int rows) 
	{ 
		_gridLayer._col.count = cols;
		_gridLayer._row.count = rows;
	}

	void SetRowSize(int size, int spacing, bool half = false)
	{
		_gridLayer._row.size  = size;
		_gridLayer._row.spacing = spacing;
		_gridLayer._row.half = half;
	}

	void SetColSize(int size, int spacing, bool half = false)
	{
		_gridLayer._col.size  = size;
		_gridLayer._col.spacing = spacing;
		_gridLayer._col.half = half;
	}

	void SetColor(COLORREF foreColor, COLORREF backColor)
	{
		_gridLayer._foreColor = foreColor;
		_gridLayer._backColor = backColor;
	}

	void SetListener(CGridControlListener* pListener) { _pListener = pListener; };
	CGridLayer& GetGrid()            { return _gridLayer; }
	CRect  GetCellRect(int c, int r) { return _gridLayer.GetCellRect(c, r); }
	CPoint GetScrollPos()            { return _scrollPos; }
	CRect  GetVisibleRect();

protected:
	DECLARE_MESSAGE_MAP()
	CScrollHelper _scrollHelper;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

protected:
	CPoint _scrollPos;
	CGridLayer _gridLayer;

private:
	BOOL RegisterWindowClass();
	bool _hasScrollbars;
	CGridControlListener* _pListener;
};


