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

	//由于开始阶段会加载离线消息，在这个阶段，不去重复刷新。只在运行过程中才会
	void SetInitState();

	//如果群建立成功了，就需要将相应的pack列表初始化到文件清单中
	void InitPackFiles(tstring& strProjID, tstring& strRoomID);

	//我是否有执行的权限
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

	//聊天框区域的“文件”选择图标被点击
	void OnGotoEditFilePage(const int nIndex);

	//聊天框区域--发送
	void OnSendMessage();

	//编辑资料--”文件“被点击
	void OnSelectSendingFile(const int nIndex);

	//编辑资料-”保存“被点击
	void OnBnClickSaveProject();

	//编辑资料--”发送“被点击
	void OnSendResPacket();

	//资料清单--打开
	void OnOpenSelectedFile();

	//发送合约按钮
	void OnSendContract();

	//加载长协
	void OnGetGroupContactProperty();

	//记账签约接口
	void OnSendContactMsgToCss();

	//清空已发送合约页面
	void ClearContractPage();

protected:
	void InitComboItems();
	
	void OnBnClickRemoveOneFile(CControlUI* pControl);
	void OnSaveLocalFiles();
	bool CheckIfDataChanged();
	
	//文件上传相关===这些服务应该是由文件传输模块提供基础服务接口。。。
	bool uploadFiles( tstring& strProjectID, tstring& strPackID, fileInfo& lstFiles, IOssEventBase* pEventHandle);
	static unsigned  __stdcall  uploadThreadFunc( void*pArguments); 
	bool uploadSignalFile(tstring& strProjectID, tstring& strPackID, fileInfo& infoFile, IOssEventBase* pEventHandle);
	
	//文件下载相关
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
			strResProperty = "无属性";
		}

	}sProjectData;
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


	bool m_bSendFileMode;

	//放置多此打开同一个资料包
	map<tstring, CGroupPackFileDownloadWnd*> m_mapPack2DownloadWnd;

	vector<tstring> m_vecMyDutys;

	eSendMsgMode m_eSendMode;
};

extern CMsgWnd* g_pMsgWnd;