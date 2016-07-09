#include "StdAfx.h"
#include "Login.h"

#include "Controller.h"

#include "..\gloox-1.0.14\src\disco.h"
#include "..\gloox-1.0.14\src\stanza.h"
#include "..\gloox-1.0.14\src\gloox.h"
#include "..\gloox-1.0.14\src\lastactivity.h"
#include "..\gloox-1.0.14\src\connectiontcpclient.h"
#include "UserManager.h"
#include "../include/MsgWndDefine.h"
  
#include   <stdio.h>  
#include   <process.h> 

#include "..\include\Global_Def.h"

CLogin::CLogin(std::tstring& strUsrName, std::tstring& strPwd)
{
	m_pJClient = NULL;
	//m_pHandler = NULL;
	//构建MM-PC客户端的domain，和Resource:hard code：
	m_strUserName = strUsrName + MMPC_DOMAIN;
	m_strPwd = strPwd;

	Init();
}

CLogin::~CLogin()
{
	delete m_pJClient;
	m_pJClient = NULL;
}

bool CLogin::Init()
{
	JID jid( m_strUserName); 
	m_pJClient = new Client( jid, m_strPwd);

	m_pJClient->registerConnectionListener( this ); 
	
	//塞服务到roster管理模块。
	/*CUserInfoManager::GetInstance()->Initialize(m_pJClient);*/

	//设置服务，具体意思不详，照这样写就可以了（与服务发现有关）
	m_pJClient->disco()->setVersion( "messageTest", /*GLOOX_VERSION*/"1.0.14", "Windows" );              
	m_pJClient->disco()->setIdentity( "client", "bot", "");

	//关于数字证书认证方面的东东，照抄就行了。
	StringList ca;
	ca.push_back( "/path/to/cacert.crt" );
	m_pJClient->setCACerts( ca );

	return m_pJClient != NULL;
}
bool CLogin::LoginToServer()
{
	HANDLE   hThread;  
	unsigned   threadID;   

	hThread = (HANDLE)_beginthreadex(NULL,0,SecondThreadFunc,this,0,&threadID);
	CloseHandle(hThread); 
 
	return true;
}

unsigned  __stdcall CLogin::SecondThreadFunc(   void*   pArguments   )  
{  
	CLogin* pThis = (CLogin*)(pArguments);
	if( pThis->m_pJClient->connect( /*false*/) )
	{

	}    

	//_endthreadex(0);  
	return   0;  
}    


void CLogin::onConnect()
{
// 	if (NULL != m_pHandler)
// 	{
// 		m_pHandler->OnLoginSuccess();
// 	}

	CController::Instance()->CreateServices(m_pJClient);
	//g_pMsgWnd->SendMessage(WM_CONNECT_SUCCESS);
}

void CLogin::onDisconnect( ConnectionError e )
{
	StreamError eError = m_pJClient->streamError();

	CController::Instance()->NotifyLoginFailed(eError);
	//printf( "断开连接: %d/n", e );
	//delete( m_pJClient );        
}

///该该方法即为实现ConnectionListener监听器接口中的安全连接成功的方法实现。
bool CLogin::onTLSConnect( const CertInfo& info )
{    
	return true;
}


bool CLogin::LogOutFromServer(std::tstring& strMyName)
{
	return true;
}