#pragma once
#include "StdAfx.h"

namespace DuiLib
{
	struct DataListNodeInfo
	{
		CDuiString m_strLogo;
		CDuiString m_strType;
		CDuiString m_strTime;
	};
	class CDataListUI : public CListUI
	{
	public:
		CDataListUI(CPaintManagerUI *paint_manager);
		~CDataListUI();
		bool AddItem(DataListNodeInfo* pNodeInfo);
		bool AddOfferItem();    //询报价
		bool AddConsultationItem();   //技术咨询
	private:
		CControlUI *createOfferControl();
		CControlUI *createConsultationControl();
	private:
		CPaintManagerUI* m_pPaintManager;
		//CDialogBuilder m_DlgBuilder;
		//CListContainerElementUI* pControl, CTreeNodeUI* pParent=NULL
	};
}