#pragma once
#include "mmwinimplbase.h"
#include "ProjectProcessor.h"
#include "UIDefine.h"
#include "UIBussinessDataList.h"
#include "FileControl.h"

struct sProjectInfo
{
	tstring strProjectName;
	tstring strProperty;
	tstring strTime;
	tstring strSender;
	tstring strRecipient;
	vector<fileInfo> m_vcFiles;
};

struct strItemInfo
{
	tstring strPackid;
	tstring strPackName;
	tstring strFileName;
};




class CFrameBusinessData :
	public MMWindowImplBase
	, public IOssEventBase
{
public:
	CFrameBusinessData();
	~CFrameBusinessData(void);
	static CFrameBusinessData* Instance();
	void setProjectItem(sProjectItem*);
public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleSelectedChangedEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual void HandleItemClickEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

	virtual void OnProcess(tstring& fileName, int iValue, tstring& strKey);
	CUIBussinessDataList* getFavoritelst();
private:
	void initTabs();
	void initContract();
	void initFavorite();
	void initTrans();
	void addContractItem(sProjectInfo&);
	void addContractChildItem(fileInfo&, CListContainerElementUI*);
	void updateWindow();

private:
	static CFrameBusinessData* m_pInstance;
	CTabLayoutUI*   m_pTabLayout;
	CListUI*          m_pLstContract;
	CUIBussinessDataList*          m_pLstFavorite;
	CUIBussinessDataList*          m_pLstTrans;
	sProjectItem*     m_pProItem;
	vector<sProjectInfo> m_vcContractItems;
	tstring m_strOrgName;
	CFileControl m_FileControl;   //文件上传下载类
	vector<fileInfo> m_vecFiles;
	map<tstring, CButtonUI*> m_mapBtnOpen;
};

