#include "SysMsgListCtrl.h"
#include "Utility.h"
#include "../Common/Logger.h"

using namespace DuiLib;

const int kContactListItemNormalHeight = 50;
const int kContactListItemSelectedHeight = 60;

CListSysMsg::CListSysMsg( CPaintManagerUI *paint_manager )
	:m_pPaintManager(paint_manager)
{
	SetItemShowHtml(true);
}
CListSysMsg::~CListSysMsg()
{

}
bool CListSysMsg::Add( CControlUI *pControl )
{
	if (!pControl)
		return false;
	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;
	return CListUI::Add(pControl);
}
bool CListSysMsg::AddAt( CControlUI *pControl, int iIndex )
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool CListSysMsg::Remove( CControlUI *pControl )
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;
	return CListUI::Remove(pControl);		
}
bool CListSysMsg::RemoveAt( int iIndex )
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::RemoveAt(iIndex);
}
void CListSysMsg::RemoveAll()
{
	CListUI::RemoveAll();
}
void CListSysMsg::DoEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_RBUTTONUP )
	{
		m_pManager->SendNotify(this, _T("rclick"));
	}
	else if (event.Type == UIEVENT_DBLCLICK)
	{
		m_pManager->SendNotify(this, _T("dbclick"));
	}
	else if (event.Type == UIEVENT_BUTTONDOWN)
	{
		m_pManager->SendNotify(this, _T("itemclick"));
	}

	CListUI::DoEvent(event);
	//处理右键消息
		
}

void CListSysMsg::AddItem(tstring& strTips, int uTime, tstring& strSender, ESysMsgSubType eSubType, int indx /*= -1*/)
{
	CListContainerElementUI *pListElement = NULL;
	CDialogBuilder dlgBuilder;
	if (!dlgBuilder.GetMarkup()->IsValid())
	{
		pListElement = static_cast<CListContainerElementUI*>(dlgBuilder.Create(_T("sys_msg_item_invite_request.xml"),
			(UINT)0, NULL, m_pPaintManager));
	}
	else
	{
		pListElement = static_cast<CListContainerElementUI*>(dlgBuilder.Create((UINT)0, m_pPaintManager));
	}
	if (pListElement == NULL)
		return;
 		
	tstring strType = _T("好友信息");
	if(strTips.find(_T("请求添加您为好友")) != -1)
		strType = _T("好友邀请");
	else if(strTips.find(_T("已经同意添加您为好友")) != -1)
		strType = _T("好友验证");
	//msg type
	CLabelUI *pLabelType = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, "labelType"));
	if (pLabelType != NULL)
	{
		pLabelType->SetText(strType.c_str());
	}

	//tips
 	CLabelUI *pLabelTips = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, "labelRequestTips"));
 	if (pLabelTips != NULL)
 	{
 		pLabelTips->SetText(strTips.c_str());
 	}
 
	//time
 	CLabelUI *pLabelTime = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, "labelRecvtime"));
 	if (pLabelTime != NULL)
 	{
 		pLabelTime->SetText((GetFullTimeString(uTime)).c_str());
 	}

	sSysMsg* pCacheSysMsg = new sSysMsg;
	pCacheSysMsg->eSubType = eSubType;
	if (eSubType != enmSysMsgSubType_AddFriend)
	{
		CButtonUI *pBtnAgree = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, "ButtonAgreed"));
		if (pBtnAgree != NULL)
		{
			pBtnAgree->SetVisible(false);
		}

		CButtonUI *pBtnDeny = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, "ButtonDeny"));
		if (pBtnDeny != NULL)
		{
			pBtnDeny->SetVisible(false);
		}
	}
	else
	{
		CLogger::GetInstance()->PrintErrLog( "add friend request msg comed");	
	}
	pCacheSysMsg->strMsg = strTips;
	pCacheSysMsg->strSender = strSender;
	pCacheSysMsg->uTime = uTime;

	pListElement->SetTag((INT_PTR)pCacheSysMsg);
	pListElement->SetFixedHeight(kContactListItemSelectedHeight);
	pListElement->SetName("System_Item");
	if (NULL != pListElement)
	{
		if (-1 == indx)
		{
			if (!CListUI::Add(pListElement))
			{
				delete pListElement;
			}
		}
		else if(indx >= 0)
		{
			if (!CListUI::AddAt(pListElement,indx))
			{
				delete pListElement;
			}
		}

	}
}

void CListSysMsg::AddOrgItem( tstring& strTips, int uTime, tstring& strSender, ESysMsgSubType eSubType, int indx /*= -1*/ )
{
	CListContainerElementUI *pListElement = NULL;
	CDialogBuilder dlgBuilder;
	if (!dlgBuilder.GetMarkup()->IsValid())
	{
		pListElement = static_cast<CListContainerElementUI*>(dlgBuilder.Create(_T("sys_msg_item_org_request.xml"),
			(UINT)0, NULL, m_pPaintManager));
	}
	else
	{
		pListElement = static_cast<CListContainerElementUI*>(dlgBuilder.Create((UINT)0, m_pPaintManager));
	}
	if (pListElement == NULL)
		return;

	//tips
	CLabelUI *pLabelTips = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, "labelRequestTips"));
	if (pLabelTips != NULL)
	{
		pLabelTips->SetText(strTips.c_str());
	}

	//time
	CLabelUI *pLabelTime = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, "labelRecvtime"));
	if (pLabelTime != NULL)
	{
		pLabelTime->SetText((GetFullTimeString(uTime)).c_str());
	}

	sSysMsg* pCacheSysMsg = new sSysMsg;
	pCacheSysMsg->eSubType = eSubType;
	if (eSubType != enmSysMsgSubType_AddOrgFriend)
	{
		CButtonUI *pBtnAgree = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, "ButtonAgreedOrg"));
		if (pBtnAgree != NULL)
		{
			pBtnAgree->SetVisible(false);
		}

		CButtonUI *pBtnDeny = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, "ButtonDeny"));
		if (pBtnDeny != NULL)
		{
			pBtnDeny->SetVisible(false);
		}
	}
	pCacheSysMsg->strMsg = strTips;
	pCacheSysMsg->strSender = strSender;
	pCacheSysMsg->uTime = uTime;

	pListElement->SetTag((INT_PTR)pCacheSysMsg);
	pListElement->SetFixedHeight(kContactListItemSelectedHeight);
	pListElement->SetName("System_Item");
	if (NULL != pListElement)
	{
		if (-1 == indx)
		{
			if (!CListUI::Add(pListElement))
			{
				delete pListElement;
			}
		}
		else if(indx >= 0)
		{
			if (!CListUI::AddAt(pListElement,indx))
			{
				delete pListElement;
			}
		}

	}
}

//遍历List item
void CListSysMsg::RemoveItem(int nItemID)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
			if (pListItem->GetTag() == nItemID)
			{
				sSysMsg* pSysMsg = (sSysMsg*)(pListItem->GetTag());
				if (NULL != pSysMsg)
				{
					delete pSysMsg;
					pSysMsg = NULL;
				}
				Remove(pListItem);
				break;
			}
		}
	}
}

void CListSysMsg::RemoveItem(sSysMsg* pSysMsg)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
			if (pListItem != NULL)
			{
				sSysMsg* pLocalMsg = (sSysMsg*)(pListItem->GetTag());
				if (NULL != pLocalMsg)
				{
					if ((_tcsicmp(pSysMsg->strSender.c_str(), pLocalMsg->strSender.c_str()) == 0)
						&& pSysMsg->uTime == pLocalMsg->uTime
						&& pSysMsg->eSubType == pLocalMsg->eSubType)
					{
						delete pLocalMsg;
						pLocalMsg = NULL;
					}

					Remove(pListItem);
					break;
				}
			}
		}
	}
}


void CListSysMsg::DisableButton(sSysMsg* pSysMsg)
{
	int nItemCount = GetCount();
	for (int i=0; i<nItemCount; i++)
	{
		CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(i));
		sSysMsg* pLocal = (sSysMsg*)pElem->GetTag();
		if ((pLocal->eSubType == pSysMsg->eSubType) 
			&& (_tcsicmp(pLocal->strSender.c_str(), pSysMsg->strSender.c_str()) == 0)
			&& (pLocal->uTime == pSysMsg->uTime))
		{
			CButtonUI *pBtnAgree = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pElem, "ButtonAgreed"));
			if (pBtnAgree != NULL)
			{
				pBtnAgree->SetVisible(false);
			}

			CButtonUI *pBtnDeny = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pElem, "ButtonDeny"));
			if (pBtnDeny != NULL)
			{
				pBtnDeny->SetVisible(false);
			}

			CButtonUI *pBtnAgreeOrg = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pElem, "ButtonAgreedOrg"));
			if (pBtnAgreeOrg != NULL)
			{
				pBtnAgreeOrg->SetVisible(false);
			}
		}
	}
}

void CListSysMsg::CacheSysMsg(sSysMsg& sysMsg)
{

}



