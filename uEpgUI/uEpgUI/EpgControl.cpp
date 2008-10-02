#include "stdafx.h"
#include "EpgControl.h"
#include "SaveDC.h"

const int EPG_TIME_WIDTH = 40;
const int EPG_TITLE_HEIGHT = 35;

CEpgTitleControl::CEpgTitleControl():
	CGridControl(false)
{
}

void CEpgTitleControl::Scroll(int dx, int dy)
{
	_scrollPos -= CPoint(dx, dy);
	
	Render();
	Invalidate(FALSE);
}

void CEpgTitleControl::Render()
{
	CGridControl::Render();
}

CEpgTimeControl::CEpgTimeControl():
	CGridControl(false)
{
	_font.CreatePointFont(80, L"Verdana");
}

void CEpgTimeControl::SetTimeStep(CTime start, CTimeSpan step)
{
	_timeStart = start;
	_timeStep  = step;
	_timeEnd   = _timeStart;

	for (int x=0; x<_gridLayer._row.count; x++)
		_timeEnd += _timeStep;
}

void CEpgTimeControl::Scroll(int dx, int dy)
{
	_scrollPos -= CPoint(dx, dy);
	
	Render();
	Invalidate(FALSE);
}

void CEpgTimeControl::RenderTimebar()
{
	CTime now = CTime::GetCurrentTime();
	CTimeSpan t1 = now - _timeStart;
	CTimeSpan t2 = _timeEnd - _timeStart;
	int gridHeight = GetGrid().GetHeight();
	float ratio = float(t1.GetTotalSeconds())/t2.GetTotalSeconds();
	int y = static_cast<int>(ratio*gridHeight) - GetScrollPos().y;
	const int barHeight = 20;
	
	Graphics graphics(GetMemDC());
	LinearGradientBrush linGrBrush(Point(0, y-barHeight),
								   Point(0, y),
								   Color(0, 255, 0, 0),   // opaque red
								   Color(255, 200, 200, 255));  // opaque blue

	graphics.FillRectangle(&linGrBrush, 1, y-barHeight, EPG_TIME_WIDTH-2, barHeight);
}

void CEpgTimeControl::Render()
{
	CGridControl::Render();
	RenderTimebar();

	GetMemDC().SelectObject(&_font);
	GetMemDC().SetBkMode(TRANSPARENT);

	CTime t = _timeStart;
	for (int r=0; r<_gridLayer._row.count; r++)
	{
		CRect cellRect = GetCellRect(0, r);
		cellRect.OffsetRect(-GetScrollPos());

		CString str = t.Format(L"%H:%M");
		GetMemDC().DrawText(str, &cellRect, DT_RIGHT);

		t += _timeStep;
	}
}

void CEpgGridControl::Render()
{
	CGridControl::Render();

	CRect  srcRect = GetVisibleRect();

	EntryList::iterator i;
	for (i=_entryList.begin(); i!= _entryList.end(); i++)
	{
		CEntryLayer* entry = *i;
		CPoint dstPoint;

		entry->RenderVisible(srcRect, &dstPoint);

		dstPoint -= srcRect.TopLeft();
		const CRect& visibleRect = entry->GetVisibleRect();

		BitBlt(dstPoint, &entry->GetMemDC(), visibleRect);
	}
}

void CEpgControl::OnGridScroll(CGridControl* src, int dx, int dy)
{
	if (src == &_gridControl)
	{
		if (dy)
		{
			_timeControl.Scroll(0, dy);
			_timeControl.Invalidate();
		}

		if (dx)
		{
			_titleControl.Scroll(dx, 0);
			_titleControl.Invalidate();
		}
	}
}

void CEpgControl::SetCountInfo(int cols, int rows)
{
	_gridControl.SetCount(cols, rows);
	_timeControl.SetCount(1, rows);
	_titleControl.SetCount(cols, 1);

	_cols = cols;
	_rows = rows;
}

void CEpgControl::SetSizeInfo(int colSize, int colSpacing,
							  int rowSize, int rowSpacing)
{
	_gridControl.SetColSize(colSize, colSpacing);
	_gridControl.SetRowSize(rowSize, rowSpacing, true);

	_timeControl.SetColSize(EPG_TIME_WIDTH-2, colSpacing);
	_timeControl.SetRowSize(rowSize, rowSpacing);

	_titleControl.SetColSize(colSize, colSpacing, false);
	_titleControl.SetRowSize(EPG_TITLE_HEIGHT-2, rowSpacing);
}

void CEpgControl::SetColorInfo(COLORREF foreGrid,
							   COLORREF backGrid,
							   COLORREF backTime,
							   COLORREF backTitle)
{
	_gridControl.SetColor(foreGrid, backGrid);
	_timeControl.SetColor(foreGrid, backTime);
	_titleControl.SetColor(foreGrid, backTitle);
}

void CEpgControl::Create(CWnd* pParent, CRect& rect)
{
	CGridLayer& grid = _gridControl.GetGrid();

	SetCountInfo(6, 24);
	SetSizeInfo(130, 2, 60, 0);
	SetColorInfo(0x888888, 0xFFFFFF, 0xFFEEEE, 0xFFEEEE);

	CTime now = CTime::GetCurrentTime();
	CTime start(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
	CTimeSpan step(0, 1, 0, 0);
	_timeControl.SetTimeStep(start, step);

	// main grid rect
	CRect gridRect = rect;
	gridRect.TopLeft() += CPoint(EPG_TIME_WIDTH, EPG_TITLE_HEIGHT);
	_gridControl.Create(0, L"", WS_VISIBLE|WS_CLIPCHILDREN, gridRect, pParent, 0);

	// time grid
	CRect timeRect(
		CPoint(rect.left, rect.top + EPG_TITLE_HEIGHT),
		CSize(EPG_TIME_WIDTH, gridRect.Height() - GetSystemMetrics(SM_CYVSCROLL)));

	_timeControl.Create(0, L"", WS_VISIBLE, timeRect, pParent, 0);

	// title grid
	CRect titleRect(
		CPoint(rect.left + EPG_TIME_WIDTH, rect.top),
		CSize(gridRect.Width() - GetSystemMetrics(SM_CXHSCROLL), EPG_TITLE_HEIGHT));

	_titleControl.Create(0, L"", WS_VISIBLE, titleRect, pParent, 0);

	// listen to the main grid
	_gridControl.SetListener(this);

#if 1
	for (int x=0; x<_cols; x++)
	{
		for (int y=0; y<_rows; y++)
		{
			// add one entry
			if (!((x+y)&1))
				continue;
			CEntryLayer* entry = new CEntryLayer();
			CRect entryRect= _gridControl.GetCellRect(x, y);

			entry->SetPosition(_gridControl.GetDC(), entryRect);
			entry->setTitle("The Simpsons");
			entry->setDescription("It is up to Homer to save Christmas, when bonuses are cut at the nuclear power plant and Marge is forced to use the Christmas savings on an emergency.");
			_gridControl._entryList.push_back(entry);
		}
	}
#endif

	_gridControl.Render();
}
