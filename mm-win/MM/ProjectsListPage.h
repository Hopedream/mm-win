#pragma once

#include "StdAfx.h"
#include <string>
#include <vector>
#include "ProjectProcessor.h"

using namespace DuiLib;
using namespace std;

class CProjectProcessor;
class CAddNewGroupMember;
class CProjectInfoPage : public CVerticalLayoutUI, public INotifyUI
{
public:
	CProjectInfoPage(CPaintManagerUI* ppm = NULL);
	virtual ~CProjectInfoPage();

	bool Initialize();
	void RefreshPage();
	virtual void Notify(TNotifyUI& msg) ;
	void FetchProjects(tstring& strStartDataTime,const tstring& strSearch = "");

protected:
	void HandleWindowInitEvent( TNotifyUI& msg );
	void RemoveAllListItems();
	void AddListItem(int nFromIndex, int nCount);
	void UpdateParas(int nCount);
	void UpdatePageButton();
	void GotoPage(int nPageIndex);
	bool IsCacheEmpty();
	bool IsFirstPage();
	bool IsEndPage();
protected:
	DuiLib::CPaintManagerUI* m_PaintManager;
	CButtonUI* m_pBtnFirstpage;
	CButtonUI* m_pBtnNextPage;
	CButtonUI* m_pBtnEndPage;
	CButtonUI* m_pBtnPrevPage;
	CListUI* m_pListProjs;
	vector<sProjectItem> m_vecProjects;
protected:
	CProjectProcessor* m_pProjProcessor;
	//CAddNewGroupMember* m_pAddGroupMemWnd;
	int m_nCurrentPageIndex;
	int m_nEndPageIndex;
};