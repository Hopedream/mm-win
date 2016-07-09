#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

using namespace DuiLib;

class CTileLayoutDestEmployeeList :  public CTileLayoutUI
{
public:
	CTileLayoutDestEmployeeList(CPaintManagerUI* ppm=NULL);
	virtual ~CTileLayoutDestEmployeeList();

	//void AddOneItem(tstring& strAccount, tstring& strAvatar, tstring& strShowName, bool bDefault=false);
	void AddOneItem(sFriendListItem& oneFriend,vector<tstring>&vecProperty, bool bDefault=false);
	void DoEvent(TEventUI& event) ;
	void DeSelectItem(tstring& strAccount);
	void SetItemSeleted(tstring& strAccout);
	bool getRecipient();        //获取默认接收人
	void GetDestParticipant(vector<tstring>& vecParticipant);
protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;
};