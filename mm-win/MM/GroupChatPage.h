#pragma once

#include "StdAfx.h"
#include "GroupChatLogic.h"
#include "MsgWnd.h"
#include "oss_event.h"
#include "oss_interface.h"
#include "ListResPackets.h"
#include "UIDefine.h"
#include "FileControl.h"
#include "..\Common\MMDefine.h"

using namespace DuiLib;

class CTileLayoutResPacket;
class CGroupPackFileDownloadWnd;
class CGroupChatPage : public CHorizontalLayoutUI
					 , public INotifyUI
					 , public CGroupChatLogicEvent
					 , public IMsgWndEvent
					 , public IOssEventBase
{
public:
	CGroupChatPage(CPaintManagerUI* ppm = NULL);
	virtual ~CGroupChatPage();

	void SetMainHwnd(HWND hMainWnd)
	{
		m_hMainWnd = hMainWnd;
	}


	//oss_event
	virtual void OnProcess(tstring& fileName, int iValue, tstring &strKey);


	//IMsgWnd
	virtual void OnMessage(UINT uMsg, WPARAM w, LPARAM l);

	virtual void Notify(TNotifyUI& msg);

	//CGroupChatLogicEvent
	virtual void OnReceveGroupMsg(sGroupChatItem& newMsg);

	void InitGroupMsg(tstring& strRoomId);

	void CacheGroupMessage(tstring& strRoomId);

	void SetCurrentRoom(tstring& strRoomID);

	void RefreshGroupChatList(tstring& strRoomID);

	void RefreshGroupPackList(tstring& strRoomID);

	//���ڿ�ʼ�׶λ����������Ϣ��������׶Σ���ȥ�ظ�ˢ�¡�ֻ�����й����вŻ�
	void SetInitState();

	//���Ⱥ�����ɹ��ˣ�����Ҫ����Ӧ��pack�б��ʼ�����ļ��嵥��
	void InitPackFiles(tstring& strProjID, tstring& strRoomID);

	//���Ƿ���ִ�е�Ȩ��
	bool IsAbleToExecute();

	void InitMyDutys();

	void UpdatePackCache(tstring& strRoomID, tstring& strPackID, tstring& strNewStatus);

	bool IsPackExist(tstring& strRoomID, tstring& strPackID);
	void AddNewPackItem(tstring& strRoomID, tstring& strPackID);

	void SetSendMsgKeyMode(eSendMsgMode mode);
	bool IsEnterSendMode();
	void SendGroupMessage();
	void SendGroupMessage(tstring& strContent);

	bool CheckIfPackRequest(tstring& strContent, sPackItem& onePack);
	bool CheckIfPackDownloadReceipt(tstring& strContent, tstring& sPackName);

	CGroupChatLogic* GetGroupChatLogic()
	{
		return m_pGroupChatLogic;
	}

	void InsertFirstPack(tstring& strRoom,tstring& strPackID);

protected:
	void Initialize();

	void ClearEditInfo();

	//���������ġ��ļ���ѡ��ͼ�걻���
	void OnGotoEditFilePage(const int nIndex);

	//���������--����
	void OnSendMessage();

	//�༭����--���ļ��������
	void OnSelectSendingFile(const int nIndex);

	//�༭����-�����桰�����
	void OnBnClickSaveProject();

	//�༭����--�����͡������
	void OnSendResPacket();

	//�����嵥--��
	void OnOpenSelectedFile();

	//���ͺ�Լ��ť
	void OnSendContract();

	//���س�Э
	void OnGetGroupContactProperty();

	//����ǩԼ�ӿ�
	void OnSendContactMsgToCss();

	//����ѷ��ͺ�Լҳ��
	void ClearContractPage();

protected:
	void InitComboItems();
	
	void OnBnClickRemoveOneFile(CControlUI* pControl);
	void OnSaveLocalFiles();
	bool CheckIfDataChanged();
	
	//�ļ��ϴ����===��Щ����Ӧ�������ļ�����ģ���ṩ��������ӿڡ�����
	bool uploadFiles( tstring& strProjectID, tstring& strPackID, fileInfo& lstFiles, IOssEventBase* pEventHandle);
	static unsigned  __stdcall  uploadThreadFunc( void*pArguments); 
	bool uploadSignalFile(tstring& strProjectID, tstring& strPackID, fileInfo& infoFile, IOssEventBase* pEventHandle);
	
	//�ļ��������
	bool downloadFiles(fileInfo& lstFiles, IOssEventBase* pEventHandle);
	static unsigned  __stdcall  downloadThreadFunc( void*pArguments);
	bool downloadSignalFile(fileInfo& infoFile, IOssEventBase* pEventHandle);
	
	bool IsSavingFile();
	bool ClearTaskInfo(tstring& strFullPath);
	bool IsParamEmpty();
	//tstring GetSenderOrgAndNick(tstring& struid);
	void HandleItemClickEvent( TNotifyUI& msg );

	bool UpdatePackStatus(CControlUI* pSender, tstring& strStatus);

public:
	CListResPacket* m_pFilePacketList;

protected:
	CPaintManagerUI* m_pPaintManager;
	CGroupChatLogic* m_pGroupChatLogic;
	tstring m_strLastRoomID;
	HWND m_hMainWnd;

	CComboUI* m_pComboResProperty;
	CTileLayoutResPacket* m_pListWithProperty;
	CTileLayoutResPacket* m_pContractFiles;
	CRITICAL_SECTION m_csFLSection;
	map<tstring, MapPackID2Packs> m_mapRoom2PackList;


//////////////////////////////////////////////////////////////////////////
//
	typedef struct tagProjectData
	{
		tstring strProjName;
		tstring strResProperty;
		tstring strResName;
		tstring strMemo;
		tstring strProjectID;
		map<tstring, DWORD> mapFile2Size;
		tagProjectData()
		{
			strResProperty = "������";
		}

	}sProjectData;
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


	bool m_bSendFileMode;

	//���ö�˴�ͬһ�����ϰ�
	map<tstring, CGroupPackFileDownloadWnd*> m_mapPack2DownloadWnd;

	vector<tstring> m_vecMyDutys;

	eSendMsgMode m_eSendMode;
};

extern CMsgWnd* g_pMsgWnd;