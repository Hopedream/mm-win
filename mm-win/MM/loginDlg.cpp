#include "loginDlg.h"
#include "mainDlg.h"
#include "Utility.h"
#include "Controller.h"
#include "HttpClient.h"
#include "MsgBox.h"
#include "UCSNetServiceCallBackInterface.h"
#include "json/json.h"
#include "FileHelper.h"
#include <fstream>
#include <ShellAPI.h>
#include "..\Include\DBTaskModule.h"
#include "..\include\Global_Def.h"
#include "..\include\TrayTaskModule.h"
#include "../Common/Logger.h"
#include "webClient.h"
// #include "MainDialog.h"
// #include "CDialogBuilderCallbackEx.h"

TCHAR  WEB_DOMAIN_URL[MAX_PATH] = {0};
TCHAR  WEB_SERVER_BASE_URL[MAX_PATH] = {0};
TCHAR  WEB_SERVER_SITE_URL[MAX_PATH] = {0};
TCHAR  WEB_GET_PASSWORD_URL[MAX_PATH] = {0};
TCHAR  WEB_WALLET_URL[MAX_PATH] = {0};
TCHAR  WEB_FILE_URL[MAX_PATH] = {0};
TCHAR  WEB_ROOT[MAX_PATH] = {0};
TCHAR  REFER_EXECUTE[MAX_PATH] = {0};
TCHAR  REFER_FACE_DEAL[MAX_PATH] = {0};
TCHAR  REFER_FINANCE[MAX_PATH] = {0};
TCHAR  REFER_MOTOR_TYPE[MAX_PATH] = {0};
TCHAR  REFER_VIN[MAX_PATH] = {0};
TCHAR  SUGGESTIONS_URL[MAX_PATH] = {0};

const TCHAR* const kLoginButtonControlName	= _T("buttonLogin");
const TCHAR* const kUserEditControlName		= _T("editUser");
const TCHAR* const kpasswordEditControlName = _T("editPassword");
const TCHAR* const kbuttonMinControlName	= _T("buttonMinize");
const TCHAR* const kbuttonCloseControlName	= _T("buttonClose");
const TCHAR* const kbuttonInputUserTips		= _T("btnUserTips");
const TCHAR* const kbuttonInputPwdTips		= _T("btnpwdTips");
const TCHAR* const kbuttonRegisterCtrlName	= _T("btnRegister");
const TCHAR* const kOptionRemberPwd			= _T("optionRemberPwd");
const TCHAR* const kOptionAutoLogin			= _T("optionAutoLogin");
const TCHAR* const kButtonGetPwd			= _T("btnGetpwd");

loginDlg::loginDlg()
	: m_bLogining(false)
	, m_bRemeberPwd(false)
	, m_bAutoLogin(false)
	, m_hEvent(NULL)
	, re_connect_request_(false)
	, re_connect_response_(false)
{
	InitUCSNetServiceCallBackInterface();
	strcpy(WEB_DOMAIN_URL , gloox_get_addr("DOMAIN"));
	strcpy(WEB_SERVER_BASE_URL , gloox_get_addr("SERVER_BASE"));
	strcpy(WEB_SERVER_SITE_URL , gloox_get_addr("SERVER_SITE"));
	strcpy(WEB_GET_PASSWORD_URL , gloox_get_addr("GET_PASSWORD"));
	strcpy(WEB_WALLET_URL , gloox_get_addr("WALLET_URL"));
	strcpy(WEB_FILE_URL , gloox_get_addr("FILE_URL"));
	strcpy(WEB_ROOT, gloox_get_addr("WEB_ROOT"));
	strcpy(REFER_EXECUTE, gloox_get_addr("REFER_EXECUTE"));
	strcpy(REFER_FACE_DEAL, gloox_get_addr("REFER_FACE_DEAL"));
	strcpy(REFER_FINANCE, gloox_get_addr("REFER_FINANCE"));	
	strcpy(REFER_MOTOR_TYPE, gloox_get_addr("REFER_MOTOR_TYPE"));
	strcpy(REFER_VIN, gloox_get_addr("REFER_VIN"));
	strcpy(SUGGESTIONS_URL, gloox_get_addr("SUGGESTIONS_URL"));
}

loginDlg::~loginDlg()
{
	GetTrayTaskModule()->Release();
	PostQuitMessage(0);
}


CDuiString loginDlg::GetSkinFile()
{
	return _T("loginxml.xml");
}

CDuiString loginDlg::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("..\\MM\\res\\");
#else
	//todo
	return _T("..\\MM\\res\\");
#endif
}

LRESULT loginDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{

	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

void loginDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void loginDlg::OnExit(TNotifyUI& msg)
{
	Close();
}

void loginDlg::InitWindow()
{
	SetIcon(IDI_IM_LOGO_OFFLINE);
	map<__int64, sLoginUser> mapSorted;
	vector<sLoginUser> vecLogined;
	if(GetDBTaskModule()->GetLoginParams(vecLogined))
	{
		//排序找到最近登录的用户
		for (int i=0; i<vecLogined.size(); i++)
		{
			sLoginUser oneUser = vecLogined[i];
			mapSorted.insert(make_pair(oneUser.timeLogin, oneUser));
		}
		
		map<__int64, sLoginUser>::reverse_iterator rit = mapSorted.rbegin();
		if (rit != mapSorted.rend())
		{
			m_bRemeberPwd = rit->second.bRemPwd;
			m_bAutoLogin  = rit->second.bAutoLogin;
		}
		else
		{
			return;
		}

		//这个用户没有设置记住密码和自动登陆
		if (!m_bRemeberPwd && !m_bAutoLogin)
		{
			return;
		}

		//隐藏button tips在手机编辑框里面的
		CControlUI* pBtnUserTip = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputUserTips));
		if (NULL != pBtnUserTip)
		{
			pBtnUserTip->SetVisible(false);
		}
		//给手机用户名框内设置用户名
		CEditUI* pEditUser = static_cast<CEditUI*>(m_PaintManager.FindControl(kUserEditControlName));
		if (NULL != pEditUser)
		{
			pEditUser->SetText(rit->second.strPhoneNum.c_str());
		}
		
		//隐藏密码框内的button
		CControlUI* pBtnPwdtips = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputPwdTips));
		if (NULL != pBtnPwdtips)
		{
			pBtnPwdtips->SetVisible(false);
		}
		//设置密码：
		CEditUI* pEditPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(kpasswordEditControlName));
		if (NULL != pEditPwd)
		{
			pEditPwd->SetText(rit->second.strPwd.c_str());
			//pEditPwd->SetFocus();
		}


		//仅仅是记住密码
		if (m_bRemeberPwd)
		{
			COptionUI* pOptionRemPwd = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRemberPwd));
			if (NULL != pOptionRemPwd)
			{
				pOptionRemPwd->Selected(true);
			}

			CButtonUI* pBtnLogin = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLoginButtonControlName));
			if (NULL != pBtnLogin)
			{
				pBtnLogin->SetFocus();
			}
		}

		//如果是自动登陆：两个都需要勾选
		if (m_bAutoLogin)
		{
			COptionUI* pOptionRemPwd = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRemberPwd));
			if (NULL != pOptionRemPwd)
			{
				pOptionRemPwd->Selected(true);
			}

			COptionUI* pOptionAutoLogin = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionAutoLogin));
			if(NULL != pOptionAutoLogin)
			{
				pOptionAutoLogin->Selected(true);
			}
		}


		if (m_bAutoLogin)
		{
			PostMessage(WM_AUTO_LOGIN);
		}
	}

	//set tab order
	CControlUI* pBtnClose = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonCloseControlName));
	if (pBtnClose != NULL)
	{
		m_PaintManager.SetFocus(pBtnClose);
	}
}

LRESULT loginDlg::ResponseDefaultKeyEvent(WPARAM wParam, LPARAM lPara, bool& bHandled)
{
	if (wParam == VK_RETURN)
	{
		if (GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0)
		{
			bHandled = TRUE;
			OnBtnLogin();	
		}
		return TRUE;
		//return FALSE;
	}
// 	else if (wParam == VK_ESCAPE)
// 	{
// 		return TRUE;
// 	}
// 	return FALSE;

	bHandled  = FALSE;
	return FALSE;
}

UILIB_RESOURCETYPE loginDlg::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

CControlUI* loginDlg::CreateControl(LPCTSTR pstrClass)
{

	return NULL;
}

LRESULT loginDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_AUTO_LOGIN)
	{
		OnResponseAutoLoginMsg();
	}
	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled)/*0*/;
}

void loginDlg::OnResponseAutoLoginMsg()
{
	OnBtnLogin();
}

bool loginDlg::NeedUpdate()
{
	//记得注销下面这句
	//CWebClient::SetVerison("version={\"version\":\"1.02\"}");
	bool bResult = false;

	TCHAR szVersion[64];
	tstring strAppPath = CFileHelper::GetCurrentAppPath();
	tstring strIniPath = strAppPath + ".\\System.ini";
	GetPrivateProfileString("version","version","",szVersion,64,strIniPath.c_str());

	tstring sCurVersion = szVersion;

	tstring strResponse = CWebClient::GetVersion();
	tstring sServerVersion;

	Json::Reader reader;
	Json::Value root;
	try
	{
		if (reader.parse(strResponse, root))
		{
			sServerVersion = root["version"].asString();
		}
	}
	catch (...)
	{
		CLogger::GetInstance()->PrintErrLog( "parse CWebClient::GetVersion exception: %s", strResponse.c_str());	
	}

	if (sServerVersion.compare(sCurVersion) !=0 )
	{
		bResult = true;
	}

	return bResult;
}

LRESULT loginDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LPCTSTR loginDlg::GetWindowClassName() const
{
	return _T("MMLogin");
}

void loginDlg::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("dbclick")) == 0)
	{
		return;
	}
	else if (_tcsicmp(msg.sType, _T("textchanged")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("editUser")) == 0)
		{
			CEditUI * pEditUser =static_cast<CEditUI*>(m_PaintManager.FindControl("editPassword"));
			if (pEditUser != NULL)
			{
				pEditUser->SetText(_T(""));
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{

		if(_tcsicmp(msg.pSender->GetName(), kbuttonMinControlName) == 0)
		{
			::ShowWindow(*this, SW_MINIMIZE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonCloseControlName) == 0)
		{
			OnExit(msg);
		}
		else if(_tcsicmp(msg.pSender->GetName(), kbuttonRegisterCtrlName) == 0)
		{
			RegisterNewUser();
		}
		//点击了输入用户名编辑框
		else if(_tcsicmp(msg.pSender->GetName(), kbuttonInputUserTips) == 0)
		{
			CControlUI* pBtnUserTip = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputUserTips));
			if (NULL != pBtnUserTip)
			{
				pBtnUserTip->SetVisible(false);
			}

			CEditUI* pEditUser = static_cast<CEditUI*>(m_PaintManager.FindControl(kUserEditControlName));
			if (NULL != pEditUser)
			{
				pEditUser->SetFocus();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonInputPwdTips) == 0)
		{
			CControlUI* pBtnPwdtips = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputPwdTips));
			if (NULL != pBtnPwdtips)
			{
				pBtnPwdtips->SetVisible(false);
			}

			CEditUI* pEditPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(kpasswordEditControlName));
			if (NULL != pEditPwd)
			{
			 	pEditPwd->SetFocus();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kLoginButtonControlName) == 0)
		{
			OnBtnLogin();
		}
		//记住密码
		else if (_tcsicmp(msg.pSender->GetName(), kOptionRemberPwd) == 0)
		{
			bool bSelected = false;
			COptionUI* pOptionRemPwd = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRemberPwd));
			if (NULL != pOptionRemPwd)
			{
				bSelected = pOptionRemPwd->IsSelected();
			}
			m_bRemeberPwd = !bSelected;

			//如果自动登陆也是勾选的，同步取消掉“自动登陆”
			if (!m_bRemeberPwd)
			{
				m_bAutoLogin = m_bRemeberPwd;//false;
				COptionUI* pOptionAuto = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionAutoLogin));
				if (NULL != pOptionAuto)
				{
					pOptionAuto->Selected(m_bRemeberPwd);//false;
				}
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kOptionAutoLogin) == 0)
		{
			bool bSelected = false;
			
 			COptionUI* pOptionAuto = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionAutoLogin));
 			if (NULL != pOptionAuto)
 			{
 				bSelected = pOptionAuto->IsSelected();
 			}
			//如果点击之前是没有勾选的
			if (!bSelected)
			{
				//同步更新忘记密码，如果没有勾选也要勾选上
				m_bRemeberPwd = !bSelected;
				COptionUI* pOptionRemPwd = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRemberPwd));
				if (NULL != pOptionRemPwd)
				{
					pOptionRemPwd->Selected(m_bRemeberPwd);
				}

				m_bAutoLogin = !bSelected;
			}
			//如果之前是勾选的，现在取消了===只取消自己，忘记密码不取消
			else
			{
				m_bAutoLogin = !bSelected;
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kButtonGetPwd) == 0)
		{
			ShellExecute(NULL,_T("open"),WEB_GET_PASSWORD_URL ,NULL,NULL,SW_SHOW);
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btnLoginPasswdMode") == 0)
		{
			SwitchPasswdMode();
		}
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kUserEditControlName) == 0)
		{
			CControlUI* pBtnUsertip = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputUserTips));
			CEditUI* pEditUser = static_cast<CEditUI*>(m_PaintManager.FindControl(kUserEditControlName));

			if (NULL != pBtnUsertip && NULL != pEditUser)
			{
				CDuiString strText = pEditUser->GetText();
				if (strText.IsEmpty())
				{
					strText = _T("请输入手机号");
					pBtnUsertip->SetAttribute("textcolor", "#00A0A0A4");
				}
				else
				{
					//pBtnUsertip->SetAttribute("textcolor", "#FF000000");
				}

				pBtnUsertip->SetText(strText);
				pBtnUsertip->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kpasswordEditControlName) == 0)
		{
			CControlUI* pBtnPwdTips = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputPwdTips));
			CEditUI* pEditPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(kpasswordEditControlName));
			if (NULL != pBtnPwdTips && NULL != pEditPwd)
			{
				CDuiString strText = pEditPwd->GetText();
				if (strText.IsEmpty())
				{
					strText = _T("请输入密码");
					pBtnPwdTips->SetAttribute("textcolor", "#00A0A0A4");
				}
				else
				{
					strText = _T("******");
					//pBtnPwdTips->SetAttribute("textcolor", "#FF000000");
				}

				pBtnPwdTips->SetText(strText);
				pBtnPwdTips->SetVisible(true);
			}
		}
	}

	WindowImplBase::Notify(msg);
}
void loginDlg::SwitchPasswdMode()
{
	CEditUI* pPWEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(kpasswordEditControlName));
	CEditUI* m_pBtnPasswdMode = static_cast<CEditUI*>(m_PaintManager.FindControl("btnLoginPasswdMode"));
	CControlUI* pBtnPwdtips = static_cast<CControlUI*>(m_PaintManager.FindControl(kbuttonInputPwdTips));
	if (pPWEdit->IsPasswordMode())
	{
		pBtnPwdtips->SetVisible(false);
		pPWEdit->SetPasswordMode(false);
		m_pBtnPasswdMode->SetNormalImage("register\\password-on.PNG");
	}
	else 
	{
		pPWEdit->SetPasswordMode(true);
		m_pBtnPasswdMode->SetNormalImage("register\\password-off.PNG");
	}
}
void loginDlg::OnBtnLogin()
{
	CButtonUI* pBtnLogin = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLoginButtonControlName));
	if (NULL != pBtnLogin)
	{
		pBtnLogin->SetEnabled(false);
	}
	CEditUI* pUserEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(kUserEditControlName));
	CEditUI* pPWEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(kpasswordEditControlName));
	CDuiString strUser = pUserEdit->GetText();
	CDuiString strPwd  = pPWEdit->GetText();
	if(strUser.IsEmpty()|| strPwd.IsEmpty())
	{
		CMsgBox* pMsgBox = new CMsgBox("错误",_T("用户名或密码不能为空！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		pBtnLogin->SetEnabled(true);
		return;
	}

	if (!IsLegalAccount(tstring(strUser)))
	{
		CMsgBox* pMsgBox = new CMsgBox("错误",_T("请输入正确的手机号码！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		pBtnLogin->SetEnabled(true);
		return;
	}

	if (!m_bLogining)
	{		
		//禁止同一账号在同一PC多次登录--llzhou
		m_hEvent =  CreateEvent(NULL, false, true, strUser.GetData());
		if (NULL != m_hEvent && ERROR_ALREADY_EXISTS == GetLastError())
		{
			CDuiString strMsg;
			strMsg.Format("您已登录%s，不能重复登录", strUser.GetData());
			CMsgBox::MsgBox(GetHWND(), "登录失败",  strMsg.GetData(), MSGBOX_ICON_WARNING);
			CloseHandle(m_hEvent);
			pBtnLogin->SetEnabled(true);
			return;
		}			

		//保存配置信息到数据库
		GetDBTaskModule()->SaveLoginParams(tstring(strUser.GetData()), tstring(strPwd.GetData()), m_bRemeberPwd, m_bAutoLogin, time(0));

		//登陆
		CController::Instance()->AddEventHanlder(this);
		if(CController::Instance()->InitLogin(tstring(strUser.GetData()),tstring( strPwd.GetData())))
		{
			m_bLogining = true;
			//ConnectUcPaasClient(strUser.GetData());
			CUcPaasClient::Instance()->ConnectUcPaas(strUser.GetData());			
		}
		else
		{			
			if (!m_bLogining)
			{
				CloseHandle(m_hEvent);
				CButtonUI* pBtnLogin = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLoginButtonControlName));
				if (NULL != pBtnLogin)
				{
					pBtnLogin->SetEnabled(true);
				}
			}
		}
	} 
}

LRESULT loginDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_NCLBUTTONDBLCLK)
	{
		if(HTCAPTION==wParam)  
		{  			
			return 0; 
		}
	}
	else if (uMsg == WM_LOGIN_SUCCESS)
	{
		//注意，断线重连也会跳到此界面，但是此时已经不需要new动作了
		//转入主界面
		KillTimer(this->GetHWND(), WM_TIMER_RECONNECT);
		static bool new_login(true);
		if (new_login)
		{
			mainDlg* pmainDlg = new mainDlg();
			if( pmainDlg == NULL ) 
				return 0;
			//CDialogBuilderCallbackEx cb (&pmainDlg->m_PaintManager);
			//pmainDlg->Create(_T("main_dialog.xml"), &cb);
			pmainDlg->Create(NULL, _T("MM1.0"), UI_WNDSTYLE_DIALOG, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
			pmainDlg->CenterWindow();
			::ShowWindow(*pmainDlg, SW_SHOW);
			::ShowWindow(*this, SW_HIDE);


			//CMainDialog * pdlg = new CMainDialog;
			//CDialogBuilderCallbackEx cb (&pdlg->m_pm);
			//pdlg->Create(_T("main_dialog.xml"), &cb);
			//pdlg->CenterWindow();
			//::ShowWindow(*pdlg, SW_SHOW);
			new_login = false;
		}
		else
		{
			mainDlg::getInstance()->ReConnect();
			OutputDebugString("自动重连成功！\r\n");
			CLogger::GetInstance()->PrintLog("reconnect success.", LOG_INFO);
		}
		
		return 0;
	}
	else if(uMsg == WM_LOGIN_FAILED)
	{
		//断线首先先将所有操作屏蔽
		mainDlg::getInstance()->LoseConnection();

		SetIcon(IDI_IM_LOGO_OFFLINE);
		CloseHandle(m_hEvent);
		if (StreamErrorConflict == wParam)
		{			
			if (MSGBOX_IDYES == CMsgBox::MsgBox(NULL, "下线通知", "您的账号在另一台电脑上登录，是否重新登录？", MSGBOX_ICON_ASK, MSGBOX_IDYESNO ))
			{
				char szModule[MAX_PATH];
				memset(szModule, 0, MAX_PATH);
				::GetModuleFileName(NULL, szModule, MAX_PATH);
				ShellExecute(NULL, NULL,  szModule, NULL, NULL, SW_SHOWNORMAL);
			}
			exit(0);
		}
		else
		{
			//自动重连
			if (!m_bLogining)
			{
				SetTimer(this->GetHWND(), WM_TIMER_RECONNECT, 1000, NULL);
//				if (!re_connect_request_)//重连请求已经发出就不能再发
//				{

//				}
// 				//上次发送的重连请求一直没回复，等待一分钟
// 				int wait_times(0);
// 				while (!re_connnect_response_)
// 				{
// 					Sleep(1000);
// 					wait_times++;
// 					if (wait_times > 60)
// 					{
// 						break;
// 					}
// 				}
// 
 				//发送重连请求
// 				re_connnect_response_ = false;
// 				OnBtnLogin();
			}
		}
// 		CMsgBox* pMsgBox = new CMsgBox("错误","登陆失败",MSGBOX_ICON_ERROR);
//  		if (pMsgBox->ShowModal(NULL))
//  		{
//  			if (!m_bLogining)
//  			{
// 				CButtonUI* pBtnLogin = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLoginButtonControlName));
// 				if (NULL != pBtnLogin)
// 				{
// 					pBtnLogin->SetEnabled(true);
// 				}
//  			}
//  		}		
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if ( VK_RETURN == wParam ) 
		{
			if (GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0)
			{
				OnBtnLogin();
			}
		}
		
	}
	else if (uMsg == WM_TIMER)
	{
		if (wParam == WM_TIMER_RECONNECT)
		{
			OnBtnLogin();
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void loginDlg::OnConnected()
{
// 	if (re_connect_request_)
// 	{
// 		re_connnect_response_ = true;
// 	}
	SendMessage(WM_LOGIN_SUCCESS);
}

void loginDlg::OnConnectFail(const int errCode)
{
	m_bLogining = false;
// 	if (re_connect_request_)
// 	{
// 		re_connnect_response_ = true;
// 	}
	CLogger::GetInstance()->PrintLog("Error! Lose connection.!!!", LOG_INFO);
	PostMessage(WM_LOGIN_FAILED, errCode);
}

void loginDlg::RegisterNewUser()
{
	CRegisterAccountDlg* pRegisterDlg = new CRegisterAccountDlg(REG_PAGE_VIEW);		

	pRegisterDlg->CreateWnd(this->GetHWND());
}

void loginDlg::CreateLoginDlg()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 600, 800);
	CenterWindow();
	ShowWindow(true);
}
