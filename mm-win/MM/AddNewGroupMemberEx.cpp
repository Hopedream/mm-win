#include "AddNewGroupMemberEx.h"
#include "TileLayoutLocalParticipant.h"

#include "MUCRoomMgr.h"
#include "Controller.h"
#include "UserManager.h"
#include "MsgBox.h"
#include "Utility.h"
#include "FileHelper.h"
#include "..\Include\Global_Def.h"
#include "mainDlg.h"//��Ҫ�ع���Ӧ�ò���Ҫ����mainDlg����������ص�ҵ����롣
#include <sstream>
#include "..\json\json.h"
#include "StringHelper.h"
#include "..\Common\Logger.h"


#include "mainDlg.h"


#define MW_TIMER_TOPWND 2022

using namespace std;

	
CAddNewGroupMemberEx::CAddNewGroupMemberEx(CPaintManagerUI* ppm /*= NULL*/)
{
	m_pPaintManager = ppm;
	m_pLocalParticipant = NULL;
	
	m_bAskPrice = false;
	m_bCreateNewRoom = true;
}

CAddNewGroupMemberEx::~CAddNewGroupMemberEx()
{
	
}

void CAddNewGroupMemberEx::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), "btn_new_proj_step2_close") == 0)
		{
		

			Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "local_participant_option") == 0)
		{
			
		}
		//�����ѡ��ĺ��ѽ����������Ѿ��ڵľͲ�����ˡ�
		else if (_tcsicmp(msg.pSender->GetName(), "btn_add_newMember") == 0)
		{
 			OnAddNewGroupMember();
		}
	}

}


bool CAddNewGroupMemberEx::Initialize()
{
	InitialLocalParticipant();

	//��ǰ�����Ⱥ��Ա�Ľ��棬��Ҫ��ʼ��ѡ����Щ�Ѿ���Ⱥ��Ա��Item
	if (!m_strRoomIDTobeAddNewMember.empty())
	{
		vector<tstring> vecMembers;
		if(CController::Instance()->GetRoomManager()->GetRoomMemberList(m_strRoomIDTobeAddNewMember, vecMembers))
		{
			int nSize = vecMembers.size();
			for (int i=0; i<nSize; i++)
			{
				tstring& strAccount = vecMembers[i];
				m_pLocalParticipant->InitSelectState(strAccount);
			}
		}

	}

	return true;
}

void CAddNewGroupMemberEx::InitialLocalParticipant()
{
	//����һ����֯���棬�г���֯�ڵ����г�Ա
	if (mainDlg::getInstance()->GetGroupMembersOfMyself(m_vecMyGroupMembser))
	{
		int nSize = m_vecMyGroupMembser.size();
		for (int i=0; i<nSize; i++)
		{
			sFriendListItem* pOneFriend = m_vecMyGroupMembser[i];
			
			vector<tstring> vecProperty;
 			if(GetDuty(pOneFriend->strOrgID, pOneFriend->strAccount, vecProperty))
 			{
 
 			}

			m_pLocalParticipant->AddOneItem(pOneFriend, vecProperty);
		}
	}
	//ֻ����һ���ˣ�
	else
	{
		if (m_vecMyGroupMembser.size() == 1)
		{
			sFriendListItem* pMySelf = m_vecMyGroupMembser[0];
			vector<tstring> vecProperty;
 			if (GetDuty(pMySelf->strOrgID, pMySelf->strAccount, vecProperty))
 			{
 
 			}
			m_pLocalParticipant->AddOneItem(pMySelf, vecProperty);
		}
	}
}

void CAddNewGroupMemberEx::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//��ʼ������
void CAddNewGroupMemberEx::InitWindow()
{
	CControlUI* pBtnSend2Server = static_cast<CControlUI*>(m_PaintManager.FindControl("btn_send_toserver"));
	CControlUI* pBtnAddNewMembr = static_cast<CControlUI*>(m_PaintManager.FindControl("btn_add_newMember"));

	if (NULL != pBtnSend2Server && NULL != pBtnAddNewMembr)
	{
		if (m_bCreateNewRoom)
		{
			pBtnSend2Server->SetVisible(true);
			pBtnAddNewMembr->SetVisible(false);
		}
		else
		{
			pBtnSend2Server->SetVisible(false);
			pBtnAddNewMembr->SetVisible(true);
		}
	}
}

CControlUI* CAddNewGroupMemberEx::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, "ListLocalParticipant") == 0)
	{
		m_pLocalParticipant = new CTileLayoutLocalParticipant(m_pPaintManager);
		return m_pLocalParticipant;
	}

	return NULL;
}

void CAddNewGroupMemberEx::SetResProperty(bool bAskPrice, tstring& strProjectID, tstring& strProjName)
{
	m_bAskPrice = bAskPrice;
	m_strCurrProjectID = strProjectID;
	m_strCurrProjName = strProjName;
}

LRESULT CAddNewGroupMemberEx::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//
	if (WM_TIMER == uMsg && wParam == MW_TIMER_TOPWND)
	{
		::SetWindowPos(this->GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		KillTimer(this->GetHWND(), MW_TIMER_TOPWND);
	}

	bHandled = FALSE;
	return 0;
}

void CAddNewGroupMemberEx::SetForegroundWnd()
{
	::SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetTimer(this->GetHWND(), MW_TIMER_TOPWND, 50, NULL);
}

bool CAddNewGroupMemberEx::GetDefaultAccountByOrgID(tstring& strOrgID, tstring& strAccount)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-default?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += strOrgID;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		//todo
		strAccount = strHttpResponse.substr(strHttpResponse.find("\"cell_phone\":\"") + 14, 
			strHttpResponse.find("\",\"") - (strHttpResponse.find("\"cell_phone\":\"") + 14));
		strAccount += MMPC_ACCOUNT;
		return true;
	}
}

//
void CAddNewGroupMemberEx::OnAddNewGroupMember()
{
	//�ǿյ�roomId
	if (!m_strRoomIDTobeAddNewMember.empty())
	{
		vector<tstring> vecLocalAccounts;
		//����������
		m_pLocalParticipant->GetLocalParticipant(vecLocalAccounts);
		if (vecLocalAccounts.size()==0)
		{
			CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ�񱾷������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}


		//�޳��Ѿ���Ⱥ����ĳ�Ա��
		vector<tstring> vecNewMem;
		vector<tstring> vecAlreadyExist;
		if(!CController::Instance()->GetRoomManager()->GetRoomMemberList(m_strRoomIDTobeAddNewMember, vecAlreadyExist))
		{
			CMsgBox* pMsgBox = new CMsgBox("����",_T("��ȡ��ǰȺ��Աʧ�ܣ�"),MSGBOX_ICON_WARNING, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}

		for (vector<tstring>::iterator it = vecLocalAccounts.begin(); it != vecLocalAccounts.end(); ++it)
		{
			bool bExist = false;
			tstring& strNewMember = *it;
			for (vector<tstring>::iterator itChild = vecAlreadyExist.begin(); itChild != vecAlreadyExist.end(); ++itChild)
			{
				//����Ѿ����ˣ��Ͳ����
				if (_tcsicmp(strNewMember.c_str(), itChild->c_str()) == 0)
				{
					bExist = true;
					break;
				}
			}

			if (!bExist)
			{
				vecNewMem.push_back(strNewMember);
			}
		}

		if (vecNewMem.size()>0)
		{
			CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
			if (NULL != pRoomMgr)
			{
				pRoomMgr->AddRoomNewMember(m_strRoomIDTobeAddNewMember, vecNewMem);
				
				//�����͹رմ���
				PostMessage(WM_CLOSE);
			}
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("����",_T("����ǰѡ����ϵ���Ѿ����ڸ�Ⱥ�������ˣ�"),MSGBOX_ICON_WARNING, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}
	}
}