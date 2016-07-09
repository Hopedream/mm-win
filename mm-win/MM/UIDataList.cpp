#include "UIDataList.h"

namespace DuiLib
{
	const int kDataListItemHeight = 29;
	const TCHAR* const kbuttonTypeControlName = _T("buttonType");
	const TCHAR* const klabelTypeControlName = _T("labelType");
	const TCHAR* const klabelTimeControlName = _T("labelTime");
	const TCHAR* const kcontrolShowControlName = _T("controlShow");
	/*const TCHAR* const kbuttonAccepteControlName = _T("buttonAccept");
	const TCHAR* const kbuttonRejectControlName = _T("buttonReject");
	const TCHAR* const kcontrolOfferControlName = _T("controlOffer");*/

	DuiLib::CDataListUI::CDataListUI( CPaintManagerUI *paint_manager )
	{
		m_pPaintManager = paint_manager;
	}


	DuiLib::CDataListUI::~CDataListUI()
	{
		
	}


	bool DuiLib::CDataListUI::AddItem(DataListNodeInfo* pNodeInfo)
	{
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *pTreeNode = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pTreeNode = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("tree_node.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pTreeNode = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (pTreeNode == NULL)
			return false;

		CButtonUI *pbuttonType = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pTreeNode, kbuttonTypeControlName));
		if (pbuttonType!=NULL)
		{
			pbuttonType->SetBkImage(pNodeInfo->m_strLogo);
		}

		CLabelUI *plabelType = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pTreeNode, klabelTypeControlName));
		if (plabelType != NULL)
		{
			plabelType->SetText(pNodeInfo->m_strType);
		}
		CLabelUI *plabelTime = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pTreeNode, klabelTimeControlName));
		if (plabelTime != NULL)
		{
			plabelTime->SetText(pNodeInfo->m_strTime);
		}

		CControlUI *pcontrolShow = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pTreeNode, kcontrolShowControlName));
		if (pcontrolShow != NULL)
		{
			pcontrolShow->SetBkImage("arrow1.png");
		}

		pTreeNode->SetFixedHeight(kDataListItemHeight);

		if (!CListUI::Add(pTreeNode))
		{
			delete pTreeNode;
		}
		//m_DlgBuilder.GetMarkup()->Release();
	}

	bool DuiLib::CDataListUI::AddOfferItem()
	{
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *pTreeNode = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pTreeNode = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("data_offer_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pTreeNode = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		//CContainerUI *pUi = static_cast<CContainerUI*>(m_pPaintManager->FindControl(_T("Containeroffer")));
		//pUi->Add(createOfferControl());
		//pUi->
		if (pTreeNode == NULL)
			return false;

		pTreeNode->SetFixedHeight(195);

		if (!CListUI::Add(pTreeNode))
		{
			delete pTreeNode;
		}
		//m_DlgBuilder.GetMarkup()->Release();
		return 1;
	}

	CControlUI* DuiLib::CDataListUI::createOfferControl()
	{
		CDialogBuilder m_DlgBuilder;
		CContainerUI *pOfferControl = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pOfferControl = static_cast<CContainerUI*>(m_DlgBuilder.Create(_T("price_list.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pOfferControl = static_cast<CContainerUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (pOfferControl == NULL)
			return NULL;

		//pTreeNode->SetFixedHeight(195);

		return pOfferControl;
	}

	CControlUI* DuiLib::CDataListUI::createConsultationControl()
	{
		return NULL;
	}

	bool DuiLib::CDataListUI::AddConsultationItem()
	{
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *pTreeNode = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pTreeNode = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("data_consult_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			pTreeNode = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (pTreeNode == NULL)
			return false;

		pTreeNode->SetFixedHeight(110);

		if (!CListUI::Add(pTreeNode))
		{
			delete pTreeNode;
		}
		return 1;
	}


}