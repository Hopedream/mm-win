#pragma once

#include "StdAfx.h"
#include <map>
#include "oss_event.h"
#include "UIDefine.h"

using namespace std;

#define WM_SHOW_STEP1_WND		WM_USER+501
#define WM_PROJECT_WND_CLOSED	WM_USER+502


typedef struct tagProjectData
{
	tstring strProjName;
	tstring strResProperty;
	tstring strResName;
	tstring strMemo;
	tstring strProjectID;
	tstring strPackID;
	map<tstring, DWORD> mapFile2Size;
	tagProjectData()
	{
		strResProperty = "无属性";
	}

}sProjectData;


class CTileLayoutResPacket;
class CCreateNewProjStep2;
class ossInterface;
class CssWeb;
class CWkeWebkitUI;

class CCreateNewProjStep1 : public WindowImplBase
						  , public IOssEventBase
{
public:
	CCreateNewProjStep1(CPaintManagerUI* ppm = NULL);
	virtual ~CCreateNewProjStep1();

	void SetMainWndHWND(HWND hWndMaind)
	{
		m_hMainWnd = hWndMaind;
	}

	//oss_event
	virtual void OnProcess(tstring& fileName, int iValue, tstring &strKey);

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("CreateNewProjStep1");			}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");					}
	virtual CDuiString GetSkinFile()			{	return "create_new_project_step1.xml";		}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;								}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

	void CreateWnd();
	bool Initialize();
	bool IsSavingFile();
protected:
	void InitComboProperty();
	void InitMember();
	void OnShowSecondStep();	
	void SetForegroundWnd();
	//检查当前页面的内容和上次保存的内容是不是一致的。
	bool CheckIfDataChanged();

	void InitCtrl();
	void DeleteCtrl();
	//文件上传相关===这些函数应该是由文件传输模块提供基础服务接口。。。
	bool uploadFiles( tstring& strProjectID, tstring& strPackID, fileInfo& lstFiles, IOssEventBase* pEventHandle);
	static unsigned  __stdcall  uploadThreadFunc( void*pArguments); 
	bool uploadSignalFile(tstring& strProjectID, tstring& strPackID, fileInfo& infoFile, IOssEventBase* pEventHandle);
	
	bool ClearTaskInfo(tstring& strFullPath);
	bool IsParamEmpty();
protected:
	HWND m_hMainWnd;
	CPaintManagerUI* m_pPaintManager;
	CCreateNewProjStep2*  m_pSecondStepPage;
	CTileLayoutResPacket* m_pListWithProperty;

	sProjectData m_sLastSavedProjData;


	//文件上传相关
	struct sTaskItem 
	{
		ossInterface* pOssObject;
		tstring strPathFile;		//为了标示唯一的任务
		tstring strFileName;
		bool   bSaving;
		HANDLE hExitEvent;
		sTaskItem()
		{
			hExitEvent = NULL;
			pOssObject = NULL;
			bSaving = false;
		}
	};
	CRITICAL_SECTION m_csUploadFile;
	map<tstring, sTaskItem> m_mapSaveFileState;

	bool m_bShowSecondPage;

	bool create_room_completed_;//by zyy 标记某次业务是否已经创建，应对step2返回到上一步造成的重复创建问题。

	
	CEditUI* new_project_name_edit_;//业务名称
	CComboUI* respack_property_combo_;//资料属性包的combo列表
	CEditUI* respack_name_edit_;//资料包名称
	CButtonUI* take_photo_btn;//"拍照"
	CButtonUI* select_file_btn_;//"文件"
	void OnBnClickSelectFiles();//文件按钮的响应
	CButtonUI* capture_btn_;//"截图"
	CButtonUI* collection_btn_;//"收藏"
	CButtonUI* select_car_btn_;//车型选择按钮
	void OnBnClickSelectCar();//车型选择响应
	CButtonUI* vin_btn_;//VIN码按钮
	void OnBnClickVin();//VIN码响应
	CButtonUI* delete_respack1_btn_;//"删除"资料包1	
	void OnDeleteRespack1();
	CButtonUI* select_friend_btn_;//选择接受人按钮
	void OnBnClickSelectFriend();
//	CButtonUI* delete_res_item_btn_;//删除一个资源文件
	void OnBnClickRemoveOneFile(CControlUI* pControl);//删除一个资源文件响应
	CRichEditUI* new_project_memo_richedit;//备注
	CButtonUI* save_btn_;//保存按钮
	bool OnBnClickSaveProject();
	CButtonUI* close_btn_;//关闭按钮
	bool OnBnClickClose();//关闭按钮响应
};