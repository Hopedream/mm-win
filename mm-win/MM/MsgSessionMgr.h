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
* ������ϢSession
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
	//��ʱЭ�飺����������Ϣ====>Ӧ����xmpp ��չЭ�顣
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
	//ע���ͻ��˸�serverֻ��һ��session������connectionֻ��һ����ֻ�Ǹ�����from��Ϣ���ʾ���ֻ������Ļ���pc���ġ�
	//�ͻ�����Ӧ�ù�ע��Щfrom��Ϣ��Ӧ��ֻ��Ϊ���Ǻ�һ�������졣server��ʼ�պ�mmֻ��һ��connection
	typedef map<tstring, MessageSession*> MapAccount2Sessions;
	MapAccount2Sessions m_mapAccount2Sessions;

	typedef map<tstring, MessageEventFilter*> MapMsgEventFilters;
	MapMsgEventFilters m_mapDest2MsgEventFilters;
	typedef map<tstring, ChatStateFilter*> MapChatStateFilters;
	MapChatStateFilters m_mapDest2ChatStateFilters;
};