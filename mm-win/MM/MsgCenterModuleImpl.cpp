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

/// dll接口
IMsgCenterModule* GetMsgCenterModule()
{
	static CMsgCenterModuleImpl* p = NULL;
	if (p == NULL)
	{
		p = new CMsgCenterModuleImpl;
	}

	return p;
}

/// CMsgCenterModuleImpl 实现
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
		////ASSERT(!"已经添加过该事件处理者");
	}
}

void CMsgCenterModuleImpl::RemoveEventHandler(IMsgCenterModuleEvent* pEventHandler)
{
	if (!m_ctrEventHandler.erase(pEventHandler))
	{
		////ASSERT(!"没有添加过该事件处理者，无法删除");
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
			pVoiceRequest->strMsg = strCmd;//文件packid
			pVoiceRequest->uTime = time(0);

			//扔到主线程，说有新消息来了。
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
			pFileRequest->strMsg = strCmd;//文件packid
			pFileRequest->uTime = time(0);

			//扔到主线程，说有新消息来了。
			if (NULL != g_pMsgWnd)
			{
				g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pFileRequest, (LPARAM)enmMsgType_File);
			}
		}
	}
	
}
//收到消息后，放入到未读队列，通知observer：收到xxx的消息了。
void CMsgCenterModuleImpl::OnChatMsgReceived(tstring& strSenderBare, tstring& strChatMsg)
{
	CTextMsg* pMsg = new CTextMsg;
	pMsg->strFromBare = strSenderBare;
	pMsg->strMsg = strChatMsg;
	time_t t = time(0);   // get time now
	pMsg->uTime = t;

	//缓存消息。
	m_listUnReadMsgs.push_back(pMsg);

	//写入数据库
	tstring strUsername = CFileHelper::GetUserName(strSenderBare);
	_tChatMessage oneText;
	oneText.strDestUser = strUsername;
	oneText.strChatText = strChatMsg;	
	oneText.m_timeChat = t;
	
	GetDBTaskModule()->SaveChatMessage(strSenderBare, oneText);

	//扔到主线程，说有新消息来了。
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

	////写入到数据库,便于接下来的界面显示的是，去数据库读取。
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

//系统消息-加好友请求
void CMsgCenterModuleImpl::OnRecvAddFriendReq(tstring& strFriend, const tstring& strMsg, const tstring& strSenderNick)
{
	time_t curtime = time(0);
	tstring strPhone = CFileHelper::GetUserName(strFriend);
	std::ostringstream os;
	if (strMsg.length() > 0)
	{
		os<<strSenderNick<<"("<<strPhone.c_str()<<") 请求添加您为好友。 验证信息：" << strMsg;
	}
	else
	{
		os<<strSenderNick<<"("<<strPhone.c_str()<<") 请求添加您为好友。" << strMsg;
	}

	CSysMsgAddFriend* pMsg = new CSysMsgAddFriend;
	pMsg->strFromBare = SYS_NOTIFY_ACCOUNT;
	pMsg->strMsg = os.str().c_str();
	pMsg->uTime = curtime;
	pMsg->strSender = strFriend;

	//缓存消息。
	m_listUnReadMsgs.push_back(pMsg);

	//写入数据库
	sSysMsg sTmpMsg;
	sTmpMsg.eSubType = enmSysMsgSubType_AddFriend;
	sTmpMsg.strMsg = os.str().c_str();
	sTmpMsg.strSender = strFriend;
	sTmpMsg.uTime = curtime;
	GetDBTaskModule()->SaveSysMessage(sTmpMsg);

	//扔到主线程去
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pMsg, (LPARAM)enmMsgType_Sys);
	}
}

//系统消息-对方同意加好友请求


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
	//拿出这些未读消息。
	for (ListMsgs::iterator it = m_listUnReadMsgs.begin(); it != m_listUnReadMsgs.end(); ++it)
	{
		CMsgBase* pMsg = (CMsgBase*)(*it);
		if (pMsg->strFromBare == strFrombare)
		{
			listAllMsg.push_back(pMsg);
		}
	}

	//写入数据库？或者更新数据库的未读状态？！
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
	//拿出这些未读消息。
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
				//delete pTmp;//内存泄漏。跟mmDefine.h文件中的类赋值为乱码是同一个问题。
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

void CMsgCenterModuleImpl::OnRecvAcceptAddFriendReq( tstring& strFriend, const tstring& strSenderNick/*=""*/ )
{
	time_t curtime = time(0);
	tstring strPhone = CFileHelper::GetUserName(strFriend);
	std::ostringstream os;
	os<<strSenderNick<<"("<<strPhone.c_str()<<") 已同意添加您为好友";

	CSysMsgAcceptAddFriend* pMsg = new CSysMsgAcceptAddFriend;
	pMsg->strFromBare = SYS_NOTIFY_ACCOUNT;
	pMsg->strMsg = os.str().c_str();
	pMsg->uTime = curtime;
	pMsg->strSender = strFriend;

	//缓存消息。
	m_listUnReadMsgs.push_back(pMsg);

	//写入数据库
	sSysMsg sTmpMsg;
	sTmpMsg.eSubType = enmSysMsgSubType_AcceptAddFriend;
	sTmpMsg.strMsg = os.str().c_str();
	sTmpMsg.strSender = strFriend;
	sTmpMsg.uTime = curtime;
	GetDBTaskModule()->SaveSysMessage(sTmpMsg);

	//扔到主线程去
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
		os<<strtmpFromNick<<"("<<strPhone.c_str()<<") 已退出了机构 "<<strTmpOrgName;
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if (strMsgType == "add_member")
	{
		os<<strtmpFromNick<<"("<<strPhone.c_str()<<") 加入了机构 "<<strTmpOrgName;
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if  (strMsgType == "change_auth")
	{
		os<<strtmpDestNick<<"("<<strDestPhone.c_str()<<") 已成为机构 "<<strTmpOrgName<<" 的老板";
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if (strMsgType == "req_org_buddy")
	{
		os<<strtmpFromNick<<"("<<strPhone.c_str()<<") 所在机构 ("<<strTmpOrgName<<" )" <<"申请与您的机构成为机构好友";
		eSubType = enmSysMsgSubType_AddOrgFriend;
	}
	else if (strMsgType == "apr_org_buddy")
	{
		os<<"机构( "<<strTmpOrgName<<" )"<<"已与您的机构成为机构好友";
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else if (strMsgType == "cancel_org_buddy")
	{
		os<<"机构( "<<strTmpOrgName <<" )" << "已与您的机构解除机构好友";
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	else
	{
		return;
	}



	//time_t curtime = time(0);
	//tstring strPhone = CFileHelper::GetUserName(strFriend);
	//std::ostringstream os;
	//os<<strSenderNick<<"("<<strPhone.c_str()<<") 已同意添加您为好友";

	CSysMsgAcceptAddFriend* pMsg = new CSysMsgAcceptAddFriend;
	pMsg->eSubType = eSubType;
	pMsg->strFromBare = SYS_NOTIFY_ACCOUNT;
	pMsg->strMsg = os.str().c_str();
	pMsg->uTime = curtime;
	pMsg->strSender= strReqId;
	//pMsg->strSender = strFriend;

	//缓存消息。
	m_listUnReadMsgs.push_back(pMsg);

	//写入数据库
	sSysMsg sTmpMsg;
	sTmpMsg.eSubType = eSubType;
	sTmpMsg.strMsg = os.str().c_str();
	sTmpMsg.strSender = strReqId;
	sTmpMsg.uTime = curtime;
	GetDBTaskModule()->SaveSysMessage(sTmpMsg);

	//扔到主线程去
	if (NULL != g_pMsgWnd)
	{
		g_pMsgWnd->SendMessage(WM_NEWMSG_RECEIVED, (WPARAM)pMsg, (LPARAM)enmMsgType_Sys);
	}
}




