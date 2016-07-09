#pragma once
// �˴���Ҫ����duilib��ͷ�ļ�#include <UIlib.h>��һ�Σ���ϸ�������ǰ��̳�
#include "StdAfx.h"
#include "oss_interface.h"
//#include "HttpClient.h"
//#include "oss_event.h"
#include "UIDefine.h"

class UIDownloadFiles : public WindowImplBase, 
	public IOssEventBase
{
protected:
	// ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������
	virtual ~UIDownloadFiles()
	{
		//DeleteCriticalSection(&m_cs);

		delete m_pTileLayout; 
		m_pTileLayout=NULL;
	};  

	CDuiString  m_strXMLPath;
	CTileLayoutUI* m_pTileLayout;
	vector<fileInfo> m_lstFiles;
	//map<CDuiString, CDuiString> m_lstFiles;
	//vector<CDuiString> m_sltFilepath;
	//map<CDuiString, f>

	CRITICAL_SECTION m_cs;  
	//ossInterface m_ossControl;
	//CHttpClient m_Http;
	static unsigned  __stdcall  SecondThreadFunc( void*pArguments) ; 
	int m_iTaskNum;   //����������
	int m_iCurTaskNum;   //��ǰ�����������
	vector<fileInfo> m_lstDownloadFiles;
	tstring m_strFilePath;          //�����ļ�·�������ڴ��ļ�λ��
private:
	int initOSS();
	int downloadFile();
public:
	explicit UIDownloadFiles(LPCTSTR pszXMLPath);

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

	void addItem(vector<fileInfo>& lstfiles);
	int getItemIndex(tstring&);
	void setItemProcess(tstring &strPath,int iValue);
	void setItemBkImage(tstring &strPath, tstring & strImage);
	void setItemText(tstring &strPath, tstring& strTxt);
	int getItemProcess(tstring &strPath);
	void hideItemProcess(tstring &strPath);
	void downloadFiles();
	void RemoveAll();
	static unsigned  __stdcall  DownLoadThreadFunc( void*pArguments) ; 
};
