#include "StdAfx.h"
#include <string>
#include "..\include\SingleChatModule.h"
#include "..\include\SinglechatEvent_i.h"
#include "..\include\MsgCenterModule.h"
#include "SingleChatmoduleimpl.h"
#include "Controller.h"

using namespace std;

ISingleChatModule* GetSingleChatModule()
{
	static CSingleChatModuleImpl* p = NULL;
	if (p == NULL)
	{
		p = new CSingleChatModuleImpl;
	}

	return p;
}

/// CSingleChatModuleImpl 实现
CSingleChatModuleImpl::CSingleChatModuleImpl(void)
{
	//receive chat messge.
	GetMsgCenterModule()->AddEventHandler(this);
}

CSingleChatModuleImpl::~CSingleChatModuleImpl(void)
{
}

void CSingleChatModuleImpl::AddEventHandler(ISingleChatModuleEvent* pEventHandler)
{
	if (m_ctrEventHandler.insert(pEventHandler).second == false)
	{
		ASSERT(!"已经添加过该事件处理者");
	}
}

void CSingleChatModuleImpl::RemoveEventHandler(ISingleChatModuleEvent* pEventHandler)
{
	if (!m_ctrEventHandler.erase(pEventHandler))
	{
		ASSERT(!"没有添加过该事件处理者，无法删除");
	}
}

void CSingleChatModuleImpl::Release()
{
	delete this;
}

void CSingleChatModuleImpl::ReadMessage(chatBoxDlg* pChatBoxDlg, string strFromBare)
{
	ListMsgs listAllMsg;
	GetMsgCenterModule()->GetUnReadTextMsgs(strFromBare, listAllMsg);

	//插入所有的strFromBare的消息。
	for (ListMsgs::iterator it = listAllMsg.begin(); it != listAllMsg.end(); ++it)
	{
		sTmpMsg* pMsg = *it;
		if (NULL != pMsg)
		{
			if (pMsg->strFromBare == strFromBare)
			{
				pChatBoxDlg->SetMessage(strFromBare, pMsg->strMsg);
			}
		}
	}

	GetMsgCenterModule()->ClearUnReadTextMsgs(strFromBare);
}
void CSingleChatModuleImpl::OnReceiveSinglechatTextMsg(string& strFromBare, string& strMsg)
{
	//查看是否在
	chatBoxDlg* pChatBoxDlg = FindSingleChatDlg(strFromBare);
	if (NULL != pChatBoxDlg)
	{
		ReadMessage(pChatBoxDlg, strFromBare);
	}
}

//显示窗口，并去查看是否有未读的消息，如果有，就显示到界面上。
WindowImplBase* CSingleChatModuleImpl::ShowSingleChatDlg(string& strUserBare)
{
	string strUser = strUserBare;
	chatBoxDlg* pchatBoxDlg = FindSingleChatDlg(strUser);
	if (NULL == pchatBoxDlg)
	{
		pchatBoxDlg = new chatBoxDlg();
		if( pchatBoxDlg == NULL ) 
			return NULL;
		pchatBoxDlg->setOppositeName(CDuiString(strUser.c_str()));
		
		//get self bare id.
		Client* pClient = CController::Instance()->GetXmppClient();
		if (NULL != pClient)
		{
			string strMyName = pClient->jid().bare();
			pchatBoxDlg->setUserName(CDuiString(strMyName.c_str()));
		}
		
		pchatBoxDlg->Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME,
							WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 400, 400);
		pchatBoxDlg->CenterWindow();
		pchatBoxDlg->setDestNametoCtrl(CDuiString(strUser.c_str()));

		m_mapSingChatDlg[strUser] = pchatBoxDlg;

		//聊天框当前不存在，就创建了一个新的聊天框，告知traytask可以停止闪烁了
		for (CCtrEventHandler::iterator it = m_ctrEventHandler.begin(); 
			it != m_ctrEventHandler.end(); it++)
		{
			ISingleChatModuleEvent* p = *it;
			p->OnDlgOpened(strUser);
		}
	}

	
	ReadMessage(pchatBoxDlg, strUser/*strUserBare*/);

	pchatBoxDlg->ShowWindow(SW_RESTORE);

	return pchatBoxDlg;
	//pchatBoxDlg->SetForegroundWindow();
}

chatBoxDlg* CSingleChatModuleImpl::FindSingleChatDlg(string& strUserBare)
{
	map<string,chatBoxDlg*>::iterator iter = m_mapSingChatDlg.find(strUserBare);
	if (iter != m_mapSingChatDlg.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

void CSingleChatModuleImpl::RemoveSingleChatDlg(string strUserBare)
{
	map<string,chatBoxDlg*>::iterator iter = m_mapSingChatDlg.find(strUserBare);
	if (iter != m_mapSingChatDlg.end())
	{
		chatBoxDlg* pSingleChatDlg = iter->second;
// 		if (pSingleChatDlg)
// 		{
// 			pSingleChatDlg->DestroyWindow();
// 		}
		delete pSingleChatDlg;
		pSingleChatDlg = NULL;

		m_mapSingChatDlg.erase(iter);
	}
}

void CSingleChatModuleImpl::RemoveAllSingleChatDlg()
{
	map<string,chatBoxDlg*>::iterator iter;
	for (iter=m_mapSingChatDlg.begin();iter!=m_mapSingChatDlg.end();iter++)
	{
		RemoveSingleChatDlg(iter->first);
	}
}

BOOL CSingleChatModuleImpl::IsDlgCreated(string& strUserBare)
{
	BOOL bCreated = FALSE;
	//如果存在。
	if (FindSingleChatDlg(strUserBare) != NULL)
	{
		bCreated = TRUE;
	}
	
	return bCreated;
}