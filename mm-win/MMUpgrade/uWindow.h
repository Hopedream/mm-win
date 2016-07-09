#pragma once
#include "..\DuiLib\UIlib.h"
#include "oss_event.h"
#include "oss_interface.h"
#include "FileHelper.h"
using namespace DuiLib;
using namespace std;

struct fileinfo
{
	string strFileName;
	string strFolder; //需要更新到哪个本地文件夹
	string strUrl; //下载地址
	int dwFileSize;
	
	//string strKey;
};

struct newversionfilelist
{
	string strVersion;
	vector<fileinfo> vecFiles;
};

class uWindow : public WindowImplBase, public IOssEventBase
{
public:
	uWindow(){};
	virtual ~uWindow(){ PostQuitMessage(0);};

public:

	virtual void OnFinalMessage( HWND hWnd );	
	void OnClick(TNotifyUI& msg);
	virtual UILIB_RESOURCETYPE GetResourceType() const; 
	virtual CDuiString GetSkinFolder(); 
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void Notify(TNotifyUI& msg);	

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnProcess(string& fileName, int iValue, string &strKey);
protected:
    static unsigned  __stdcall uWindow::ThreadFuncDownloadAllFiles(void* lpParamter);
	newversionfilelist* pNewVersion; 
private:
	void downloadFiles();
	int DoDownload();
    void CopyFiles();
public:
	virtual void CreateWnd(HWND hWnd = NULL);
	void SetTopMostWnd(bool bTop = true);
	void SetForegroundWnd(); //设置前景窗口
	void ShowToolTipBox();
	void DoUpdate();
	void DoUpload(string strVersion);
};

