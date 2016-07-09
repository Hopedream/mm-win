#include "UIUserInfoPage.h"
#include "..\include\DBTaskModule.h"
#include "..\include\Global_Def.h"
#include "StringHelper.h"

const TCHAR* buttonSendMsg_Userpage = "btnSendmsg_Userpage";			//发送消息按钮
const TCHAR* buttonApply_Userpage	= "btnApply_userpage";				//申请建立关系
const TCHAR* buttonClose_Userpage	= "btnClose_userpage";				//关闭界面

const TCHAR* controlAvatar		= "controlImage_userpage";			//头像
const TCHAR* labelUsername		= "labelName_userpage";				//姓名
const TCHAR* textSignature		= "textSign_userpage";				//签名
const TCHAR* labelGender		= "textSex_userpage";				//性别
const TCHAR* labelAge			= "textAge_userpage";				//年龄	
const TCHAR* labelLocation		= "textLocation_userpage";			//地址
const TCHAR* labelPhonenum		= "textPhone_userpage";				//电话
const TCHAR* labelEmail			= "textEmail_userpage";				//email
const TCHAR* labelAddress		= "textAddress_userpage";			//地址


void CUIUserInfoPage::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), buttonClose_Userpage) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		//发送消息，切换到聊天界面。
		else if (_tcsicmp(msg.pSender->GetName(), buttonSendMsg_Userpage) == 0)
		{
			ShowWindow(SW_HIDE);
			char* buf = new char[100];
			memset(buf,0,100);
			strcpy(buf,m_strCurAccount.c_str());
			::SendMessage(m_hWndParent, WM_SHOW_FRIEND_CHATWND, (WPARAM)buf,0);
			delete [] buf;
			//Close();
		}
		//申请建立关系
		else if (_tcsicmp(msg.pSender->GetName(), buttonApply_Userpage) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}

	}
}
//获取用户信息，并初始化到界面
bool CUIUserInfoPage::Initialize(tstring& strAccount, tstring strAvatar)
{

	m_strCurAccount = strAccount;


	//头像
	if (strAvatar.empty())
	{
		strAvatar = DEF_AVATAR_NOR;
	}
	CControlUI* pLogo = static_cast<CControlUI*>(m_PaintManager.FindControl(controlAvatar));
	if (pLogo != NULL)
	{
		pLogo->SetBkImage(strAvatar.c_str());
	}

	sUserVCard oneUser;
	if(GetDBTaskModule()->LoadUserVCard(strAccount, oneUser))
	{
		
		//姓名
		CLabelUI* pUsername = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelUsername));
		if (NULL != pUsername)
		{			
			pUsername->SetText(oneUser.strUserNickname.c_str());
		}
		//签名
		CTextUI* pTextSig = static_cast<CTextUI*>(m_PaintManager.FindControl(textSignature));
		{
			if (NULL != pTextSig)
			{		
				pTextSig->SetText(oneUser.strSignature.c_str());
			}
		}
		//性别
		CLabelUI* pGender = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelGender));
		if (NULL != pGender)
		{
			pGender->SetText(oneUser.strGender.c_str());
		}
		//年龄
		CLabelUI* pAge = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelAge));
		if (NULL != pAge)
		{
			pAge->SetText(oneUser.strAge.c_str());
		}
		//location
		CLabelUI* pLocation = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelLocation));
		if (NULL != pLocation)
		{
			pLocation->SetText(oneUser.strAddr.c_str());
		}
		//电话
		CLabelUI* pPhone = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelPhonenum));
		if (NULL != pPhone)
		{
			pPhone->SetText(oneUser.strPhone.c_str());
		}
		//email
		CLabelUI* pEmail = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelEmail));
		if (NULL != pEmail)
		{
			pEmail->SetText(oneUser.strEmail.c_str());
		}
		//地址
		CLabelUI* pAddr = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelAddress));
		if (NULL != pAddr)
		{
			pAddr->SetText(oneUser.strAddr.c_str());
		}

	}
	
	return true;
}

void CUIUserInfoPage::CreateWnd(HWND& hParentWnd, POINT ptPos)
{
	m_hWndParent = hParentWnd;
	Create(m_hWndParent, _T("UserInfoPage"), UI_WNDSTYLE_CHILD,WS_EX_WINDOWEDGE);
	//::ClientToScreen(m_hWndParent, &ptPos);
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, 
						SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
}

//初始化窗口
void CUIUserInfoPage::InitWindow()
{

}

LRESULT CUIUserInfoPage::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;

	if(m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
	{
		return lRes;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}