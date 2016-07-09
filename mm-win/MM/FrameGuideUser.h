#pragma once
#include "mmwinimplbase.h"
#include "../include/UsrManagerEvent_i.h"
#include "UserManager.h"


class CFrameGuideUser :
	public MMWindowImplBase ,  public CUserManagerEventBase
{
public:
	CFrameGuideUser();
	~CFrameGuideUser(void);
	static CFrameGuideUser* Instance();

public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc);

	bool getContinue();
public:
	static CFrameGuideUser* m_pInstance;

private:
	void SavePersonInfo();
	void updateinfo();

private:
	CEditUI   *m_pEdtName;                         //Ãû³Æ
	CComboUI *m_pCombVocation;                     //ÐÐÒµ
	sUserVCard m_myUserInfo;

	bool m_bContinue;
};

