// GridControl.cpp : implementation file
//

#include "stdafx.h"
#include "uEpgUI.h"
#include "GridControl.h"
#include "GridLayer.h"
#include "SaveDC.h"

// CGridControl

IMPLEMENT_DYNAMIC(CGridControl, CWnd)

BEGIN_MESSAGE_MAP(CGridControl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

static const wchar_t* GridControl_CLASSNAME = L"GridControl";

CGridControl::CGridControl(bool hasScrollbars):
	_pListener(0),
	_scrollHelper(hasScrollbars)
{
	_scrollPos = _scrollHelper.GetScrollPos();
	RegisterWindowClass();

	SetCount(1,1);
	SetRowSize(100, 100);
	SetColSize(100, 100);
	SetColor(0, ~0);
}

CGridControl::~CGridControl()
{
}

BOOL CGridControl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, GridControl_CLASSNAME, &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = GridControl_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

CRect CGridControl::GetVisibleRect()
{
	CPoint scrollOffset = GetScrollPos();
	CRect  visibleRect = GetRect();
	visibleRect.OffsetRect(scrollOffset);

	return visibleRect;
}

void CGridControl::Render()
{
	// copy the grid
	CPoint dstPoint(0,0);
	CRect  srcRect = GetVisibleRect();

	BitBlt(dstPoint, &_gridLayer.GetMemDC(), srcRect);
}

// CGridControl message handlers

void CGridControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonDown(nFlags, point);
}

void CGridControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	_scrollHelper.OnSize(nType, cx, cy);

	CSize gridSize = _gridLayer.GetGridSize();
	_gridLayer.Resize(GetDC(), gridSize.cx, gridSize.cy);
	_gridLayer.Render();

	Resize(GetDC(), cx, cy);
	Render();
}

int CGridControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSize gridSize = _gridLayer.GetGridSize();

	_scrollHelper.AttachWnd(this);
	_scrollHelper.SetDisplaySize(gridSize.cx, gridSize.cy);

	return 0;
}

void CGridControl::Scroll(int dx, int dy)
{
	_scrollPos = _scrollHelper.GetScrollPos();

	Render();
	Invalidate(FALSE);

	if (_pListener)
		_pListener->OnGridScroll(this, dx, dy);
}

void CGridControl::OnPaint()
{
	CPaintDC paintDC(this);
	CRect    r(paintDC.m_ps.rcPaint);

	paintDC.BitBlt(r.left, r.top, r.Width(), r.Height(), 
		&GetMemDC(), r.left, r.top, SRCCOPY);
}

void CGridControl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	_scrollHelper.OnVScroll(nSBCode, nPos, pScrollBar);
}

void CGridControl::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	static int x = 0;
	_scrollHelper.OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CGridControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return _scrollHelper.OnMouseWheel(nFlags, zDelta, pt);
}
