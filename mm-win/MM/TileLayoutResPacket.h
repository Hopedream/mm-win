#pragma once

#include "StdAfx.h"
#include "UIDefine.h"

using namespace DuiLib;

class fleInfo;
class CTileLayoutResPacket :  public CTileLayoutUI
{
public:
	CTileLayoutResPacket(CPaintManagerUI* ppm=NULL);
	virtual ~CTileLayoutResPacket();

	void AddOneItem(fileInfo* pOneFile,bool isVisible = true);

	void SetProgress(tstring& strFile, int nValue);

	void DoEvent(TEventUI& event) ;

protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;
};