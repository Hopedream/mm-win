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

/// CSingleChatModuleImpl ʵ��
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
		ASSERT(!"�Ѿ���ӹ����¼�������");
	}
}

void CSingleChatModuleImpl::RemoveEventHandler(ISingleChatModuleEvent* pEventHandler)
{
	if (!m_ctrEventHandler.erase(pEventHandler))
	{
		ASSERT(!"û����ӹ����¼������ߣ��޷�ɾ��");
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

	//�������е�strFromBare����Ϣ��
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
	//�鿴�Ƿ���
	chatBoxDlg* pChatBoxDlg = FindSingleChatDlg(strFromBare);
	if (NULL != pChatBoxDlg)
	{
		ReadMessage(pChatBoxDlg, strFromBare);
	}
}

//��ʾ���ڣ���ȥ�鿴�Ƿ���δ������Ϣ������У�����ʾ�������ϡ�
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

		//�����ǰ�����ڣ��ʹ�����һ���µ�����򣬸�֪traytask����ֹͣ��˸��
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
	//������ڡ�
	if (FindSingleChatDlg(strUserBare) != NULL)
	{
		bCreated = TRUE;
	}
	
	return bCreated;
}