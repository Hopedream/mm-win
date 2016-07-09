
#include "RegisterHelper.h"


CRegisterHelper::CRegisterHelper()
:m_pClient(NULL),
 m_pRegHandler(NULL)
{
	m_bRegAction = false;
}
CRegisterHelper::~CRegisterHelper()
{
	if (NULL != m_pClient)
	{
		delete m_pClient;
		m_pClient = NULL;
	}

	if (NULL != m_reg)
	{
		//unregistered myself before delete.
		m_reg->removeRegistrationHandler();
		delete m_reg;
		m_reg = NULL;
	}
}

void CRegisterHelper::AddEventHandler(IRegisterEvent* pHandler)
{
	if (NULL != pHandler)
	{
		m_pRegHandler = pHandler;
	}
}

// be sure to read the API documentation for class Registration!
// README
// to create an account:
// - create the Client instance by passing it the server only, e.g. j = new Client( "example.net" );
// - in onConnect() (or some similar place) call Registration::fetchRegistrationFields()
// - in RegistrationHandler::handleRegistrationFields() set up an RegistrationFields struct
//   with the desired username/password and call Registration::createAccount()

void CRegisterHelper::RegisterAccount(tstring& strServer, tstring& strAccount, tstring& strPassword)
{
	//JID jid( "cheng_usrname@test-d");
	//j = new Client( jid, "hurkhurks" );

	m_bRegAction = true;
	m_strNewAccout = strAccount;
	m_strPwd = strPassword;
	tstring strFull = m_strNewAccout + "@" + strServer + "/abc";

	JID jid(strFull);
	m_pClient = new Client( jid, m_strPwd/*strServer */);
	m_pClient->disableRoster();
	m_pClient->registerConnectionListener( this );

	m_reg = new Registration( m_pClient );
	m_reg->registerRegistrationHandler( this );

	m_pClient->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

	bool bSucc = m_pClient->connect();

	m_reg->removeRegistrationHandler();
	//delete( m_reg );
	//m_reg = NULL;

	//delete( m_pClient );
	//m_pClient = NULL;
}

// to change an account's password:
// - connect to the server as usual
// - in onConnect() or at any later time call Registration::changePassword()
void CRegisterHelper::ChangePassword(tstring& strAccount, tstring& strCurrPwd, tstring& strNewPwd)
{
	if (!strAccount.empty() && !strCurrPwd.empty() && !strNewPwd.empty())
	{
		JID jidTmp(strAccount);
		Client* pClient = new Client(jidTmp, strCurrPwd);
		if (NULL != pClient)
		{
			pClient->registerConnectionListener(this);
			pClient->disco()->setVersion("MM-win","1.0.14","");
			pClient->disco()->setIdentity("MM-pc", "bot", "");

			Registration* pReg = new Registration( m_pClient );
			pReg->registerRegistrationHandler( this );

			//关于数字证书认证方面的东东，照抄就行了。
			StringList ca;
			ca.push_back( "/path/to/cacert.crt" );
			pClient->setCACerts( ca );

			//connect to server.
			bool bConnected =  pClient->connect( /*false*/ );
			if (bConnected)
			{
				// changing password
				pReg->changePassword( pClient->username(), "test" );
				pReg->removeRegistrationHandler();
			}
			//delete pReg;
			//pReg = NULL;
		}

		//delete pClient;
		//pClient = NULL;
	}
}

// to delete an account:
// - connect to the server as usual
// - in onConnect() or at any later time call Registration::removeAccount()
void CRegisterHelper::RemoveAccount(tstring& strAccount, tstring& strPwd)
{
	if (!strAccount.empty() && !strPwd.empty())
	{
		JID jidTmp(strAccount);
		Client* pClient = new Client(jidTmp, strPwd);
		if (NULL != pClient)
		{
			pClient->registerConnectionListener(this);
			pClient->disco()->setVersion("MM-win","1.0.14","");
			pClient->disco()->setIdentity("MM-pc", "bot", "");

			Registration* pReg = new Registration( m_pClient );
			pReg->registerRegistrationHandler( this );

			//关于数字证书认证方面的东东，照抄就行了。
			StringList ca;
			ca.push_back( "/path/to/cacert.crt" );
			pClient->setCACerts( ca );

			//connect to server.
			bool bConnected =  pClient->connect( /*false*/ );
			if (bConnected)
			{
				// unregistering
				pReg->removeAccount();
			}

			//delete pReg;
			//pReg = NULL;

		//	delete pClient;
		//	pClient = NULL;
		}
	}
}

void CRegisterHelper::onConnect()
{
	//注册，修改，删除账号，都会回调到此处。
	if (m_bRegAction)
	{
		//requesting reg fields
		m_reg->fetchRegistrationFields();
		m_bRegAction = !m_bRegAction;
	}
}

void CRegisterHelper::onDisconnect( ConnectionError e ) 
{ 
	//printf( "register_test: disconnected: %d\n", e ); 
}

bool CRegisterHelper::onTLSConnect( const CertInfo& info )
{
	//printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
		/*info.status, info.issuer.c_str(), info.server.c_str(),
		info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
		info.compression.c_str() );*/
	return true;
}

void CRegisterHelper::handleRegistrationFields( const JID& from, int fields, std::tstring instructions )
{
	Registration::fieldEnum;
	//printf( "fields: %d\ninstructions: %s\n", fields, instructions.c_str() );
	RegistrationFields vals;
	vals.username = "test4";/*m_strNewAccout*/;//"cotest2";
	vals.password = m_strPwd;//"test2";
	vals.email = "test3@126.com";
	m_reg->createAccount( fields, vals );
}

void CRegisterHelper::handleRegistrationResult( const JID& from, RegistrationResult result )
{
	//printf( "result: %d\n", result );
	if (NULL != m_pRegHandler)
	{
		if (RegistrationSuccess == result)
		{
			m_pRegHandler->OnRegisterSuccess();	
		}
		else
		{
			m_pRegHandler->OnRegisterFailed((int)result);
		}
	}
	
	m_pClient->disconnect();
}

void CRegisterHelper::handleAlreadyRegistered( const JID& /*from*/ )
{
	//printf( "the account already exists.\n" );
}

void CRegisterHelper::handleDataForm( const JID& /*from*/, const DataForm& /*form*/ )
{
	//printf( "datForm received\n" );
}

void CRegisterHelper::handleOOB( const JID& /*from*/, const OOB& oob )
{
	//printf( "OOB registration requested. %s: %s\n", oob.desc().c_str(), oob.url().c_str() );
}

void CRegisterHelper::handleLog( LogLevel level, LogArea area, const std::tstring& message )
{
	//printf("log: level: %d, area: %d, %s\n", level, area, message.c_str() );
}

