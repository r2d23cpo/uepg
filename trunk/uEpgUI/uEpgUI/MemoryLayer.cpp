#include "stdafx.h"
#include "MemoryLayer.h"

CMemoryLayer::CMemoryLayer():
	_pDC(0),
	_pBitmap(0),
	_pOldBitmap(0),
	_width(0),
	_height(0)
{
}

CMemoryLayer::~CMemoryLayer()
{
	DeleteBitmap();
	DeleteDC();
}

void CMemoryLayer::DeleteDC()
{
	if (_pDC)
	{
		delete _pDC;
		_pDC = 0;
	}
}

void CMemoryLayer::DeleteBitmap()
{
	if (_pBitmap)
	{
		_pDC->SelectObject(_pOldBitmap);
		_pBitmap->DeleteObject();
		delete _pBitmap;
		_pBitmap = 0;
	}
}

void CMemoryLayer::Resize(CDC* pParentDC, int width, int height)
{
	if (_width != width || _height != height)
	{
		if (_pDC)
		{
			DeleteBitmap();
		}
		else
		{
			_pDC = new CDC();
			_pDC->CreateCompatibleDC(pParentDC);
		}

		_pBitmap = new CBitmap();
		_pBitmap->CreateCompatibleBitmap(pParentDC, width, height);
		_pOldBitmap = _pDC->SelectObject(_pBitmap);

		_width = width;
		_height = height;
	}
}

void CMemoryLayer::BitBlt(CPoint dst, CDC* pSrcDC, CRect src)
{
	if (_pDC)
	{
		GetMemDC().BitBlt(
			dst.x, dst.y, src.Width()+1, src.Height()+1,
			pSrcDC, src.left, src.top, SRCCOPY);
	}
}

