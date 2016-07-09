#pragma once
#include "StdAfx.h"
#include "UIContacts.h"
#include "UIDataList.h"
#include "UIChatList.h"
#include <map>
#include "..\include\MsgCenterEvent_i.h"
#include "..\include\UsrManagerEvent_i.h"
#include "..\include\TrayTaskEvent_i.h"
#include "UIDefine.h"
#include "UCSNetServiceCallBackInterface.h"

#include "MessageBox.h"
#include "UIFriendList.h"
#include "UIChoseFile.h"
#include "UIUpdateFiles.h"
#include "UIDownloadFiles.h"
#include "FileControl.h"
#include "UIListCommonDefine.h"
#include "CatchDumpFile.h"
#include "RoomMgrEvent_i.h"

class CUserInfoManager;
class CMsgSessionManager;
class CChatAndFilePage;
class CSysMsgPage;
class CGroupChatPage;
class CPartnerInfoPage;
class CModifyMemoWnd;
class CSearchBussinessPartnerPage;
class CViewFoundPartnerInfoPage;
class CCreateNewProjStep1;
class CWkeWebkitUI;
class CProjectInfoPage;
class sGroupChatItem;
class CGroupMemberView;
class JPContractSelect;
struct findResult
{
	CDuiString m_strLog;
	CDuiString m_strFullName;
	CDuiString m_strMarks;
};

//�����б�����
#define WM_ROSTER_ARRIVED		WM_USER+10
#define WM_NEW_VCARD_ARRIVED	WM_USER+11
#define WM_FRIEND_REMOVED		WM_USER+12
#define WM_UPDATE_LASTGROUPMSG	WM_USER+13
#define WM_ROOM_NAME_UDPATED	WM_USER+14

#define USER_NO_ORG             0            //��� �޻���
#define USER_ORG_BOSS           1            //��� �����ϰ�
#define USER_ORG_MEMBER         2            //��� ����Ա��


class mainDlg : public WindowImplBase
			  , public IMsgCenterModuleEvent 
			  , public CTrayTaskEventBase
			  , public CUserManagerEventBase
			  , public IOssEventBase
			  , public CRoomManagerEventBase
{
public:

	mainDlg();
	~mainDlg();


	//RoomManagerEventBase
	virtual void OnRoomNameArrived(const std::tstring& strRoomID, tstring& strRoomNickName);
	virtual void OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<tstring>& vecMemList);

	//�������Ŀǰ���Ǹ�����麯����������д
	virtual void OnDeleteGroupFromMemberList(const std::tstring& strRoomID);

	virtual void OnEnterRoomFailed(tstring& strErrorDes);
	virtual void OnEnterRoomSucess();

	//ITrayTaskModuleEvent
	virtual void OnDbClickChatMsg(tstring strAccount);
	virtual void OnDbClickSysMessage(tstring strNewUser);
	virtual void OnDbClickGroupMsg(tstring strRoom);
	
	//IMsgCenterModuleEvent
	virtual void OnReceiveMessage(CMsgBase* pRecvMsg, EMsgType eMsgType);
	//virtual void OnReceiveCmdMsg(tstring& strFrom, ECmdType eType, tstring& strParas);

	//IUsrManagerEvent
	//�Լ�������״̬�ı�
	virtual void OnMySelfStateUpdate(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo=(_T("")));   
	//�û�����״̬�ı���
	virtual void OnUserStateUpdated(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo=(_T("")));   
	//Server�·���ȡȫ����ϵ��
	virtual void OnRosterAvalible(MapAllRosters& listUserInfos);   
	//�Լ���VCard��Ϣ
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	//İ���˵�VCard��Ϣ
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneCard);
	//����VCard��Ϣ
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	//������ϵ����Ϣ�Ľ��
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc); 
	//��Ӻ��ѳɹ�
	virtual void OnAddFriendSucc(tstring& strFriend);
	//����ɾ���ˡ�
	virtual void OnFriendRemoved(tstring& strKick,bool bActive);


	//show���Һ���ҳ��
	void OnShowSearchPartnerPage();

	//�½���Ŀ
	void OnShowCreateNewProjPage();

	//˫����ת�������ϵ��
	void OnDbClickFriendList(TNotifyUI& msg);
	//����������Ϣ
	void OnBtnSendMessage();
	//����Roster�·��ˣ����̸߳ɻ�
	void OnResponseRosters();
	//�µ�VCard��Ϣ�·��ˣ�����cache�ͺ����б�
	void OnResponseNewVCard(sUserVCard& oneUser);
	//��ʾ���촰����Ϣ��Ӧ
	void OnShowChatWnd(tstring& strAccount);
	//�޸ı�ע
	bool OnModifyPartnerMemo(tstring& strAccount);
	//
	void OnUpdateMemoBtnClicked(tstring& strAccount, tstring& strNewMemo);
	//ɾ�����Ѳ˵���Ӧ
	void OnDeleteFriend();
	//ɾ�������ϵ�˲˵���Ӧ
	void OnDeleteContact();
	//�����߳�Ͷ�ݵ����߳�ȥ��
	void OnResponseFriendRemoved(tstring& strRemoved);
	//�½���Ŀ���ڹر���
	void OnProjectEditWndClosed();

	//�Է��ǻ�����������
	void OnShowGroupChatWnd(tstring& strOrgID, tstring& strOrgName, tstring& strRoomID);
	//�Է��Ǹ��˵�������
	void OnShowGroupChatWnd_Personal(tstring& strCellPhone, tstring& strRoomID);
	//ҵ�����½��ģ���Ҫ�������������������ϰ���״̬
	void InsertPackItemToChatRoom(tstring& strRoom, tstring& strPackID);

	void OnBnClickAddFriendReq(bool bAgree, INT_PTR ptrData);

	//void OnBnClickAddOrgFriendReq(bool bAgree, INT_PTR ptrData);

	//�ϴ�/���ؽ���
	virtual void OnProcess(tstring& fileName, int iValue, tstring& strKey);

	//��ǰ��ݣ� �޻������ϰ����Ա����ȡ������
	void setUserIdentity(int iIdentity);
	int getUserIdentity();
	int apiGetUserIdentity();

	CGroupChatPage* GetGroupPage()
	{
		return m_pGroupChatPage;
	}

public:
	// 
	LPCTSTR GetWindowClassName() const;	
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam, LPARAM lPara, bool& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetResourceID() const;
	void Notify(TNotifyUI& msg);	
	void GetUidAndKey(tstring& key, tstring& uid);
	void OnExit(TNotifyUI& msg);

	//ˢ�º��ѻ���Ⱥ���б�
	//void updateDataList();
	void MMsendMessage();
	void MMSendFileMsg(const tstring& strParas);
	static mainDlg* getInstance();
	bool addFileMsg(fileInfo&info);
	bool addDownloadedFileMsg(fileInfo &info);
	tstring getCurIdentifier();
	ContactListItemInfo getCurItem();        //��ȡ��ǰ��ϵ��
	ContactListItemInfo getMyselfItem();
	void setLimitChoseFile(bool);
	bool GetCallInFriendItem(tstring strPhoneNo, sFriendListItem& friendItem);
	sUserVCard getMyUserInfo();

	//----��Ҫ�ع�����������Ϣ���뵽DataCacheģ�飬mainDlg�������ģ��
	//��ȡ�Լ�����Group�����к����б����ڴ�������Ŀ
	bool GetGroupMembersOfMyself(vector<sFriendListItem*>& vecGroupMembsers);
	bool GetOrgnizations(vector<tstring>& vecOrgs);
	bool GetFriendListData(vector<sFriendListItem>& vecAllFriends); 
	bool GetMembersByOrgID(tstring& strOrgID, vector<sFriendListItem>& vecMembers);
	void showGuideOrg();

	bool GetCurrGroupRoomID(tstring& strRoomID);
	bool GetCurrGroupOrgID(tstring& strOrgID);
	bool GetFriendItem(tstring& strAccount, sFriendListItem& OneFriend);
	std::tstring GetMyAvatar();

	//������Ϣ��ť ����Ϣ��־
	void setOptionRecentChatListUnread(bool);

	//���º����б��Ȩ��
	void UpdateDutyInFriendList(const tstring& account, const vector<tstring>& duty);

	bool m_bSysUnread;

	CWkeWebkitUI* pWebKitUI;

	void ReConnect();
	void LoseConnection(){connect_to_server_ = false;}
	void SetConnection(){connect_to_server_ = true;}
	bool isConnected(){return connect_to_server_;}
	void playMP3(tstring strPath);

private:
	//�����б�
	std::vector<sFriendListItem> m_listAllFriends;
	//�����ϵ���б�
	std::vector<ContactListItemInfo> m_lstFriends;
	//��ǰѡ�����
	ContactListItemInfo m_CurItem;          
	ContactListItemInfo m_MyselfItem;
	CUserInfoManager* m_pUsrMgrAdapter;
	CMsgSessionManager* m_pMsgSessionMgr;
	CChatListUI *m_pChatLst;
	CUIFriendList *m_pFriendLst;						//�����б�
	CContactsUI*  m_pRecentList;						//�����ϵ���б�
	//CUIUserInfoPage* m_pUserInfoPage;
	CCreateNewProjStep1* m_pCreateNewProjPage;
	CPartnerInfoPage* m_pPartnerInfopage;
	CSearchBussinessPartnerPage* m_pSearchPartnerPage;
	CViewFoundPartnerInfoPage*	 m_pViewPartnerInfoPage;
	CSysMsgPage*	  m_pSysMsgPage;
	CGroupChatPage*   m_pGroupChatPage;
	CProjectInfoPage* m_pProjectPage;
	CMessageBox* m_pMsgBox;
	CModifyMemoWnd* m_pMemoModify;
	static mainDlg* m_pInstance;
	std::vector<fileInfo> m_lstDownloadFiles;
	//sUserInfo m_myselfInfo;
	int      m_iCurIdentity;           //��ǰ���
	sUserVCard m_myselfInfo;
	//�ӳ�������ʱ�����ڴ˹����У������ĺ����б���
	//Ϊ�������������ʾϵͳ������Ϣ��
	//map<tstring, time_t> m_mapAccount2Time;

	//trick����¼��ǰ�Ƿ��Ǵ�websever���Ҹ�����Ϣ,����ǿյģ��ͱ�ʾVCard�����û��������á�
	//��֮���ͱ�ʾ�û����ڲ��ң�VCard��Ϣ�����ˡ�
	//tstring m_strSearchUser;

	bool m_bChoseFile;  //����ѡ���ļ�����       
	bool m_bAddFiles;    //�Ƿ�Ϊ׷�Ӵ����ļ�
	vector<fileInfo> m_lstUploadFiles;     //�����ϴ��ļ��б�
	tstring m_strPackID;   //��ǰ����packid
	CFileControl m_FileControl;   //�ļ��ϴ�������
	map<tstring, fileInfo> m_mapTaskFiles;          //�ϴ������б����ļ���Ϣ��Ӧ������
	map<tstring, CVerticalLayoutUI*> m_mapFileTab;    //�ϴ�����ҳ���������ϵ�˶�Ӧ������
	CVerticalLayoutUI *m_pCurFileTab;              //��ǰѡ�е�ҳǩ
	bool connect_to_server_;
	JPContractSelect *m_pContractSelect;
protected:
	//��������Ϣ��ҳǩ�����뵽"SidebarTablayout"��Tablayout������
	void InitRecentTab();
	//������tablayout����ĸ���tabҳǩ:�����¼��ϵͳ��Ϣ...
	void InitInSetTabLayoutInRecentPage();
	//��������Ŀ��ҳǩ�����뵽��SidebarTablayout������
	void InitProjectTab();
	//��������ҵ��顱ҳǩ�����뵽"SidebarTablayout"��Tablayout������
	void InitBussinessPartnerTab();
	//void InitBussinessDataTab();
	//��������Ӻ��ѡ���ҳ�浽"BussinessPartnerTabLayout"(Tablayout)����
	void InitInsetTabLayoutInBussinessPartnerPage();
	//�����������ļ�����ҳ�浽"TabFileSend"(Tablayout)����
	void InitFileSendTab();
	void InitContractTab();
	//��������Լ����ҳ�浽(Tablayout)����
	//void initTableFriendster();
	//�����ļ�ҳ����Ӵ����͵��ļ�
	void addFilesForSend(vector<tstring>& lstfiles);
	//ˢ�£����Ԫ�أ��ļ��ϴ�����ҳ��
	void refreshFilesProcess(vector<fileInfo>& lstUploadFiles);
	void refreshFilesDownloadProcess(tstring & strPack, tstring &strFrom);
	
	//Server�·������б�==>Cache.
	void CacheFriendList();

	//�����б�cacheˢ���ˣ���Ҫˢ��List��
	void InitialFriendList();
	//�����ϵ��cache�����ˣ���Ҫˢ��List
	void RefreshContactListControl();
	//����Ⱥ������ڵ��ļ����ϰ��б�
	void UpdatePackListInGroupChat(tstring& strRoomID);
	//��ȡ���ص������ϵ�ˡ���cache��ʼ��
	void CacheRecentChatList();
	//��ȡ����Ⱥ����Ϣ
	void CacheRecentGroupChat();
	//�����û���ȥȡδ����Ϣ������ʾ����Ϣ����
	void InsertUnReadMsg(tstring& strFromBare);
	//����������¼�б������ҵ�����˻�������:bInGroup�ļ��룬�ǵ�Ⱥ������ĶԷ��պ��Ǹ��ˣ�Ϊ���������ϵ�������֣�ͨ���ж��Ƿ���room��Ϣ��
	int GetAccountIndexInChatList(tstring& strAccount, bool bInGroup = false);
	//�������ϵ���б����ҵ�room������ֵ
	int GetRoomItemIndexInChatList(tstring& strRoomId);
	//�Ӻ����б������ҵ�����˻�������
	int GetAccountIndexInFriendList(tstring& strAccount);
	//���������ϵ���е����һ�ε������¼��ʱ��
	void UpdateLastChatMessage(tstring& strAccount);
	//���������ϵ�˵����һ�ε������¼��ʱ��
	void UpdateLastGroupMessage(tstring& strAccount);
	//���������ϵ�����е����һ��Ⱥ�����¼��ʱ��
	void UpdateGroupLastMessage(sGroupChatItem* pOneGroupChat, bool bTextMsg);
	//���º���ͷ��
	void UpdateAvatarInFriendList(tstring& strAccount, tstring& strAvatarPath);
	//���������ϵ��ͷ��
	void UpdateAvatarInRecentList(tstring& strAccount, tstring& strAvatarPath);

	//�ϳ�Ⱥ��ͷ�񣬽��������info����
	void ComposeGroupAvatar(ContactListItemInfo& info);

	//���������ϵ�����Ⱥ��ͷ��ͬʱ�������ݿ�ͷ��·��������������Ŀǰ��������һ����
	//UpdateAvatarInRecentList���������û�п���Ⱥ��ͷ�������
	void UpdateAvatarInRecentList(const tstring& room_id, const tstring& avatar_path, int item_id);

	//����״̬�����ˣ����������ϵ���б�״̬cache+ui
	void UpdateRecentListPresence(tstring& strAccount, EUsrState eState, tstring& strBusyInfo);
	//����״̬�����ˣ����º����б��е�״̬Cache+ui
	void UpdateFriendListPresence(tstring& strAccount, EUsrState eState, tstring& strBusyInfo);
	//��������򶥲��û���״̬��Ϣ��
	void UpdateUserInfoAtWndTop(tstring& strAccount, EUsrState eState, tstring& strFriendAvatar);
	//��������б����
	void OnRecentChatListSelected();
	//�����б������
	void OnFriendListSelected(tstring& strAccount, bool bOrgItem = false, bool bInOrg = false);
	//�����ʻ�����ȥ����db��������û���Ϣ��������һ�������ϵ��item
	void GetContackItemByAccount(tstring& strAccount, ContactListItemInfo& oneContact);
	//��ʾȺ��ĳ�Ա�б�
	void OnShowGroupMemberWnd();
	//Ⱥ���ƻ����ˣ����������ϵ�������Ⱥ���ƣ�
	void OnUpdateRoomName(tstring& strRoomID, tstring& strRoomName);

	//�����û���ȥ��ȥList����,���� -1����ʾ������List�С�
	int GetAccountIndex(tstring& strAccount);
	int GetAccountIndexInControl(tstring& strAccount);
	//�����û�����m_lstFriend�õ�����ֵ��
	int GetIndexInCache(tstring& strAccount);
	
	//���ر������ݿ����Ϣ��¼
	void InitChatMsgFromDB();
	//���ر������ݿ��ϵͳ��Ϣ
	void InitLocalSysMsg();
	//ɾ�����ѵ��������
	void UpdateCacheAndUiAfterFriendDeleted(tstring& strDeletedAccount);
	
	//�޸��˱�ע��������ĸ���
	void UpdateMemoInFriendAndChatList(tstring& strAccount, tstring& strNewMemo);
	//���º����б���ǳƣ���ע��
	void UpdataShowNameInFriendList(tstring& strAccount, tstring& strShowName);	

	//���������ϵ�˵��ǳƣ���ע��
	void UpdateShowNameInRecentChatList(tstring& strAccount, tstring& strShowName);

	void UpdateTopPosition(vector<CLabelUI*> vect_label);


	//���յ���������Ϣ
	void ProcessChatMsg(CTextMsg* pTextMsg, int nItemIndex, bool& bProcessed);
	//���յ���ϵͳ��Ϣ
	void ProcessSysMsg(CSysMsg* pSysMsg, int nItemIndex, bool& bProcessed);

	////���ϵͳ��Ϣ������������
	//void addSysItem_sys_msg_addFriend();
	////���յ���ϵͳ��Ϣ--��ͬ���������
	//void addSysItem_sys_msg_AcceptAddFriend();

	//���յ���Ⱥ��������Ϣ
	void ProcessOfflineGroupMsg(CGroupMsg* pGroupMsg, int nItemIndex, bool& bProcessed);

	//����ϵͳ��Ϣ-�Ӻ�������
	void ProcessSysMsg_AddFriendReq(tstring& strSender, tstring& strReqMsg);
	//����ϵͳ��Ϣ-ɾ������
	void ProcessSysMsg_RemoveFriend(tstring& strRemoved);
	//����ϵͳ��Ϣ-�յ��ļ�����
	void ProcessSysMsg_FileRequest(tstring& strFrom, tstring& strParas);
	//����ϵͳ��Ϣ-������Ϣ
	void ProcessSysMsg_VoiceRequest(tstring &strFrom, tstring& strParas);
	//����ϵͳ��Ϣ-�������
	void ProcessSysMsg_OrgRequest(CSysMsgOrgRequest* pOrgMsg, int nItemIndex, bool& bProcessed);
	void welcomeMsg();
	//ע��
	void LogOff();
	//static DWORD CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
	static unsigned  __stdcall  DownLoadThreadFunc( void*pArguments) ; 
	void OnClickSystemSettings();
	void OnClickCallTypeDrop(POINT pt);
	void HandleItemClickEvent( TNotifyUI& msg );
	void ShowNetCallBtn(bool bVisible);
	void HandleChangePassword();
	void HandleCreateOrg();
	void HandleUserInfo();
	void HandleOrgInfo();
	void HandleChangeOrganization();
	void HandleEmployeeManage();
	void HandleSysAbout();
	void HandleSysQuit();
	void HandleSysQuitOrg();
	bool IsNetCalling();
	void InitVideoFrm();
	void InitialWalletTab();
	void HandleOrgUpdateEvent(UINT uMsg);
	void RefreshGroupList(const tstring strAccount, bool bDelayUpdate = true);
	LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	void SortFriendList();
	void InitialHomePage();
	void HandleMouseEnterEvent(TNotifyUI& msg);
	void RefreshHomePagePic();
	void ResetPassWord();
	list<tstring> m_ltUcPaasClientList;	
	UCS_Sid  m_sidInfo; 
	CButtonUI* m_pVoiceBtn;
	CButtonUI* m_pVideoBtn;
	CButtonUI* m_pBussinessMaterialBtn;
	CButtonUI* m_pBussinessQuitBtn;
	UIUPdateFiles *m_pUpdateFiles;
	UIDownloadFiles *m_pDownFiles;
	tstring m_strFilePath;          //�����ļ�·�������ڴ��ļ�λ��
	//Node* m_FriendListRootParent;
	map<tstring, DuiLib::Node*> m_mapOrg2Node;

	
	CatchDumpFile::CDumpCatch m_catchDump;

	//CGroupMemberView* m_pGroupMemView;

	CTextUI* m_pHomePageHelpText;
	CVerticalLayoutUI* m_pHomePageVLBkImg;
	CButtonUI* m_pHomePageInvitePartner;
	CButtonUI* m_pHomePageNetCall;
	CButtonUI* m_pHomePageCreateOrg;
	CButtonUI* m_pHomePageBussiness;

	map<tstring, CGroupMemberView*> m_mapRoom2Memberview;

	tstring m_strSelectedIndex;			//��֪����ô���¡�m_nLastSelectIndex�ĸ�ֵ�����⡣ֻ��ʹ���ַ���ȡ��
	//int m_nLastSelectIndex;			//�ڵ�� ��ҳ ֮ǰ�������ϵ�˵�ѡ�е�ҳ�����ĸ���CONST_RECENT_TAB_P2P_CHAT_LIST ��0 1 2 3��
	bool select_business_;				//by zyy δ��׷�ݵ� ��ҵ�������ѵı�־�����ֻ�����ﴴ��һ��bool�͡�
										//����ֵΪtrue���Ҽ��˵����� menu_in_business�� ��������menu_in_friendlist
										//�����˽����֮���޸ġ�

	CRITICAL_SECTION compose_vatar_cs_;

	private:
		void initOrgProperty();
};