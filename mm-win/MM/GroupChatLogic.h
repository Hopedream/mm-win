#pragma once

#include "RoomMgrEvent_i.h"
#include <map>
#include <list>
#include <set>
#include <Windows.h>

using namespace std;

struct sGroupChatItem
{
	tstring strRoomID;
	tstring strSenderNick;	//默认是手机号！！！
	tstring strSenderAccount;
	tstring strTextContent;
	tstring strPackID;		//群组聊天中的文件资料包的发送
	time_t tSendTime;
	sGroupChatItem()
	{
		tSendTime = 0;
	}
};

typedef list<sGroupChatItem> ListGroupChat;

class CGroupChatLogicEvent
{
public:
	virtual void OnReceveGroupMsg(sGroupChatItem& newMsg){}
};

class CGroupChatLogic : public CRoomManagerEventBase
{
public:
	CGroupChatLogic();
	~CGroupChatLogic();

	void AddHandler(CGroupChatLogicEvent* pHandler);
	void RemoveHandler(CGroupChatLogicEvent* pHandler);

	//CRoomManagerEventBase
	virtual void OnReceveGroupMessage(const std::tstring& strRoomID, const std::tstring& strSenderAccount, 
		const std::tstring& strSenderNick, const std::tstring& strMsg, bool bPrivate);

	bool GetCachedChatList(const tstring& strRoomID, ListGroupChat& chatList);
	void CacheChatData(const sGroupChatItem& oneGroupItem);

	//cache未读消息
	//void AddUnReadGroupMsg(const tstring& strRoomID, tstring& );
	//获取未读消息个数
	int GetUnReadGroupMsgCount(const tstring& strRoomID);
	//清楚未读消息
	void ClearUnReadGroupMsg(const tstring& strRoomID);

	void AddOneUnReadCount(const tstring& strRoomID);

	void RegisterToRoom();

protected:
	std::map<tstring, ListGroupChat> m_mapRoom2ChatList;
	CRITICAL_SECTION m_csGCData;

	set<CGroupChatLogicEvent*> m_setHandlers;

	std::map<tstring, int> m_mapRoom2Unreadcount;
	CRITICAL_SECTION m_csUnRead;
};