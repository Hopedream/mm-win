#pragma once
// 此处需要包含duilib的头文件#include <UIlib.h>那一段，详细代码请见前面教程
#include "StdAfx.h"
#include "oss_interface.h"
#include "HttpClient.h"
#include "oss_event.h"
#include "UIDefine.h"

class UIUPdateFiles : public WindowImplBase, 
					  public IOssEventBase
{
protected:
	// 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	virtual ~UIUPdateFiles();
	//{
	//	//DeleteCriticalSection(&m_cs);

	//	delete m_pTileLayout; 
	//	m_pTileLayout=NULL;
	//};  

	CDuiString  m_strXMLPath;
	CTileLayoutUI* m_pTileLayout;
	vector<fileInfo> m_lstFiles;
	//map<CDuiString, CDuiString> m_lstFiles;
	//vector<CDuiString> m_sltFilepath;
	//map<CDuiString, f>

	CRITICAL_SECTION m_cs;  
	ossInterface m_ossControl;
	CHttpClient m_Http;
	static unsigned  __stdcall  SecondThreadFunc( void*pArguments) ; 
	int m_iTaskNum;   //总任务数量
	int m_iCurTaskNum;   //当前完成任务数量
	bool m_bAdd;         //是否为追加上传
	tstring m_strPackID;   //当前任务packid
	bool m_bStarted;           //任务是否开始
private:
	int initOSS();
	void uploadSignalFile(fileInfo& infoFile);
	tstring UrlEncode(const std::tstring& szToEncode);
public:

	explicit UIUPdateFiles(LPCTSTR pszXMLPath);

	virtual LPCTSTR    GetWindowClassName()const{ return _T("UIChoseFile "); }
	virtual CDuiString GetSkinFolder()          { return _T("");            }
	virtual CDuiString GetSkinFile()            { return m_strXMLPath;      }
	virtual void       OnFinalMessage(HWND hWnd){ delete this;              }
	virtual void Notify(TNotifyUI& msg);
	virtual void OnProcess(tstring& fileName, int iValue);
	void Init(HWND hWndParent, POINT ptPos)
	{
		Create(hWndParent, _T("updateFiles"), UI_WNDSTYLE_CHILD, WS_EX_WINDOWEDGE);
		//::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		if(m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))                                                                                                                                                                                                            
		{
			return lRes;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void addItem(vector<tstring>& lstfiles);
	int getItemIndex(tstring&);
	void setItemProcess(tstring &strPath,int iValue);
	void setItemBkImage(tstring &strPath, tstring & strImage);
	void setItemText(tstring &strPath, tstring& strTxt);
	int getItemProcess(tstring &strPath);
	void uploadFiles();
	void RemoveAll();
};

