#pragma once
#include "mmwinimplbase.h"
class CFrameSysAbout :
	public MMWindowImplBase
{
public:
	static CFrameSysAbout* Instance();
	CFrameSysAbout();
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	~CFrameSysAbout(void);
private:
	static CFrameSysAbout* m_pInstance;
};

