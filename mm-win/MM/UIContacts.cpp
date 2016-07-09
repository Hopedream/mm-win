#include "UIContacts.h"

namespace DuiLib
{
	const int kContactListItemNormalHeight = 50;
	const int kContactListItemSelectedHeight = 60;

	const TCHAR* const kButtonLogoControlName = _T("btnLogo");
	const TCHAR* const kLabelNickNameControlName = _T("lblNickName");
	const TCHAR* const kLabelLastMsgControlName = _T("lblLastMsg");
	const TCHAR* const kLabelLastTmControlName = _T("lblLastTm");
	const TCHAR* const kLabelMsgCountControlName = _T("lblMsgCount");

	DuiLib::CContactsUI::CContactsUI( CPaintManagerUI *paint_manager )
		:m_pPaintManager(paint_manager)
	{
		SetItemShowHtml(true);
	}
	DuiLib::CContactsUI::~CContactsUI()
	{

	}
	bool DuiLib::CContactsUI::Add( CControlUI *pControl )
	{
		if (!pControl)
			return false;
		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;
		return CListUI::Add(pControl);
	}
	bool DuiLib::CContactsUI::AddAt( CControlUI *pControl, int iIndex )
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::AddAt(pControl, iIndex);
	}

	bool DuiLib::CContactsUI::Remove( CControlUI *pControl )
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;
		return CListUI::Remove(pControl);		
	}
	bool DuiLib::CContactsUI::RemoveAt( int iIndex )
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::RemoveAt(iIndex);
	}
	void DuiLib::CContactsUI::RemoveAll()
	{
		CListUI::RemoveAll();
	}
	void DuiLib::CContactsUI::DoEvent( TEventUI& event )
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

	void DuiLib::CContactsUI::AddItem( ContactListItemInfo *item, int indx/*=-1*/ )
	{
		CListContainerElementUI *pListElement = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("recent_chat_list_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (pListElement == NULL)
			return;

		CButtonUI* pBtnLogo = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, kButtonLogoControlName));
		if (pBtnLogo != NULL)
		{
			if (!item->m_strLogo.IsEmpty())
			{
				//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item->m_strLogo);
				pBtnLogo->SetNormalImage(item->m_strLogo);
			}
		}

		//CDuiString strNickName;
		CLabelUI *pLblNickName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelNickNameControlName));
		if (pLblNickName != NULL)
		{
			//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item->m_strNickName);
			//strNickName += szBuf;
			//nick_name->SetShowHtml(true);
			
			pLblNickName->SetText(item->m_strAccount.IsEmpty()?item->m_strAccount.GetData(): item->m_strNickName.GetData()/*m_strNickName*/);
		}

		//CDuiString strLastMsg;

		CLabelUI *pLblLastMsg = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelLastMsgControlName));
		if (pLblLastMsg != NULL)
		{
			if ( (item->m_strLastMsg.Find("#COMMAND#type=fileRequest") == -1) && (item->m_strLastMsg.Find("#COMMAND#type=filedownloaded") == -1) 
				&& (item->m_strLastMsg.Find("#COMMAND#type=filerejected") == -1))
				pLblLastMsg->SetText(item->m_strLastMsg);
		}

		//CDuiString strLastMsg;
		CLabelUI *pLblLastTm = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelLastTmControlName));
		if (pLblLastTm != NULL)
		{
			pLblLastTm->SetText(item->m_strLastTm);
		}

		CLabelUI *pLblMsgCount = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelMsgCountControlName));
		if (pLblMsgCount != NULL)
		{
			if (item->m_strMsgCount == "0" || item->m_strMsgCount == "")
				pLblMsgCount->SetVisible(false);
			pLblMsgCount->SetText(item->m_strMsgCount);
		}

		pListElement->SetTag(item->nItemID);
		pListElement->SetFixedHeight(kContactListItemNormalHeight);
		pListElement->SetName(item->m_strAccount);
		if (item->nItemID == 10000)
		{
			pListElement->SetName("System_WelCome_Item");
		}
		
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

	void CContactsUI::AddWelcomeItem(ContactListItemInfo *item, int indx/* = -1*/)
	{
		CListContainerElementUI *pListElement = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("recent_chat_list_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (pListElement == NULL)
			return;

		CButtonUI* pBtnLogo = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, kButtonLogoControlName));
		if (pBtnLogo != NULL)
		{
			if (!item->m_strLogo.IsEmpty())
			{
				//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item->m_strLogo);
				pBtnLogo->SetNormalImage(item->m_strLogo);
			}
		}

		//CDuiString strNickName;
		CLabelUI *pLblNickName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelNickNameControlName));
		if (pLblNickName != NULL)
		{
			//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item->m_strNickName);
			//strNickName += szBuf;
			//nick_name->SetShowHtml(true);

			pLblNickName->SetText(item->m_strAccount.IsEmpty()?item->m_strAccount.GetData(): item->m_strNickName.GetData()/*m_strNickName*/);
		}

		//CDuiString strLastMsg;

		CLabelUI *pLblLastMsg = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelLastMsgControlName));
		if (pLblLastMsg != NULL)
		{
			if ( (item->m_strLastMsg.Find("#COMMAND#type=fileRequest") == -1) && (item->m_strLastMsg.Find("#COMMAND#type=filedownloaded") == -1)
				&& (item->m_strLastMsg.Find("#COMMAND#type=filerejected") == -1))
				pLblLastMsg->SetText(item->m_strLastMsg);
		}

		//CDuiString strLastMsg;
		CLabelUI *pLblLastTm = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelLastTmControlName));
		if (pLblLastTm != NULL)
		{
			pLblLastTm->SetText(item->m_strLastTm);
		}

		CLabelUI *pLblMsgCount = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLabelMsgCountControlName));
		if (pLblMsgCount != NULL)
		{
			if (item->m_strMsgCount == "0" || item->m_strMsgCount == "")
				pLblMsgCount->SetVisible(false);
			pLblMsgCount->SetText(item->m_strMsgCount);
		}

		pListElement->SetTag(item->nItemID);
		pListElement->SetFixedHeight(kContactListItemNormalHeight);
		pListElement->SetName("System_WelCome_Item");

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

	void CContactsUI::UpdateShowName(tstring& strShowName, int nIndex)
	{
		CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(nIndex));

		CLabelUI* pShowName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pElem, kLabelNickNameControlName));
		if (pShowName != NULL)
		{
			if (!strShowName.empty())
			{
				pShowName->SetText(strShowName.c_str());
			}
		}
	}
	void CContactsUI::UpdateHeadImage(const tstring& strImageFile, int index)
	{
		CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(index));
	
		CButtonUI* pBtnLogo = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pElem, kButtonLogoControlName));
		if (pBtnLogo != NULL)
		{
			if (!strImageFile.empty())
			{
				pBtnLogo->SetNormalImage(strImageFile.c_str());
			}
		}

	}

	void CContactsUI::SetLastMessage(tstring& strMsg, tstring& strTime,  int index)
	{
		CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(index));
		tstring strTemp;
		//最后一条消息
		if (strMsg.find("#COMMAND#type=fileRequest") != -1)
		{
			return;
		}
		else if (strMsg.find("#COMMAND#type=filedownloaded") != -1)
		{
			//tstring strTitle = "对方接收了您发送的文件";
			/* int nStart = strMsg.find("\"filename\":\"");
			int nEnd = strMsg.find(",\"from\":\"");
			if(nStart > 0 && nEnd > 0 && nEnd > nStart)
				fileName = strMsg.substr(nStart + strlen("\"filename\":\""),nEnd - nStart -strlen("\"filename\":\""));
			*/

			strTemp = "对方接收了您发送的文件";
		}
		else if (strMsg.find("#COMMAND#type=filerejected") != -1)
		{
			//tstring strTitle = "对方接收了您发送的文件";
			/* int nStart = strMsg.find("\"filename\":\"");
			int nEnd = strMsg.find(",\"from\":\"");
			if(nStart > 0 && nEnd > 0 && nEnd > nStart)
				fileName = strMsg.substr(nStart + strlen("\"filename\":\""),nEnd - nStart -strlen("\"filename\":\""));
			*/

			strTemp = "对方拒收了您发送的文件";
		}
		else
		{
			strTemp = strMsg;
		}

		CLabelUI *pLblLastMsg = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pElem, kLabelLastMsgControlName));
		if (pLblLastMsg != NULL)
		{
			pLblLastMsg->SetText(strTemp.c_str());
		}

		//时间
		CLabelUI *pLblLastTm = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pElem, kLabelLastTmControlName));
		if (pLblLastTm != NULL)
		{
			pLblLastTm->SetText(strTime.c_str());
		}

	}

	void CContactsUI::setMsgNum( CDuiString &strNum, int iIndex )
	{
		CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(iIndex));
		//pElem->GetItemAt()
		CLabelUI *pLblMsgCount = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pElem, kLabelMsgCountControlName));
		if (strNum == "0")
		{
			//pLblMsgCount->SetBkImage(_T(""));
			pLblMsgCount->SetVisible(false);
		}
		else
		{
			pLblMsgCount->SetVisible(true);
		}
		pLblMsgCount->SetText(strNum);	
	}

	void CContactsUI::topItem( int nItemID )
	{
		CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(nItemID));
		if (pElem != NULL)
			SetItemIndex(pElem, 0);
		//RemoveItem(nItemID);
		//AddAt(pElem, 0);
	}

}

bool CContactsUI::CheckIfBubbleExist(int nItemIndex)
{
	CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(nItemIndex));
	CLabelUI *pLblMsgCount = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pElem, kLabelMsgCountControlName));
	
	tstring strNumber = pLblMsgCount->GetText();
	if (strNumber.empty() || strNumber == tstring("0"))
	{
		return false;
	}

	return true;
}
//遍历List item
void CContactsUI::RemoveItem(int nItemID)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
			if (pListItem->GetTag() == nItemID)
			{
				Remove(pListItem);
				break;
			}
		}
	}
}
int CContactsUI::GetIndex(int nItemID)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
			if (pListItem->GetTag() == nItemID)
			{
				return i;
			}
		}
	}

	return -1;
}