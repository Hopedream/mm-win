#include "MsgSessionMgr.h"
#include "StringHelper.h"
#include "FileHelper.h"
#include <sstream>

CMsgSessionManager::CMsgSessionManager(Client* pXmppClient) 
	: j(pXmppClient)
	, m_pHandler(NULL)
{

}
CMsgSessionManager::~CMsgSessionManager()
{

}

void CMsgSessionManager::AddHandler(IMsgSessionMgrEvent* pHandler)
{
	if (pHandler != NULL)
	{
		m_pHandler = pHandler;
	}
}

void CMsgSessionManager::Initialize()
{
	j->registerMessageSessionHandler( this, 0 );
	//j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

	//
	// this code connects to a jabber server through a SOCKS5 proxy
	//
	//       ConnectionSOCKS5Proxy* conn = new ConnectionSOCKS5Proxy( j,
	//                                   new ConnectionTCP( j->logInstance(),
	//                                                      "sockshost", 1080 ),
	//                                   j->logInstance(), "example.net" );
	//       conn->setProxyAuth( "socksuser", "sockspwd" );
	//       j->setConnectionImpl( conn );

	//
	// this code connects to a jabber server through a HTTP proxy through a SOCKS5 proxy
	//
	//       ConnectionTCP* conn0 = new ConnectionTCP( j->logInstance(), "old", 1080 );
	//       ConnectionSOCKS5Proxy* conn1 = new ConnectionSOCKS5Proxy( conn0, j->logInstance(), "old", 8080 );
	//       conn1->setProxyAuth( "socksuser", "sockspwd" );
	//       ConnectionHTTPProxy* conn2 = new ConnectionHTTPProxy( j, conn1, j->logInstance(), "jabber.cc" );
	//       conn2->setProxyAuth( "httpuser", "httppwd" );
	//       j->setConnectionImpl( conn2 );


	//if( j->connect( /*false*/ ) )
	//{
	// 			ConnectionError ce = ConnNoError;
	// 			while( ce == ConnNoError )
	// 			{
	// 				ce = j->recv();
	// 			}
	// 			printf( "ce: %d\n", ce );
	//}

	//delete( j );
}

//"#COMMAND#type=fileRequest,packId=xxx"
void CMsgSessionManager::SendCmd(tstring strDestAccount, ECmdType eType, tstring& strParas)
{
	//check if session exist;
	MessageSession* pDestSesion = NULL;
	MapAccount2Sessions::iterator itFind = m_mapAccount2Sessions.find(strDestAccount);
	if (itFind != m_mapAccount2Sessions.end())
	{
		pDestSesion = itFind->second;
	}

	if (NULL == pDestSesion)
	{
		JID jid(strDestAccount);
		pDestSesion = new MessageSession(j, jid);
		if (NULL != pDestSesion)
		{
			pDestSesion->registerMessageHandler(this);
			m_mapAccount2Sessions.insert(make_pair(strDestAccount, pDestSesion));
		}
	}

	if (NULL != pDestSesion)
	{
		pDestSesion->send(strParas.c_str());
	}
}
void CMsgSessionManager::SendMsg(tstring strDestAccount, tstring& strMsg)
{
	//check if session exist;
	MessageSession* pDestSesion = NULL;
	MapAccount2Sessions::iterator itFind = m_mapAccount2Sessions.find(strDestAccount);
	if (itFind != m_mapAccount2Sessions.end())
	{
		pDestSesion = itFind->second;
	}

	if (NULL == pDestSesion)
	{
		JID jid(strDestAccount);
		pDestSesion = new MessageSession(j, jid);
		if (NULL != pDestSesion)
		{
			pDestSesion->registerMessageHandler(this);
			m_mapAccount2Sessions.insert(make_pair(strDestAccount, pDestSesion));
		}
	}

	if (NULL != pDestSesion)
	{
		MapMsgEventFilters::iterator itMsgFiler = m_mapDest2MsgEventFilters.find(strDestAccount);
		if (itMsgFiler != m_mapDest2MsgEventFilters.end())
		{
			MessageEventFilter*pFilter = itMsgFiler->second;
			if (NULL != pFilter)
			{
				pFilter->raiseMessageEvent(MessageEventComposing);
			}
		}

		MapChatStateFilters::iterator itStatefilter = m_mapDest2ChatStateFilters.find(strDestAccount);
		if (itStatefilter != m_mapDest2ChatStateFilters.end())
		{
			ChatStateFilter* pFilter = itStatefilter->second;
			if (NULL != pFilter)
			{
				pFilter->setChatState(ChatStateComposing);
			}
		}

		tstring strOut;
		CChineseCode::GB2312ToUTF_8(strOut, (char*)strMsg.c_str(), strMsg.size());
		pDestSesion->send(/*strMsg*/strOut, "");
	}

}

void CMsgSessionManager::handleMessage( const Message& msg, MessageSession * session )
{
	printf( "type: %d, subject: %s, message: %s, thread id: %s\n", msg.subtype(),
		msg.subject().c_str(), msg.body().c_str(), msg.thread().c_str() );


	tstring receipient = session->target().bare();
	MapAccount2Sessions::iterator itFind = m_mapAccount2Sessions.find(receipient);
	if (itFind == m_mapAccount2Sessions.end())
	{
		//这个应该不会调到，在handleMessageHandle处已经做过这些事情了。
		session->registerMessageHandler(this);
		m_mapAccount2Sessions.insert(make_pair(receipient, session));

		MessageEventFilter* pMsgEventFilter = new MessageEventFilter( session );
		pMsgEventFilter->registerMessageEventHandler( this );
		m_mapDest2MsgEventFilters.insert(make_pair(receipient, pMsgEventFilter));

		pMsgEventFilter->raiseMessageEvent( MessageEventDisplayed );

		ChatStateFilter* pChatStateFilter = new ChatStateFilter( session );
		pChatStateFilter->registerChatStateHandler( this );
		m_mapDest2ChatStateFilters.insert(make_pair(receipient, pChatStateFilter));
	}

	if (NULL != m_pHandler)
	{
		tstring strMsg = msg.body();
		if (!strMsg.empty())
		{
			tstring strOut;
			CChineseCode::UTF_8ToGB2312(strOut, (char*)strMsg.c_str(), strMsg.size());

			if (msg.subtype() == Message::MessageType::Chat)
			{
				//判断消息类型：命令 or 文本
				if (strOut.find("#COMMAND#") != -1)
				{
					//#COMMAND#type=filerequest,packid=xxx
					//1.如果是发送文件消息
					if (strOut.find("fileRequest") != -1 || strOut.find("imgRequest") != -1)
					{
						int index = strOut.find(",");
						if (index > 0)
						{
							int nLeftLen = strOut.length()-index-1;
							tstring sPara = strOut.substr(index+1, nLeftLen);
							m_pHandler->OnCmdMsgReceived(receipient, sPara, enmCmd_FileReq);
						}
					}
					else if (strOut.find("voiceRequest") != -1 )
					{
						int index = strOut.find(",");
						if (index > 0)
						{
							int nLeftLen = strOut.length()-index-1;
							tstring sPara = strOut.substr(index+1, nLeftLen);
							m_pHandler->OnCmdMsgReceived(receipient, sPara, enmCmd_VoiceReq);
						}
					}
					else if (strOut.find("filedownloaded") != -1 )
					{
						m_pHandler->OnChatMsgReceived(receipient, strOut/*strMsg*/);
					}
					else if (strOut.find("filerejected") != -1 )
					{
						m_pHandler->OnChatMsgReceived(receipient, strOut/*strMsg*/);
					}
				}
				else
				{
					m_pHandler->OnChatMsgReceived(receipient, strOut/*strMsg*/);
				}
			}
			else if(msg.subtype() == Message::MessageType::Groupchat)
			{
				tstring strRoomID = msg.from().username();
				tstring strSenderPhone = msg.from().resource();
				tstring strMsg = msg.body().c_str();

				m_pHandler->OnGroupChatMsgReceived(strRoomID, strSenderPhone, strOut/*strMsg*/);
			}
		}
		/*if (receipient == "pubsub.test-d")
		{
			int i = 0;

		}*/
	}
}

void CMsgSessionManager::handleMessageEvent( const JID& from, MessageEventType event )
{
	printf( "received event: %d from: %s\n", event, from.bare().c_str() );
}


void CMsgSessionManager::handleChatState( const JID& from, ChatStateType state )
{
	printf( "received state: %d from: %s\n", state, from.bare().c_str() );
}

void CMsgSessionManager::handleMessageSession( MessageSession *session )
{
	printf( "got new session\n");
	//if (m_session != NULL)
	//{
	//	j->disposeMessageSession( m_session );
	//}

	tstring strFrom = session->target().bare();
	MapAccount2Sessions::iterator itFind = m_mapAccount2Sessions.find(strFrom);
	if (itFind == m_mapAccount2Sessions.end())
	{
		session->registerMessageHandler(this);
		m_mapAccount2Sessions.insert(make_pair(strFrom, session));

		//// this example can handle only one session. so we get rid of the old session
		//m_session = session;
		MessageEventFilter* pMsgEventFilter = new MessageEventFilter( session );
		pMsgEventFilter->registerMessageEventHandler( this );
		m_mapDest2MsgEventFilters.insert(make_pair(strFrom, pMsgEventFilter));

		ChatStateFilter* pChatStateFilter = new ChatStateFilter( session );
		pChatStateFilter->registerChatStateHandler( this );
		m_mapDest2ChatStateFilters.insert(make_pair(strFrom, pChatStateFilter));
	}
}

void CMsgSessionManager::handleOrgReq( tstring strFromJid, tstring strFromNick, tstring strOrgId, 
	tstring strOrgName, tstring strMsgType, tstring strReqId, tstring strDestJid, tstring strDestNick)
{
	if (NULL != m_pHandler)
	{
		m_pHandler->OnRecvOrgReq(strFromJid, strFromNick, strOrgId, strOrgName, strMsgType, strReqId, strDestJid, strDestNick);
	}
}

