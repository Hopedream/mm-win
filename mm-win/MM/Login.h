#pragma once


#include "..\gloox-1.0.14\src\client.h"
#include "..\gloox-1.0.14\src\connectionlistener.h"

#include <string>
#include <windows.h>
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
using namespace gloox;


// class ILoginEvent
// {
// public:
// 	virtual void OnLoginSuccess()			= 0;
// 	virtual void OnLoginFailed(int nError)	= 0;
// };

class CLogin : public ConnectionListener
{
public:
	CLogin(std::tstring& strUsrName, std::tstring& strPwd);
	virtual ~CLogin();

	//service 
	//void AddEventHandler(ILoginEvent* pHandler);
	bool LoginToServer();
	bool LogOutFromServer(std::tstring& strMyName);

	//ConnectionListener
	virtual void onConnect();
	virtual void onDisconnect( ConnectionError e );
	virtual bool onTLSConnect( const CertInfo& info );
	
	Client* GetXmppClient()
	{
		return m_pJClient;
	}

protected:
	bool Init();
	static unsigned  __stdcall  SecondThreadFunc( void*pArguments) ; 

private:
	std::tstring m_strUserName;
	std::tstring m_strPwd;
	Client *m_pJClient;//客户端实例对象 
	//ILoginEvent* m_pHandler;
};

