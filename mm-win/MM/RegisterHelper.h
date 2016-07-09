#pragma once

/************************************************************************/
/* Functions:
	1):Register account to server.
	2):Change password of current logined account
	3):Remove currently logined account                                                                    
/************************************************************************/

#include "..\gloox-1.0.14\src\client.h"
#include "..\gloox-1.0.14\src\registration.h"
#include "..\gloox-1.0.14\src\connectionlistener.h"
#include "..\gloox-1.0.14\src\loghandler.h"
#include "..\gloox-1.0.14\src\messagehandler.h"
#include "..\gloox-1.0.14\src\message.h"
#include "..\gloox-1.0.14\src\disco.h"

using namespace gloox;
using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class IRegisterEvent
{
public:
	virtual void OnRegisterSuccess() = 0;
	virtual void OnRegisterFailed(int nErrorCode) = 0;
};


class CRegisterHelper : public RegistrationHandler, ConnectionListener, LogHandler
{
public: 
	CRegisterHelper();
	virtual ~CRegisterHelper();

	void AddEventHandler(IRegisterEvent* pHandler);

	void RegisterAccount(tstring& strServer, tstring& strAccount, tstring& strPassword);
	void ChangePassword(tstring& strAccount, tstring& strCurrPwd, tstring& strNewPwd);
	void RemoveAccount(tstring& strAccount, tstring& strPwd);
	

	//ConnectionListener
	virtual void onConnect();
	virtual void onDisconnect( ConnectionError e );
	virtual bool onTLSConnect( const CertInfo& info );

	//RegistrationHandler
	virtual void handleRegistrationFields( const JID& /*from*/, int fields, std::tstring instructions );
	virtual void handleRegistrationResult( const JID& /*from*/, RegistrationResult result );
	virtual void handleAlreadyRegistered( const JID& /*from*/ );
	virtual void handleDataForm( const JID& /*from*/, const DataForm& /*form*/ );
	virtual void handleOOB( const JID& /*from*/, const OOB& oob );

	//LogHandler
	virtual void handleLog( LogLevel level, LogArea area, const std::tstring& message );

private:
	IRegisterEvent* m_pRegHandler;
	Client* m_pClient;
	Registration *m_reg;

	bool m_bRegAction;
	tstring m_strNewAccout;
	tstring m_strPwd;
};