#include "StdAfx.h"
#include "MsgCentermoduleimpl.h"
#include "..\include\MsgCenterEvent_i.h"
#include "..\include\MsgWndDefine.h"
#include "..\include\DBTaskModule.h"
#include "..\include\Global_Def.h"
#include "..\Common\MMDefine.h"
#include "FileHelper.h"
#include <sstream>
#include "StringHelper.h"

/// dll�ӿ�
IMsgCenterModule* GetMsgCenterModule()
{
	static CMsgCenterModuleImpl* p = NULL;
	if (p == NULL)
	{
		p = new CMsgCenterModuleImpl;
	}

	return p;
}

/// CMsgCenterModuleImpl ʵ��
CMsgCenterModuleImpl::CMsgCenterModuleImpl(void)
{
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->AddEventHandler(this);
	}
}

CMsgCenterModuleImpl::~CMsgCenterModuleImpl(void)
{
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->RemoveEventHandler(this);
	}
}


void CMsgCenterModuleImpl::AddEventHandler(IMsgCenterModuleEvent* pEventHandler)
{
	if (m_ctrEventHandler.insert(pEventHandler).second == false)
	{
		////ASSERT(!"�Ѿ���ӹ����¼�������");
	}
}

void CMsgCenterModuleImpl::RemoveEventHandler(IMsgCenterModuleEvent* pEventHandler)
{
	if (!m_ctrEventHandler.erase(pEventHandler))
	{
		////ASSERT(!"û����ӹ����¼������ߣ��޷�ɾ��");
	}
}

void CMsgCenterModuleImpl::Release()
{
	
}
//just loop message.
void CMsgCenterModuleImpl::OnCmdMsgReceived(tstring& strSender, tstring& strCmd, ECmdType eType)
{
	if (eType == enmCmd_VoiceReq)
	{
		CSysMsgVoiceRequest* pVoiceRequest = new CSysMsgVoiceRequest;
		if (NULL != pVoiceRequest)
		{
			pVoiceRequest->strFromBare = strSender;
			pVoiceRequest->strMsg = strCmd;//�ļ�packid
			pVoiceRequest->uTime = time(0);

			//�ӵ����̣߳�˵������Ϣ���ˡ�
			if (NULL != g_pMsgWnd)
			{
				g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pVoiceRequest, (LPARAM)enmMsgType_Voice);
			}
		}
	}
	else if (enmCmd_FileReq == eType || enmMsgType_Image == eType)
	{
		CSysMsgFileRequest* pFileRequest = new CSysMsgFileRequest;
		if (NULL != pFileRequest)
		{
			pFileRequest->strFromBare = strSender;
			pFileRequest->strMsg = strCmd;//�ļ�packid
			pFileRequest->uTime = time(0);

			//�ӵ����̣߳�˵������Ϣ���ˡ�
			if (NULL != g_pMsgWnd)
			{
				g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pFileRequest, (LPARAM)enmMsgType_File);
			}
		}
	}
	
}
//�յ���Ϣ�󣬷��뵽δ�����У�֪ͨobserver���յ�xxx����Ϣ�ˡ�
void CMsgCenterModuleImpl::OnChatMsgReceived(tstring& strSenderBare, tstring& strChatMsg)
{
	CTextMsg* pMsg = new CTextMsg;
	pMsg->strFromBare = strSenderBare;
	pMsg->strMsg = strChatMsg;
	time_t t = time(0);   // get time now
	pMsg->uTime = t;

	//������Ϣ��
	m_listUnReadMsgs.push_back(pMsg);

	//д�����ݿ�
	tstring strUsername = CFileHelper::GetUserName(strSenderBare);
	_tChatMessage oneText;
	oneText.strDestUser = strUsername;
	oneText.strChatText = strChatMsg;	
	oneText.m_timeChat = t;
	
	GetDBTaskModule()->SaveChatMessage(strSenderBare, oneText);

	//�ӵ����̣߳�˵������Ϣ���ˡ�
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pMsg, (LPARAM)enmMsgType_Chat);
	}
}

void CMsgCenterModuleImpl::OnGroupChatMsgReceived(tstring& strRoomID, tstring& strSenderPhone, tstring& strText)
{
	CGroupMsg* pGroupMsg = new CGroupMsg;
	pGroupMsg->strRoomID = strRoomID;
	pGroupMsg->strFromBare = strSenderPhone+"@test-d";
	pGroupMsg->strMsg = strText;
	pGroupMsg->uTime = time(0);

	////д�뵽���ݿ�,���ڽ������Ľ�����ʾ���ǣ�ȥ���ݿ��ȡ��
	//_tGroupChatMessage oneGroupMsg;
	//oneGroupMsg.strMsg = strText;
	//oneGroupMsg.m_timeChat = pGroupMsg->uTime;
	//oneGroupMsg.strRoomID = strRoomID;
	//oneGroupMsg.strSenderPhone = strSenderPhone;
	//oneGroupMsg.strSenderAccount = pGroupMsg->strFromBare;

	//GetDBTaskModule()->SaveGroupChatMsg(strRoomID, oneGroupMsg);


	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage/*PostMessage*/(WM_NEWMSG_RECEIVED, (WPARAM)pGroupMsg, (LPARAM)enmMsgType_Group);
	}
}

//ϵͳ��Ϣ-�Ӻ�������
void CMsgCenterModuleImpl::OnRecvAddFriendReq(tstring& strFriend, const tstring& strMsg, const tstring& strSenderNick)
{
	time_t curtime = time(0);
	tstring strPhone = CFileHelper::GetUserName(strFriend);
	std::ostringstream os;
	if (strMsg.length() > 0)
	{
		os<<strSenderNick<<"("<<strPhone.c_str()<<") ���������Ϊ���ѡ� ��֤��Ϣ��" << strMsg;
	}
	else
	{
		os<<strSenderNick<<"("<<strPhone.c_str()<<") ���������Ϊ���ѡ�" << strMsg;
	}

	CSysMsgAddFriend* pMsg = new CSysMsgAddFriend;
	pMsg->strFromBare = SYS_NOTIFY_ACCOUNT;
	pMsg->strMsg = os.str().c_str();
	pMsg->uTime = curtime;
	pMsg->strSender = strFriend;

	//������Ϣ��
	m_listUnReadMsgs.push_back(pMsg);

	//д�����ݿ�
	sSysMsg sTmpMsg;
	sTmpMsg.eSubType = enmSysMsgSubType_AddFriend;
	sTmpMsg.strMsg = os.str().c_str();
	sTmpMsg.strSender = strFriend;
	sTmpMsg.uTime = curtime;
	GetDBTaskModule()->SaveSysMessage(sTmpMsg);

	//�ӵ����߳�ȥ
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pMsg, (LPARAM)enmMsgType_Sys);
	}
}

//ϵͳ��Ϣ-�Է�ͬ��Ӻ�������


void CMsgCenterModuleImpl::OnMessage(UINT uMsg, WPARAM w, LPARAM l)
{
	if (uMsg == WM_NEWMSG_RECEIVED)
	{
		EMsgType eMsgType = (EMsgType)(l);
		CMsgBase* pMsg = (CMsgBase*)w;
		if (NULL != pMsg)
		{
			for(SETEventHandlers::iterator it = m_ctrEventHandler.begin();
				it != m_ctrEventHandler.end(); ++it)
			{
				IMsgCenterModuleEvent* pHandler = *it;
				if (NULL != pHandler)
				{
					pHandler->OnReceiveMessage(pMsg, eMsgType);
				}
			}
		}	
	}
}

int CMsgCenterModuleImpl::GetUnreadMsgCount(tstring strFrombare)
{
	int nCount=0;
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); 
					it != m_listUnReadMsgs.end(); ++it)
	{
		CMsgBase* pMsg = (CMsgBase*)(*it);
		if (pMsg->strFromBare == strFrombare)
		{
			nCount++;
		}
	}

	return nCount;
}

bool CMsgCenterModuleImpl::doHaveUnReadMsg()
{
	if (m_listUnReadMsgs.size() > 0)
		return true;
	else
		return false;
}

void CMsgCenterModuleImpl::GetUnReadTextMsgs(tstring strFrombare, ListMsgs& listAllMsg)
{
	//�ó���Щδ����Ϣ��
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); it != m_listUnReadMsgs.end(); ++it)
	{
		CMsgBase* pMsg = (CMsgBase*)(*it);
		if (pMsg->strFromBare == strFrombare)
		{
			listAllMsg.push_back(pMsg);
		}
	}

	//д�����ݿ⣿���߸������ݿ��δ��״̬����
}

void CMsgCenterModuleImpl::GetUnReadSysMsgs( ListMsgs& listAllMsg )
{
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); it != m_listUnReadMsgs.end(); ++it)
	{
		CMsgBase* pMsg = (CMsgBase*)(*it);
		if (pMsg->strFromBare == SYS_NOTIFY_ACCOUNT)
		{
			listAllMsg.push_back(pMsg);
		}
	}
}



void CMsgCenterModuleImpl::ClearUnReadTextMsgs(tstring strFrombare)
{
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); it != m_listUnReadMsgs.end();)
	{
		CTextMsg* pTmp = (CTextMsg*)(*it);
		if (NULL != pTmp)
		{
			if (pTmp->strFromBare == strFrombare)
			{
				delete pTmp;
				it = m_listUnReadMsgs.erase(it);
			}
			else
			{
				++it;
			}
		}
		
	}
}

void CMsgCenterModuleImpl::ClearUnReadSysMsgs()
{
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); it != m_listUnReadMsgs.end();)
	{
		CSysMsg* pTmp = (CSysMsg*)(*it);
		if (NULL != pTmp)
		{
			if (pTmp->strFromBare == SYS_NOTIFY_ACCOUNT)
			{
				delete pTmp;
				it = m_listUnReadMsgs.erase(it);
			}
			else
			{
				++it;
			}
		}

	}
}


bool CMsgCenterModuleImpl::GetLatestUnReadMsg(tstring& strFrombare, CMsgBase*& pOneMsg)
{
	//�ó���Щδ����Ϣ��
	for (ListMsgs::reverse_iterator rit = m_listUnReadMsgs.rbegin(); 
		rit != m_listUnReadMsgs.rend(); ++rit)
	{
		CMsgBase* pMsg = (CMsgBase*)(*rit);
		if (pMsg->strFromBare == strFrombare)
		{
			pOneMsg = pMsg;
			return true;
		}
	}

	return false;
}
void CMsgCenterModuleImpl::RemoveLatestUnReadMsg(tstring& strFrombare, __int64 uTime)
{
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); it != m_listUnReadMsgs.end();)
	{
		CMsgBase* pTmp = (CMsgBase*)(*it);
		if (NULL != pTmp)
		{
			if (pTmp->strFromBare == strFrombare && uTime == pTmp->uTime)
			{
				//delete pTmp;//�ڴ�й©����mmDefine.h�ļ��е��ำֵΪ������ͬһ�����⡣
				m_listUnReadMsgs.erase(it);
				return;
			}
			else
			{
				++it;
			}
		}
	}
}

void CMsgCenterModuleImpl::OnRecvAcceptAddFriendReq( tstring& strFriend, const tstring& strSenderNick/*=(_T(""))*/ )
{
	time_t curtime = time(0);
	tstring strPhone = CFileHelper::GetUserName(strFriend);
	std::ostringstream os;
	os<<strSenderNick<<"("<<strPhone.c_str()<<") ��ͬ�������Ϊ����";

	CSysMsgAcceptAddFriend* pMsg = new CSysMsgAcceptAddFriend;
	pMsg->strFromBare = SYS_NOTIFY_ACCOUNT;
	pMsg->strMsg = os.str().c_str();
	pMsg->uTime = curtime;
	pMsg->strSender = strFriend;

	//������Ϣ��
	m_listUnReadMsgs.push_back(pMsg);

	//д�����ݿ�
	sSysMsg sTmpMsg;
	sTmpMsg.eSubType = enmSysMsgSubType_AcceptAddFriend;
	sTmpMsg.strMsg = os.str().c_str();
	sTmpMsg.strSender = strFriend;
	sTmpMsg.uTime = curtime;
	GetDBTaskModule()->SaveSysMessage(sTmpMsg);

	//�ӵ����߳�ȥ
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pMsg, (LPARAM)enmMsgType_Sys);
	}
}

void CMsgCenterModuleImpl::OnRecvOrgReq( tstring strFromJid, tstring strFromNick, tstring strOrgId, 
	tstring strOrgName, tstring strMsgType, tstring strReqId, tstring strDestJid, tstring strDestNick )
{
	tstring strtmpFromNick = CChineseCode::DecodeUTF8(strFromNick);
	tstring strTmpOrgName = CChineseCode::DecodeUTF8(strOrgName);
	tstring strtmpDestNick = CChineseCode::DecodeUTF8(strDestNick);
	time_t curtime = time(0);
	tstring strPhone = CFileHelper::GetUserName(strFromJid);
	tstring strDestPhone = CFileHelper::GetUserName(strDestJid);
	std::ostringstream os;
	ESysMsgSubType eSubType;
	if (strMsgType == "del_member")
	{
		os<<strtmpFromNick<<"("<<strPhone.c_str()<<") ���˳��˻��� "<<strTmpOrgName;
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if (strMsgType == "add_member")
	{
		os<<strtmpFromNick<<"("<<strPhone.c_str()<<") �����˻��� "<<strTmpOrgName;
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if  (strMsgType == "change_auth")
	{
		os<<strtmpDestNick<<"("<<strDestPhone.c_str()<<") �ѳ�Ϊ���� "<<strTmpOrgName<<" ���ϰ�";
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if (strMsgType == "req_org_buddy")
	{
		os<<strtmpFromNick<<"("<<strPhone.c_str()<<") ���ڻ��� ("<<strTmpOrgName<<" )" <<"���������Ļ�����Ϊ��������";
		eSubType = enmSysMsgSubType_AddOrgFriend;
	}
	else if (strMsgType == "apr_org_buddy")
	{
		os<<"����( "<<strTmpOrgName<<" )"<<"�������Ļ�����Ϊ��������";
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if (strMsgType == "cancel_org_buddy")
	{
		os<<"����( "<<strTmpOrgName <<" )" << "�������Ļ��������������";
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else
	{
		return;
	}



	//time_t curtime = time(0);
	//tstring strPhone = CFileHelper::GetUserName(strFriend);
	//std::ostringstream os;
	//os<<strSenderNick<<"("<<strPhone.c_str()<<") ��ͬ�������Ϊ����";

	CSysMsgAcceptAddFriend* pMsg = new CSysMsgAcceptAddFriend;
	pMsg->eSubType = eSubType;
	pMsg->strFromBare = SYS_NOTIFY_ACCOUNT;
	pMsg->strMsg = os.str().c_str();
	pMsg->uTime = curtime;
	pMsg->strSender= strReqId;
	//pMsg->strSender = strFriend;

	//������Ϣ��
	m_listUnReadMsgs.push_back(pMsg);

	//д�����ݿ�
	sSysMsg sTmpMsg;
	sTmpMsg.eSubType = eSubType;
	sTmpMsg.strMsg = os.str().c_str();
	sTmpMsg.strSender = strReqId;
	sTmpMsg.uTime = curtime;
	GetDBTaskModule()->SaveSysMessage(sTmpMsg);

	//�ӵ����߳�ȥ
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pMsg, (LPARAM)enmMsgType_Sys);
	}
}




