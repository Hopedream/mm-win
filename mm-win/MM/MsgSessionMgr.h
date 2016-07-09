#pragma once

#include "Login.h"
#include <map>

#include "..\gloox-1.0.14\src\client.h"
#include "..\gloox-1.0.14\src\messagesessionhandler.h"
#include "..\gloox-1.0.14\src\messageeventhandler.h"
#include "..\gloox-1.0.14\src\messageeventfilter.h"
#include "..\gloox-1.0.14\src\chatstatehandler.h"
#include "..\gloox-1.0.14\src\chatstatefilter.h"
#include "..\gloox-1.0.14\src\connectionlistener.h"
#include "..\gloox-1.0.14\src\disco.h"
#include "..\gloox-1.0.14\src\message.h"
#include "..\gloox-1.0.14\src\gloox.h"
#include "..\gloox-1.0.14\src\lastactivity.h"
#include "..\gloox-1.0.14\src\loghandler.h"
#include "..\gloox-1.0.14\src\logsink.h"
#include "..\gloox-1.0.14\src\connectiontcpclient.h"
#include "..\gloox-1.0.14\src\connectionsocks5proxy.h"
#include "..\gloox-1.0.14\src\connectionhttpproxy.h"
#include "..\gloox-1.0.14\src\messagehandler.h"
using namespace gloox;
using namespace std;


#ifndef _WIN32
# include <unistd.h>
#endif

#include <stdio.h>
#include <string>
#include <cstdio> // [s]print[f]

#if defined( WIN32 ) || defined( _WIN32 )
# include <windows.h>
#endif


#include "..\include\MsgSessionMgrEvent_i.h"

/**
* 管理消息Session
*/
class CMsgSessionManager : public MessageSessionHandler
						  ,public MessageEventHandler
					      ,public MessageHandler
						  ,public ChatStateHandler
{
public:
	CMsgSessionManager(Client* pXmppClient);
	virtual ~CMsgSessionManager();

	void AddHandler(IMsgSessionMgrEvent* pHandler);
	void Initialize();
	void SendMsg(tstring strDestAccount, tstring& strMsg);
	//临时协议：发送命令消息====>应该走xmpp 扩展协议。
	void SendCmd(tstring strDestAccount, ECmdType eType, tstring& strParas);

	virtual void handleMessage( const Message& msg, MessageSession * session );
	virtual void handleMessageEvent( const JID& from, MessageEventType event );
	virtual void handleChatState( const JID& from, ChatStateType state );
	virtual void handleMessageSession( MessageSession *session );
	virtual void handleOrgReq(tstring strFromJid, tstring strFromNick, tstring strOrgId, tstring strOrgName, 
		tstring strMsgType, tstring strReqId, tstring strDestJid, tstring strDestNick);
private:
	IMsgSessionMgrEvent* m_pHandler;
	Client *j;

	//MessageSession *m_session;
	//all dest users's session.
	//注：客户端跟server只有一个session，即，connection只有一个，只是附带的from信息会表示从手机端来的还是pc来的。
	//客户都不应该关注这些from信息，应该只认为都是和一个人聊天。server端始终和mm只有一个connection
	typedef map<tstring, MessageSession*> MapAccount2Sessions;
	MapAccount2Sessions m_mapAccount2Sessions;

	typedef map<tstring, MessageEventFilter*> MapMsgEventFilters;
	MapMsgEventFilters m_mapDest2MsgEventFilters;
	typedef map<tstring, ChatStateFilter*> MapChatStateFilters;
	MapChatStateFilters m_mapDest2ChatStateFilters;
};