#pragma once
#include "StdAfx.h"
#include "..\Common\MMDefine.h"

	
class CListSysMsg :public CListUI
{
public:
	CListSysMsg(CPaintManagerUI *paint_manager);
	~CListSysMsg();
	bool Add(CControlUI *pControl);
	bool AddAt(CControlUI *pControl, int iIndex);
	bool Remove(CControlUI *pControl);
	bool RemoveAt(int iIndex);
	void RemoveAll();
	void DoEvent(TEventUI& event);

	void AddItem(tstring& strTips, int uTime, tstring& strSender, ESysMsgSubType eSubType, int indx = -1);
	void AddOrgItem(tstring& strTips, int uTime, tstring& strSender, ESysMsgSubType eSubType, int indx = -1);
	//É¾³ýÕËºÅÎªstrAccountµÄList item
	void RemoveItem(int nItemID);

	void RemoveItem(sSysMsg* pSysMsg);

	void DisableButton(sSysMsg* pSysMsg);

	void CacheSysMsg(sSysMsg& sysMsg);

private:
	CPaintManagerUI* m_pPaintManager;
	CDialogBuilder m_DlgBuilder;
	vector<sSysMsg> m_vecSysMsgs;
};
