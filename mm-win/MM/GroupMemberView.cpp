#include "GroupMemberView.h"
#include "TileLayoutGroupMember.h"
#include "mainDlg.h"
#include "MUCRoomMgr.h"
#include "Controller.h"
#include "Utility.h"
#include <vector>
#include "HttpClient.h"
#include "AddNewGroupMember.h"
#include "AddNewGroupMemberEx.h"
#include "Controller.h"
#include "UserManager.h"
#include "..\Common\Logger.h"
#include <sstream>

CGroupMemberView::CGroupMemberView(CPaintManagerUI* ppm/* = NULL*/)
{
	m_pPaintManager = ppm;
	m_pGroupMemberList = NULL;
	//m_pAddGroupMemWnd  = NULL;
}

CGroupMemberView::~CGroupMemberView()
{

}

void CGroupMemberView::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), "gm_close_button") == 0)
		{
// 			if (NULL != m_hMainWnd)
// 			{
// 				::SendMessage(m_hMainWnd, WM_GROUPMEM_CLOSED, 0, 0);
// 			}
//
//			CController::Instance()->GetRoomManager()->RemoveHandler(this);

//			Close();
			ShowWindow(false);
		}
		//���Ⱥ��Ա
		else if (_tcsicmp(msg.pSender->GetName(), "gm_add_member") == 0)
		{
			//if (NULL  == m_pAddGroupMemWnd)
			//{
				CAddNewGroupMemberEx* pAddGroupMemWnd = new CAddNewGroupMemberEx;
				if (NULL != pAddGroupMemWnd)
				{
					//���÷������ڼӺ��ѵ�ʱ�򣬻���Ҫ��
					
					pAddGroupMemWnd->SetCurrentRoomInfo(m_strRoomID);
					pAddGroupMemWnd->SetShowMode(false);
					pAddGroupMemWnd->CreateWnd();
					pAddGroupMemWnd->Initialize();//��ʼ��һ�Ρ�
					pAddGroupMemWnd->ShowWindow(true);
				}
			//}

			
		}
	}
	else if(_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		//HandleItemClickEvent(msg);		
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		
	}
	else if(_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		int bac=100;
	}
}
//��ʼ������
void CGroupMemberView::InitWindow()
{

}
LRESULT CGroupMemberView::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;
}

CControlUI* CGroupMemberView::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, "ListGroupMember") == 0)
	{
		m_pGroupMemberList = new CTileLayoutGroupMember(m_pPaintManager);
		return m_pGroupMemberList;
	}

	return NULL;
}
bool CGroupMemberView::Initialize(tstring& strCurRoomID)
{
	m_strRoomID = strCurRoomID;

	//��ʼ����Ա�б�
	{
		vector<tstring> vecMemberList;
		if(CController::Instance()->GetRoomManager()->GetRoomMemberList(strCurRoomID, vecMemberList))
		{
			//release memory before remove all nodes.
			m_pGroupMemberList->RemoveAll();

			for (int i=0; i<vecMemberList.size(); i++)
			{
				sFriendListItem oneFriend;
				//�ȴӺ�������ȥ�ҡ�
				if (mainDlg::getInstance()->GetFriendItem(vecMemberList[i], oneFriend))
				{
					
				}
				else
				{
					//��İ��������ȥ�ҡ�
					sUserVCard oneVCard;
					CController::Instance()->GetUserInfoMgr()->GetUserVCard(vecMemberList[i], oneVCard);
					oneFriend.strAccount = oneVCard.strAccount;
					oneFriend.strAvatar = oneVCard.strPhotoPath.empty()? DEF_AVATAR_NOR : oneVCard.strPhotoPath;
					oneFriend.strOrgID = oneVCard.strOrganization;
					oneFriend.strOrgnization = oneVCard.strOrganization;
					oneFriend.strShowName = oneVCard.strUserNickname;
				}

				//��ȡduty���п��ܻ�õ��ǿյġ�
				vector<tstring> vecProperty;
 				if(GetDuty(oneFriend.strOrgID, oneFriend.strAccount, vecProperty))
 				{
 
 				}

				m_pGroupMemberList->AddOneItem(oneFriend, vecProperty);
			}

			CController::Instance()->GetRoomManager()->AddHandler(this);
		}
	}
	

	return true;
}

void CGroupMemberView::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_DIALOG, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

void CGroupMemberView::OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<tstring>& vecMemList)
{
	//ˢ�½����Ա
	if (_tcsicmp(strRoomID.c_str(), m_strRoomID.c_str()) == 0)
	{
		//clear first.
		m_pGroupMemberList->RemoveAll();

		//add new.
		for (int i=0; i<vecMemList.size(); i++)
		{
			sFriendListItem oneFriend;
			//if (mainDlg::getInstance()->GetFriendItem(vecMemList[i], oneFriend))
			//{
			//	vector<tstring> vecProperty;
			//	//�п����� �ҵĺ��� û�л���ID������GetDuty�ͻ�ʧ�ܣ�
			//	//if(GetDuty(oneFriend.strOrgID, oneFriend.strAccount, vecProperty))
			//	//{
			//		m_pGroupMemberList->AddOneItem(oneFriend, vecProperty);	
			//	//}
			//}

			//�ȴӺ�������ȥ�ҡ�
			if (mainDlg::getInstance()->GetFriendItem(vecMemList[i], oneFriend))
			{

			}
			else
			{
				//��İ��������ȥ�ҡ�
				sUserVCard oneVCard;
				CController::Instance()->GetUserInfoMgr()->GetUserVCard(vecMemList[i], oneVCard);
				oneFriend.strAccount = oneVCard.strAccount;
				oneFriend.strAvatar = oneVCard.strPhotoPath.empty()? DEF_AVATAR_NOR : oneVCard.strPhotoPath;
				oneFriend.strOrgID = oneVCard.strOrganization;
				oneFriend.strOrgnization = oneVCard.strOrganization;
				oneFriend.strShowName = oneVCard.strUserNickname;
			}

			//��ȡduty���п��ܻ�õ��ǿյġ�
			vector<tstring> vecProperty;
 			if(GetDuty(oneFriend.strOrgID, oneFriend.strAccount, vecProperty))
 			{
 
 			}

			m_pGroupMemberList->AddOneItem(oneFriend, vecProperty);

		}
	}
}