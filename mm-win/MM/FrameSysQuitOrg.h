#pragma once
#include "mmwinimplbase.h"
class CFrameSysQuitOrg :
	public MMWindowImplBase
{
public:
	static CFrameSysQuitOrg* Instance();
	CFrameSysQuitOrg();
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	~CFrameSysQuitOrg(void);
private:
	void quitOrg();
	void getOrgInfo();
private:
	static CFrameSysQuitOrg* m_pInstance;
	tstring m_strOid;
	tstring m_strPhone;
};

