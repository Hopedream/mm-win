#include "StdAfx.h"
#include "CallFrame.h"
#include "ucpaas/UCSClient.h"
#include "MsgBox.h"
#include "mainDlg.h"
#include "../Common/regexHelper.h"
#include "Utility.h"
//#include "voiceDlg.h"
#include "Controller.h"
#include "UserManager.h"
#include "MsgBox.h"
#include "videoDlg.h"



#define TIMER_UPDATE_CALL_STATUS 1234
#define TIMER_UPDATE_CALL_TIME	 1235

const TCHAR* const kRecentCallContactListUI = _T("recentContactsListUI");
const TCHAR* const kCallFriendsListUI = _T("callFriendsListUI");
const TCHAR* const kVoIPCallBtnName = _T("VoIPCall");
const TCHAR* const kSmartPhoneCallBtnName = _T("smartPhoneCall");
const TCHAR* const kShowFindDlgBtnName = _T("btnShowFindDlg");
const TCHAR* const kCloseDlgBtnName = _T("btnclose");

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE  0xFF21A7D4
#define COLOR_GRAY  0xff808080
#define COLOR_BLANK 0xff000000

#define REARCH_TIP_TEXT "手机号码 拼音首字母 拼音全拼 汉字"

#define CALL_HISTORY_TABVIEW 0
#define FRIENT_LIST_TABVIEW 1

CCallFrame* CCallFrame::m_pInstance = NULL;

void CCallFrame::InitalFriendList(vector<sFriendListItem>& lsAllFriendsList, bool bForceUpdate /*= false*/)
{
	if (!m_bIsInitialFriends || bForceUpdate)
	{
		m_lsFriendList = lsAllFriendsList;		
		InitFriendsList();
		m_bIsInitialFriends = true;
	}	
	ShowWindow(true);
	SetForegroundWnd();
	InitCallHistoryList();
}

CCallFrame::~CCallFrame(void)
{
}


void CCallFrame::InitWindow()
{		
	m_pBtnCallHistoryView = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnCallHistory")));
	m_pBtnFriendListView = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnFriendsList")));	
	m_pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switchLayout")));
	m_pCallHistoryListUI = static_cast<CRecentCallContactUI*>(m_PaintManager.FindControl(kRecentCallContactListUI));	
	m_pFriendListUI = static_cast<CCallFriendsUI*>(m_PaintManager.FindControl(kCallFriendsListUI));		
	m_pRearchEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txeSearchKeyword")));	
	SetFocusRearchEditBox(false);	
}

void CCallFrame::SetFocusRearchEditBox(bool bFocus)
{
	if (bFocus)
	{
		m_pRearchEdit->SetTextColor(COLOR_BLANK);
		m_pRearchEdit->SetText(_T(""));
	}
	else
	{
		m_pRearchEdit->SetTextColor(COLOR_GRAY);
		//m_pRearchEdit->SetText(REARCH_TIP_TEXT);
		m_pRearchEdit->SetToolTip(REARCH_TIP_TEXT);
	}	
}

DuiLib::CDuiString CCallFrame::GetSkinFile()
{
	return _T("Call\\CallFrameUI.xml");
}


void CCallFrame::UpdateSwitchBtnBkColor(bool bClickCallHistoryBtn)
{
	if (bClickCallHistoryBtn)
	{
		m_pBtnCallHistoryView->SetBkColor(COLOR_BLUE);
		m_pBtnCallHistoryView->SetTextColor(COLOR_WHITE);
		m_pBtnFriendListView->SetBkColor(COLOR_WHITE);
		m_pBtnFriendListView->SetTextColor(COLOR_BLUE);
	}
	else
	{
		m_pBtnFriendListView->SetBkColor(COLOR_BLUE);
		m_pBtnFriendListView->SetTextColor(COLOR_WHITE);
		m_pBtnCallHistoryView->SetBkColor(COLOR_WHITE);
		m_pBtnCallHistoryView->SetTextColor(COLOR_BLUE);
	}
}

void CCallFrame::HandleClickEvent( TNotifyUI& msg )
{
	if (msg.pSender->GetName() == kSmartPhoneCallBtnName)
	{
		int itemIdx  = msg.pSender->GetTag();
		LauchNetCall(itemIdx, VOICE_CALL_LANDING);
	}
	else if (msg.pSender->GetName() == kVoIPCallBtnName)
	{
		int itemIdx  = msg.pSender->GetTag();
		LauchNetCall(itemIdx, VOICE_CALL);
	}
	else if (msg.pSender == m_pBtnFriendListView)
	{
		m_pTabLayout->SelectItem(FRIENT_LIST_TABVIEW);
		UpdateSwitchBtnBkColor(false);
	}
	else if (msg.pSender == m_pBtnCallHistoryView)
	{
		m_pTabLayout->SelectItem(CALL_HISTORY_TABVIEW);
		UpdateSwitchBtnBkColor(true);		
	}
	else if (msg.pSender->GetName() == kCloseDlgBtnName)
	{
		ShowWindow(false);
	}
}


CControlUI* CCallFrame::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(pstrClass, "recentContactsList") == 0)
	{
		return new CRecentCallContactUI(&m_PaintManager);
	}
	if (_tcsicmp(pstrClass, "callFriendsList") == 0)
	{
		return new CCallFriendsUI(m_PaintManager);
	}	
	return NULL;
}


void CCallFrame::InitFriendsList()
{
	m_pFriendListUI->RemoveAll();

	for (std::vector<sFriendListItem>::iterator iter = m_lsFriendList.begin(); 
		iter != m_lsFriendList.end(); ++iter)
	{
		FriendListItemInfo item;
		item.empty = false;
		item.folder = false;
		item.strOrgName = iter->strOrgnization.empty() ? ORGANIZATION_MYFRIEND : iter->strOrgnization;
		item.strOrgID = iter->strOrgID;
		item.itemId = iter->nItemID;
		item.logo = iter->strAvatar;
		item.nick_name =iter->strShowName;
		item.account = iter->strAccount;

		//
		map<tstring, Node*>::iterator itFind = m_mapOrg2Node.find(item.strOrgID);
		if (itFind == m_mapOrg2Node.end())
		{
			FriendListItemInfo rootItem;
			rootItem.folder = true;
			rootItem.empty = false;
			rootItem.strOrgName = item.strOrgName;
			rootItem.strOrgID = item.strOrgID;
			rootItem.account = item.account;
			rootItem.itemId = GetNextItemID();			
			DuiLib::Node* pOrgNode = m_pFriendListUI->AddNode(rootItem, NULL);	
			m_mapOrg2Node.insert(make_pair(rootItem.strOrgID, pOrgNode));
		}
		m_pFriendListUI->AddNode(item,m_mapOrg2Node[item.strOrgID]);
	}
}

void CCallFrame::HandleItemClickEvent( TNotifyUI& msg )
{
	if ((m_pFriendListUI != NULL) &&  m_pFriendListUI->GetItemIndex(msg.pSender) != -1)
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			Node* node = (Node*)msg.pSender->GetTag();

			if (m_pFriendListUI->CanExpand(node))
			{
				m_pFriendListUI->SetChildVisible(node, !node->data().child_visible_);
			}
		}
	}
}

void CCallFrame::InitCallHistoryList()
{
	m_pCallHistoryListUI->RemoveAll();
	vector<CallHistoryItem> vecRecentList;
	if(GetDBTaskModule()->LoadCallHistoryList(vecRecentList))
	{
		for (int idx = 0; idx < vecRecentList.size(); idx++)
		{
			for (vector<sFriendListItem>::iterator iter = m_lsFriendList.begin(); iter != m_lsFriendList.end(); ++iter)
			{
				if (0 == vecRecentList[idx].strUid.compare(ExtractPhoneNo(iter->strAccount)))
				{
					UpdateCallHistory(*iter, vecRecentList[idx].eDialIndicate);
					break;
				}
			}
		}
	}
}

void CCallFrame::UpdateCallHistory(const sFriendListItem friendItem, EDialIndicate eDialIndicate)
{
	for (int idx = 0; idx < m_pCallHistoryListUI->GetCount(); ++idx)
	{
		CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(m_pCallHistoryListUI->GetItemAt(idx));
		if (_tcsicmp(pListElement->GetClass(), _T("ListContainerElementUI")) == 0
			&& pListElement->GetTag() == friendItem.nItemID)
		{
			if (0 == idx) //是最近一个聊天记录,不需要update
			{
				return;
			}
			else
			{
				m_pCallHistoryListUI->RemoveAt(idx);
			}
		}
	}

	recentCallContactItem item;
	item.m_strNickName = friendItem.strShowName.c_str();
	item.m_strOrgName=(_T(""));
	item.m_strLogo = friendItem.strAvatar.c_str();
	item.nItemID = friendItem.nItemID;
	item.dialIndicate = eDialIndicate;
	m_pCallHistoryListUI->AddItem(&item, 0);	
}

void CCallFrame::HandleSetFocusEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pRearchEdit)
	{
		SetFocusRearchEditBox(true);
		UpdateSwitchBtnBkColor(false);
		m_pTabLayout->SelectItem(FRIENT_LIST_TABVIEW);
	}
}

void CCallFrame::HandleKillFocusEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pRearchEdit && m_pRearchEdit->GetText().IsEmpty())
	{
		SetFocusRearchEditBox(false);
	}
}


//支持中文字符、中文拼音首字母、中文全拼、英文、账号手机号码等搜索。。。---llzhou、
//UI 刷新优化问题就给接受者了。。。
void CCallFrame::HandleTextChangedEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pRearchEdit)
	{
		CDuiString strPattern = m_pRearchEdit->GetText();	
		if (strPattern.IsEmpty() || -1!= strPattern.Find(REARCH_TIP_TEXT))
		{
			for (map<tstring, DuiLib::Node*>::iterator nodeIter = m_mapOrg2Node.begin(); nodeIter != m_mapOrg2Node.end(); ++nodeIter)
			{
				m_pFriendListUI->ShowAllNode(nodeIter->second);				
			}
			return;
		}
		tstring _pattern = CRegexHelper::ConvertChineseUnicodeToPyt(strPattern.GetData());

		for (map<tstring, DuiLib::Node*>::iterator nodeIter = m_mapOrg2Node.begin(); nodeIter != m_mapOrg2Node.end(); ++nodeIter)
		{
			nodeIter->second->data().child_visible_ = true;
			//m_pFriendListUI->SetChildVisible(nodeIter->second,false);
			m_pFriendListUI->ShowAllNode(nodeIter->second, false);
			for (vector<sFriendListItem>::iterator iter = m_lsFriendList.begin(); iter != m_lsFriendList.end(); ++iter)
			{
				tstring strRearch = ExtractPhoneNo(iter->strAccount);
				strRearch += iter->strShowName;
				strRearch += CRegexHelper::ConvertChineseUnicodeToPyt(iter->strShowName);				
				if (CRegexHelper::IsFirstPYMatch(strRearch.c_str(), _pattern.c_str()))
				{
					m_pFriendListUI->SetChildVisible(nodeIter->second, iter->nItemID, true);
				}			
			}
		}
	}
}

 bool CCallFrame::GetFriendItem(int itemIdx, sFriendListItem& friendItem)
{
	for (vector<sFriendListItem>::iterator iter = m_lsFriendList.begin(); iter != m_lsFriendList.end(); ++iter)
	{
		if (iter->nItemID == itemIdx)
		{
			friendItem = *iter;
			return true;
		}
	}
	return false;
}


void CCallFrame::LauchNetCall(int itemIdx, UCS_DIALTYPE eDialType)
{
	sFriendListItem friendItem;
	if (!GetFriendItem(itemIdx, friendItem))
	{
		return;
	}

	bool bIsOnline = IsFriendOnline(friendItem.strAccount);
	if (VOICE_CALL == eDialType && !bIsOnline)
	{		
			CMsgBox* pMsgBox = new CMsgBox("网络电话","对方不在线，无法拨打网络电话");
			pMsgBox->ShowModal();
			return;			
	}

	CVideoDlg::Instance()->InitalDialInfo(ExtractPhoneNo(friendItem.strAccount).c_str(), friendItem.strShowName.c_str(), "", friendItem.strAvatar.c_str());
	if(CVideoDlg::Instance()->LauchCall(eDialType))
	{
		CVideoDlg::Instance()->ActiveVoiceDlg(true);
		UpdateCallHistory(friendItem);
		GetDBTaskModule()->SaveCallHistoryList(friendItem.strAccount, eDialCallOut);
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("连接超时","连接云之讯服务器失败！", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}
}

bool CCallFrame::IsFriendOnline(const tstring strAccount)
{
	CUserInfoManager* pUserMrg = CController::Instance()->GetUserInfoMgr();
	if (pUserMrg && enmState_Online == CController::Instance()->GetUserInfoMgr()->GetState(strAccount))
	{
		return true;
	}
	return false;
}


CCallFrame* CCallFrame::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CCallFrame;
		m_pInstance->CreateWnd();
	}
	return m_pInstance;
}

CCallFrame::CCallFrame()
{
	m_bIsInitialFriends = false;
}




