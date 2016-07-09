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
		strResProperty = "������";
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

	//ע�⣺���������const���ͻᱨ˵������Ȼ�����ࡣ
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
	//��鵱ǰҳ������ݺ��ϴα���������ǲ���һ�µġ�
	bool CheckIfDataChanged();

	void InitCtrl();
	void DeleteCtrl();
	//�ļ��ϴ����===��Щ����Ӧ�������ļ�����ģ���ṩ��������ӿڡ�����
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


	//�ļ��ϴ����
	struct sTaskItem 
	{
		ossInterface* pOssObject;
		tstring strPathFile;		//Ϊ�˱�ʾΨһ������
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

	bool create_room_completed_;//by zyy ���ĳ��ҵ���Ƿ��Ѿ�������Ӧ��step2���ص���һ����ɵ��ظ��������⡣

	
	CEditUI* new_project_name_edit_;//ҵ������
	CComboUI* respack_property_combo_;//�������԰���combo�б�
	CEditUI* respack_name_edit_;//���ϰ�����
	CButtonUI* take_photo_btn;//"����"
	CButtonUI* select_file_btn_;//"�ļ�"
	void OnBnClickSelectFiles();//�ļ���ť����Ӧ
	CButtonUI* capture_btn_;//"��ͼ"
	CButtonUI* collection_btn_;//"�ղ�"
	CButtonUI* select_car_btn_;//����ѡ��ť
	void OnBnClickSelectCar();//����ѡ����Ӧ
	CButtonUI* vin_btn_;//VIN�밴ť
	void OnBnClickVin();//VIN����Ӧ
	CButtonUI* delete_respack1_btn_;//"ɾ��"���ϰ�1	
	void OnDeleteRespack1();
	CButtonUI* select_friend_btn_;//ѡ������˰�ť
	void OnBnClickSelectFriend();
//	CButtonUI* delete_res_item_btn_;//ɾ��һ����Դ�ļ�
	void OnBnClickRemoveOneFile(CControlUI* pControl);//ɾ��һ����Դ�ļ���Ӧ
	CRichEditUI* new_project_memo_richedit;//��ע
	CButtonUI* save_btn_;//���水ť
	bool OnBnClickSaveProject();
	CButtonUI* close_btn_;//�رհ�ť
	bool OnBnClickClose();//�رհ�ť��Ӧ
};