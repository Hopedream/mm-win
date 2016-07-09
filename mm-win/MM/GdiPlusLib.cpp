
#include "stdafx.h"
#include "..\include\gdiplusLib.h"

#pragma comment (lib,"GdiPlus")

//////////////////////////////////////////////////////////////////////////
// CGDIPlusInitializer

CGDIPlusInitializer::CGDIPlusInitializer()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CGDIPlusInitializer::~CGDIPlusInitializer()
{
	GdiplusShutdown(m_gdiplusToken);
}


//////////////////////////////////////////////////////////////////////////
// CBufferGraphics & helper

CBufferGraphics::CBufferGraphics(Gdiplus::Graphics* pGraphics) : m_pOriginalGraphics(pGraphics)
{
	m_pOriginalGraphics->GetVisibleClipBounds(&m_rcDest);

	m_pBitmap = new Bitmap(m_rcDest.GetRight(), m_rcDest.GetBottom());

	m_pBufferGraphics = Graphics::FromImage(m_pBitmap);
}

CBufferGraphics::~CBufferGraphics()
{
	m_pOriginalGraphics->DrawImage(m_pBitmap, m_rcDest,
		m_rcDest.GetLeft(), m_rcDest.GetTop(),
		m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), UnitPixel);

	delete m_pBufferGraphics;
	delete m_pBitmap;
}


bool Grayscale(Bitmap* pBitmap)
{
	// GDI+ still lies to us - the return format is BGR, NOT RGB. 

	Rect rcBitmap(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
	BitmapData bmData;

	Status status = pBitmap->LockBits(
		&rcBitmap, ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppARGB, &bmData);

	if(Ok != status)
	{
		return false;
	}

	INT stride = bmData.Stride; 
	byte * p = (byte *)bmData.Scan0;

	int nOffset = stride - pBitmap->GetWidth() * 4; 

	byte red, green, blue;

	for(UINT y = 0; y < pBitmap->GetHeight(); ++y)
	{
		for(UINT x = 0; x < pBitmap->GetWidth(); ++x )
		{
			blue  = p[0];
			green = p[1];
			red   = p[2];

			p[0] = p[1] = p[2] = (byte)(
				0.299 * red +
				0.587 * green +
				0.114 * blue);

			p += 4;
		}
		p += nOffset;
	}

	pBitmap->UnlockBits(&bmData);

	return true;
}

