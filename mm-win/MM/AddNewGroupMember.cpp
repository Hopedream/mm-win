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
#include "mainDlg.h"//需要重构，应该不需要依赖mainDlg，而依赖相关的业务代码。
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

		//关闭界面
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
							//by zyy test
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
		//创建一个room
		else if (_tcsicmp(msg.pSender->GetName(), "btn_send_toserver") == 0)
		{
			OnPreCreateGroupRoom();
		}
		//添加新选择的好友进来：对于已经在的就不添加了。
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

void CAddNewGroupMember::HandleItemClickEvent( TNotifyUI& msg )
{
}

bool CAddNewGroupMember::Initialize()
{
	InitialLocalParticipant();

	InitialDestOrgList();

	//当前是添加群成员的界面，需要初始化选中那些已经是群成员的Item
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
				
				//对于对方的，是需要additem
				//m_pOppositeEmployeList->SetItemSeleted(strAccount);
			}
		}

	}

	return true;
}

void CAddNewGroupMember::InitialLocalParticipant()
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
}

void CAddNewGroupMember::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//初始化窗口
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
	//本方参与人
	m_pLocalParticipant->GetLocalParticipant(vecLocalAccounts);
	if (vecLocalAccounts.size()==0)
	{
		CMsgBox* pMsgBox = new CMsgBox("错误",_T("请选择【本方】参与人！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
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
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("创建者必须在群组内！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}
	}


	//对方参与人
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
		CMsgBox* pMsgBox = new CMsgBox("错误",_T("请选择【对方】参与人！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}

	//合并
	for (int i=0; i<vecLocalAccounts.size(); i++)
	{
		vecDestAccounts.push_back(vecLocalAccounts[i]);
	}

	//创建聊天组
	CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();
	if (NULL != pRoomMgr)
	{	
		pRoomMgr->AddHandler(this);
		tstring strMyNick = CController::Instance()->GetXmppClient()->jid().username();
		pRoomMgr->PreCreateRoom(vecDestAccounts, m_strCurrProjName, strMyNick);
	}

	//清空
	m_strCurrRoomID.clear();
}

void CAddNewGroupMember::OnGetRoomNameSuccess(const std::tstring& strUniqueRoom)
{
	if (strUniqueRoom.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("新建群组失败","唯一的群组名不能为空！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
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
		if (!strResponse.empty() && strResponse.find("result") != -1)      //保存web数据库失败 
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

				//如果是个人群聊天
				if (_tcsicmp(strOrgID.c_str(), DEF_ORGID_MYFRIEND) == 0 
					|| _tcsicmp(strOrgName.c_str(), ORGANIZATION_MYFRIEND) == 0)
				{
					mainDlg::getInstance()->OnShowGroupChatWnd_Personal(CFileHelper::GetUserName(strAccount), m_strCurrRoomID);
				}
				//如果对方是机构的群聊天
				else
				{
					mainDlg::getInstance()->OnShowGroupChatWnd(strOrgID, strOrgName, m_strCurrRoomID);
				}

				//发送上次收藏的资料包到群里面去。。。
				SendInitPackItem(m_strCurrProjectID);

				//刷新项目列表的sProjectData数据信息，避免重复进群。
				HWND hMainWnd = mainDlg::getInstance()->GetHWND();
				if (NULL != hMainWnd)
				{
					::PostMessage(hMainWnd, WM_USER+912, 0, 0);
				}
				
			}
			else
			{
				CMsgBox* pMsgBox = new CMsgBox("新建群组失败","您当前没有选择机构或者联系人！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return;
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

	//Close();
	PostMessage(WM_CLOSE);
}

//
void CAddNewGroupMember::OnAddNewGroupMember()
{
	//非空的roomId
	if (!m_strRoomIDTobeAddNewMember.empty())
	{
		vector<tstring> vecLocalAccounts;
		//本方参与人
		m_pLocalParticipant->GetLocalParticipant(vecLocalAccounts);
		if (vecLocalAccounts.size()==0)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("请选择【本方】参与人！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}


		//对方参与人
		vector<tstring> vecDestAccounts;
		if(m_pOppositeSelectedEmployeeList->GetCount() == 0)
			m_pOppositeEmployeList->GetDestParticipant(vecDestAccounts);
		else
			m_pOppositeSelectedEmployeeList->GetDestParticipant(vecDestAccounts);

		if (vecDestAccounts.size() == 0)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("请选择【对方】参与人！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}
		//合并
		for (int i=0; i<vecLocalAccounts.size(); i++)
		{
			vecDestAccounts.push_back(vecLocalAccounts[i]);
		}


		//剔除已经在群里面的成员。
		vector<tstring> vecNewMem;
		vector<tstring> vecAlreadyExist;
		if(!CController::Instance()->GetRoomManager()->GetRoomMemberList(m_strRoomIDTobeAddNewMember, vecAlreadyExist))
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("获取当前群成员失败！"),MSGBOX_ICON_WARNING, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
			return;
		}

		for (vector<tstring>::iterator it = vecDestAccounts.begin(); it != vecDestAccounts.end(); ++it)
		{
			bool bExist = false;
			tstring& strNewMember = *it;
			for (vector<tstring>::iterator itChild = vecAlreadyExist.begin(); itChild != vecAlreadyExist.end(); ++itChild)
			{
				//如果已经有了，就不添加
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
				
				//添加完就关闭窗口
				PostMessage(WM_CLOSE);
			}
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("提醒",_T("您当前选中联系人已经都在该群组里面了！"),MSGBOX_ICON_WARNING, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}
	}
}
void CAddNewGroupMember::SendInitPackItem(tstring& strProjID)
{

	tstring strPackID;

	//先拿到这个项目下的资料包:肯定只有一个！！！！
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

		//弱判断：
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

						//增加新的数据
						for (int  i=0;  i<nPackCount;  i++)
						{
							CChineseCode::UTF_8ToGB2312(strPackID, root[i]["id"].asString().c_str(), root[i]["id"].asString().length());	

							//tstring strPackName;
							//CChineseCode::UTF_8ToGB2312(strPackName, root[i]["name"].asString().c_str(), root[i]["name"].asString().length());	
							//tstring strPackTime ;
							//CChineseCode::UTF_8ToGB2312(strPackTime, root[i]["create_time"].asString().c_str(),root[i]["create_time"].asString().length());
							//tstring strHMMTime = strPackTime.substr(11, strPackTime.length()-11);//hardcode,只去了时间没有日期
							//tstring struid   = root[i]["create_by"].asString();
							//tstring strSender;
							//CChineseCode::UTF_8ToGB2312(strSender, root[i]["create_by"].asString().c_str(), root[i]["create_by"].asString().length());

							//tstring strAtrtibute;
							//CChineseCode::UTF_8ToGB2312(strAtrtibute, root[i]["attribute"].asString().c_str(), root[i]["attribute"].asString().length());
							////包的状态
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
			CMsgBox* pMsgBox = new CMsgBox("错误", "获取资料包列表失败了！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}

	}

	//向这个新群里面发送一个
	mainDlg::getInstance()->InsertPackItemToChatRoom(m_strCurrRoomID, strPackID);
}