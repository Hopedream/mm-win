#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"


using namespace DuiLib;

class CTileLayoutDestEmployeeSelectedList :  public CTileLayoutUI
{
public:
	CTileLayoutDestEmployeeSelectedList(CPaintManagerUI* ppm=NULL);
	virtual ~CTileLayoutDestEmployeeSelectedList();

	//����ط���Ҫ����û������ݽṹ��Ϣ��Ϊ������Ϣ��avatar,showname...
	void AddOneItem(sFriendListItem* pOneFriend, vector<tstring>& vecProperty);

	void RemoveOneItem(tstring& strAccount);

	void DoEvent(TEventUI& event) ;

	bool CheckIfExist(tstring& strAccount);

	void GetSelectedOrgIDs(vector<tstring>& vecOrgIDs);

	void GetDestParticipant(vector<tstring>& vecParticipant);

	void GetOrgInfoByAccount(const tstring& strAccount, tstring&strOrgID, tstring& strOrgName);
protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;

	map<tstring, sFriendListItem> m_mapAccount2Friend;

};