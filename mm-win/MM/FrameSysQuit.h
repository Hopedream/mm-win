#pragma once
#include "mmwinimplbase.h"
class CFrameSysQuit :
	public MMWindowImplBase
{
public:
	static CFrameSysQuit* Instance();
	CFrameSysQuit();
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	~CFrameSysQuit(void);
private:
	static CFrameSysQuit* m_pInstance;
};

