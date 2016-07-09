#include "StdAfx.h"
#include "TrayTaskmoduleimpl.h"
#include "Resource.h"
#include "Controller.h"
#include "UserManager.h"
#include "..\include\MsgCenterModule.h"
#include "..\include\TrayTaskEvent_i.h"
#include "..\include\MsgWndDefine.h"
#include <Windows.h>
#include <MMSystem.h>
#include "FileHelper.h"

/**
* TrayTask module�ķ����ȡ�ӿ�
*/
ITrayTaskModule* GetTrayTaskModule()
{
	static CTrayTaskModuleImpl* p = NULL;
	if (p == NULL)
	{
		p = new CTrayTaskModuleImpl;
	}

	return p;
}


CTrayTaskModuleImpl::CTrayTaskModuleImpl(void)
{

	m_hInstIcon = NULL;
	m_hOfflineIcon = NULL;
	m_hSysNotifyIcon = NULL;
	m_hCallIncommingIcon = NULL;
	//GetMsgCenterModule()->AddEventHandler(this);
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->AddEventHandler(this);
	}
}

CTrayTaskModuleImpl::~CTrayTaskModuleImpl(void)
{
	//GetMsgCenterModule()->RemoveEventHandler(this);
}


void CTrayTaskModuleImpl::AddEventHandler(ITrayTaskModuleEvent* pEventHandler)
{
	if (m_setTrayModuleHandlers.insert(pEventHandler).second == false)
	{
		//ASSERT(!"�Ѿ���ӹ����¼�������");
	}
}

void CTrayTaskModuleImpl::Release()
{
	Uninitialize();
	delete this;
}

void CTrayTaskModuleImpl::Initialize(HINSTANCE hInstance)
{
	m_theTrayMgr.Init();
	m_hInstIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IM_LOGO_ONLINE));//::LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)/*(LPCWSTR)IDI_SMALL*/);
	m_hOfflineIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IM_LOGO_OFFLINE));
	m_hSysNotifyIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IM_SYS_NOTIFY));
	m_hCallIncommingIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IM_CALL_IMCOMMING));
	m_theTrayMgr.AddTaskbarIcon(m_hInstIcon, _T("MM"));
	m_theTrayMgr.AddEventHandler(this);
}

void CTrayTaskModuleImpl::Uninitialize()
{
	m_theTrayMgr.Delete();
}

BOOL CTrayTaskModuleImpl::IsFlashing()
{
	return m_theTrayMgr.IsFlashing();
}

void CTrayTaskModuleImpl::FlashGroup(tstring& strRoomId)
{
	CDuiString strAppPath = CFileHelper::GetCurrentAppPath();
	strAppPath += _T("sound\\sys.wav");

	PlaySound(strAppPath.GetData(),NULL,SND_FILENAME|SND_SYNC);

	if (!IsGroupMsgFlashing(strRoomId))
	{
		SGroupFlashData* pData = new SGroupFlashData(strRoomId, m_hInstIcon);
		m_theTrayMgr.Flash(pData->hicon, (DWORD)pData);
	}
}
void CTrayTaskModuleImpl::Flash(tstring strAccount, EMsgType eType)
{
	CDuiString strAppPath = CFileHelper::GetCurrentAppPath();
	strAppPath += _T("sound\\sys.wav");
	PlaySound(strAppPath.GetData(),NULL,SND_FILENAME|SND_SYNC);

	if (eType == enmMsgType_Chat)
	{
		//strAccount�Ƿ��Ѿ�����˸�ˡ�
		if(!IsSingleChatFlashing(strAccount))
		{
			SSinglechatFlashData* pData = new SSinglechatFlashData(strAccount, m_hInstIcon);
			m_theTrayMgr.Flash(pData->hicon, (DWORD)pData);
		}
	}
	else if (eType == enmMsgType_Sys)
	{
		//strAccount�Ƿ��Ѿ�����˸�ˡ�
		if(!IsSysMsgFlashing(strAccount))
		{
			SSysNotifyFlashData* pData = new SSysNotifyFlashData(strAccount, m_hSysNotifyIcon);
			m_theTrayMgr.Flash(pData->hicon, (DWORD)pData);
		}
	}
	else if ( enmMsgType_incomeCall ==  eType)
	{
		SCallIncomeFlashData* pData = new SCallIncomeFlashData(strAccount, m_hCallIncommingIcon);
		m_theTrayMgr.Flash(pData->hicon, (DWORD)pData);
	}
}

void CTrayTaskModuleImpl::StopGroupFlash(tstring& strRoomID)
{
	int idxFlash = 0;
	if (!IsGroupMsgFlashing(strRoomID, &idxFlash))
	{
		return;
	}

	SGroupFlashData* pData = (SGroupFlashData*)m_theTrayMgr.GetParam(idxFlash);
	delete pData;
	m_theTrayMgr.StopFlash(idxFlash);
	
}

void CTrayTaskModuleImpl::StopFlash(tstring strAccount, EMsgType eType)
{
	if (eType == enmMsgType_Chat)
	{
		StopSinglechatFlash(strAccount);
	}
	else if (eType == enmMsgType_Sys)
	{
		StopLatestSysMsgFlash(strAccount);
	}
	else if (eType == enmMsgType_incomeCall)
	{
		StopIncomeCallMsgFlash(strAccount);
	}
}

/**
* TrayMgr�ϱ����¼�����
*/
void CTrayTaskModuleImpl::OnTrayEvent(WPARAM w, LPARAM l)
{
// 	if (!GetMainFrameModule()->GetMainFrameDlg())
// 	{
// 		return;
// 	}

	UINT uMsgId = l;

	switch (uMsgId)
	{
	case ITrayTaskModuleEvent::eTrayEvent_LBUTTONDOWN://Tray�ϱ������Down��Ϣ
		{

		}
		break;
	case ITrayTaskModuleEvent::eTrayEvent_LBUTTONDBLCLK://Tray�ϱ������˫����Ϣ
		{
			if (!IsFlashing())
			{
// 				if (!GetMainFrameModule()->GetMainFrameDlg()->IsWindowVisible())
// 				{
// 					GetMainFrameModule()->GetMainFrameDlg()->ShowWindow(SW_RESTORE);
// 				}
			}
		}
		break;
	case ITrayTaskModuleEvent::eTrayEvent_RBUTTONDOWN://Tray�ϱ����Ҽ�������Ϣ
		{
// 			CCursorPos pt;
// 			CMenu m;
// 			m.LoadMenu(IDR_MENU_STATUSMENU_TOPDLG);
// 			m.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, GetMainFrameModule()->GetMainFrameDlg());
		}
		break;
	case ITrayTaskModuleEvent::eTrayEvent_LBUTTONDOWN_Late:
		{
// 			GetMainFrameModule()->GetMainFrameDlg()->SetForegroundWindow();
// 			GetMainFrameModule()->GetMainFrameDlg()->ShowWindow(SW_RESTORE);
		}
		break;
	case ITrayTaskModuleEvent::eTrayEvent_TrayFlashStopped:
		{
			SFlashData* pData = (SFlashData*)w;

			//�����������Ϣ
			if (pData->eFlashType == SFlashData::eFlashType_Singlechat)
			{
				//��֪observer��
				for(SetTrayModuleEventHanders::iterator it = m_setTrayModuleHandlers.begin(); 
					it != m_setTrayModuleHandlers.end(); ++it)
				{
					ITrayTaskModuleEvent* pHandler = *it;
					if (NULL != pHandler)
					{
						pHandler->OnDbClickChatMsg(((SSinglechatFlashData*)pData)->u);
					}
				}

				//ֹͣ��˸��
 				StopSinglechatFlash(((SSinglechatFlashData*)pData)->u);
			}
			//�����ϵͳ֪ͨ����Ϣ��
			else if (pData->eFlashType == SFlashData::eFlashType_SysNotify)
			{
				tstring strAccount = ((SSysNotifyFlashData*)pData)->strAccount;

				//ֹͣ��˸===>����ĺ������ܻᱻmessagebox����ס��
				StopLatestSysMsgFlash(strAccount);

				//��֪observer��
				for(SetTrayModuleEventHanders::iterator it = m_setTrayModuleHandlers.begin(); 
					it != m_setTrayModuleHandlers.end(); ++it)
				{
					ITrayTaskModuleEvent* pHandler = *it;
					if (NULL != pHandler)
					{
						pHandler->OnDbClickSysMessage(strAccount);
					}
				}
			}
			else if (pData->eFlashType == SFlashData::eFlashType_IncomeCall)
			{
				tstring strAccount = ((SCallIncomeFlashData*)pData)->strAccount;
				StopIncomeCallMsgFlash(strAccount);

				//��֪observer��
				for(SetTrayModuleEventHanders::iterator it = m_setTrayModuleHandlers.begin(); 
					it != m_setTrayModuleHandlers.end(); ++it)
				{
					ITrayTaskModuleEvent* pHandler = *it;
					if (NULL != pHandler)
					{
						pHandler->OnDbClickCallIncomeMessage(strAccount);
					}
				}
			}
			else if (pData->eFlashType == SFlashData::eFlashType_Group)
			{
				//��֪observer��
				for(SetTrayModuleEventHanders::iterator it = m_setTrayModuleHandlers.begin(); 
					it != m_setTrayModuleHandlers.end(); ++it)
				{
					ITrayTaskModuleEvent* pHandler = *it;
					if (NULL != pHandler)
					{
						pHandler->OnDbClickGroupMsg(((SGroupFlashData*)pData)->gid);
					}
				}

				//ֹͣ��˸��
				StopGroupFlash(((SGroupFlashData*)pData)->gid);
			}
		}
		break;
	}
}


//void CTrayTaskModuleImpl::OnChangeStatus(BOOL bSucc,unsigned short nStatus)
//{
//	if (!bSucc)
//	{
//		//ASSERT(0);
//		return;
//	}
//
//	m_theTrayMgr.ChangeIcon(GetCurStatusIcon());
//}
//
//void CTrayTaskModuleImpl::OnLogout(IM_uint32 uReason)
//{
//	m_theTrayMgr.ChangeIcon(GetCurStatusIcon());
//}
//
//void CTrayTaskModuleImpl::OnLogin(BOOL bSucc,IM_uint32 uErrorCode)
//{
//	m_theTrayMgr.ChangeIcon(GetCurStatusIcon());
//}
//

//lee�����Ӧ����ָֹͣջ������Ϣ��˸
void CTrayTaskModuleImpl::StopLatestSysMsgFlash(tstring& strAccount)
{
	int idxFlash = 0;
	if (!IsSysMsgFlashing(strAccount, &idxFlash))
	{
		return;
	}

	SSysNotifyFlashData* pData = (SSysNotifyFlashData*)m_theTrayMgr.GetParam(idxFlash);
	delete pData;
	m_theTrayMgr.StopFlash(idxFlash);
}


bool CTrayTaskModuleImpl::IsCallIncomeMsgFlashing(tstring& strSysAccount, int* pidx /*= NULL*/)
{
	vector<STrayFlashStackItem>* pStack = m_theTrayMgr.GetFlashStack();
	for (vector<STrayFlashStackItem>::iterator it = pStack->begin(); it != pStack->end(); it++)
	{
		STrayFlashStackItem& r = *it;
		SFlashData* pData = (SFlashData*)r.dwParam;
		if (((SCallIncomeFlashData*)pData)->strAccount == strSysAccount)
		{
			if (pidx)
			{
				*pidx = it - pStack->begin();
			}
			return true;
		}
	}
	return false;
}

void CTrayTaskModuleImpl::StopIncomeCallMsgFlash(tstring& strAccount)
{
	int idxFlash = 0;
	if (!IsCallIncomeMsgFlashing(strAccount, &idxFlash))
	{
		return;
	}

	SSysNotifyFlashData* pData = (SSysNotifyFlashData*)m_theTrayMgr.GetParam(idxFlash);
	delete pData;
	m_theTrayMgr.StopFlash(idxFlash);
}

//lee�����Ӧ��ֻ�ж�ջ����һ��SysMsg�Ƿ�����˸����===>ò�Ʋ��ԣ���Ҫ���ԣ����ǲ����õ���ջ����Ϣ
bool CTrayTaskModuleImpl::IsSysMsgFlashing(tstring& strSysAccount, int* pidx /*= NULL*/)
{
	vector<STrayFlashStackItem>* pStack = m_theTrayMgr.GetFlashStack();
	for (vector<STrayFlashStackItem>::iterator it = pStack->begin(); it != pStack->end(); it++)
	{
		STrayFlashStackItem& r = *it;
		SFlashData* pData = (SFlashData*)r.dwParam;
		if (((SSysNotifyFlashData*)pData)->strAccount == strSysAccount)
		{
			if (pidx)
			{
				*pidx = it - pStack->begin();
			}
			return true;
		}
	}
	return false;
}

bool CTrayTaskModuleImpl::IsGroupMsgFlashing(tstring& strRoomId, int* pidx/* =NULL */)
{
	vector<STrayFlashStackItem>* pStack = m_theTrayMgr.GetFlashStack();
	for (vector<STrayFlashStackItem>::iterator it = pStack->begin(); it != pStack->end(); it++)
	{
		STrayFlashStackItem& r = *it;
		SFlashData* pData = (SFlashData*)r.dwParam;
		if ( ((SGroupFlashData*)pData)->gid == strRoomId)
		{
			if (pidx)
			{
				*pidx = it - pStack->begin();
			}
			return true;
		}
	}
	return false;
}

bool CTrayTaskModuleImpl::IsSingleChatFlashing(tstring u, int* pidx/* = NULL*/)
{
	vector<STrayFlashStackItem>* pStack = m_theTrayMgr.GetFlashStack();
	for (vector<STrayFlashStackItem>::iterator it = pStack->begin(); it != pStack->end(); it++)
	{
		STrayFlashStackItem& r = *it;
		SFlashData* pData = (SFlashData*)r.dwParam;
		if (/*pData->eFlashType == SFlashData::eFlashType_Singlechat &&*/ ((SSinglechatFlashData*)pData)->u == u)
		{
			if (pidx)
			{
				*pidx = it - pStack->begin();
			}
			return true;
		}
	}
	return false;
}

void CTrayTaskModuleImpl::StopSinglechatFlash(tstring u)
{
	int idxFlash = 0;
	if (!IsSingleChatFlashing(u, &idxFlash))
	{
		return;
	}

	SSinglechatFlashData* pData = (SSinglechatFlashData*)m_theTrayMgr.GetParam(idxFlash);
	delete pData;
	m_theTrayMgr.StopFlash(idxFlash);
}

//�����л������߳�ȥflash������timer ��������
void CTrayTaskModuleImpl::OnMessage(UINT uMsg, WPARAM w, LPARAM l)
{
// 	if (WM_RECV_ADD_REQUEST == uMsg)
// 	{
// 		SSysNotifyFlashData* pData = (SSysNotifyFlashData*)w;
// 		if (NULL != pData)
// 		{
// 			m_theTrayMgr.Flash(pData->hicon, (DWORD)pData);
// 		}
// 	}
// 	else 
	/*if(WM_CONNECT_SUCCESS == uMsg)
	{
	CController::Instance()->GetUserInfoMgr()->AddEventHandler((IUserManagerEvent*)this);
	}*/
}
//������˸����С����
// void CTrayTaskModuleImpl::OnRecvAddFriendReq(tstring& strFriend)
// {
// 	if (g_pMsgWnd != NULL)
// 	{
// 		SSysNotifyFlashData* pData = new SSysNotifyFlashData(strFriend, m_hSysNotifyIcon);
// 		g_pMsgWnd->PostMessage(WM_RECV_ADD_REQUEST, (WPARAM)pData, 0);
// 	}
// }
//����С���ȣ���֪�Ӻ��ѳɹ��ˡ�
void CTrayTaskModuleImpl::OnAddFriendSucc(tstring& strFriend)
{

}
//����С���ȣ���֪xxxɾ�����ҡ�
void CTrayTaskModuleImpl::OnFriendRemoved(tstring& strRemovedAccout, bool bActive)
{
	//�����ĲŻ�С���ȡ�
	if (!bActive)
	{

	}
}