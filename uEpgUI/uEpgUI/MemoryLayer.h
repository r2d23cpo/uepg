#pragma once
#include "stdafx.h"

class CMemoryLayer
{
public:
	CMemoryLayer();
	~CMemoryLayer();

	void Resize(CDC* pParentDC, int width, int height);
	void BitBlt(CPoint dst, CDC* pSrcDC, CRect src);
	CDC& GetMemDC() { return *_pDC; };
	virtual void Render() {};

	CRect GetDimensions() { return CRect(0, 0, _width, _height); }
	CRect GetRect()       { return CRect(0, 0, _width-1, _height-1); }
	int GetWidth()        { return _width; }
	int GetHeight()       { return _height; }

protected:
	void DeleteBitmap();
	void DeleteDC();

private:
	CDC* _pDC;
	CBitmap* _pBitmap;
	CBitmap* _pOldBitmap;
	int  _width;
	int  _height;
};
