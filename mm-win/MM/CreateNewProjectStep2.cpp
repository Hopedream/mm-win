#include "CreateNewProjectStep2.h"
#include "TileLayoutLocalParticipant.h"
#include "UIOrgMembersList.h"
#include "TileLayoutDestOrgList.h"
#include "TileLayoutDestEmployeeList.h"
#include "TileLayoutDestEmployeeSelectedList.h"
#include "CreateNewProjectStep1.h"
#include "StringHelper.h"

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
#include "MsgBox.h"
#include "..\Common\Logger.h"

#include "mainDlg.h"


#define MW_TIMER_TOPWND 2022

using namespace std;

	
CCreateNewProjStep2::CCreateNewProjStep2(CPaintManagerUI* ppm /*= NULL*/)
{
	//m_hMainWnd = NULL;
	m_pPaintManager = ppm;
	m_pLocalParticipant = NULL;
	//m_pOrgMemberList	= NULL;
	m_pDestOrgList		= NULL;
	m_pOppositeEmployeList = NULL;
	m_pOppositeSelectedEmployeeList = NULL;
	m_bAskPrice = false;
}

CCreateNewProjStep2::~CCreateNewProjStep2()
{
	CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
	if (NULL != pRoomMgr)
	{
		pRoomMgr->RemoveHandler(this);
	}
}

void CCreateNewProjStep2::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), "btn_new_proj_step2_close") == 0)
		{
			//��֪�����ڡ�
			if (NULL != m_pParent)
			{
				m_pParent->PostMessage(WM_STEP2_CLOSED);
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
		else if (_tcsicmp(msg.pSender->GetName(), "btn_returnto_step1") == 0)
		{
			ShowWindow(false);
			if (m_pParent != NULL)
			{
				m_pParent->PostMessage(WM_SHOW_STEP1_WND);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_send_toserver") == 0)
		{
			if (NULL != m_pParent)
			{
				CControlUI* pBtnSend2Server = static_cast<CControlUI*>(m_PaintManager.FindControl("btn_send_toserver"));
				pBtnSend2Server->SetEnabled(false);

				if (m_pParent->IsSavingFile())
				{
					CMsgBox* pMsgBox = new CMsgBox("����",_T("��ǰ���ڱ����ļ������Ժ����·��ͣ�"),MSGBOX_ICON_WARNING, MSGBOX_IDOK);
					pMsgBox->ShowModal(NULL);
					return;
				}
				else
				{
					OnPreCreateGroupRoom();
				}
				pBtnSend2Server->SetEnabled(true);
			}
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

void CCreateNewProjStep2::HandleListItemSelected(TNotifyUI& msg )
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
			char * nOrgID = (char*)pItem->GetTag();
			std::ostringstream stm;
			std::tstring strResult;
			stm << nOrgID;
			strResult = stm.str();

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

void CCreateNewProjStep2::HandleItemClickEvent( TNotifyUI& msg )
{
// 	tstring strName = msg.pSender->GetName();
// 
// 	if (0 == msg.pSender->GetName().Find(STR_ORG_MEM_LIST_ITEM)
// 		&& (m_pOrgMemberList!= NULL) 
// 		&&  m_pOrgMemberList->GetItemIndex(msg.pSender) != -1)
// 	{
// 		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
// 		{
// 			Node* node = (Node*)msg.pSender->GetTag();
// 			if (m_pOrgMemberList->CanExpand(node))
// 			{
// 				m_pOrgMemberList->SetChildVisible(node, !node->data().child_visible_);
// 			}
// 			else 
// 			{
// 			}
// 		}
// 	}
}

bool CCreateNewProjStep2::Initialize()
{
	InitialLocalParticipant();

	InitialDestOrgList();

	return true;
}

void CCreateNewProjStep2::InitialLocalParticipant()
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
// 			if (GetDuty(pMySelf->strOrgID, pMySelf->strAccount, vecProperty))
// 			{
// 
// 			}
			m_pLocalParticipant->AddOneItem(pMySelf, vecProperty);
		}
	}


}
void CCreateNewProjStep2::InitialDestOrgList()
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
	

	

	//vector<tstring> vecOrgs;
	//map<tstring, DuiLib::Node*> mapOrg2Nodes;
	//if (mainDlg::getInstance()->GetOrgnizations(vecOrgs))
	//{
	//	//�������ڵ�
	//	for (vector<tstring>::iterator it = vecOrgs.begin();
	//		it != vecOrgs.end(); ++it)
	//	{
	//		FriendListItemInfo item;
	//		item.folder = true;
	//		item.empty = false;
	//		item.strOrgName = (*it);
	//		item.itemId = GetNextItemID();
	//		DuiLib::Node* pOrgNode = m_pOrgMemberList->AddNode(item, NULL);	

	//		mapOrg2Nodes.insert(make_pair(item.strOrgName, pOrgNode));
	//	}

	//	//�����ӽڵ�
	//	//vector<sFriendListItem> vecAllFriends;
	//	m_vecAllFriends.clear();
	//	if (mainDlg::getInstance()->GetFriendListData(m_vecAllFriends))
	//	{
	//		//2��ˢ�º����б�
	//		for (std::vector<sFriendListItem>::iterator iter = m_vecAllFriends.begin(); 
	//			iter != m_vecAllFriends.end(); ++iter)
	//		{
	//			FriendListItemInfo item;
	//			item.empty = false;
	//			item.folder = false;
	//			item.strOrgName = iter->strOrgnization.empty() ? ORGANIZATION_MYFRIEND : iter->strOrgnization;
	//			item.itemId = GetNextItemID();;
	//			item.logo = iter->strAvatar;
	//			item.nick_name =iter->strShowName;
	//			m_pOrgMemberList->AddNode(item, mapOrg2Nodes[item.strOrgName]);
	//		}
	//	}
	//}
}

void CCreateNewProjStep2::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_DIALOG, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//��ʼ������
void CCreateNewProjStep2::InitWindow()
{

}

CControlUI* CCreateNewProjStep2::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, "ListLocalParticipant") == 0)
	{
		m_pLocalParticipant = new CTileLayoutLocalParticipant(m_pPaintManager);
		return m_pLocalParticipant;
	}
// 	else if (_tcsicmp(pstrClass, "ListOrgMembers") == 0)
// 	{
// 		m_pOrgMemberList = new CUIOrgMembersList(m_pPaintManager);
// 		return m_pOrgMemberList;
// 	}
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

void CCreateNewProjStep2::SetResProperty(bool bAskPrice, tstring& strProjectID, tstring& strProjName, tstring& strPackID)
{
	m_bAskPrice = bAskPrice;
	m_strCurrProjectID = strProjectID;
	m_strCurrProjName = strProjName;
	m_strCurrPackID = strPackID;
}

bool CCreateNewProjStep2::IsAllowedToSelected(tstring& strOrgID, tstring& strErrorMsg)
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

LRESULT CCreateNewProjStep2::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

void CCreateNewProjStep2::SetForegroundWnd()
{
	::SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetTimer(this->GetHWND(), MW_TIMER_TOPWND, 50, NULL);
}

void CCreateNewProjStep2::OnParticipantSelected()
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

bool CCreateNewProjStep2::GetDefaultAccountByOrgID(tstring& strOrgID, tstring& strAccount)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-default?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += strOrgID;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.empty())
	{
		return false;
	}


	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else if(strHttpResponse.find("cell_phone") != -1)
	{
		//todo
		strAccount = strHttpResponse.substr(strHttpResponse.find("\"cell_phone\":\"") + 14, 
			strHttpResponse.find("\",\"") - (strHttpResponse.find("\"cell_phone\":\"") + 14));
		strAccount += MMPC_ACCOUNT;
		return true;
	}

	return false;
}

void CCreateNewProjStep2::OnPreCreateGroupRoom()
{
	vector<tstring> vecAccounts;
	//����������
	m_pLocalParticipant->GetLocalParticipant(vecAccounts);
	int nLocalPerson = vecAccounts.size();
	if (nLocalPerson == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ��Ⱥ�����졾�����������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}

	//�Է�������
	if(m_pOppositeSelectedEmployeeList->GetCount() == 0)
	{
		m_pOppositeEmployeList->GetDestParticipant(vecAccounts);
	}
	else
	{
		m_pOppositeSelectedEmployeeList->GetDestParticipant(vecAccounts);
	}
	int nDestPerson = vecAccounts.size() - nLocalPerson;
	if (nDestPerson == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("����",_T("��ѡ��Ⱥ�����졾�Է��������ˣ�"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}
	else
	{
		bool find_myself(false);
		tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
        for (int i=0;i!=vecAccounts.size();i++)
        {
			int find_user(vecAccounts[i].find(strUser));
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

	

	//����������
	CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
	if (NULL != pRoomMgr)
	{	
		pRoomMgr->AddHandler(this);
		tstring strMyNick = CController::Instance()->GetXmppClient()->jid().username();
		pRoomMgr->PreCreateRoom(vecAccounts, m_strCurrProjName, strMyNick);
	}

	//���
	m_strCurrRoomID.clear();
}

void CCreateNewProjStep2::OnGetRoomNameSuccess(const std::tstring& strUniqueRoom)
{
	if (strUniqueRoom.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("�½�Ⱥ��ʧ��","Ψһ��Ⱥ��������Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}
	m_strCurrRoomID = strUniqueRoom;
}
void CCreateNewProjStep2::OnCreateRoomResult(bool bSucc)
{
	if (bSucc)
	{
		if (m_strCurrProjectID.empty() || m_strCurrRoomID.empty())
		{
			return;
		}

	
		CHttpClient reqClient;
		tstring strResponse;
		tstring strToken = CController::Instance()->GetToken();

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

		//�Է������˵�ѡ��
		tstring strDestUsers;
		vector<tstring> vecDestPartiPants;
		if(m_pOppositeSelectedEmployeeList->GetCount() > 0)
		{
			// ���ѡ���˶Է����뷽,������ѡ��ĳ�Ա��������Ⱥ��
			m_pOppositeSelectedEmployeeList->GetDestParticipant(vecDestPartiPants);
		}
		else
		{
			// ���Ϊѡ����뷽,�������ȼ�ѡ��һ��������(�� -> ��һ������Ա -> Ĭ�Ͻ��շ�(����) -> ��һ������Ա -> Ĭ�Ͻ��շ�(����))
			m_pOppositeEmployeList->GetDestParticipant(vecDestPartiPants);
		}
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
//		tstring strResponse;
// 		CHttpClient reqClient;
// 		tstring strToken = CController::Instance()->GetToken();
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

		if (nReturn == 1 )
		{
			if (vecDestPartiPants.size()>0)
			{
				tstring strAccount = vecDestPartiPants[0];
				tstring strOrgID;
				tstring strOrgName;
				//�˴�û�п��ǣ����ҵĺ��ѡ�û�л����µ���ʾ��
				m_pOppositeSelectedEmployeeList->GetOrgInfoByAccount(strAccount, strOrgID, strOrgName);

				tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
				CController::Instance()->GetRoomManager()->EnterRoom(m_strCurrRoomID, strUser);
				if (_tcsicmp(strOrgID.c_str(), DEF_ORGID_MYFRIEND) == 0 
					|| _tcsicmp(strOrgName.c_str(), ORGANIZATION_MYFRIEND) == 0)
				{
					mainDlg::getInstance()->OnShowGroupChatWnd_Personal(CFileHelper::GetUserName(strAccount), m_strCurrRoomID);
				}
				else
				{
					mainDlg::getInstance()->OnShowGroupChatWnd(strOrgID, strOrgName, m_strCurrRoomID);
				}

				//�������Ⱥ���淢��һ��
				mainDlg::getInstance()->InsertPackItemToChatRoom(m_strCurrRoomID, m_strCurrPackID);
			}

			//ˢ����Ŀ�б��sProjectData������Ϣ�������ظ���Ⱥ��
			HWND hMainWnd = mainDlg::getInstance()->GetHWND();
			if (NULL != hMainWnd)
			{
				::PostMessage(hMainWnd, WM_USER+912, 0, 0);
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

	//��֪�����ڡ�
	if (NULL != m_pParent)
	{
		m_pParent->PostMessage(WM_STEP2_CLOSED);
	}

	Close();
}





// tstring CCreateNewProjStep2::GetAccountByItemID(int nItemID)
// {
// 	tstring strAccount;
// 	for(vector<sFriendListItem>::iterator it = m_vecAllFriends.begin();
// 		it != m_vecAllFriends.end(); ++it)
// 	{
// 		sFriendListItem& oneFriend = (*it);
// 		if (oneFriend.nItemID == nItemID)
// 		{
// 			strAccount = oneFriend.strAccount;
// 		}
// 	}
// 
// 	return strAccount;
// }

