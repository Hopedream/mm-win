#include "AddNewGroupMember.h"
#include "TileLayoutLocalParticipant.h"
#include "UIOrgMembersList.h"
#include "TileLayoutDestOrgList.h"
#include "TileLayoutDestEmployeeList.h"
#include "TileLayoutDestEmployeeSelectedList.h"

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

#include <sstream>


#include "mainDlg.h"


#define MW_TIMER_TOPWND 2022

using namespace std;

	
CAddNewGroupMember::CAddNewGroupMember(CPaintManagerUI* ppm /*= NULL*/)
{
	//m_hMainWnd = NULL;
	m_pPaintManager = ppm;
	m_pLocalParticipant = NULL;
	//m_pOrgMemberList	= NULL;
	m_pDestOrgList		= NULL;
	m_pOppositeEmployeList = NULL;
	m_pOppositeSelectedEmployeeList = NULL;
	m_bAskPrice = false;
	m_bCreateNewRoom = true;
}

CAddNewGroupMember::~CAddNewGroupMember()
{
	CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
	if (NULL != pRoomMgr)
	{
		pRoomMgr->RemoveHandler(this);
	}
}

void CAddNewGroupMember::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), "btn_new_proj_step2_close") == 0)
		{
			CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
			if (NULL != pRoomMgr)
			{
				pRoomMgr->RemoveHandler(this);
			}

			Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "local_participant_option") == 0)
		{
			////ע�⣺��ȡ���ǵ��֮ǰ��״̬������
			//bool bSelected = ((COptionUI*)(msg.pSender))->IsSelected();

			//CContainerUI* pContainer = (CContainerUI*)msg.pSender->GetParent();
			//if (NULL != pContainer)
			//{
			//	sFriendListItem* pFriend = (sFriendListItem*)pContainer->GetTag();
			//	if (NULL != pFriend)
			//	{
			//		//֮ǰ��ѡ�еģ�����ȡ����==>�ӱ�ѡ���б���ɾ��
			//		if (bSelected)
			//		{
			//			m_pOppositeSelectedEmployeeList->RemoveOneItem(pFriend->nItemID);
			//		}
			//		//֮ǰ��ȡ���ģ�����ѡ����==>���ӵ���ѡ���б�
			//		else
			//		{
			//			m_pOppositeSelectedEmployeeList->AddOneItem(pFriend);
			//		}
			//	}
			//}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "option_oppo_employe_selected") == 0)
		{
			//�����ǰ����ѯ�۵����Ͳ�����ѡ��ͬ��֯�ġ�
			bool bSelected = ((COptionUI*)(msg.pSender))->IsSelected();
			CContainerUI* pContainer = (CContainerUI*)msg.pSender->GetParent();
			if (NULL != pContainer)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pContainer->GetTag();
				if (NULL != pFriend)
				{
					if (bSelected)
					{
						m_pOppositeSelectedEmployeeList->RemoveOneItem(pFriend->strAccount);
					}
					else
					{
						tstring strErrorMsg;
						if (IsAllowedToSelected(pFriend->strOrgID, strErrorMsg))
						{
							vector<tstring> vecDuty;
 							if (GetDuty(pFriend->strOrgID, pFriend->strAccount, vecDuty))
 							{
 
 							}
							//by zyy test
							m_pOppositeSelectedEmployeeList->AddOneItem(pFriend, vecDuty);
						}
						else
						{
							CMsgBox* pMsgBox = new CMsgBox("ѡ�������",strErrorMsg.c_str(), MSGBOX_ICON_ERROR, MSGBOX_IDOK);
							pMsgBox->ShowModal();

							char* pBuf = new char[256];
							_tcscpy(pBuf, pFriend->strAccount.c_str());
							PostMessage(WM_UNSELECT_CHECKBOX, (WPARAM)pBuf);
							//((COptionUI*)msg.pSender)->Selected(false);
							return;			
						}
					}
				}
			}}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_newproj_select_employee") == 0)
		{
			OnParticipantSelected();
		}
		//ɾ����һ����ѡ�����ߣ�ͬ�����µ�m_pLocalParticipantΪ�ǹ�ѡ״̬
		else if (_tcsicmp(msg.pSender->GetName(), "btn_remove_selected_employee") == 0)
		{
			//1.����ѡListɾ����
			tstring strAccount;
			CContainerUI* pContainer = (CContainerUI*)msg.pSender->GetParent();
			if (NULL != pContainer)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pContainer->GetTag();
				strAccount = pFriend->strAccount;
// 				delete pFriend;
// 				pFriend = NULL;
// 				m_pOppositeSelectedEmployeeList->Remove(pContainer);
				m_pOppositeSelectedEmployeeList->RemoveOneItem(strAccount);
			}

			//2.ͬ������
			m_pOppositeEmployeList->DeSelectItem(strAccount);
		}
		//����һ��room
		else if (_tcsicmp(msg.pSender->GetName(), "btn_send_toserver") == 0)
		{
			OnPreCreateGroupRoom();
		}
		//�����ѡ��ĺ��ѽ����������Ѿ��ڵľͲ�����ˡ�
		else if (_tcsicmp(msg.pSender->GetName(), "btn_add_newMember") == 0)
		{
			OnAddNewGroupMember();
		}
	}
	else if(_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		//HandleItemClickEvent(msg);		
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		HandleListItemSelected(msg);
	}
	else if(_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		int bac=100;
	}
}

void CAddNewGroupMember::HandleListItemSelected(TNotifyUI& msg )
{
	//���֮ǰorg�µĳ�Ա�б�
	if (NULL != m_pOppositeEmployeList)
	{
		int nCount = m_pOppositeEmployeList->GetCount();
		if (nCount>0)
		{
			for (int i=0; i<nCount; i++)
			{
				sFriendListItem* pFriend = (sFriendListItem*)m_pOppositeEmployeList->GetTag();
				if (NULL != pFriend)
				{
					delete pFriend;
					pFriend = NULL;
				}
			}

			m_pOppositeEmployeList->RemoveAll();
		}
	}

	//������֯�µĳ�Ա���뵽���б�
	int nSelectedItem = m_pDestOrgList->GetCurSel();
	if (nSelectedItem != -1)
	{
		CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pDestOrgList->GetItemAt(nSelectedItem));
		if (pItem != NULL)
		{
			char * org_id = (char*)pItem->GetTag();
			std::tstring strResult;
			strResult = org_id;

			vector<sFriendListItem> vecMembers;
			if(mainDlg::getInstance()->GetMembersByOrgID(strResult, vecMembers))
			{
				tstring strDefaultAccount;
				if(GetDefaultAccountByOrgID(strResult, strDefaultAccount))
				{
				}

				for (int i=0; i<vecMembers.size(); i++)
				{
					sFriendListItem& oneFriend = vecMembers[i];
					vector<tstring> vecProperty;
 					if (GetDuty(oneFriend.strOrgID, oneFriend.strAccount, vecProperty))
 					{
 					}
					bool bDefault = _tcsicmp(oneFriend.strAccount.c_str(), strDefaultAccount.c_str()) == 0;


					m_pOppositeEmployeList->AddOneItem(oneFriend, vecProperty, bDefault);

					//����Ѿ�������ӽ��������棬�͹�ѡ�ϡ�
					if (m_pOppositeSelectedEmployeeList->CheckIfExist(oneFriend.strAccount))
					{
						m_pOppositeEmployeList->SetItemSeleted(oneFriend.strAccount);
					}
				}
			}
			//��ǰ���ǿյġ�
			else
			{
				int abc=100;
			}
		}
	}
}

void CAddNewGroupMember::HandleItemClickEvent( TNotifyUI& msg )
{
}

bool CAddNewGroupMember::Initialize()
{
	InitialLocalParticipant();

	InitialDestOrgList();

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
				
				//���ڶԷ��ģ�����Ҫadditem
				//m_pOppositeEmployeList->SetItemSeleted(strAccount);
			}
		}

	}

	return true;
}

void CAddNewGroupMember::InitialLocalParticipant()
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
void CAddNewGroupMember::InitialDestOrgList()
{
	CUserInfoManager* pUserManager = CController::Instance()->GetUserInfoMgr();
	if (NULL != pUserManager)
	{
		MapAllRosters listUserInfos;
		if(!pUserManager->GetAllRosters(listUserInfos))
		{
			return;
		}	

		//1.cache�����б����Ϣ��
		map<tstring,tstring> mapOrgID2Orgname;
		mapOrgID2Orgname.insert(make_pair("-1", "�ҵĺ���"));
		for (MapAllRosters::iterator iter = listUserInfos.begin(); iter !=listUserInfos.end(); iter++)
		{
			map<tstring,tstring>::iterator itFind = mapOrgID2Orgname.find(iter->second.strOrgID);
			if ( itFind == mapOrgID2Orgname.end())
			{
				mapOrgID2Orgname.insert(make_pair(iter->second.strOrgID, iter->second.strOrgnization));
			}
		}

		//���뵽org�б�
		tstring strMyOrgName;
		tstring strMyOrgID;
		bool bInOrg = false;//���������֯���棬����Ҫ���˵������ڵ���֯����������ڡ��ҵĺ��ѡ�
		if(pUserManager->GetMyOrganization(strMyOrgName, strMyOrgID))
		{
			bInOrg = _tcsicmp(strMyOrgID.c_str(), DEF_ORGID_MYFRIEND)!=0;
		}

		for (map<tstring, tstring>::iterator it2 = mapOrgID2Orgname.begin();
			it2 != mapOrgID2Orgname.end(); ++it2)
		{
			//����orgID��ȡ org ��avatar;
			tstring strOrgAvatar = DEF_AVATAR_NOR;
			
			tstring strOrgId = it2->first;
			int nOrgID = _tcstoul(strOrgId.c_str(), NULL, 10);

			int nOnlineCount = 0;
			vector<sFriendListItem> vecMembers;
			if(mainDlg::getInstance()->GetMembersByOrgID(strOrgId, vecMembers))
			{
				for (vector<sFriendListItem>::iterator it = vecMembers.begin();
					it != vecMembers.end(); ++it)
				{
					if (it->eState == enmState_Online || it->eState == enmState_Busy)
					{
						nOnlineCount++;
					}
				}
			}
			ostringstream oss;
			oss<<nOnlineCount<<"\/"<<vecMembers.size();
			tstring strOnlineMembers = oss.str();
			
			if (bInOrg)
			{
				//������ҵ���֯���Ͳ�����
				if (_tcsicmp(strOrgId.c_str(), strMyOrgID.c_str()) == 0)
				{
					continue;
				}
			}
			m_pDestOrgList->AddOneItem(strOrgId, strOrgAvatar, it2->second, strOnlineMembers);
		}
		
	}
}

void CAddNewGroupMember::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//��ʼ������
void CAddNewGroupMember::InitWindow()
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

CControlUI* CAddNewGroupMember::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, "ListLocalParticipant") == 0)
	{
		m_pLocalParticipant = new CTileLayoutLocalParticipant(m_pPaintManager);
		return m_pLocalParticipant;
	}
	else if (_tcsicmp(pstrClass, "ListOrgnization") == 0)
	{
		m_pDestOrgList = new CTileLayoutDestOrgList(m_pPaintManager);
		return m_pDestOrgList;
	}
	else if (_tcsicmp(pstrClass, "ListOppositeUnSelectedEmployee") == 0)
	{
		m_pOppositeEmployeList = new CTileLayoutDestEmployeeList(m_pPaintManager);
		return m_pOppositeEmployeList;
	}
	else if (_tcsicmp(pstrClass, "ListOppositeSelectedEmployee") == 0)
	{
		m_pOppositeSelectedEmployeeList = new CTileLayoutDestEmployeeSelectedList(m_pPaintManager);
		return m_pOppositeSelectedEmployeeList;
	}
	else if (_tcsicmp(pstrClass, "ListGroupMember") == 0)
	{

	}

	return NULL;
}

void CAddNewGroupMember::SetResProperty(bool bAskPrice, tstring& strProjectID, tstring& strProjName)
{
	m_bAskPrice = bAskPrice;
	m_strCurrProjectID = strProjectID;
	m_strCurrProjName = strProjName;
}

bool CAddNewGroupMember::IsAllowedToSelected(tstring& strOrgID, tstring& strErrorMsg)
{
	//�����ѯ�۵������Զ�ѡ����ò��ֻ��ѡ���������ҵ�OrgID
	bool bAllowed = false;
	vector<tstring> vecOrgID;
	m_pOppositeSelectedEmployeeList->GetSelectedOrgIDs(vecOrgID);

	if (vecOrgID.size()>0)//��ǰ�Ѿ��и�������ѡ����
	{
		if (_tcsicmp(strOrgID.c_str(), DEF_ORGID_MYFRIEND) == 0 &&//��ǰ��ѡ�еĻ����ǡ��ҵĺ��ѡ�
			_tcsicmp(vecOrgID[0].c_str(), DEF_ORGID_MYFRIEND ) == 0)//��ǰ�Ѿ���һ��"�ҵĺ���"�ĳ�Ա��ѡ����
		{
			strErrorMsg = "ֻ��ѡ��һ�� �ҵĺ��� ����ҵ��ͨ��";
			return false;
		}
	}

	if (m_bAskPrice)
	{
 		if (vecOrgID.size() == 0)
 		{
 			bAllowed = true;
 		}
 		else if (vecOrgID.size() > 0)
		{
			bool bAlreadyExist = false;
			for (int i=0; i<vecOrgID.size(); i++)
			{
				if (_tcsicmp(strOrgID.c_str(), vecOrgID[i].c_str()) == 0)
				{
					bAlreadyExist = true;
					break;
				}
			}

			if (bAlreadyExist)
			{
				strErrorMsg  ="һ������ֻ��ѡ��һ�������˽���Ѱ���ۣ�";
			}
			else
			{
				if (vecOrgID.size() == 3)
				{
					strErrorMsg = "�����ֻ�ܺ����һ���Ѱ���ۣ�";
				}
			}
		}
	}
	else
	{
		if (vecOrgID.size() == 0)
		{
			bAllowed = true;
		}
		else if (vecOrgID.size() == 1)
		{
			if (_tcsicmp(strOrgID.c_str(), vecOrgID[0].c_str()) == 0)
			{
				bAllowed = true;
			}
			else
			{
				strErrorMsg = "��ѡ��Ĳ����˱�����ͬһ������";
			}
		}
	}

	return bAllowed;
}

LRESULT CAddNewGroupMember::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//
	if (uMsg == WM_UNSELECT_CHECKBOX)
	{
		char* pBuf = (char*)wParam;
		if (NULL != pBuf)
		{
			tstring strAccount = pBuf;
			m_pOppositeEmployeList->DeSelectItem(strAccount);
			delete [] pBuf;
		}
		
	}
	else if (WM_TIMER == uMsg && wParam == MW_TIMER_TOPWND)
	{
		::SetWindowPos(this->GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		KillTimer(this->GetHWND(), MW_TIMER_TOPWND);
	}

	bHandled = FALSE;
	return 0;
}

void CAddNewGroupMember::SetForegroundWnd()
{
	::SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetTimer(this->GetHWND(), MW_TIMER_TOPWND, 50, NULL);
}

void CAddNewGroupMember::OnParticipantSelected()
{
// 	if (NULL != m_pOrgMemberList)
// 	{
// 		int nCount = m_pOrgMemberList->GetCount();
// 		for (int i=0; i<nCount; i++)
// 		{
// 			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pOrgMemberList->GetItemAt(i));
// 			if (pItem != NULL)
// 			{
// 				Node* node  = (Node*)pItem->GetTag();
// 				if (!node->folder())
// 				{
// 					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pItem, "option_oppo_employe_selected"));
// 					if (NULL != pOption)
// 					{
// 						bool bSelected = pOption->IsSelected();
// 						if (bSelected)
// 						{
// 							sFriendListItem oneFriend;
// 
// 							oneFriend.nItemID = node->data().value;
// 							//�ӿؼ��ϻ�ȡ�ַ�����
// 							CLabelUI* pShowName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pItem, "label_oppo_unselect_name"));
// 							CControlUI* pAvatar = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pItem, "control_oppo_unselect_avatar"));
// 							if (NULL != pShowName)
// 							{
// 								oneFriend.strShowName = pShowName->GetText();
// 							}
// 							if (NULL != pAvatar)
// 							{
// 								oneFriend.strAvatar = pAvatar->GetBkImage();
// 							}
// 							
// 							oneFriend.strAccount = GetAccountByItemID(node->data().value);
// 
// 							m_pOppositeSelectedEmployeeList->AddOneItem(&oneFriend);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
}

bool CAddNewGroupMember::GetDefaultAccountByOrgID(tstring& strOrgID, tstring& strAccount)
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

void CAddNewGroupMember::OnPreCreateGroupRoom()
{
	vector<tstring> vecLocalAccounts;
	//����������
	m_pLocalParticipant->GetLocalParticipant(vecLocalAccounts);
	if (vecLocalAccounts.size()==0)
	{
		CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ�񡾱����������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}
	else
	{
		bool find_myself(false);
		tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
		for (int i=0;i!=vecLocalAccounts.size();i++)
		{
			int find_user(vecLocalAccounts[i].find(strUser));
			if (find_user >= 0)
			{
				find_myself = true;
				break;
			}
		}
		if (!find_myself)
		{
			CMsgBox* pMsgBox = new CMsgBox("����",_T("�����߱�����Ⱥ���ڣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}
	}


	//�Է�������
	vector<tstring> vecDestAccounts;
	if(m_pOppositeSelectedEmployeeList->GetCount() == 0)
	{
		m_pOppositeEmployeList->GetDestParticipant(vecDestAccounts);
	}
	else
	{
		m_pOppositeSelectedEmployeeList->GetDestParticipant(vecDestAccounts);
	}

	if (vecDestAccounts.size() == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ�񡾶Է��������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}

	//�ϲ�
	for (int i=0; i<vecLocalAccounts.size(); i++)
	{
		vecDestAccounts.push_back(vecLocalAccounts[i]);
	}

	//����������
	CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
	if (NULL != pRoomMgr)
	{	
		pRoomMgr->AddHandler(this);
		tstring strMyNick = CController::Instance()->GetXmppClient()->jid().username();
		pRoomMgr->PreCreateRoom(vecDestAccounts, m_strCurrProjName, strMyNick);
	}

	//���
	m_strCurrRoomID.clear();
}

void CAddNewGroupMember::OnGetRoomNameSuccess(const std::tstring& strUniqueRoom)
{
	if (strUniqueRoom.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("�½�Ⱥ��ʧ��","Ψһ��Ⱥ��������Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}
	m_strCurrRoomID = strUniqueRoom;
}
void CAddNewGroupMember::OnCreateRoomResult(bool bSucc)
{
	if (bSucc)
	{
		if (m_strCurrProjectID.empty() || m_strCurrRoomID.empty())
		{
			return;
		}

		tstring strParam(WEB_SERVER_BASE_URL);
		strParam += "business/save-participants";
		tstring strPost = "bid=" + m_strCurrProjectID + "&rid="+m_strCurrRoomID 
						+ "&self_users=[";

		//����������
		tstring strSelfUsers;
		vector<tstring> vecAccounts;
		m_pLocalParticipant->GetLocalParticipant(vecAccounts);
		if (vecAccounts.size()>0)
		{
			for (int i=0; i<vecAccounts.size(); i++)
			{
				strSelfUsers +="\"";
				strSelfUsers += CFileHelper::GetUserName(vecAccounts[i]);
				strSelfUsers += "\",";
			}
		}
		if (!strSelfUsers.empty())
		{
			strPost += strSelfUsers.substr(0, strSelfUsers.length()-1);//ȥ��������Ķ��š�
			strPost += "]";
		}


		tstring strDestUsers;
		vector<tstring> vecDestPartiPants;
		if(m_pOppositeSelectedEmployeeList->GetCount() == 0)
			m_pOppositeEmployeList->GetDestParticipant(vecDestPartiPants);
		else
			m_pOppositeSelectedEmployeeList->GetDestParticipant(vecDestPartiPants);
		if (vecDestPartiPants.size()>0)
		{
			for (int i=0; i<vecDestPartiPants.size(); i++)
			{
				strDestUsers += "\"";
				strDestUsers += CFileHelper::GetUserName(vecDestPartiPants[i]);
				strDestUsers += "\",";
			}
		}
		if (!strDestUsers.empty())
		{
			strPost += "&other_users=[";
			strPost += strDestUsers.substr(0, strDestUsers.length()-1);
			strPost += "]";
		}
		
			

		int nReturn=0;
		tstring strResponse;
		CHttpClient reqClient;
		tstring strToken = CController::Instance()->GetToken();
		reqClient.Post(strParam,strPost, strToken, strResponse );	
		if (!strResponse.empty() && strResponse.find("result") != -1)      //����web���ݿ�ʧ�� 
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(strResponse, root))
			{
				nReturn = root["result"].asInt();
			}
		}

		if (nReturn == 1)
		{
			if (vecDestPartiPants.size()>0)
			{
				tstring strAccount = vecDestPartiPants[0];
				tstring strOrgID;
				tstring strOrgName;
				m_pOppositeSelectedEmployeeList->GetOrgInfoByAccount(strAccount, strOrgID, strOrgName);

				tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
				CController::Instance()->GetRoomManager()->EnterRoom(m_strCurrRoomID, strUser);

				//����Ǹ���Ⱥ����
				if (_tcsicmp(strOrgID.c_str(), DEF_ORGID_MYFRIEND) == 0 
					|| _tcsicmp(strOrgName.c_str(), ORGANIZATION_MYFRIEND) == 0)
				{
					mainDlg::getInstance()->OnShowGroupChatWnd_Personal(CFileHelper::GetUserName(strAccount), m_strCurrRoomID);
				}
				//����Է��ǻ�����Ⱥ����
				else
				{
					mainDlg::getInstance()->OnShowGroupChatWnd(strOrgID, strOrgName, m_strCurrRoomID);
				}

				//�����ϴ��ղص����ϰ���Ⱥ����ȥ������
				SendInitPackItem(m_strCurrProjectID);

				//ˢ����Ŀ�б��sProjectData������Ϣ�������ظ���Ⱥ��
				HWND hMainWnd = mainDlg::getInstance()->GetHWND();
				if (NULL != hMainWnd)
				{
					::PostMessage(hMainWnd, WM_USER+912, 0, 0);
				}
				
			}
			else
			{
				CMsgBox* pMsgBox = new CMsgBox("�½�Ⱥ��ʧ��","����ǰû��ѡ�����������ϵ�ˣ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return;
			}
			
		}
		else
		{
			tstring strError ="����Ⱥ[";
			strError += m_strCurrRoomID;
			strError += "]ʧ�ܣ�";
			CMsgBox* pMsgBox = new CMsgBox("����", strError.c_str(), MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}

	}

	//Close();
	PostMessage(WM_CLOSE);
}

//
void CAddNewGroupMember::OnAddNewGroupMember()
{
	//�ǿյ�roomId
	if (!m_strRoomIDTobeAddNewMember.empty())
	{
		vector<tstring> vecLocalAccounts;
		//����������
		m_pLocalParticipant->GetLocalParticipant(vecLocalAccounts);
		if (vecLocalAccounts.size()==0)
		{
			CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ�񡾱����������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}


		//�Է�������
		vector<tstring> vecDestAccounts;
		if(m_pOppositeSelectedEmployeeList->GetCount() == 0)
			m_pOppositeEmployeList->GetDestParticipant(vecDestAccounts);
		else
			m_pOppositeSelectedEmployeeList->GetDestParticipant(vecDestAccounts);

		if (vecDestAccounts.size() == 0)
		{
			CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ�񡾶Է��������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}
		//�ϲ�
		for (int i=0; i<vecLocalAccounts.size(); i++)
		{
			vecDestAccounts.push_back(vecLocalAccounts[i]);
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

		for (vector<tstring>::iterator it = vecDestAccounts.begin(); it != vecDestAccounts.end(); ++it)
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
void CAddNewGroupMember::SendInitPackItem(tstring& strProjID)
{

	tstring strPackID;

	//���õ������Ŀ�µ����ϰ�:�϶�ֻ��һ����������
	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();

	tstring strUrlGetPackFiles(WEB_SERVER_BASE_URL);
	strUrlGetPackFiles += "file/get-pack-files?";
	ostringstream oss;
	oss<<"bid="<<strProjID;
	strUrlGetPackFiles += oss.str().c_str();
	tstring strOutUTF8;
	CChineseCode::GB2312ToUTF_8(strOutUTF8, strUrlGetPackFiles.c_str() , strUrlGetPackFiles.length());

	tstring strGetPackIDResult;
	reqClient.Get(strOutUTF8,strToken, strGetPackIDResult);
	if (!strGetPackIDResult.empty())
	{
		if(strGetPackIDResult.find("{\"result\":0}") != -1)
		{
			return;
		}

		//���жϣ�
		if (strGetPackIDResult.find("id") != -1 && strGetPackIDResult.find("name") != -1 &&
			strGetPackIDResult.find("create_time") != -1 && strGetPackIDResult.find("create_by") != -1 &&
			strGetPackIDResult.find("org_name") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strGetPackIDResult, root))
				{
					int nPackCount = root.size();
					if (nPackCount>0)
					{

						//�����µ�����
						for (int  i=0;  i<nPackCount;  i++)
						{
							CChineseCode::UTF_8ToGB2312(strPackID, root[i]["id"].asString().c_str(), root[i]["id"].asString().length());	

							//tstring strPackName;
							//CChineseCode::UTF_8ToGB2312(strPackName, root[i]["name"].asString().c_str(), root[i]["name"].asString().length());	
							//tstring strPackTime ;
							//CChineseCode::UTF_8ToGB2312(strPackTime, root[i]["create_time"].asString().c_str(),root[i]["create_time"].asString().length());
							//tstring strHMMTime = strPackTime.substr(11, strPackTime.length()-11);//hardcode,ֻȥ��ʱ��û������
							//tstring struid   = root[i]["create_by"].asString();
							//tstring strSender;
							//CChineseCode::UTF_8ToGB2312(strSender, root[i]["create_by"].asString().c_str(), root[i]["create_by"].asString().length());

							//tstring strAtrtibute;
							//CChineseCode::UTF_8ToGB2312(strAtrtibute, root[i]["attribute"].asString().c_str(), root[i]["attribute"].asString().length());
							////����״̬
							//int nStatus = root[i]["status"].asInt();

							//tstring strOrgname;
							//CChineseCode::UTF_8ToGB2312(strOrgname, root[i]["org_name"].asString().c_str(), root[i]["org_name"].asString().length());

							//tstring strCreaterPhone;
							//CChineseCode::UTF_8ToGB2312(strCreaterPhone, root[i]["cell_phone"].asString().c_str(),root[i]["cell_phone"].asString().length());

							//sPackItem newPack;
							//newPack.strPackID = strPackID;
// 							newPack.strPackName = strPackName;
// 							newPack.strPackTime = strHMMTime/*strPackTime*/;
// 							newPack.strPackSender = strOrgname+"-"+strSender;
// 							newPack.strPackProp = strAtrtibute;
// 							newPack.nPackStatus = nStatus;
// 							newPack.strPackCreaterPhone = strCreaterPhone;

// 							int nFileCount = root[i]["file_lists"].size();
// 							int nPackFileSize = 0;
// 							for (int j=0; j<nFileCount; j++)
// 							{
// 								sPackFileItem oneFile;
// 								oneFile.strPackID = newPack.strPackID;
// 								oneFile.strPackName = newPack.strPackName;
// 
// 								CChineseCode::UTF_8ToGB2312(oneFile.strFileName, root[i]["file_lists"][j]["file_name"].asString().c_str(), root[i]["file_lists"][j]["file_name"].asString().length());				
// 								oneFile.strFileSizeAccurate = root[i]["file_lists"][j]["file_size"].asString().c_str();
// 								int nFileSize = _tcstoul(oneFile.strFileSizeAccurate.c_str(), NULL, 10);
// 								nPackFileSize += nFileSize;
// 								oneFile.strFileSizeShow = GetFileSizeFormat(nFileSize);
// 								oneFile.strFileType  = root[i]["file_lists"][j]["file_type"].asString();
// 								oneFile.strFileUrl   = root[i]["file_lists"][j]["ali_file_url"].asString();
// 								tstring strCreater = root[i]["file_lists"][j]["create_by"].asString();
// 								newPack.listFile.push_back(oneFile);
// 							}
// 
// 							newPack.strPackSize = GetFileSizeFormat(nPackFileSize);

						}
					}
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}	
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("����", "��ȡ���ϰ��б�ʧ���ˣ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}

	}

	//�������Ⱥ���淢��һ��
	mainDlg::getInstance()->InsertPackItemToChatRoom(m_strCurrRoomID, strPackID);
}