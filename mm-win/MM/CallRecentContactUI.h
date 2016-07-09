#pragma once
#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"


namespace DuiLib
{
	struct recentCallContactItem
	{
		int		   nItemID;
		int         dialIndicate;
		CDuiString m_strOrgName;	
		CDuiString m_strNickName;	//”√ªßÍ«≥∆
		CDuiString m_strLogo;		
		CDuiString m_strLastCallTime;		
		recentCallContactItem()
		{
			nItemID = -1;
		}
	};
	
	class CRecentCallContactUI :public CListUI
	{
	public:
		CRecentCallContactUI(CPaintManagerUI *paint_manager);
		~CRecentCallContactUI();

		void AddItem(recentCallContactItem *item, int indx = -1);
		bool RemoveAt(int iIndex);
		void RemoveAll();		
	private:
		CPaintManagerUI* m_pPaintManager;
		CDialogBuilder m_DlgBuilder;
		//CDuiRect       m_textPadding;
	private:
		bool Add(CControlUI *pControl);
		bool AddAt(CControlUI *pControl, int iIndex);
		bool Remove(CControlUI *pControl);		
		void DoEvent(TEventUI& event);
	};
}