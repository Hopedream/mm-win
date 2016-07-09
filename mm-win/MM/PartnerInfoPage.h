#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include <string>
#include <map>

using namespace DuiLib;
using namespace std;

class CProjectListView;
class CPartnerInfoPage : public CHorizontalLayoutUI, public INotifyUI
{
public:
	CPartnerInfoPage(CPaintManagerUI* ppm = NULL);
	virtual ~CPartnerInfoPage();

	bool Initialize(tstring strAccount, sUserVCard& pUserInfo, tstring strOrg, tstring strOid);
	bool updateView(sUserVCard& pUserInfo);
	void InitOrgInfoPage();
	void UpdataMemo(tstring strNewMemo);
	virtual void Notify(TNotifyUI& msg) ;

	


	void ShowGroupProjectList();

    void NetCall();
	void PhoneCall(); 
	void SendMsg();
	void BusinessDetail();
	void addOrgFriend();
	void delOrgFriend(int iOrgid);
	bool isOrgBuddy(tstring strOrgid, tstring strOrgidDest);
private:
	void initOrgInfo(tstring& strOrgId);
public:
	std::tstring m_strCurAccount;
	std::tstring m_strCurOrgID;
	std::tstring m_strMyOrgID;
protected:
	DuiLib::CPaintManagerUI* m_PaintManager;
	CLabelUI* m_pLabelBeizhu;
	map<tstring, CProjectListView*> m_mapId2Projview;
};