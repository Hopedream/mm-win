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
#include "mainDlg.h"//需要重构，应该不需要依赖mainDlg，而依赖相关的业务代码。
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

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), "btn_new_proj_step2_close") == 0)
		{
			//告知父窗口。
			if (NULL != m_pParent)
			{
				m_pParent->PostMessage(WM_STEP2_CLOSED);
			}

			Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "local_participant_option") == 0)
		{
			////注意：获取的是点击之前的状态！！！
			//bool bSelected = ((COptionUI*)(msg.pSender))->IsSelected();

			//CContainerUI* pContainer = (CContainerUI*)msg.pSender->GetParent();
			//if (NULL != pContainer)
			//{
			//	sFriendListItem* pFriend = (sFriendListItem*)pContainer->GetTag();
			//	if (NULL != pFriend)
			//	{
			//		//之前是选中的，现在取消了==>从被选中列表中删除
			//		if (bSelected)
			//		{
			//			m_pOppositeSelectedEmployeeList->RemoveOneItem(pFriend->nItemID);
			//		}
			//		//之前是取消的，现在选中了==>增加到被选中列表
			//		else
			//		{
			//			m_pOppositeSelectedEmployeeList->AddOneItem(pFriend);
			//		}
			//	}
			//}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "option_oppo_employe_selected") == 0)
		{
			//如果当前不是询价单，就不可以选不同组织的。
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
							CMsgBox* pMsgBox = new CMsgBox("选择接收人",strErrorMsg.c_str(), MSGBOX_ICON_ERROR, MSGBOX_IDOK);
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
		//删除了一个已选参与者，同步更新到m_pLocalParticipant为非勾选状态
		else if (_tcsicmp(msg.pSender->GetName(), "btn_remove_selected_employee") == 0)
		{
			//1.从已选List删除掉
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

			//2.同步更新
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
					CMsgBox* pMsgBox = new CMsgBox("提醒",_T("当前正在保存文件，请稍候重新发送！"),MSGBOX_ICON_WARNING, MSGBOX_IDOK);
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
	//清空之前org下的成员列表
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

	//将该组织下的成员加入到该列表
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

					//如果已经在已添加接收人里面，就勾选上。
					if (m_pOppositeSelectedEmployeeList->CheckIfExist(oneFriend.strAccount))
					{
						m_pOppositeEmployeList->SetItemSeleted(oneFriend.strAccount);
					}
				}
			}
			//当前组是空的。
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
	//我在一个组织里面，列出组织内的所有成员
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
	//只有我一个人！
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

		//1.cache好友列表的信息。
		map<tstring,tstring> mapOrgID2Orgname;
		mapOrgID2Orgname.insert(make_pair("-1", "我的好友"));
		for (MapAllRosters::iterator iter = listUserInfos.begin(); iter !=listUserInfos.end(); iter++)
		{
			map<tstring,tstring>::iterator itFind = mapOrgID2Orgname.find(iter->second.strOrgID);
			if ( itFind == mapOrgID2Orgname.end())
			{
				mapOrgID2Orgname.insert(make_pair(iter->second.strOrgID, iter->second.strOrgnization));
			}
		}

		//插入到org列表
		tstring strMyOrgName;
		tstring strMyOrgID;
		bool bInOrg = false;//我如果再组织里面，就需要过滤掉我所在的组织。否则就是在”我的好友“
		if(pUserManager->GetMyOrganization(strMyOrgName, strMyOrgID))
		{
			bInOrg = _tcsicmp(strMyOrgID.c_str(), DEF_ORGID_MYFRIEND)!=0;
		}

		for (map<tstring, tstring>::iterator it2 = mapOrgID2Orgname.begin();
			it2 != mapOrgID2Orgname.end(); ++it2)
		{
			//根据orgID获取 org 的avatar;
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
				//如果是我的组织，就不加入
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
	//	//创建父节点
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

	//	//创建子节点
	//	//vector<sFriendListItem> vecAllFriends;
	//	m_vecAllFriends.clear();
	//	if (mainDlg::getInstance()->GetFriendListData(m_vecAllFriends))
	//	{
	//		//2：刷新好友列表
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

//初始化窗口
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
	//如果是询价单，可以多选：但貌似只能选不超过三家的OrgID
	bool bAllowed = false;
	vector<tstring> vecOrgID;
	m_pOppositeSelectedEmployeeList->GetSelectedOrgIDs(vecOrgID);

	if (vecOrgID.size()>0)//当前已经有个机构被选中了
	{
		if (_tcsicmp(strOrgID.c_str(), DEF_ORGID_MYFRIEND) == 0 &&//当前的选中的机构是“我的好友”
			_tcsicmp(vecOrgID[0].c_str(), DEF_ORGID_MYFRIEND ) == 0)//当前已经有一个"我的好友"的成员被选中了
		{
			strErrorMsg = "只能选择一个 我的好友 进行业务沟通！";
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
				strErrorMsg  ="一个机构只能选择一个参与人进行寻报价！";
			}
			else
			{
				if (vecOrgID.size() == 3)
				{
					strErrorMsg = "您最多只能和三家机构寻报价！";
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
				strErrorMsg = "您选择的参与人必须在同一机构！";
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
// 							//从控件上获取字符串。
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
	//本方参与人
	m_pLocalParticipant->GetLocalParticipant(vecAccounts);
	int nLocalPerson = vecAccounts.size();
	if (nLocalPerson == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("错误",_T("请选择群组聊天【本方】参与人！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}

	//对方参与人
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
		CMsgBox* pMsgBox = new CMsgBox("错误",_T("请选择群组聊天【对方】参与人！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
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
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("创建者必须在群组内！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}
	}

	

	//创建聊天组
	CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
	if (NULL != pRoomMgr)
	{	
		pRoomMgr->AddHandler(this);
		tstring strMyNick = CController::Instance()->GetXmppClient()->jid().username();
		pRoomMgr->PreCreateRoom(vecAccounts, m_strCurrProjName, strMyNick);
	}

	//清空
	m_strCurrRoomID.clear();
}

void CCreateNewProjStep2::OnGetRoomNameSuccess(const std::tstring& strUniqueRoom)
{
	if (strUniqueRoom.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("新建群组失败","唯一的群组名不能为空！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
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

		//本方参与人
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
			strPost += strSelfUsers.substr(0, strSelfUsers.length()-1);//去掉最后多余的逗号。
			strPost += "]";
		}

		//对方参与人的选择
		tstring strDestUsers;
		vector<tstring> vecDestPartiPants;
		if(m_pOppositeSelectedEmployeeList->GetCount() > 0)
		{
			// 如果选择了对方参与方,将所有选择的成员加入聊天群组
			m_pOppositeSelectedEmployeeList->GetDestParticipant(vecDestPartiPants);
		}
		else
		{
			// 如果为选择参与方,则按照优先级选择一个参与者(无 -> 任一离线人员 -> 默认接收方(离线) -> 任一在线人员 -> 默认接收方(在线))
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
		if (!strResponse.empty() && strResponse.find("result") != -1)      //保存web数据库失败 
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
				//此处没有考虑，“我的好友”没有机构下的显示。
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

				//向这个新群里面发送一个
				mainDlg::getInstance()->InsertPackItemToChatRoom(m_strCurrRoomID, m_strCurrPackID);
			}

			//刷新项目列表的sProjectData数据信息，避免重复进群。
			HWND hMainWnd = mainDlg::getInstance()->GetHWND();
			if (NULL != hMainWnd)
			{
				::PostMessage(hMainWnd, WM_USER+912, 0, 0);
			}
		}
		else
		{
			tstring strError ="保存群[";
			strError += m_strCurrRoomID;
			strError += "]失败！";
			CMsgBox* pMsgBox = new CMsgBox("错误", strError.c_str(), MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}

	}

	//告知父窗口。
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

