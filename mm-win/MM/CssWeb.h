#pragma once
#include "mmwinimplbase.h"

#define WEB_MOTOR_TYPE 0
#define WEB_VIN 1

class CWkeWebkitUI;
class CssWeb :
	public MMWindowImplBase
{
public:
	static CssWeb* Instance();
	CssWeb();
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void SetParams(int webtype, const tstring& url);
	~CssWeb(void);
private:
	static CssWeb* m_pInstance;
	CWkeWebkitUI* web_kit_ui_;
};

