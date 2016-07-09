#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

using namespace DuiLib;

class CTileLayoutGroupMember :  public CTileLayoutUI
{
public:
	CTileLayoutGroupMember(CPaintManagerUI* ppm=NULL);
	virtual ~CTileLayoutGroupMember();

	//void AddOneItem(tstring& strAccount, tstring& strAvatar, tstring& strShowName, bool bDefault=false);
	void AddOneItem(sFriendListItem& oneFriend,vector<tstring>&vecProperty, bool bDefault=false);
	void DoEvent(TEventUI& event) ;
	void DeSelectItem(tstring& strAccount);
	void SetItemSeleted(tstring& strAccout);


protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;
};