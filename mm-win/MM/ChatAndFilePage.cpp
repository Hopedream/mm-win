#include "ChatAndFilePage.h"
#include "CDialogBuilderCallbackEx.h"
#include "mainDlg.h"
#include "UIMenu.h"
#include "../include/DBTaskModule.h"
#include "../include/Global_Def.h"

using namespace std;

const TCHAR* const kbuttonSendButtonControlName		= _T("buttonSend");				//发送消息按钮
const TCHAR* const kBtnChooseSendKeyControlName		= _T("btnChooseSendtype");				//发送消息按钮

CChatAndFilePage* CChatAndFilePage::m_pInstance = NULL;
bool CChatAndFilePage::m_bInitialized = false;


CChatAndFilePage::CChatAndFilePage()
{
	m_eSendMode = (eSendMsgMode)GetDBTaskModule()->GetSendMsgHotKey();
}

CChatAndFilePage::~CChatAndFilePage()
{
	if (m_bInitialized)
	{
		m_PaintManager->RemoveNotifier(this);
	}
}


void CChatAndFilePage::Notify( TNotifyUI& msg )
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == kbuttonSendButtonControlName)
		{
			mainDlg::getInstance()->OnBtnSendMessage(); //有待重构，把聊天相关的操作全部放在该类中 --llzhou
		}
		if (msg.pSender->GetName() == kBtnChooseSendKeyControlName)
		{
			POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
			//::ClientToScreen(mainDlg::getInstance()->GetHWND(), &pt);
			CDuiMenu *pMenu = new CDuiMenu(_T("sendTypeDropUISkin.xml"), eUIMenuSendMsgKeyDrop);
			pMenu->Init(mainDlg::getInstance()->GetHWND(), pt);
			pMenu->ShowWindow(TRUE);
		}
	}
}

CChatAndFilePage* CChatAndFilePage::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CChatAndFilePage;
	}
	return m_pInstance;
}

void CChatAndFilePage::InitialChatPage( CPaintManagerUI* ppm )
{
	if (m_bInitialized)
	{
		return;
	}

	m_bInitialized = true;
	m_PaintManager = ppm;

	CDialogBuilder DlgBuilder1;
	CDialogBuilderCallbackEx cb1(m_PaintManager);
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager->FindControl("RecentChatTabLayout"));
	if (NULL == pLayout)
	{
		return;
	}

	CHorizontalLayoutUI* pChatList = NULL;
	if (!DlgBuilder1.GetMarkup()->IsValid())
	{
		pChatList = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create(_T("tab_chat_list.xml"),
			(UINT)0, &cb1, m_PaintManager));
	}
	else
	{
		pChatList = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create((UINT)0, m_PaintManager));
	}

	//作为第1个页签
	if (NULL != pChatList)
	{
		pLayout->AddAt(pChatList, CONST_RECENT_TAB_P2P_CHAT_LIST/*0*/);
	}
	m_PaintManager->AddNotifier(this);
}


void CChatAndFilePage::SetSendMsgKeyMode(eSendMsgMode mode)
{
	m_eSendMode  = mode;
	GetDBTaskModule()->SaveSendMsgHotKey(mode);
}

bool CChatAndFilePage::IsEnterSendMode()
{
	return m_eSendMode == eEnterKey;
}