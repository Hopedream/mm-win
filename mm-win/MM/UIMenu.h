#pragma once
// �˴���Ҫ����duilib��ͷ�ļ�#include <UIlib.h>��һ�Σ���ϸ�������ǰ��̳�
#include "StdAfx.h"
#include "MMWinImplBase.h"

enum eContextMenu{
	eUIMenuDeleteFriend,
	eUIMenuCallTypeDrop,
	eUIMenuSysSetting,
	eUIMenuSendMsgKeyDrop,
	eUIMenuMsgEmpty,
	eUIMenuMsgTooLog,
	eUIMenuRichEditContextMenu,
	eUIInvalidate,
};

class CXMLWnd : public MMWindowImplBase
{
public:
	explicit CXMLWnd(LPCTSTR pszXMLPath) 
		: m_strXMLPath(pszXMLPath){}

public:
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("XMLWnd");
	}

	virtual CDuiString GetSkinFile()
	{
		return m_strXMLPath;
	}

	virtual CDuiString GetSkinFolder()
	{
		return _T("");
	}

protected:
	CDuiString m_strXMLPath;
};

class CDuiMenu: public CXMLWnd
{
public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, eContextMenu _contextMenuType = eUIInvalidate);

protected:
	virtual ~CDuiMenu();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	void Init(HWND hWndParent, POINT ptPos);
	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKillFocus   (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleWindowInitEvent(TNotifyUI& msg);
	void HandleCallTypeContextMenuEvent(TNotifyUI& msg);
	void HandleSysSettingMenuEvent(TNotifyUI& msg);
	void HandleSendMsgKeyMenuEvent( TNotifyUI& msg );
	void HandleKillFocusEvent(TNotifyUI& msg);
	void HandleSetFocusEvent(TNotifyUI& msg);
	void HandleRichEditContextMenu(TNotifyUI& msg);
	void SetRichEditUI(CRichEditUI* pRichEditUI);

//	void Notify(TNotifyUI& msg);
private:
	HWND m_hParentWnd;
	eContextMenu m_eContextMenuType;
	CRichEditUI* m_pRichEditUI;
};