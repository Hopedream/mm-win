#pragma once
#include "MMWinImplBase.h"

#define MSGBOX_ICON_NORMAL				0
#define MSGBOX_ICON_ERROR					1
#define MSGBOX_ICON_ASK							2
#define MSGBOX_ICON_WARNING				3

#define MSGBOX_IDOK									1
#define MSGBOX_IDYES									2
#define MSGBOX_IDNO									4
#define MSGBOX_IDYESNO								6

class CMsgBox : public MMWindowImplBase
{
public:	
	CMsgBox(const tstring strTile = (_T("")), const tstring strText = (_T("")), UINT icon = MSGBOX_ICON_NORMAL, UINT uMsgBoxType = MSGBOX_IDOK);
	UINT ShowModal(HWND hParen = NULL);
	void ShowWindow(HWND hParen = NULL);
	static int MsgBox(HWND hParen, const tstring strTile = (_T("")), const tstring strText = (_T("")), UINT icon = MSGBOX_ICON_NORMAL, UINT uMsgBoxType = MSGBOX_IDOK);
protected:	
	virtual CDuiString GetSkinFile() {return _T("systemMsgBox\\MsgBox.xml");}
	void InitWindow();
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleWindowInitEvent(TNotifyUI& msg);
	
private:		
	 CControlUI *m_pFlagCtrl;
	 CControlUI *m_pText;
	 CControlUI *m_pTile;	

	 CControlUI *m_pBnOK;
	 CControlUI *m_pBnYes;
	 CControlUI *m_pBnNO;

	 tstring    m_strText;
	 tstring    m_strTitle;
	 UINT       m_iconIdx;
	 UINT   m_msgboxType;
};


