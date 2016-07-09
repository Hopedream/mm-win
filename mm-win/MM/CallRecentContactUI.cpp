#include "CallRecentContactUI.h"

namespace DuiLib
{
	const int kContactListItemNormalHeight = 40;
	const TCHAR* const kOrgNameLabel = _T("orgName");
	const TCHAR* const kNickNameLabel = _T("nickname");
	const TCHAR* const kPhoneNumberLabel = _T("phoneNumber");
	const TCHAR* const kLastCallTimeLabel = _T("lastCallTime");
	const TCHAR* const kLogoControl = _T("logo");
	const TCHAR* const kVedioCallBtnName = _T("VoIPCall");
	const TCHAR* const kVoiceCallBtnName = _T("smartPhoneCall");

	DuiLib::CRecentCallContactUI::CRecentCallContactUI( CPaintManagerUI *paint_manager )
		:m_pPaintManager(paint_manager)
	{
		SetItemShowHtml(true);
	}
	DuiLib::CRecentCallContactUI::~CRecentCallContactUI()
	{

	}
	bool DuiLib::CRecentCallContactUI::Add( CControlUI *pControl )
	{
		if (!pControl)
			return false;
		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;
		return CListUI::Add(pControl);
	}
	bool DuiLib::CRecentCallContactUI::AddAt( CControlUI *pControl, int iIndex )
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::AddAt(pControl, iIndex);
	}

	bool DuiLib::CRecentCallContactUI::Remove( CControlUI *pControl )
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;
		return CListUI::Remove(pControl);		
	}

	bool DuiLib::CRecentCallContactUI::RemoveAt( int iIndex )
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::RemoveAt(iIndex);
	}

	void DuiLib::CRecentCallContactUI::RemoveAll()
	{
		CListUI::RemoveAll();
	}

	void DuiLib::CRecentCallContactUI::DoEvent( TEventUI& event )
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

	void DuiLib::CRecentCallContactUI::AddItem( recentCallContactItem *item, int indx/*=-1*/ )
	{
		CListContainerElementUI *pListElement = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("Call\\recent_call_contacts_list_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (pListElement == NULL)
			return;

		CControlUI* pBtnLogo = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pListElement, kLogoControl));
		if (pBtnLogo != NULL)
		{			
			pBtnLogo->SetBkImage(item->m_strLogo);			
		}

		CLabelUI *pLblNickName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kNickNameLabel));
		if (pLblNickName != NULL)
		{			
			pLblNickName->SetText(item->m_strNickName);
		}

		CLabelUI *pOrgNameLabel = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, kOrgNameLabel));
		if (pOrgNameLabel != NULL)
		{			
			pOrgNameLabel->SetText(item->m_strOrgName);
		}

		CControlUI *pDialIndicate = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pListElement, "dialIndicateLogo"));
		if (pDialIndicate != NULL)
		{			
			CDuiString strImg;
			strImg.Format("Call\\call%d.png", item->dialIndicate);
			pDialIndicate->SetBkImage(strImg);
		}
			

		pListElement->SetTag(item->nItemID);
		pListElement->SetFixedHeight(kContactListItemNormalHeight);

		CButtonUI* pBtnVoiceCall= static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, kVoiceCallBtnName));
		CButtonUI* pBtnVedioCall= static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pListElement, kVedioCallBtnName));
		if (NULL != pBtnVoiceCall)  pBtnVoiceCall->SetTag(item->nItemID);
		if (NULL != pBtnVedioCall)  pBtnVedioCall->SetTag(item->nItemID);


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

	




