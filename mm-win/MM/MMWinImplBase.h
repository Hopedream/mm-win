#pragma once
#include "StdAfx.h"
#include "resource.h"

class MMWindowImplBase : public WindowImplBase
{
public:
	MMWindowImplBase(){};
	virtual ~MMWindowImplBase(){};

public:

	virtual void OnFinalMessage( HWND hWnd );	
	void OnClick(TNotifyUI& msg);
	virtual UILIB_RESOURCETYPE GetResourceType() const; 
	virtual CDuiString GetSkinFolder(); 
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void Notify(TNotifyUI& msg);	
	void HandleNotifyEvent(TNotifyUI& msg);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	virtual void HandleClickEvent(TNotifyUI& msg){}
	virtual void HandleSetFocusEvent(TNotifyUI& msg){}
	virtual void HandleKillFocusEvent(TNotifyUI& msg){}	
	virtual void HandleButtonDownEvent(TNotifyUI& msg){}
	virtual void HandleSelectedChangedEvent(TNotifyUI& msg){}
	virtual void HandleTextChangedEvent(TNotifyUI& msg){}
	virtual void HandleValueChangedEvent(TNotifyUI& msg){}
	virtual void HandleItemSelectEvent(TNotifyUI& msg){}
	virtual void HandleItemClickEvent(TNotifyUI& msg){}
	virtual void HandleItemDBClickEvent(TNotifyUI& msg){}
	virtual void HandleTabSelectEvent(TNotifyUI& msg){}
	virtual void HandleItemExpandEvent(TNotifyUI& msg){}
	virtual void HandleWindowInitEvent(TNotifyUI& msg){}
	virtual void HandleMouseEnterEvent(TNotifyUI& msg){}
	virtual void HandleMouseLeaveEvent(TNotifyUI& msg){}
	virtual void HandleHeaderClickEvent(TNotifyUI& msg){}
	virtual void HandleMenuEvent(TNotifyUI& msg){}
	virtual void HandleLinkEvent(TNotifyUI& msg){}
	virtual void HandleTimerEvent(TNotifyUI& msg){}
	virtual void HandleReturnEvent(TNotifyUI& msg){}
	virtual void HandleScrollEvent(TNotifyUI& msg){}
	virtual void HandleDropdownEvent(TNotifyUI& msg){}
	virtual void HandleShowActivXEvent(TNotifyUI& msg){}
	virtual void HandleItemCollapseEvent(TNotifyUI& msg){}
	virtual void HandleItemAtivateEvent(TNotifyUI& msg){}
	virtual void HandleRichEditContextMenu(TNotifyUI& msg);
	virtual void CreateWnd(HWND hWnd = NULL);
	void SetTopMostWnd(bool bTop = true);
	void SetForegroundWnd(); //…Ë÷√«∞æ∞¥∞ø⁄
	void ShowToolTipBox();
};







