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
	//����MM-PC�ͻ��˵�domain����Resource:hard code��
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
	
	//������roster����ģ�顣
	/*CUserInfoManager::GetInstance()->Initialize(m_pJClient);*/

	//���÷��񣬾�����˼���꣬������д�Ϳ����ˣ���������йأ�
	m_pJClient->disco()->setVersion( "messageTest", /*GLOOX_VERSION*/"1.0.14", "Windows" );              
	m_pJClient->disco()->setIdentity( "client", "bot", "");

	//��������֤����֤����Ķ������ճ������ˡ�
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
	//printf( "�Ͽ�����: %d/n", e );
	//delete( m_pJClient );        
}

///�ø÷�����Ϊʵ��ConnectionListener�������ӿ��еİ�ȫ���ӳɹ��ķ���ʵ�֡�
bool CLogin::onTLSConnect( const CertInfo& info )
{    
	return true;
}


bool CLogin::LogOutFromServer(std::tstring& strMyName)
{
	return true;
}