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

//好友列表来了
#define WM_ROSTER_ARRIVED		WM_USER+10
#define WM_NEW_VCARD_ARRIVED	WM_USER+11
#define WM_FRIEND_REMOVED		WM_USER+12
#define WM_UPDATE_LASTGROUPMSG	WM_USER+13
#define WM_ROOM_NAME_UDPATED	WM_USER+14

#define USER_NO_ORG             0            //身份 无机构
#define USER_ORG_BOSS           1            //身份 机构老板
#define USER_ORG_MEMBER         2            //身份 机构员工


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

	//这个函数目前不是父类的虚函数，后面会改写
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
	//自己的在线状态改变
	virtual void OnMySelfStateUpdate(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo = "");   
	//用户在线状态改变了
	virtual void OnUserStateUpdated(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo = "");   
	//Server下发获取全部联系人
	virtual void OnRosterAvalible(MapAllRosters& listUserInfos);   
	//自己的VCard信息
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	//陌生人的VCard信息
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneCard);
	//好友VCard信息
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	//保存联系人信息的结果
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc); 
	//添加好友成功
	virtual void OnAddFriendSucc(tstring& strFriend);
	//好友删除了。
	virtual void OnFriendRemoved(tstring& strKick,bool bActive);


	//show查找好友页面
	void OnShowSearchPartnerPage();

	//新建项目
	void OnShowCreateNewProjPage();

	//双击跳转到最近联系人
	void OnDbClickFriendList(TNotifyUI& msg);
	//单击发送消息
	void OnBtnSendMessage();
	//好友Roster下发了：主线程干活
	void OnResponseRosters();
	//新的VCard信息下发了，更新cache和好友列表
	void OnResponseNewVCard(sUserVCard& oneUser);
	//显示聊天窗口消息响应
	void OnShowChatWnd(tstring& strAccount);
	//修改备注
	bool OnModifyPartnerMemo(tstring& strAccount);
	//
	void OnUpdateMemoBtnClicked(tstring& strAccount, tstring& strNewMemo);
	//删除好友菜单响应
	void OnDeleteFriend();
	//删除最近联系人菜单响应
	void OnDeleteContact();
	//工作线程投递到主线程去做
	void OnResponseFriendRemoved(tstring& strRemoved);
	//新建项目窗口关闭了
	void OnProjectEditWndClosed();

	//对方是机构的聊天室
	void OnShowGroupChatWnd(tstring& strOrgID, tstring& strOrgName, tstring& strRoomID);
	//对方是个人的聊天室
	void OnShowGroupChatWnd_Personal(tstring& strCellPhone, tstring& strRoomID);
	//业务是新建的，需要在聊天框区域发送这个资料包的状态
	void InsertPackItemToChatRoom(tstring& strRoom, tstring& strPackID);

	void OnBnClickAddFriendReq(bool bAgree, INT_PTR ptrData);

	//void OnBnClickAddOrgFriendReq(bool bAgree, INT_PTR ptrData);

	//上传/下载进度
	virtual void OnProcess(tstring& fileName, int iValue, tstring& strKey);

	//当前身份， 无机构，老板或者员工获取与设置
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

	//刷新好友或者群组列表
	//void updateDataList();
	void MMsendMessage();
	void MMSendFileMsg(const tstring& strParas);
	static mainDlg* getInstance();
	bool addFileMsg(fileInfo&info);
	bool addDownloadedFileMsg(fileInfo &info);
	tstring getCurIdentifier();
	ContactListItemInfo getCurItem();        //获取当前联系人
	ContactListItemInfo getMyselfItem();
	void setLimitChoseFile(bool);
	bool GetCallInFriendItem(tstring strPhoneNo, sFriendListItem& friendItem);
	sUserVCard getMyUserInfo();

	//----需要重构：将好友信息放入到DataCache模块，mainDlg依赖这个模块
	//获取自己所在Group的所有好友列表，用于创建新项目
	bool GetGroupMembersOfMyself(vector<sFriendListItem*>& vecGroupMembsers);
	bool GetOrgnizations(vector<tstring>& vecOrgs);
	bool GetFriendListData(vector<sFriendListItem>& vecAllFriends); 
	bool GetMembersByOrgID(tstring& strOrgID, vector<sFriendListItem>& vecMembers);
	void showGuideOrg();

	bool GetCurrGroupRoomID(tstring& strRoomID);
	bool GetCurrGroupOrgID(tstring& strOrgID);
	bool GetFriendItem(tstring& strAccount, sFriendListItem& OneFriend);
	std::tstring GetMyAvatar();

	//设置消息按钮 新消息标志
	void setOptionRecentChatListUnread(bool);

	//更新好友列表的权限
	void UpdateDutyInFriendList(const tstring& account, const vector<tstring>& duty);

	bool m_bSysUnread;

	CWkeWebkitUI* pWebKitUI;

	void ReConnect();
	void LoseConnection(){connect_to_server_ = false;}
	void SetConnection(){connect_to_server_ = true;}
	bool isConnected(){return connect_to_server_;}
	void playMP3(tstring strPath);

private:
	//好友列表
	std::vector<sFriendListItem> m_listAllFriends;
	//最近联系人列表
	std::vector<ContactListItemInfo> m_lstFriends;
	//当前选择对象
	ContactListItemInfo m_CurItem;          
	ContactListItemInfo m_MyselfItem;
	CUserInfoManager* m_pUsrMgrAdapter;
	CMsgSessionManager* m_pMsgSessionMgr;
	CChatListUI *m_pChatLst;
	CUIFriendList *m_pFriendLst;						//好友列表
	CContactsUI*  m_pRecentList;						//最近联系人列表
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
	int      m_iCurIdentity;           //当前身份
	sUserVCard m_myselfInfo;
	//从程序启动时算起。在此过程中，新增的好友列表缓存
	//为了在聊天框中显示系统提醒消息。
	//map<tstring, time_t> m_mapAccount2Time;

	//trick：记录当前是否是从websever查找个人信息,如果是空的，就表示VCard不是用户查找所得。
	//反之，就表示用户正在查找，VCard信息返回了。
	//tstring m_strSearchUser;

	bool m_bChoseFile;  //限制选择文件操作       
	bool m_bAddFiles;    //是否为追加传输文件
	vector<fileInfo> m_lstUploadFiles;     //用于上传文件列表
	tstring m_strPackID;   //当前任务packid
	CFileControl m_FileControl;   //文件上传下载类
	map<tstring, fileInfo> m_mapTaskFiles;          //上传下载列表与文件信息对应的容器
	map<tstring, CVerticalLayoutUI*> m_mapFileTab;    //上传下载页面与最近联系人对应的容器
	CVerticalLayoutUI *m_pCurFileTab;              //当前选中的页签
	bool connect_to_server_;
	JPContractSelect *m_pContractSelect;
protected:
	//创建“信息”页签并加入到"SidebarTablayout"（Tablayout）容器
	void InitRecentTab();
	//创建该tablayout下面的各种tab页签:聊天记录，系统消息...
	void InitInSetTabLayoutInRecentPage();
	//创建“项目”页签并加入到“SidebarTablayout”容器
	void InitProjectTab();
	//创建“商业伙伴”页签并加入到"SidebarTablayout"（Tablayout）容器
	void InitBussinessPartnerTab();
	//void InitBussinessDataTab();
	//创建“添加好友”的页面到"BussinessPartnerTabLayout"(Tablayout)容器
	void InitInsetTabLayoutInBussinessPartnerPage();
	//创建“发送文件”的页面到"TabFileSend"(Tablayout)容器
	void InitFileSendTab();
	void InitContractTab();
	//创建“合约”的页面到(Tablayout)容器
	//void initTableFriendster();
	//发送文件页面添加待发送的文件
	void addFilesForSend(vector<tstring>& lstfiles);
	//刷新（添加元素）文件上传进度页面
	void refreshFilesProcess(vector<fileInfo>& lstUploadFiles);
	void refreshFilesDownloadProcess(tstring & strPack, tstring &strFrom);
	
	//Server下发好友列表==>Cache.
	void CacheFriendList();

	//好友列表cache刷新了，需要刷新List。
	void InitialFriendList();
	//最近联系人cache更新了，需要刷新List
	void RefreshContactListControl();
	//更新群聊天框内的文件资料包列表
	void UpdatePackListInGroupChat(tstring& strRoomID);
	//读取本地的最近联系人。并cache初始化
	void CacheRecentChatList();
	//读取本地群组信息
	void CacheRecentGroupChat();
	//根据用户名去取未读消息，并显示到消息框中
	void InsertUnReadMsg(tstring& strFromBare);
	//从最近聊天记录列表里面找到这个账户的索引:bInGroup的加入，是当群组聊天的对方刚好是个人，为了在最近联系人中区分，通过判断是否有room信息。
	int GetAccountIndexInChatList(tstring& strAccount, bool bInGroup = false);
	//从最近联系人列表中找到room的索引值
	int GetRoomItemIndexInChatList(tstring& strRoomId);
	//从好友列表里面找到这个账户的索引
	int GetAccountIndexInFriendList(tstring& strAccount);
	//更新最近联系人中的最后一次的聊天记录和时间
	void UpdateLastChatMessage(tstring& strAccount);
	//更新最近联系人的最后一次的聊天记录和时间
	void UpdateLastGroupMessage(tstring& strAccount);
	//更新最近联系人呢中的最后一次群聊天记录和时间
	void UpdateGroupLastMessage(sGroupChatItem* pOneGroupChat, bool bTextMsg);
	//更新好友头像
	void UpdateAvatarInFriendList(tstring& strAccount, tstring& strAvatarPath);
	//更新最近联系人头像
	void UpdateAvatarInRecentList(tstring& strAccount, tstring& strAvatarPath);

	//合成群组头像，结果返存在info里面
	void ComposeGroupAvatar(ContactListItemInfo& info);

	//更新最近联系人里的群组头像，同时更新数据库头像路径，这两个更新目前看来必须一起做
	//UpdateAvatarInRecentList这个函数，没有考虑群组头像的问题
	void UpdateAvatarInRecentList(const tstring& room_id, const tstring& avatar_path, int item_id);

	//好友状态更改了，更新最近联系人列表状态cache+ui
	void UpdateRecentListPresence(tstring& strAccount, EUsrState eState, tstring& strBusyInfo);
	//好友状态更改了，更新好友列表中的状态Cache+ui
	void UpdateFriendListPresence(tstring& strAccount, EUsrState eState, tstring& strBusyInfo);
	//更新聊天框顶部用户的状态信息：
	void UpdateUserInfoAtWndTop(tstring& strAccount, EUsrState eState, tstring& strFriendAvatar);
	//最近聊天列表被点击
	void OnRecentChatListSelected();
	//好友列表被点击了
	void OnFriendListSelected(tstring& strAccount, bool bOrgItem = false, bool bInOrg = false);
	//给定帐户名，去本地db查找相关用户信息，并返回一个最近联系的item
	void GetContackItemByAccount(tstring& strAccount, ContactListItemInfo& oneContact);
	//显示群组的成员列表
	void OnShowGroupMemberWnd();
	//群名称回来了，更新最近联系人里面的群名称：
	void OnUpdateRoomName(tstring& strRoomID, tstring& strRoomName);

	//根据用户名去拿去List索引,返回 -1，表示不存在List中。
	int GetAccountIndex(tstring& strAccount);
	int GetAccountIndexInControl(tstring& strAccount);
	//根据用户名从m_lstFriend得到索引值。
	int GetIndexInCache(tstring& strAccount);
	
	//加载本地数据库的消息记录
	void InitChatMsgFromDB();
	//加载本地数据库的系统消息
	void InitLocalSysMsg();
	//删除好友的清除功能
	void UpdateCacheAndUiAfterFriendDeleted(tstring& strDeletedAccount);
	
	//修改了备注后，主界面的更新
	void UpdateMemoInFriendAndChatList(tstring& strAccount, tstring& strNewMemo);
	//更新好友列表的昵称（备注）
	void UpdataShowNameInFriendList(tstring& strAccount, tstring& strShowName);	

	//更新最近联系人的昵称（备注）
	void UpdateShowNameInRecentChatList(tstring& strAccount, tstring& strShowName);

	void UpdateTopPosition(vector<CLabelUI*> vect_label);


	//接收到了聊天消息
	void ProcessChatMsg(CTextMsg* pTextMsg, int nItemIndex, bool& bProcessed);
	//接收到了系统消息
	void ProcessSysMsg(CSysMsg* pSysMsg, int nItemIndex, bool& bProcessed);

	////添加系统消息——好友请求
	//void addSysItem_sys_msg_addFriend();
	////接收到了系统消息--已同意好友请求
	//void addSysItem_sys_msg_AcceptAddFriend();

	//接收到了群组离线消息
	void ProcessOfflineGroupMsg(CGroupMsg* pGroupMsg, int nItemIndex, bool& bProcessed);

	//处理系统消息-加好友请求
	void ProcessSysMsg_AddFriendReq(tstring& strSender, tstring& strReqMsg);
	//处理系统消息-删除好友
	void ProcessSysMsg_RemoveFriend(tstring& strRemoved);
	//处理系统消息-收到文件请求
	void ProcessSysMsg_FileRequest(tstring& strFrom, tstring& strParas);
	//处理系统消息-语音消息
	void ProcessSysMsg_VoiceRequest(tstring &strFrom, tstring& strParas);
	//处理系统消息-机构相关
	void ProcessSysMsg_OrgRequest(CSysMsgOrgRequest* pOrgMsg, int nItemIndex, bool& bProcessed);
	void welcomeMsg();
	//注销
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
	tstring m_strFilePath;          //下载文件路径，用于打开文件位置
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

	tstring m_strSelectedIndex;			//不知道怎么回事。m_nLastSelectIndex的赋值有问题。只能使用字符串取代
	//int m_nLastSelectIndex;			//在点击 主页 之前，最近联系人的选中的页面是哪个：CONST_RECENT_TAB_P2P_CHAT_LIST （0 1 2 3）
	bool select_business_;				//by zyy 未能追溯到 商业伙伴与好友的标志，因此只能这里创建一个bool型。
										//当该值为true，右键菜单载入 menu_in_business， 否则载入menu_in_friendlist
										//深入了解程序之后修改。

	CRITICAL_SECTION compose_vatar_cs_;

	private:
		void initOrgProperty();
};