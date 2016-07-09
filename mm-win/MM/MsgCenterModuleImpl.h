#pragma once
#include <set>
#include "..\Include\MsgCenterModule.h"
#include "..\Common\MMDefine.h"
#include "..\include\MsgSessionMgrEvent_i.h"
#include "..\Include\UsrManagerEvent_i.h"
#include "MsgWnd.h"


class CMsgCenterModuleImpl : public IMsgCenterModule
						   , public IMsgSessionMgrEvent
						   , public IMsgWndEvent
						   , public CUserManagerEventBase
{
public:
	CMsgCenterModuleImpl(void);
	~CMsgCenterModuleImpl(void);

	//Service api
	virtual void AddEventHandler(IMsgCenterModuleEvent* pEventHandler);
	virtual void RemoveEventHandler(IMsgCenterModuleEvent* pEventHandler);
	virtual void Release();
	
	virtual int  GetUnreadMsgCount(tstring strFrombare);
	virtual bool doHaveUnReadMsg();
	virtual void GetUnReadTextMsgs(tstring strFrombare, ListMsgs& listAllMsg);
	virtual void GetUnReadSysMsgs(ListMsgs& listAllMsg);
	virtual void ClearUnReadTextMsgs(tstring strFrombare);
	virtual void ClearUnReadSysMsgs();
	virtual bool GetLatestUnReadMsg(tstring& strFrombare, CMsgBase*& pOneMsg);
	virtual void RemoveLatestUnReadMsg(tstring& strFrombare, __int64 uTime);

	//IMsgSessionMgrEvent
	virtual void OnChatMsgReceived(tstring& strSenderAccount, tstring& strChatMsg);
	virtual void OnCmdMsgReceived(tstring& strSender, tstring& strCmd, ECmdType eType);
	virtual void OnGroupChatMsgReceived(tstring& strRoomID, tstring& strSenderPhone, tstring& strText); 

	//IMsgWndEvent
	virtual void OnMessage(UINT uMsg, WPARAM w, LPARAM l);

	//CUserManagerEventBase
	virtual void OnRecvAddFriendReq(tstring& strFriend, const tstring& strMsg=(_T("")), const tstring& strSenderNick=(_T("")));
	virtual void OnRecvAcceptAddFriendReq(tstring& strFriend, const tstring& strSenderNick=(_T("")));
	virtual void OnRecvOrgReq(tstring strFromJid, tstring strFromNick, tstring strOrgId, 
		tstring strOrgName, tstring strMsgType,tstring strReqId, tstring strDestJid, tstring strDestNick);
private:
	typedef std::set<IMsgCenterModuleEvent*> SETEventHandlers;
	SETEventHandlers m_ctrEventHandler;

	//链表缓存文本消息。
	ListMsgs m_listUnReadMsgs;
};