#include "Controller.h"
#include "Login.h"
#include "MsgSessionMgr.h"
#include "UserManager.h"
#include "MUCRoomMgr.h"
#include "MUCInvitationListener.h"
#include "HttpClient.h"
#include "MsgBox.h"
#include "..\include\MsgCenterModule.h"
#include "..\include\TrayTaskModule.h"
#include "..\include\DBTaskModule.h"

#include <sstream>
using namespace std;

CController* CController::m_pInstance = NULL;

CController::CController()
{
	m_pClientAdapter = NULL;
	m_pLogin = NULL;
	m_pMsgSessionMgr = NULL;
	m_pUsrInfoMgr = NULL;
	m_pRoomMgr	  = NULL;
	m_pMsgCenter = NULL;
	m_pLocalDB = NULL;
	m_pMUCInviteListener = NULL;
}

CController* CController::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CController;
	}
	return m_pInstance;
}

void CController::DestroyInstance()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CController::~CController()
{
	if (NULL != m_pLogin)
	{
		delete m_pLogin;
		m_pLogin = NULL;
	}

	if (NULL != m_pMsgSessionMgr)
	{
		delete m_pMsgSessionMgr;
		m_pMsgSessionMgr = NULL;
	}

	if (NULL != m_pUsrInfoMgr)
	{
		delete m_pUsrInfoMgr;
		m_pUsrInfoMgr = NULL;
	}

	if (NULL != m_pMsgCenter)
	{
		delete m_pMsgCenter;
		m_pMsgCenter = NULL;
	}

	if (NULL != m_pLocalDB)
	{
		delete m_pLocalDB;
		m_pLocalDB = NULL;
	}

	if (NULL != m_pRoomMgr)
	{
		delete m_pRoomMgr;
		m_pRoomMgr = NULL;
	}

	if (NULL != m_pMUCInviteListener)
	{
		delete m_pMUCInviteListener;
		m_pMUCInviteListener = NULL;
	}
}

void CController::AddEventHanlder(IControllerEvent* pEvent)
{
	if (NULL != pEvent)
	{
		for (list<IControllerEvent*>::iterator it = m_listHandlers.begin();
			it != m_listHandlers.end(); ++it)
		{
			IControllerEvent* pLocal = *it;
			if (pLocal == pEvent)
			{
				return;
			}
		}

		m_listHandlers.push_back(pEvent);
	}
}

void CController::RemoveHandler(IControllerEvent* pHanlder)
{
	for (list<IControllerEvent*>::iterator it = m_listHandlers.begin();
		it != m_listHandlers.end(); ++it)
	{
		IControllerEvent* pTmp = (IControllerEvent*)(*it);
		if (pHanlder == pTmp)
		{
			m_listHandlers.erase(it);
			break;
		}
	}
}

bool CController::InitLogin(tstring& strPhone, tstring& strPwd)
{
	static int init_times(0);
	init_times++;

	//注册 todo
	if (NULL  != m_pLogin)
	{
		delete m_pLogin;
		m_pLogin = NULL;
	}

	//verify
	ostringstream osPost;
	osPost<<"cell_phone="
		<<strPhone.c_str()
		<<"&"
		<<"passwd="
		<<strPwd.c_str();

	tstring strUrl(WEB_SERVER_SITE_URL);
	strUrl += "verify";
	tstring strPost = osPost.str().c_str();
	tstring strResponse;

	CHttpClient reqClient;
	int nSucc = reqClient.Post(strUrl, strPost, strResponse);

	if (strResponse.empty())
	{
		//只有在第一次连接的时候才会弹出对话框，以后为自动重连，不能弹出对话框
		if (init_times <= 1)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("连接服务器超时！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}		
		return false;
	}

	//if (nSucc == 1)	
	tstring strFind("access_token");
	int index = strResponse.find(strFind);
	if (index == -1)
	{
		//解析错误码："code":"1001"=密码错误，"code":"1003"="用户不存在"
		tstring strErrorPwd("\"code\":\"1001\"");
		tstring strErrorNotExist("\"code\":\"1003\"");
		int nIndex1 = strResponse.find(strErrorPwd);
		int nIndex2 = strResponse.find(strErrorNotExist);

		if (nIndex1 != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("密码错误！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		} 
		else if (nIndex2 != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("账户不存在！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("用户验证失败！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}

		return false;
	}
	index += strFind.length()+3;

	if (!strResponse.empty())
	{
		tstring strToken = strResponse.substr(index, 32/*strResponse.length()-index-2*/);

		//计算token长度
		if (!strToken.empty())
		{
			m_strToken = strToken;
			m_pLogin = new CLogin(strPhone, strPwd);
			if (NULL != m_pLogin)
			{
				return m_pLogin->LoginToServer();
			}
		}
	}

	return false;
}

Client* CController::GetXmppClient()
{
	if (NULL != m_pClientAdapter)
	{
		return m_pClientAdapter;
	}
	return NULL;
}

CUserInfoManager* CController::GetUserInfoMgr()
{
	if (NULL != m_pUsrInfoMgr)
	{
		return m_pUsrInfoMgr;
	}
	
	return NULL;
}

CMUCRoomManager* CController::GetRoomManager()
{
	if (NULL != m_pRoomMgr)
	{
		return m_pRoomMgr;
	}

	return NULL;
}

CMsgSessionManager* CController::GetMsgSessionMgr()
{
	if (NULL != m_pMsgSessionMgr)
	{
		return m_pMsgSessionMgr;
	}

	return NULL;
}

/**
* 连接建立成功了，创建服务，连接通信
*/
void CController::CreateServices(Client* pClient)
{
	m_pClientAdapter = pClient;


	//Init MsgCenter module;
	m_pMsgCenter = GetMsgCenterModule();
	if (NULL == m_pMsgCenter)
	{
		//LogError(...);
	}
	//Init message session module.
	m_pMsgSessionMgr = new CMsgSessionManager(pClient);
	if (NULL != m_pMsgSessionMgr)
	{
		m_pMsgSessionMgr->AddHandler(dynamic_cast<IMsgSessionMgrEvent*>(m_pMsgCenter));
		m_pMsgSessionMgr->Initialize();
	}

	//Init roster module.
	m_pUsrInfoMgr = new CUserInfoManager;
	if (NULL != m_pUsrInfoMgr)
	{
		m_pUsrInfoMgr->Initialize(pClient);
	}
	m_pUsrInfoMgr->AddEventHandler(dynamic_cast<IUserManagerEvent*>(m_pMsgCenter));
	//m_pUsrInfoMgr->AddEventHandler(dynamic_cast<IUserManagerEvent*>(m_pMsgCenter));

	//初始化room manager
	m_pRoomMgr = new CMUCRoomManager;

	m_pMUCInviteListener = new CMUCInvitationListener(m_pClientAdapter);
	m_pClientAdapter->registerMUCInvitationHandler(m_pMUCInviteListener);


	//m_pPubSubMgr = new Manager(m_pClientAdapter);

	//register to controller.
	//初始化本地数据库路径
	m_pLocalDB = GetDBTaskModule();
	m_pLocalDB->Initialize();

	//Notify to observers connected
	for (list<IControllerEvent*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		IControllerEvent* pHandler = (IControllerEvent*)(*it);
		if (NULL != pHandler)
		{
			pHandler->OnConnected();
		}
	}
}

void CController::NotifyLoginFailed(const StreamError errCode)
{
	for (list<IControllerEvent*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		IControllerEvent* pHandler = (IControllerEvent*)(*it);
		if (NULL != pHandler)
		{
			pHandler->OnConnectFail(errCode);
		}
	}
}

std::tstring CController::GetToken()
{
	return m_strToken;
}

////If success login, create logic services and register module events.
//void CController::OnLoginSuccess()
//{
//	//m_pMsgSessionMgr = new CMsgSessionManager()
//}
//
//void OnLoginFailed(int nError)
//{
//
//}