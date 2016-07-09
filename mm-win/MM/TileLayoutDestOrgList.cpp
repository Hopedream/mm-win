#include "TileLayoutDestOrgList.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include "..\Common\UserInfoDef.h"

CTileLayoutDestOrgList::CTileLayoutDestOrgList(CPaintManagerUI* ppm)
{
	m_pPaintManager = ppm;
}

CTileLayoutDestOrgList::~CTileLayoutDestOrgList()
{

}

bool CTileLayoutDestOrgList::Add( CControlUI *pControl )
{
	if (!pControl)
		return false;
	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;
	return CListUI::Add(pControl);
}
bool CTileLayoutDestOrgList::AddAt( CControlUI *pControl, int iIndex )
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool CTileLayoutDestOrgList::Remove( CControlUI *pControl )
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;
	return CListUI::Remove(pControl);		
}
bool CTileLayoutDestOrgList::RemoveAt( int iIndex )
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::RemoveAt(iIndex);
}
void CTileLayoutDestOrgList::RemoveAll()
{
	CListUI::RemoveAll();
}
void CTileLayoutDestOrgList::DoEvent( TEventUI& event )
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


//注意：create_project_opposite_org_item.xml节点必须是ListContainerElement属性，
//不像create_project_opposite_employee.xml这些节点都是Container属性。
//否则，CTileLayoutDestOrgList不会收到itemselect消息。
/*void CTileLayoutDestOrgList::AddOneItem(int nOrgID, tstring& strAvatar, tstring& strOrgName, tstring& strOnlineMems)
{
	//SetItemSize(CDuiSize(185, 30));

	CListContainerElementUI *pListElement = NULL;
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("create_project_opposite_org_item.xml"),
			(UINT)0, NULL, m_pPaintManager));
	}
	else
	{
		pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
	}
	if (pListElement == NULL)
		return;


	
	//1. avatar
	CControlUI* pOrgAvatar = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pListElement, _T("control_dest_org_avatar")));
	if (NULL != pOrgAvatar)
	{
		pOrgAvatar->SetBkImage(strAvatar.c_str());
	}

	//2. 设置名字
	CLabelUI *pOrgName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, _T("label_dest_org_name")));
	if (NULL != pOrgName)
	{
		pOrgName->SetText(strOrgName.c_str());
	}

	//3. 设置在线人数
	CLabelUI* pLabelOrgMembers = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, _T("label_dest_org_members")));
	if (NULL != pLabelOrgMembers)
	{
		pLabelOrgMembers->SetText(strOnlineMems.c_str());
	}

	pListElement->SetTag(nOrgID);
	pListElement->SetFixedHeight(30);

	if (NULL != pListElement)
	{
		if (!CListUI::Add(pListElement))
		{
			delete pListElement;
		}
	}
}*/

void CTileLayoutDestOrgList::AddOneItem(tstring& nOrgID, tstring& strAvatar, tstring& strOrgName, tstring& strOnlineMems)
{
	//SetItemSize(CDuiSize(185, 30));

	CListContainerElementUI *pListElement = NULL;
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("create_project_opposite_org_item.xml"),
			(UINT)0, NULL, m_pPaintManager));
	}
	else
	{
		pListElement = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
	}
	if (pListElement == NULL)
		return;



	//1. avatar
	CControlUI* pOrgAvatar = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pListElement, _T("control_dest_org_avatar")));
	if (NULL != pOrgAvatar)
	{
		pOrgAvatar->SetBkImage(strAvatar.c_str());
	}

	//2. 设置名字
	CLabelUI *pOrgName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, _T("label_dest_org_name")));
	if (NULL != pOrgName)
	{
		pOrgName->SetText(strOrgName.c_str());
	}

	//3. 设置在线人数
	CLabelUI* pLabelOrgMembers = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pListElement, _T("label_dest_org_members")));
	if (NULL != pLabelOrgMembers)
	{
		pLabelOrgMembers->SetText(strOnlineMems.c_str());
	}
	
	char * str = new char[nOrgID.length() + 1];
	sprintf(str,nOrgID.c_str());
	pListElement->SetTag((UINT_PTR)str);
	pListElement->SetFixedHeight(30);

	if (NULL != pListElement)
	{
		if (!CListUI::Add(pListElement))
		{
			delete pListElement;
		}
	}
}