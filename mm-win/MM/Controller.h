#pragma once

#include <list>
#include <string>
#include "ControllerEvent_i.h"
#include "..\gloox-1.0.14\src\client.h"
#include "..\gloox-1.0.14\src\\pubsubmanager.h"



/**
* Hold Login Logic.
* If Login Success, Create Session manager, Roster manager...etc logic modules.
*/

class CLogin;
class CMsgSessionManager;
class CUserInfoManager;
class CMUCRoomManager;
class CMUCInvitationListener;
class IMsgCenterModule;
class IDBTaskModule;

using namespace gloox;
using namespace PubSub;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class CController //: public ILoginEvent
{
public:
	~CController();
	static CController* Instance();
	static void DestroyInstance();

	void AddEventHanlder(IControllerEvent* pHanlder);
	void RemoveHandler(IControllerEvent* pHanlder);
	
	bool InitLogin(std::tstring& strPhone, std::tstring& strPwd);
	std::tstring GetToken();

	Client* GetXmppClient();
	CMsgSessionManager* GetMsgSessionMgr();
	CUserInfoManager*   GetUserInfoMgr();
	CMUCRoomManager*    GetRoomManager();
	//Create logic modules if connected success.
	void CreateServices(Client* pClient);
	void NotifyLoginFailed(const StreamError errCode);

private:
	CController();
	static CController* m_pInstance;

protected:
	std::tstring m_strToken;
	std::list<IControllerEvent*> m_listHandlers;
	CLogin* m_pLogin;
	Client* m_pClientAdapter;
	CMsgSessionManager* m_pMsgSessionMgr;
	CUserInfoManager* m_pUsrInfoMgr;
	CMUCRoomManager*  m_pRoomMgr;
	CMUCInvitationListener* m_pMUCInviteListener;
	IMsgCenterModule* m_pMsgCenter;
	IDBTaskModule* m_pLocalDB;
	Manager *m_pPubSubMgr;
};