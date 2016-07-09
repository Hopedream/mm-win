#pragma once

#include "StdAfx.h"
#include <string>
#include <vector>
#include "ProjectProcessor.h"

using namespace DuiLib;
using namespace std;

class CAddNewGroupMember;
class CProjectListView : public WindowImplBase
{
public:
	CProjectListView(CPaintManagerUI* ppm = NULL);
	virtual ~CProjectListView();

	//ע�⣺���������const���ͻᱨ˵������Ȼ�����ࡣ
	virtual LPCTSTR GetWindowClassName()const	{	return _T("project_list_view");				}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");					}
	virtual CDuiString GetSkinFile()			{	return "bussiness_listview.xml";			}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;								}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);


	void CreateWnd();
	bool Initialize(tstring& strOrgIdOrAcount, bool bOrgID);
	bool FetchProjects();
protected:
	void RemoveAllListItems();
	void AddListItem(int nFromIndex, int nCount);
	void UpdateParas(int nCount);
	void UpdatePageButton();
	void GotoPage(int nPageIndex);
	bool IsCacheEmpty();
	bool IsFirstPage();
	bool IsEndPage();

protected:
	CButtonUI* m_pBtnNextPage;
	CButtonUI* m_pBtnEndPage;
	CButtonUI* m_pBtnPrevPage;
	CListUI* m_pListProjs;
	vector<sProjectItem> m_vecProjects;
	CProjectProcessor* m_pProjProcessor;
	//CAddNewGroupMember* m_pAddGroupMemWnd;
	int m_nCurrentPageIndex;
	int m_nEndPageIndex;

	tstring m_strCurrAccount;//�����������ֻ���
	bool m_bOrgId;

	HWND m_hMainWnd;
};