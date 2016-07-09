#pragma once

#include "StdAfx.h"
#include <string>
#include "..\Common\MMDefine.h"


using namespace DuiLib;
using namespace std;

// enum eSendMsgMode{
// 	eEnterKey,
// 	eCtrlEnterKey
// };
class CChatAndFilePage : public CHorizontalLayoutUI, public INotifyUI
{
public:
	static CChatAndFilePage* Instance();
	void InitialChatPage(CPaintManagerUI*);
	void SetSendMsgKeyMode(eSendMsgMode mode);
	bool IsEnterSendMode();

	virtual void Notify(TNotifyUI& msg) ; 
public:
	//std::tstring m_strCurAccount;

protected:
	CChatAndFilePage();
	virtual ~CChatAndFilePage();
	DuiLib::CPaintManagerUI* m_PaintManager;
	static CChatAndFilePage* m_pInstance;
	static bool m_bInitialized;
	eSendMsgMode m_eSendMode;
};