#pragma once
#include "StdAfx.h"
#include <string>
#include <vector>
#include <time.h>
#include "ProjectProcessor.h"
#include "UIDefine.h"

using namespace DuiLib;
using namespace std;

class CTileLayoutResPacket;
#include "properties_resource.h"

struct JP_CONSTRACT_INFO
{
	tstring strTime ;
	tstring strType;
	tstring strMoney;
	tstring strNO;
	tstring strName;
	tstring strIdentity;
	tstring strMark;
	tstring strOrgid;
	vector<fileInfo> m_vecFiles;
	JP_CONSTRACT_INFO()
	{
		strTime = "";
		strMoney = "";
		strNO = "";
		strName = "";
		strIdentity = "";
		strMark = "";
		strType = "";
		strOrgid = "";
	}
	/* 该重载编译无效,原因不明
	bool operator==(JP_CONSTRACT_INFO& rhs) const 
	{
		return strNO == rhs.strNO;
	}
 
	bool operator > (JP_CONSTRACT_INFO& rhs) const 
	{
		return (_tcsicmp(strTime.c_str(),rhs.strTime.c_str()) > 0);
	}

	bool operator < (JP_CONSTRACT_INFO& rhs) const 
	{
		return (_tcsicmp(strTime.c_str(),rhs.strTime.c_str()) < 0);
	}*/
};
class JPContractSelect: public CVerticalLayoutUI, public INotifyUI
{
public:
	JPContractSelect(CPaintManagerUI* ppm = NULL);
	~JPContractSelect(void);
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;}
	virtual void Notify(TNotifyUI& msg);
	void		 InitData();
protected:
	void OnSearchButtonClick();
	void SelectItem(int nIndex,bool isSearch = false);
	void ShowItemInfo(JP_CONSTRACT_INFO* info);
	void CleanPage();
	void AddListItem(int nFromIndex, int nCount);
	void UpdatePageButton();
	void UpdateParas(int nCount);
	void GotoPage(int nPageIndex);
	bool IsCacheEmpty();
	bool IsFirstPage();
	bool IsEndPage();
	void RemoveAllListItems();
	bool GetContractInfo(vector<JP_CONSTRACT_INFO>& vecInfo,const tstring status = "1",const tstring type = "",const tstring& stime="",const tstring& etime="",const tstring& key="");
	void HideBtn();
protected:
	CPaintManagerUI* m_PaintManager;
	CButtonUI* m_pBtnFirstpage;
	CButtonUI* m_pBtnPrevPage;
	CButtonUI* m_pBtnNextPage;
	CButtonUI* m_pBtnEndPage;

	CLabelUI*	m_pLabelIdentityTag;
	CListUI*	m_pListInfo;
	CTileLayoutResPacket* m_pContractFiles;

	vector<JP_CONSTRACT_INFO> m_vecPayee;
	vector<JP_CONSTRACT_INFO> m_vecDrawee;
	map<tstring,tstring> dealType;
private:
	bool isPayee;
	int  m_nEndPageIndex;
	int m_nCurrentPageIndex;
};

