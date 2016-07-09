#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include "UIDefine.h"
using namespace DuiLib;

class CTileLayoutGroupPackList :  public CTileLayoutUI
{
public:
	CTileLayoutGroupPackList(CPaintManagerUI* ppm=NULL);
	virtual ~CTileLayoutGroupPackList();

	//void AddOneItem(tstring& strAccount, tstring& strAvatar, tstring& strShowName, bool bDefault=false);
	void AddOneItem(sPackFileItem& onePackFile);
	void SetProgress(tstring& strFile, int nValue);
	void DoEvent(TEventUI& event) ;
	void DeSelectItem(tstring& strAccount);
	void SetItemSeleted(tstring& strAccout);


protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;
};