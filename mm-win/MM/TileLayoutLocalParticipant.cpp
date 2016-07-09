#include "TileLayoutLocalParticipant.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include <sstream>

CTileLayoutLocalParticipant::CTileLayoutLocalParticipant(CPaintManagerUI* mmPm)
{
	m_pPaintManager = mmPm;
}

CTileLayoutLocalParticipant::~CTileLayoutLocalParticipant()
{
	//m_pPaintManager->RemoveNotifier(this);
}

void CTileLayoutLocalParticipant::GetLocalParticipant(vector<tstring>& vecParticipant)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("local_participant_option")));
				if (NULL != pOption)
				{
					if(pOption->IsSelected())
					{
						vecParticipant.push_back(pFriend->strAccount);
					}
				}
				
			}
		}
	}
}

void CTileLayoutLocalParticipant::InitSelectState(tstring& strAccount)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				if(_tcsicmp(pFriend->strAccount.c_str(), strAccount.c_str()) == 0)
				{
					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("local_participant_option")));
					if (NULL != pOption)
					{
						pOption->Selected(true);
						break;
					}
				}
			}
		}
	}
}
void CTileLayoutLocalParticipant::DeSelectItem(/*tstring& strAccount*/int nSrcItemID)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				if(pFriend->nItemID == nSrcItemID)
				{
					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("local_participant_option")));
					if (NULL != pOption)
					{
						pOption->Selected(false);
						break;
					}
				}
			}
		}
	}
}

void CTileLayoutLocalParticipant::AddOneItem(sFriendListItem* pOneFriend, vector<tstring>& vecProperty)
{
	SetItemSize(CDuiSize(210, 37));

 	CDialogBuilder builder;
 	CContainerUI* pOneParticipant = static_cast<CContainerUI*>(builder.Create(_T("create_project_local_participant_item.xml"), (UINT)0));
 	if( pOneParticipant != NULL ) 
 	{
 		if( pOneParticipant == NULL ) 
 			pOneParticipant = static_cast<CContainerUI*>(builder.Create());
 
 		if( pOneParticipant != NULL ) 
 		{
			pOneFriend->nItemID = GetNextItemID();
			pOneParticipant->SetTag((UINT_PTR)pOneFriend);
 			this->Add(pOneParticipant);
 		}
 		else 
 		{
 			this->RemoveAll();
 			return;
 		}

		//1.设置姓名
		CLabelUI *pLableFileName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneParticipant, _T("local_participant_name")));
		if (NULL != pLableFileName)
		{
			pLableFileName->SetText(pOneFriend->strShowName.c_str());
		}
		
		//2.设置属性
		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "local_participant_property_" << i;
			CLabelUI *pLabelProperty = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneParticipant, ostr.str().c_str()));
			if (NULL != pLabelProperty)
			{
				if (i>vecProperty.size())
				{
					pLabelProperty->SetVisible(false);
				}
				else
				{
					pLabelProperty->SetText(vecProperty[i-1].c_str());
					pLabelProperty->SetVisible(true);
				}
			}
		}
 	}
}

 void CTileLayoutLocalParticipant::DoEvent(TEventUI& event)
 {
// 	/*if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
// 	if( m_pParent != NULL ) m_pParent->DoEvent(event);
// 	else CTileLayoutUI::DoEvent(event);
// 	return;
// 	}
// 
// 	if( event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID )
// 	{
// 	if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
// 	POINT pt = m_pManager->GetMousePos();
// 	LONG cy = (pt.y - m_ptLastMouse.y);
// 	m_ptLastMouse = pt;
// 	SIZE sz = GetScrollPos();
// 	sz.cy -= cy;
// 	SetScrollPos(sz);
// 	return;
// 	}
// 	else if( m_dwDelayLeft > 0 ) {
// 	--m_dwDelayLeft;
// 	SIZE sz = GetScrollPos();
// 	LONG lDeltaY =  (LONG)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
// 	if( (lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ) ) {
// 	sz.cy -= lDeltaY;
// 	SetScrollPos(sz);
// 	return;
// 	}
// 	}
// 	m_dwDelayDeltaY = 0;
// 	m_dwDelayNum = 0;
// 	m_dwDelayLeft = 0;
// 	m_pManager->KillTimer(this, SCROLL_TIMERID);
// 	return;
// 	}
// 	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
// 	{
// 	m_uButtonState |= UISTATE_CAPTURED;
// 	m_ptLastMouse = event.ptMouse;
// 	m_dwDelayDeltaY = 0;
// 	m_dwDelayNum = 0;
// 	m_dwDelayLeft = 0;
// 	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
// 	m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
// 	return;
// 	}
// 	if( event.Type == UIEVENT_BUTTONUP )
// 	{
// 	if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
// 	m_uButtonState &= ~UISTATE_CAPTURED;
// 	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
// 	if( m_ptLastMouse.y != event.ptMouse.y ) {
// 	m_dwDelayDeltaY = (event.ptMouse.y - m_ptLastMouse.y);
// 	if( m_dwDelayDeltaY > 120 ) m_dwDelayDeltaY = 120;
// 	else if( m_dwDelayDeltaY < -120 ) m_dwDelayDeltaY = -120;
// 	m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 5;
// 	m_dwDelayLeft = m_dwDelayNum;
// 	}
// 	else 
// 	m_pManager->KillTimer(this, SCROLL_TIMERID);
// 	}
// 	return;
// 	}
// 	if( event.Type == UIEVENT_SCROLLWHEEL )
// 	{
// 	LONG lDeltaY = 0;
// 	if( m_dwDelayNum > 0 ) lDeltaY =  (LONG)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
// 	switch( LOWORD(event.wParam) ) {
// 	case SB_LINEUP:
// 	if( m_dwDelayDeltaY >= 0 ) m_dwDelayDeltaY = lDeltaY + 8;
// 	else m_dwDelayDeltaY = lDeltaY + 12;
// 	break;
// 	case SB_LINEDOWN:
// 	if( m_dwDelayDeltaY <= 0 ) m_dwDelayDeltaY = lDeltaY - 8;
// 	else m_dwDelayDeltaY = lDeltaY - 12;
// 	break;
// 	}
// 	if( m_dwDelayDeltaY > 100 ) m_dwDelayDeltaY = 100;
// 	else if( m_dwDelayDeltaY < -100 ) m_dwDelayDeltaY = -100;
// 	m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 5;
// 	m_dwDelayLeft = m_dwDelayNum;
// 	m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
// 	return;
// 	}*/
 	CTileLayoutUI::DoEvent(event);
 }

//void CTileLayoutLocalParticipant::Notify(TNotifyUI& msg)
//{
//	if (msg.sType != DUI_MSGTYPE_CLICK)
//	{
//		return;
//	}
//
//	if (_tcsicmp(msg.pSender->GetName(), _T("local_participant_option")) == 0)
//	{
//		int abc=100;
//	}
//}