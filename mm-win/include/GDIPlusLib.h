#pragma once


#include <GdiPlus.h>
#include <intsafe.h>


//////////////////////////////////////////////////////////////////////////
// CGDIPlusInitializer

class CGDIPlusInitializer
{
public:
	CGDIPlusInitializer();
	~CGDIPlusInitializer();

protected:
	ULONG_PTR m_gdiplusToken;
};


//////////////////////////////////////////////////////////////////////////
// CBufferGraphics & helper

class CBufferGraphics
{
public:
	CBufferGraphics(Gdiplus::Graphics* pGraphics);

	~CBufferGraphics();

public:
	Gdiplus::Graphics* GetGraphics()
	{
		return m_pBufferGraphics;
	}

public:
	Gdiplus::Bitmap* m_pBitmap;
	Gdiplus::Rect    m_rcDest;
	Gdiplus::Graphics* m_pOriginalGraphics;
	Gdiplus::Graphics* m_pBufferGraphics;
};


bool Grayscale(Gdiplus::Bitmap* pBitmap);