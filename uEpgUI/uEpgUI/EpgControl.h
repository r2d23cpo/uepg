#pragma once

#include "GridControl.h"
#include "GridLayer.h"
#include "EntryLayer.h"
#include <list>

class CEpgTimeControl: public CGridControl
{
public:
	CEpgTimeControl();

	void Render();
	void RenderTimebar();
	void Scroll(int dx, int dy);

	void SetTimeStep(CTime start, CTimeSpan step);

private:

	CTime     _timeStart;
	CTimeSpan _timeStep;
	CTime     _timeEnd;
	CFont _font;
};

class CEpgTitleControl: public CGridControl
{
public:
	CEpgTitleControl();

	void Render();
	void Scroll(int dx, int dy);
};

class CEpgGridControl: public CGridControl
{
public:
	void Render();

	typedef std::list<CEntryLayer* > EntryList;
	EntryList _entryList;
};

class CEpgControl: public CGridControlListener
{
public:
	void Create(CWnd* pParent, CRect& rect);
	void OnGridScroll(CGridControl* src, int dx, int dy) ;

	void SetTimeRange(CTime start, CTime end);
	void SetTimeStep(CTime start, CTimeSpan step);

	void SetCountInfo(int cols, int rows);
	void SetSizeInfo(int colSize, int colSpacing, int rowSize, int rowSpacing);
	void SetColorInfo(COLORREF foreGrid, COLORREF backGrid, COLORREF backTime, COLORREF backTitle);

private:
	CEpgGridControl _gridControl;
	CEpgTimeControl _timeControl;
	CEpgTitleControl _titleControl;

	int _rows, _cols;
};