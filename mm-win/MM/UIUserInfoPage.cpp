#include "UIUserInfoPage.h"
#include "..\include\DBTaskModule.h"
#include "..\include\Global_Def.h"
#include "StringHelper.h"

const TCHAR* buttonSendMsg_Userpage = "btnSendmsg_Userpage";			//������Ϣ��ť
const TCHAR* buttonApply_Userpage	= "btnApply_userpage";				//���뽨����ϵ
const TCHAR* buttonClose_Userpage	= "btnClose_userpage";				//�رս���

const TCHAR* controlAvatar		= "controlImage_userpage";			//ͷ��
const TCHAR* labelUsername		= "labelName_userpage";				//����
const TCHAR* textSignature		= "textSign_userpage";				//ǩ��
const TCHAR* labelGender		= "textSex_userpage";				//�Ա�
const TCHAR* labelAge			= "textAge_userpage";				//����	
const TCHAR* labelLocation		= "textLocation_userpage";			//��ַ
const TCHAR* labelPhonenum		= "textPhone_userpage";				//�绰
const TCHAR* labelEmail			= "textEmail_userpage";				//email
const TCHAR* labelAddress		= "textAddress_userpage";			//��ַ


void CUIUserInfoPage::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), buttonClose_Userpage) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		//������Ϣ���л���������档
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
		//���뽨����ϵ
		else if (_tcsicmp(msg.pSender->GetName(), buttonApply_Userpage) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}

	}
}
//��ȡ�û���Ϣ������ʼ��������
bool CUIUserInfoPage::Initialize(tstring& strAccount, tstring strAvatar)
{

	m_strCurAccount = strAccount;


	//ͷ��
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
		
		//����
		CLabelUI* pUsername = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelUsername));
		if (NULL != pUsername)
		{			
			pUsername->SetText(oneUser.strUserNickname.c_str());
		}
		//ǩ��
		CTextUI* pTextSig = static_cast<CTextUI*>(m_PaintManager.FindControl(textSignature));
		{
			if (NULL != pTextSig)
			{		
				pTextSig->SetText(oneUser.strSignature.c_str());
			}
		}
		//�Ա�
		CLabelUI* pGender = static_cast<CLabelUI*>(m_PaintManager.FindControl(labelGender));
		if (NULL != pGender)
		{
			pGender->SetText(oneUser.strGender.c_str());
		}
		//����
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
		//�绰
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
		//��ַ
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

//��ʼ������
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