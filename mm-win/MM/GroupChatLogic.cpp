#include "GroupChatLogic.h"
#include <time.h>
#include "Controller.h"
#include "MUCRoomMgr.h"
#include "..\include\DBTaskModule.h"

CGroupChatLogic::CGroupChatLogic()
{
	::InitializeCriticalSection(&m_csGCData);
	::InitializeCriticalSection(&m_csUnRead);
	CController::Instance()->GetRoomManager()->AddHandler(this);
}

CGroupChatLogic::~CGroupChatLogic()
{
	::DeleteCriticalSection(&m_csGCData);
	::DeleteCriticalSection(&m_csUnRead);
}

void CGroupChatLogic::AddHandler(CGroupChatLogicEvent* pHandler)
{
	if (m_setHandlers.insert(pHandler).second == false)
	{
		
	}
}

void CGroupChatLogic::RemoveHandler(CGroupChatLogicEvent* phandler)
{
	for (set<CGroupChatLogicEvent*>::iterator it = m_setHandlers.begin(); 
		it != m_setHandlers.end(); ++it)
	{
		if (*it == phandler)
		{
			m_setHandlers.erase(it);
			break;
		}
	}
}

void CGroupChatLogic::CacheChatData(const sGroupChatItem& oneGroupItem)
{
	sGroupChatItem oneChatItem;
	oneChatItem.strRoomID = oneGroupItem.strRoomID;
	oneChatItem.strSenderAccount = oneGroupItem.strSenderAccount;
	oneChatItem.strSenderNick = oneGroupItem.strSenderNick;
	oneChatItem.strTextContent = oneGroupItem.strTextContent;
	oneChatItem.tSendTime = oneGroupItem.tSendTime;

	::EnterCriticalSection(&m_csGCData);
	std::map<tstring, ListGroupChat>::iterator itFind = m_mapRoom2ChatList.find(oneGroupItem.strRoomID);
	if ( itFind != m_mapRoom2ChatList.end())
	{
		ListGroupChat& listChatList = itFind->second;
		listChatList.push_back(oneChatItem);
	}
	else
	{
		ListGroupChat listChatList;
		listChatList.push_back(oneChatItem);

		m_mapRoom2ChatList.insert(make_pair(oneChatItem.strRoomID, listChatList));
	}

	::LeaveCriticalSection(&m_csGCData);
}

bool CGroupChatLogic::GetCachedChatList(const tstring& strRoomID, ListGroupChat& chatList)
{
	::EnterCriticalSection(&m_csGCData);
	for(std::map<tstring, ListGroupChat>::iterator it = m_mapRoom2ChatList.begin();
		it != m_mapRoom2ChatList.end(); ++it)
	{
		if(strRoomID == it->first)
		{
			chatList = it->second;
			break;
		}
	}
	::LeaveCriticalSection(&m_csGCData);

	return chatList.size()>0;
}

void CGroupChatLogic::OnReceveGroupMessage(const std::tstring& strRoomID, const std::tstring& strSenderAccount, 
	const std::tstring& strSenderNick, const std::tstring& strMsg, bool bPrivate)
{
	
	sGroupChatItem oneChatItem;
	oneChatItem.strRoomID = strRoomID;
	oneChatItem.strSenderAccount = strSenderNick+"@test-d";//strSenderAccount;
	oneChatItem.strSenderNick = strSenderNick;
	oneChatItem.strTextContent = strMsg;
	oneChatItem.tSendTime = time(0);

	//写入到数据库
	_tGroupChatMessage oneGroupMsg;
	oneGroupMsg.strMsg = oneChatItem.strTextContent;
	oneGroupMsg.m_timeChat = oneChatItem.tSendTime;
	oneGroupMsg.strRoomID = strRoomID;
	oneGroupMsg.strSenderPhone = oneChatItem.strSenderNick;
	oneGroupMsg.strSenderAccount = oneChatItem.strSenderAccount;
	GetDBTaskModule()->SaveGroupChatMsg(strRoomID, oneGroupMsg);


	::EnterCriticalSection(&m_csGCData);
	std::map<tstring, ListGroupChat>::iterator itFind = m_mapRoom2ChatList.find(strRoomID);
	if ( itFind != m_mapRoom2ChatList.end())
	{
		ListGroupChat& listChatList = itFind->second;
		listChatList.push_back(oneChatItem);
	}
	else
	{
		ListGroupChat listChatList;
		listChatList.push_back(oneChatItem);

		m_mapRoom2ChatList.insert(make_pair(strRoomID, listChatList));
	}

	//更新未读消息个数
	::EnterCriticalSection(&m_csUnRead);
	std::map<tstring, int>::iterator itUnread = m_mapRoom2Unreadcount.find(strRoomID);
	if ( itUnread != m_mapRoom2Unreadcount.end())
	{
		int nValue = itUnread->second;
		itUnread->second = ++nValue;
	}
	else
	{
		m_mapRoom2Unreadcount.insert(make_pair(strRoomID, 1));
	}
	::LeaveCriticalSection(&m_csUnRead);

	for (set<CGroupChatLogicEvent*>::iterator it = m_setHandlers.begin();
		it != m_setHandlers.end(); ++it)
	{
		CGroupChatLogicEvent* pHandler = *it;
		if (NULL != pHandler)
		{
			pHandler->OnReceveGroupMsg(oneChatItem);
		}
	}

	::LeaveCriticalSection(&m_csGCData);
	
}

int CGroupChatLogic::GetUnReadGroupMsgCount(const tstring& strRoomID)
{
	int nCount = 0;
	::EnterCriticalSection(&m_csUnRead);
	std::map<tstring, int>::iterator itUnread = m_mapRoom2Unreadcount.find(strRoomID);
	if ( itUnread != m_mapRoom2Unreadcount.end())
	{
		nCount = itUnread->second;
	}
	::LeaveCriticalSection(&m_csUnRead);

	return nCount;
}

void CGroupChatLogic::ClearUnReadGroupMsg(const tstring& strRoomID)
{
	::EnterCriticalSection(&m_csUnRead);
	std::map<tstring, int>::iterator itUnread = m_mapRoom2Unreadcount.find(strRoomID);
	if ( itUnread != m_mapRoom2Unreadcount.end())
	{
		itUnread->second = 0;
	}

	::LeaveCriticalSection(&m_csUnRead);
}

void CGroupChatLogic::AddOneUnReadCount(const tstring& strRoomID)
{
	//更新未读消息个数
	::EnterCriticalSection(&m_csUnRead);
	std::map<tstring, int>::iterator itUnread = m_mapRoom2Unreadcount.find(strRoomID);
	if ( itUnread != m_mapRoom2Unreadcount.end())
	{
		int nValue = itUnread->second;
		itUnread->second = ++nValue;
	}
	else
	{
		m_mapRoom2Unreadcount.insert(make_pair(strRoomID, 1));
	}
	::LeaveCriticalSection(&m_csUnRead);
}

void CGroupChatLogic::RegisterToRoom()
{
	CController::Instance()->GetRoomManager()->AddHandler(this);
}
