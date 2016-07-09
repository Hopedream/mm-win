#include "mainDlg.h"
#include "userInfoDlg.h"
#include "Utility.h"
#include "StringHelper.h"
#include "../Common/Logger.h"
#include "Controller.h"
#include "UserManager.h"
#include "MsgSessionMgr.h"
#include "..\Common\UserInfoDef.h"
#include "..\include\MsgCenterModule.h"
#include "..\include\TrayTaskModule.h"
#include "..\include\DBTaskModule.h"
#include "../include/Global_Def.h"
#include "HttpClient.h"

#include "..\gloox-1.0.14\src\client.h"
#include "..\gloox-1.0.14\src\rosterlistener.h"
#include "..\gloox-1.0.14\src\rostermanager.h"
#include "..\gloox-1.0.14\src\rosteritem.h"
#include "../gloox-1.0.14/src/base64.h"

#include "CDialogBuilderCallbackEx.h"
#include "UIMenu.h"
#include "UIChoseFile.h"
#include "PartnerInfoPage.h"
#include "SearchBussinessPartnerPage.h"
#include "ViewFoundPartnerInfoPage.h"
#include "SendAddRequestPage.h"
#include "CreateNewProjectStep1.h"
#include "ModifyMemoWnd.h"
#include "SysMsgListCtrl.h"
#include "ChatAndFilePage.h"
#include "SysMsgPage.h"
#include "GroupChatPage.h"
#include "ProjectsListPage.h"
#include "GroupMemberView.h"
#include "oss_interface.h"
#include <sstream>
#include "FileHelper.h"
#include "MUCRoomMgr.h"
#include "webClient.h"
#include "MagicKHelper.h"
#include "GroupChatLogic.h"
#include <MMSystem.h>

#include "json/json.h"
#include "MsgBox.h"
#include "systemSetting.h"
#include "CallFrame.h"
//#include "voiceDlg.h"
#include "videoDlg.h"
//#include "MiniDumper.h"
#include "webClient.h"
#include <ShellAPI.h>
#include "FrameChangePassword.h"
#include "FrameChangeOrganization.h"
#include "FrameEmployeeManage.h"
#include "FrameEmployeeManage1.h"
#include "FrameSysAbout.h"
#include "FrameSysQuit.h"
#include "FrameCreateOrg.h"
#include "FrameUserInfo.h"
#include "FrameSysQuitOrg.h"
#include "FrameGuideOrg.h"
#include "FrameGuideUser.h"
#include "FrameEmployee.h"
#include "../Common/regexHelper.h"
#include "invitePartner.h"
#include "WkeWebkit.h"
#include <algorithm>
#include "sortHelper.h"
#include <set>
#include <iterator>     // std::distance
#include "FrameBusinessData.h"
#include "MiniDumper.h"
#include "IImageOle.h"
#include "RichEditUtil.h"
#include "jp_contract_select.h"

#include "registerAccountDlg.h"
#include "FrameBussinessData.h"

using namespace gloox;
using namespace std;

struct sChatMsgItem
{
	tstring strSender;
	tstring strMsg;
};

#define TAB_VIEW_RECENTCHAT_PAGE 0
#define TAB_VIEW_PROJECT_PAGE 1
#define TAB_VIEW_BUSSINESS_PARTNER_PAGE 2
#define TAB_VIEW_WALLET_PAGE 3
//#define TAB_VIEW_HOME_PAGE 4
#define TAB_VIEW_CONTRACT 4

const TCHAR* const kListContactsControlName			= _T("listRecentChat");			//最近联系人列表List
const TCHAR* const kListContactsFriendList			= _T("allFriendList");			//我的好友列表List			
const TCHAR* const kcomboTimeControlName			= _T("combTime");
const TCHAR* const kcomboTypeControlName			= _T("combType");
const TCHAR* const kcomboGroupControlName			= _T("combGroup");
const TCHAR* const ktreeDataListControlName			= _T("treeDataList");
//const TCHAR* const kbuttonContactNameControlName	= _T("buttonContactName");
const TCHAR* const kbuttonSendButtonControlName		= _T("buttonSend");				//发送消息按钮
const TCHAR* const krichEditInputEditControlName	= _T("inputEdit_maindlg");
const TCHAR* const krichEditOutputEditControlName	= _T("outputEdit");
const TCHAR* const kbuttonMinControlName			= _T("ButtonMinize");			//主界面最小化按钮
const TCHAR* const kbuttonMaxControlName			= _T("ButtonMaxmize");			//主界面最大化按钮
const TCHAR* const kbuttonCloseControlName			= _T("ButtonClose");			//主界面关闭按钮
const TCHAR* const kListChatControlName				= _T("listChatControl");		//聊天记录列表List
//const TCHAR* const koptionRosterControlName			= _T("optionRoster");
//const TCHAR* const koptionFriendsterControlName		= _T("optionFriendster");
//const TCHAR* const kTabRecentChat					= _T("tabRecentChat");			//最近联系人Tab页签
const TCHAR* const kTabChatAndFileList				= _T("tabChatAndFileList");		//聊天框和文件列表Tab页签
const TCHAR* const kbuttonAddRosterControlName		= _T("btnAddBPartner");			//"添加商业伙伴"
const TCHAR* const kbuttonFileControlName			= _T("buttonFile");
const TCHAR* const kBtnDialPhoneName			= _T("ButtonDialPhone");
const TCHAR* const kHLayoutNetCallName			= _T("HLayoutNetCall");
const TCHAR* const kHLayoutContactInfo			= _T("VLayoutContactInfo");
const TCHAR* const kBtnCallTypeDropName			= _T("btnCallTypeDrop");
const TCHAR* const kBtnHomePage			= _T("btnHomePage");

const TCHAR* const kTabLayoutFileSend               = _T("TabFileSend_roster");               //聊天框右侧添加"发送文件"页签 （里面子项对应每个最近联系人）
//tabFileSend的控件
const TCHAR* const kbuttonTabFile                   = _T("btn_tab_file");             //选择本地文件按钮
const TCHAR* const kTileFilesforSend                = _T("tile_FilesforSend");        //展示待发送文件TileLayout
const TCHAR* const kbuttonFileSend                  = _T("buttonFileSend");           //发送文件按钮

const TCHAR* const kTabLayoutSidebar				= _T("SidebarTabContainer");		//主界面的侧边栏Layout控件，目前容纳了：“信息”+“项目”+“商业伙伴”...等6个Tab页签
const TCHAR* const kBussinesPartnerTabLayout		= _T("BussinessPartnerTabLayout");	//“商业伙伴”页签内部的TabLayout容器，目前容纳了：加好友页面， 商家信息页面
const TCHAR* const kOptionRecentChat				= _T("TabRecentChatList");			//关联最近联系人的页签的Option按钮	_T("optionChat");
const TCHAR* const kOptionProject					= _T("TabProject");					//关联项目的页签的Option按钮
const TCHAR* const kOptionBussinessPartner			= _T("TabBusinessPartner");			//关联商业伙伴页签的Option按钮
const TCHAR* const kOptionMoney						= _T("TabMoney");					//关联钱包页签的Option按钮
const TCHAR* const kOptionExecute					= _T("TabExecute");					//关联执行的Option按钮
const TCHAR* const kOptionFaceDeal					= _T("TabFaceDeal");				//关联当面交易的Option按钮
const TCHAR* const kOptionFinance					= _T("TabFinance");					//关联财务的Option按钮
const TCHAR* const kOptionSysSetting				= _T("TabSysSetting");				//关联系统设置页签的Option按钮
const TCHAR* const kOptionCollection				= _T("TabCollection");				//关联收藏页签的Option按钮
const TCHAR* const kButtonMySelfAvatar				= _T("ButtonMySelfAvatar");			//自己的头像Avatar
const TCHAR* const kLabelMySelfNickname				= _T("LabelMySelfNickName");		//自己的昵称
const TCHAR* const kLabelMySelfCompany				= _T("LabelMySelfCompany");			//自己的机构名

const TCHAR* const kButtonShowGroupMember			= _T("btn_show_groupMember");		//显示群组信息
const TCHAR* const kLabelFriendNickName				= _T("LabelFriendNickname");		//好友(商业伙伴)的昵称
const TCHAR* const kButtonFriendAvatar				= _T("ButtonFriendAvatar");			//好友(商业伙伴)的头像Avatar
const TCHAR* const kLabelFriendCompany				= _T("LabelFriendCompany");			//好友(商业伙伴)的机构名
const TCHAR* const kLabelGroupMemCount				= _T("lable_groupmem_count");		//群组的人数

const TCHAR* const kRecentChatLayout				= _T("RecentChatTabLayout");		//最近联系人内部的Tab容器：聊天记录tab，系统消息tab，群组消息tab...
const TCHAR* const kSysMsgListCtrl					= _T("invitefriendListctrl");		//系统消息页面的Listctrl控件

const TCHAR* const kButtonCreateNewProject			= _T("ButtonCreateNewProject");		//新建项目

jsValue JS_CALL js_msgBox(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
	const wchar_t* title = jsToStringW(es, jsArg(es, 1));

	HWND hwnd = mainDlg::getInstance()->GetHWND();
	if (::IsWindow(hwnd) )
	{
//		::PostMessage(hwnd, WM_TIMER_CHATLISTENDDOWN, (WPARAM)text, (LPARAM)title);
		CMsgBox* pMsgBox = new CMsgBox("测试","本地程序获取网页点击取消按钮响应。关闭本页面");
		pMsgBox->ShowModal();
	}
	//	MessageBox(hwnd, text, title, 0);

	mainDlg::getInstance()->pWebKitUI->SetInternVisible(false);
	return jsUndefined();
}

jsValue JS_CALL JsFinance(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));

	int length = WideCharToMultiByte(CP_ACP, 0, text, -1, 0, 0, 0, 0 );   
	char* nowchar = new char[length+1];  
	memset(nowchar,0,length+1);  
	WideCharToMultiByte( CP_ACP, 0, text, -1, nowchar,length, 0, 0 ); 

	CMsgBox* pMsgBox = new CMsgBox("测试", nowchar);
	pMsgBox->ShowModal();

	delete nowchar;
	nowchar = NULL;

	return jsUndefined();
}

jsValue JS_CALL JsCall(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));

	int length = WideCharToMultiByte(CP_ACP, 0, text, -1, 0, 0, 0, 0 );   
	char* nowchar = new char[length+1];  
	memset(nowchar,0,length+1);  
	WideCharToMultiByte( CP_ACP, 0, text, -1, nowchar,length, 0, 0 ); 

	CMsgBox* pMsgBox = new CMsgBox("测试", nowchar);
	pMsgBox->ShowModal();

	delete nowchar;
	nowchar = NULL;

	return jsUndefined();
}

jsValue JS_CALL JsChat(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));

	int length = WideCharToMultiByte(CP_ACP, 0, text, -1, 0, 0, 0, 0 );   
	char* nowchar = new char[length+1];  
	memset(nowchar,0,length+1);  
	WideCharToMultiByte( CP_ACP, 0, text, -1, nowchar,length, 0, 0 ); 

	CMsgBox* pMsgBox = new CMsgBox("测试", nowchar);
	pMsgBox->ShowModal();

	delete nowchar;
	nowchar = NULL;

	return jsUndefined();
}


mainDlg* mainDlg::m_pInstance = NULL;


mainDlg::mainDlg()
:m_pUsrMgrAdapter(NULL)
,m_pPartnerInfopage(NULL)
,m_pSearchPartnerPage(NULL)
,m_pSysMsgPage(NULL)
,m_pGroupChatPage(NULL)
,m_pViewPartnerInfoPage(NULL)
,m_pCreateNewProjPage(NULL)
,m_pMsgBox(NULL)
,m_pMemoModify(NULL)
,m_pChatLst(NULL)
,m_pFriendLst(NULL)
,m_pRecentList(NULL)
,m_bChoseFile(false)
,m_pUpdateFiles(NULL)
,m_pDownFiles(NULL)
,m_bAddFiles(false)
,m_strPackID(_T(""))
,pWebKitUI(NULL)
,m_pHomePageHelpText(NULL)
,m_pHomePageVLBkImg(NULL)
,m_pHomePageInvitePartner(NULL)
,m_pHomePageNetCall(NULL)
,m_pHomePageCreateOrg(NULL)
,m_pHomePageBussiness(NULL)
,m_pProjectPage(NULL)
,m_iCurIdentity(USER_NO_ORG)
, select_business_(true)
,m_bSysUnread(false)
,connect_to_server_(true)
,m_pContractSelect(NULL)
{	
	InitializeCriticalSection(&compose_vatar_cs_);
	m_pUsrMgrAdapter = CController::Instance()->GetUserInfoMgr();
	m_pUsrMgrAdapter->AddEventHandler(this);

	m_pMsgSessionMgr = NULL;
	CMsgSessionManager* pSessionMgr = CController::Instance()->GetMsgSessionMgr();
	if (NULL != pSessionMgr)
	{
		m_pMsgSessionMgr = pSessionMgr;
	}

	//注册接收聊天消息。
	GetMsgCenterModule()->AddEventHandler(this);
	GetTrayTaskModule()->AddEventHandler((ITrayTaskModuleEvent*)this);
	m_pInstance = this;

	CController::Instance()->GetRoomManager()->AddHandler(this);
	CWebClient::api_getRegions();
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)MiniDump_Unhandled);
	//InitUCSNetServiceCallBackInterface();	
}

mainDlg::~mainDlg()
{
	DeleteCriticalSection(&compose_vatar_cs_);	
	//remove icon in taskbar.	
	FreeUCSNetServiceCallBackInterface();
	GetTrayTaskModule()->Release();
	PostQuitMessage(0);
}

void mainDlg::RefreshContactListControl()
{
	ShowNetCallBtn(m_lstFriends.size()>0);

	if (m_lstFriends.size()>0)
	{
		if (NULL != m_pRecentList)
		{
			m_pRecentList->RemoveAll();
			for (int i=0; i<m_lstFriends.size(); i++)
			{
				ContactListItemInfo info = m_lstFriends[i];

				//room成员会从server端获取，因此这里不能把成员个数判断作为能不能添加的标志，在软件第一次启动时，本地获取的list
				//而此时room成员并没有拿到
				//因此list直接add，而成员图像需要获得成员后才能合并
				m_pRecentList->AddItem(&info);		
				
			}
			m_pRecentList->SelectItem(0);			
		}
	}
}

void mainDlg::InitialFriendList()
{	
	sort(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper::CompareFunc);

	//1：先获取机构名称
	//“我的好友”分组没有人员的情况下，保证分组排序正确。
	m_mapOrg2Node.clear();
	set<t_strstrPair> tOrgIdGroupPairSet;
	for (std::vector<sFriendListItem>::iterator iter = m_listAllFriends.begin(); 
		iter != m_listAllFriends.end(); ++iter)
	{
		tOrgIdGroupPairSet.insert(t_strstrPair(iter->strOrgID, iter->strOrgnization));		
	}
	tOrgIdGroupPairSet.insert( make_pair(DEF_ORGID_MYFRIEND,ORGANIZATION_MYFRIEND));		

	vector<t_strstrPair> vecOrgIdGroupPairVector;
	copy(tOrgIdGroupPairSet.begin(), tOrgIdGroupPairSet.end(), back_inserter(vecOrgIdGroupPairVector));
	sort(vecOrgIdGroupPairVector.begin(), vecOrgIdGroupPairVector.end(), CSortHelper::CompareGroupFunc);

	//由于消息的不同步，这个地方的m_pFriend可能没有初始化。
	if (m_pFriendLst == NULL)
	{
		return;
	}

	for (vector<t_strstrPair>::iterator iter = vecOrgIdGroupPairVector.begin(); iter != vecOrgIdGroupPairVector.end(); ++iter)
	{
		FriendListItemInfo rootItem;
		rootItem.folder = true;
		rootItem.strOrgName =iter->second;
		rootItem.strOrgID = iter->first;
		rootItem.itemId = GetNextItemID();
		DuiLib::Node* pOrgNode = m_pFriendLst->AddNode(rootItem, NULL);	
		m_mapOrg2Node.insert(make_pair(rootItem.strOrgID, pOrgNode));
	}

	for (std::vector<sFriendListItem>::iterator iter = m_listAllFriends.begin(); 
		iter != m_listAllFriends.end(); ++iter)
	{
		FriendListItemInfo item;
		item.empty = false;
		item.folder = false;
		item.strOrgName = iter->strOrgnization.empty() ? ORGANIZATION_MYFRIEND : iter->strOrgnization;
		item.strOrgID = iter->strOrgID;
		item.itemId = iter->nItemID;
		item.logo = iter->strAvatar;
		item.nick_name =iter->strShowName;
		item.account = iter->strAccount;
	
		m_pFriendLst->AddNode(item,m_mapOrg2Node[item.strOrgID]);
	}

	SetTimer(this->GetHWND(), WM_TIEMER_UPDATE_FRIEND_LIST, 1000, NULL);
}

bool mainDlg::GetOrgnizations(vector<tstring>& vecOrgs)
{
	for (map<tstring,DuiLib::Node*>::iterator it = m_mapOrg2Node.begin();
		it != m_mapOrg2Node.end(); ++it)
	{
		vecOrgs.push_back(it->first);
	}

	return vecOrgs.size()>0;
}

bool mainDlg::GetFriendListData(vector<sFriendListItem>& vecAllFriends)
{
	vecAllFriends = m_listAllFriends;
	return vecAllFriends.size()>0;
}

void mainDlg::GetContackItemByAccount(tstring& strAccount, ContactListItemInfo& oneContactItem)
{
	//初始化化好友信息
	oneContactItem.nItemID = GetNextItemID();
	oneContactItem.m_strAccount = strAccount.c_str();

	//查询最后一次的聊天内容和时间
	_tChatMessage oneChatMsg;
	if (GetDBTaskModule()->LoadLastMessage(strAccount, oneChatMsg))
	{
		//tstring strOut;
		//CChineseCode::UTF_8ToGB2312(strOut, (char*)oneChatMsg.strChatText.c_str(), oneChatMsg.strChatText.size());
		//oneContactItem.m_strLastMsg = strOut.c_str();
		
		oneContactItem.m_strLastMsg = oneChatMsg.strChatText.c_str();
		oneContactItem.m_strLastTm  = oneChatMsg.strTimeStrFormat.c_str();
	}

	//获取用户的状态：这个时候拿的到吗？！
	EUsrState eState = m_pUsrMgrAdapter->GetState(strAccount);
	oneContactItem.m_State = eState;

	oneContactItem.m_strLogo = (CFileHelper::GetHigtLightAvatar(strAccount)).c_str();
	

	//策略修改：取消了用户的备注名
	sUserVCard oneUser;
	if (GetDBTaskModule()->LoadUserVCard(strAccount,  oneUser))
	{
		oneContactItem.m_strNickName = oneUser.strUserNickname.c_str();
	}
	sUserVCard oneVCard;
	if(m_pUsrMgrAdapter != NULL && m_pUsrMgrAdapter->GetUserVCard(strAccount, oneVCard))
	{
		if (oneContactItem.m_strNickName.IsEmpty())
		{
			oneContactItem.m_strNickName = oneVCard.strUserNickname.c_str();
		}
		else if (!oneVCard.strUserNickname.empty() && 
			_tcsicmp(oneContactItem.m_strNickName.GetData(), oneVCard.strUserNickname.c_str()) != 0)
		{
			//如果更改了
			oneContactItem.m_strNickName = oneVCard.strUserNickname.c_str();
		}
	}

	oneContactItem.m_strOrgName = m_pUsrMgrAdapter->GetOrgName(strAccount).c_str();
}

void mainDlg::CacheRecentChatList()
{
	//1.to do...从本地的DB获取最近联系人信息。
	vector<RECENTITEM> vecRecentList;
	if(GetDBTaskModule()->LoadRecentFriendList(vecRecentList))
	{
		int nSize = vecRecentList.size();
		if (nSize>0)
		{
			for (int i=0; i<nSize; i++)
			{
				RECENTITEM& oneFriend = vecRecentList[i];
				ContactListItemInfo oneContact;
				tstring strAccount = oneFriend.strUid+"@test-d";
				//由于数据库删除操作会失败所以加了个判断。
				if (m_pUsrMgrAdapter->IsMyFriend(strAccount))
				{
					GetContackItemByAccount(strAccount, oneContact);
					m_lstFriends.push_back(oneContact);
				}
				//如果是系统消息标签
				else if (_tcsicmp(strAccount.c_str(), SYS_NOTIFY_ACCOUNT) == 0)
				{
					if (NULL != m_pRecentList)
					{
						m_pRecentList->RemoveItem(10000);
					}
					ContactListItemInfo iteminfo;
					iteminfo.nItemID = GetNextItemID();
					iteminfo.m_strAccount = SYS_NOTIFY_ACCOUNT;
					iteminfo.m_strNickName = "系统消息";
					//CSysMsg sysMsg;
					sSysMsg sysMsg;
					if (GetDBTaskModule()->GetLastSysMsg(sysMsg))
					{
						iteminfo.m_strLastMsg = sysMsg.strMsg.c_str();
					}
					iteminfo.m_strLogo = SYSNOTIFY_AVATAR;
					iteminfo.m_strLastTm = oneFriend.strTime.c_str();
					m_lstFriends.push_back(iteminfo);
				}
			}
		}
		
	}	
}

void mainDlg::CacheRecentGroupChat()
{
	//1.to do...从本地的DB获取最近联系人信息。
	vector<RecentGroupItem> vecGroupList;
	if(GetDBTaskModule()->LoadRecentGroupItem(vecGroupList))
	{
		int nSize = vecGroupList.size();
		if (nSize>0)
		{
			for (int i=0; i<nSize; i++)
			{
				RecentGroupItem& oneGroup = vecGroupList[i];

				//Join room.
				tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
				CController::Instance()->GetRoomManager()->EnterRoom(oneGroup.strRoomID, strUser);
				
				//这里好像不需要这样做，by zyy 待验证
				//换成下方的直接插入m_lstFriends

				//如果对方是个人
// 				if (oneGroup.strOrgID.empty())
// 				{
// 					OnShowGroupChatWnd_Personal(oneGroup.strParticipant, oneGroup.strRoomID);
// 				}
// 				else
// 				{
// 					OnShowGroupChatWnd(oneGroup.strOrgID, oneGroup.strOrgName, oneGroup.strRoomID);
// 				}

				
				ContactListItemInfo info;
				info.m_strAccount = SYS_GROUP_CHAT_ACCOUNT;
				info.m_strNickName = oneGroup.strGroupNick.c_str();
				info.m_strOrgName  = oneGroup.strOrgName.c_str();
				info.m_strOrgID    = oneGroup.strOrgID.c_str();
				info.m_strRoomID   = oneGroup.strRoomID.c_str();
				info.m_strLastMsg =(_T(""));
				info.m_strLastTm   = GetFullTimeString(time(0)).c_str();//"2016-04-05 16:27:23";
				info.nItemID	   = GetNextItemID();				
				info.m_strLogo     = oneGroup.strAvatarPath.c_str();				
				m_lstFriends.push_back(info);
				
				//初始化群本地消息记录
				if (NULL != m_pGroupChatPage)
				{
					m_pGroupChatPage->CacheGroupMessage(oneGroup.strRoomID);
				}
			}
		}

	}	
}

void mainDlg::CacheFriendList()
{
	MapAllRosters listUserInfos;
	if(!m_pUsrMgrAdapter->GetAllRosters(listUserInfos))
	{
		return;
	}
	
	//1.cache好友列表的信息。
	for (MapAllRosters::iterator iter = listUserInfos.begin(); iter !=listUserInfos.end(); iter++)
	{
		sFriendListItem oneFriend;
		oneFriend.nItemID	 = GetNextItemID();
		oneFriend.strAccount = iter->second.strAccount;
		oneFriend.strShowName = iter->second.strMemo;//备注名
		oneFriend.strOrgnization = iter->second.strOrgnization;
		oneFriend.strOrgID = iter->second.strOrgID;


		//从本地数据库取头像和昵称，如果没有，就拉取vcard
		sUserVCard oneVcard;
		if (GetDBTaskModule()->LoadUserVCard(oneFriend.strAccount, oneVcard))
		{
			//备注是空的就显示昵称
			if (oneFriend.strShowName.empty())
			{
				oneFriend.strShowName = oneVcard.strUserNickname;
			}
			//昵称是空的，就显示手机账号
			if (oneFriend.strShowName.empty())
			{
				oneFriend.strShowName = oneVcard.strPhone;
			}

			//默认设置离线的头像
			if (!oneVcard.strGrayAvatar.empty())
			{
				oneFriend.strAvatar = oneVcard.strGrayAvatar;
			}
			else
			{
				oneFriend.strAvatar = DEF_AVATAR_GRAY;
			}
		}
		//没有VCard
		else
		{
			//如果没有备注，就直接显示手机号
			if (oneFriend.strShowName.empty())
			{
				oneFriend.strShowName = CFileHelper::GetUserName(oneFriend.strAccount);
			}

			oneFriend.strAvatar = DEF_AVATAR_GRAY;
			
			//拉取VCard。
			m_pUsrMgrAdapter->FetchVCardByAccount(iter->second.strAccount);
		}

		//cache!!!
		m_listAllFriends.push_back(oneFriend);
	}

}

CDuiString mainDlg::GetSkinFile()
{
	return "main_dialog.xml";//_T("mainDlg.xml");
}

CDuiString mainDlg::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("..\\MM\\res\\");
#else
	//todo
	return _T("..\\MM\\res\\");
#endif
}

LRESULT mainDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{

	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

void mainDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void mainDlg::OnExit(TNotifyUI& msg)
{
	Close();
}

//lee：这个函数可以重构到一个CRecentTabPage:public CHorizontalLayout里面去
//再在该类的里面做InitInSetTabLayoutInRecentPage的初始化工作
void mainDlg::InitRecentTab()
{
	CDialogBuilder m_DlgBuilder;
	CHorizontalLayoutUI *pRecentChat = NULL;
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pRecentChat = static_cast<CHorizontalLayoutUI*>(m_DlgBuilder.Create(_T("tab_recent_chat.xml"),
			(UINT)0, &cb, &m_PaintManager));
	}
	else
	{
		pRecentChat = static_cast<CHorizontalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	
	//追加到TabLayout Container容器内，索引值+1
 	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->Add(pRecentChat);	


	m_pRecentList = static_cast<CContactsUI*>(m_PaintManager.FindControl(kListContactsControlName));
	ASSERT(m_pRecentList != NULL);

}

void mainDlg::InitInSetTabLayoutInRecentPage()
{
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout));
	if (NULL == pLayout)
	{
		return;
	}

	//1.加载个人聊天的Page到pLayout， index=0
	CChatAndFilePage::Instance()->InitialChatPage(&m_PaintManager);

	//2.加载系统消息的Page到pLayout, index=1
	if (NULL == m_pSysMsgPage)
	{
		m_pSysMsgPage = new CSysMsgPage(&m_PaintManager);
	}

	//3.初始化Homepage
	InitialHomePage();

	//4.加载群聊天的Page到pLayout,  index=2
	if (NULL == m_pGroupChatPage)
	{
		m_pGroupChatPage = new CGroupChatPage(&m_PaintManager);
		m_pGroupChatPage->SetMainHwnd(this->GetHWND());
	}

	//激活当前页面聊天记录的页面
	if (pLayout->GetCount()>0)
	{
		pLayout->SelectItem(CONST_RECENT_TAB_P2P_CHAT_LIST/*0*/);
		m_pBussinessMaterialBtn->SetVisible(false);
		m_pBussinessQuitBtn->SetVisible(false);
	}
}

void mainDlg::InitProjectTab()
{
	if (NULL == m_pProjectPage)
	{
		m_pProjectPage = new CProjectInfoPage(&m_PaintManager);
		m_pProjectPage->Initialize();
	}
}

void mainDlg::InitFileSendTab()
{
	CDialogBuilder  m_DlgBuilder1;
	CVerticalLayoutUI *pRecentChat = NULL;
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	CVerticalLayoutUI *pFileProcess = NULL;
	if (!m_DlgBuilder1.GetMarkup()->IsValid())
	{
		pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder1.Create(_T("tab_file_process.xml"),
			(UINT)0, &cb, &m_PaintManager));
	}
	else
	{
		pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder1.Create((UINT)0, &m_PaintManager));
	}

	//追加到TabLayout Container容器内，索引值+1
	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->Add(pFileProcess);
	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(0);
	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(false);
}

void mainDlg::addFilesForSend(vector<tstring>& lstfiles)
{
	int isz = m_lstUploadFiles.size();
	m_lstUploadFiles.clear();
	int iCount = lstfiles.size();
	if (iCount <= 0)
		return;
	if (iCount > 10)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示","您选择的文件超过10个，请删除多余文件再上传！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	else
	{
		for (int i=0; i<lstfiles.size(); i++)
		{
			tstring filePath = lstfiles[i];
			tstring strValue = filePath.c_str();
			//截取文件名
			//1. 去除文件夹
			int iPos=strValue.find_last_of("\\");
			strValue = strValue.substr(iPos + 1); //strValue.Mid(i, strValue.GetLength());
			CDuiString strFileFullName = strValue.c_str();
			//2. 去除文件类型后缀
			iPos = strValue.find_last_of('.');
			CDuiString strfileType = strValue.substr(iPos+1).c_str();
			CDuiString strFileName = strValue.substr(0, iPos).c_str();
		
			fileInfo infoValue;
			infoValue.strFileFullName = strFileFullName;
			infoValue.strFileName = strFileName;
			infoValue.strFilePath = filePath;
			infoValue.strFileType = strfileType;
			//infoValue.strPackID = m_strPackID;
			infoValue.strKey = filePath + "upload" + tstring(m_CurItem.m_strAccount);
			infoValue.stropposite = m_CurItem.m_strAccount;
			//3. 获取文件大小
			infoValue.dwFileSize = getFileSize(infoValue.strFilePath);
			if (infoValue.dwFileSize <= 0)
			{
				CMsgBox* pMsgBox = new CMsgBox("错误","空文件无法发送！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return;
			}
				
			if (m_mapTaskFiles.find(infoValue.strKey) != m_mapTaskFiles.end())   //已存在，不重复上传
				return;
			m_lstUploadFiles.push_back(infoValue);
		}	
	}
}

void mainDlg::refreshFilesDownloadProcess(tstring & strPack, tstring& strFrom)
{
	//这里不知道为啥josn不能解析
	fileInfo downLoadInfo; 

	//strpack = strResponse.substr(strResponse.find("\"id\":\"") + 6, strResponse.find("\",\"name\"") - (strResponse.find("\"id\":\"") + 6));
	//m_strPackID = strpack.substr(0, strpack.find("\""));
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strPack, root))
	{
		try
		{
			downLoadInfo.strFileFullName = root["file_name"].asString();
			downLoadInfo.strFileUrl = root["ali_file_url"].asString();
			tstring strSz = root["file_size"].asString();
			downLoadInfo.dwFileSize = atol(strSz.c_str());
			tstring strName = downLoadInfo.strFileFullName;
			tstring strfileName = strName.substr(0, strName.find_last_of("."));
			tstring strfileType = strName.substr(strName.find_last_of(".") + 1, strName.length());
			downLoadInfo.strFileName = strfileName;
			downLoadInfo.strFileType = strfileType;
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strPack.c_str());	
		}		
	}
	else
	{
		tstring strRight = strPack;
		downLoadInfo.strFileFullName = strRight.substr(strRight.find("\"file_name\":\"") + 13, strRight.find("\",\"file_type\"") - (strRight.find("\"file_name\":\"") + 13));
		//m_bAddFiles = false;
		strRight = strRight.substr(strRight.find("\"file_type\":\""), strRight.length());
		downLoadInfo.strFileType = strRight.substr(strRight.find("\"file_type\":\"") + 13, strRight.find("\",\"file_size\"") - (strRight.find("\"file_type\":\"") + 13));
		strRight = strRight.substr(strRight.find("\"file_size\":\""), strRight.length());
		downLoadInfo.dwFileSize = atol(strRight.substr(strRight.find("\"file_size\":\"") + 13, strRight.find("\",\"ali_file_url\"") - (strRight.find("\"file_size\":\"") + 13)).c_str());
		strRight = strRight.substr(strRight.find("\"ali_file_url\":\""), strRight.length());
		downLoadInfo.strFileUrl = strRight.substr(strRight.find("\"ali_file_url\":\"") + 16, strRight.find("\",\"file_path\"") - (strRight.find("\"ali_file_url\":\"") + 16));
		tstring strName = downLoadInfo.strFileFullName;
		tstring strfileName = strName.substr(0, strName.find_last_of("."));
		downLoadInfo.strFileName = strfileName;
	}


	//设置下载路径
		CFileHelper fileHelpter;
		tstring strBasePath = fileHelpter.GetUserDir() +"User\\";  //+ m_MyselfItem.m_strAccount + "\\RecFiles\\";
		if (!fopen(strBasePath.c_str(), "w") )
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		strBasePath += mainDlg::getInstance()->getMyselfItem().m_strAccount + "\\";
		if (!fopen(strBasePath.c_str(), "w"))
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		strBasePath += "RecvFiles\\";
		if (!fopen(strBasePath.c_str(), "w"))
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		m_strFilePath = strBasePath;
		strBasePath += downLoadInfo.strFileFullName;
		downLoadInfo.strFilePath = strBasePath;
		downLoadInfo.strKey = strBasePath + "download" + strFrom;
		downLoadInfo.stropposite = strFrom;

		if (m_mapTaskFiles.find(downLoadInfo.strKey) != m_mapTaskFiles.end())
		{
			//已存在，或者正在下载
			return;
		}
	//ui

		CVerticalLayoutUI *pFileProcess = NULL;
		map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(strFrom);
		if (it != m_mapFileTab.end())
		{
			//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(it->second);
			//m_pCurFileTab = it->second;
			pFileProcess = it->second;
		}
		else
		{
			//新建一个
			CDialogBuilder  m_DlgBuilder;
			CVerticalLayoutUI *pRecentChat = NULL;
			CDialogBuilderCallbackEx cb(&m_PaintManager);
			//CVerticalLayoutUI *pFileProcess = NULL;
			if (!m_DlgBuilder.GetMarkup()->IsValid())
			{
				pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create(_T("tab_file_process.xml"),
					(UINT)0, &cb, &m_PaintManager));
			}
			else
			{
				pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
			}

			//追加到TabLayout Container容器内，索引值+1
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->Add(pFileProcess);
			//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(pFileProcess);
			//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
			m_mapFileTab.insert(make_pair(strFrom, pFileProcess));
		}
		//CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(m_pCurFileTab,"listfileProcess"));
	CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(pFileProcess,"listfileProcess"));
	CDialogBuilder m_DlgBuilder;
	CListContainerElementUI *pRecentChat = NULL;
	tstring filePath = downLoadInfo.strFilePath;
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pRecentChat = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("element_file_download_progress.xml"),
			(UINT)0, &cb, &m_PaintManager));
	}
	else
	{
		pRecentChat = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (pRecentChat != NULL)
	{
		pRecentChat->SetName(downLoadInfo.strKey.c_str());
		bool bRes = plstProcess->Add(pRecentChat);
	}
	//1. 设置icon
	//todo
	CControlUI *pCtrlLogo = static_cast<CControlUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("controlFileLogo")));
	pCtrlLogo->SetBkImage(GetPackFileIcon(downLoadInfo.strFileType).c_str());
	//2. 设置名字
	CLabelUI *plableFileName = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileName")));
	
	//3. 设置文件大小
	double sz = double(downLoadInfo.dwFileSize) / 1024;
	//sz = sz/1024;
	CDuiString strsz;
	strsz.Format("%.2f", sz);
	strsz += _T("KB");
	tstring strValue = downLoadInfo.strFileName + "(" + tstring(strsz)+")";
	plableFileName->SetText(strValue.c_str());
	//4. 放入map容器
	m_mapTaskFiles.insert(make_pair(downLoadInfo.strKey, downLoadInfo));
	int nSize = m_mapTaskFiles.size();

	//5. ui显示
	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
}

void mainDlg::refreshFilesProcess( vector<fileInfo>& lstUploadFiles )
{
	for(int i=0; i< lstUploadFiles.size(); i++)
	{
		CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(m_pCurFileTab,"listfileProcess"));
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *pRecentChat = NULL;
		tstring filePath = lstUploadFiles[i].strFilePath;
		CDialogBuilderCallbackEx cb(&m_PaintManager);
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pRecentChat = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("element_file_upload_progress.xml"),
				(UINT)0, &cb, &m_PaintManager));
		}
		else
		{
			pRecentChat = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
		}
		if (pRecentChat != NULL)
		{
			pRecentChat->SetName(lstUploadFiles[i].strKey.c_str());
			plstProcess->Add(pRecentChat);
		}
		//1. 设置icon
		CControlUI *pCtrlLogo = static_cast<CControlUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("controlFileLogo")));
		pCtrlLogo->SetBkImage(GetPackFileIcon(lstUploadFiles[i].strFileType).c_str());
			//todo
		//2. 设置名字
		CLabelUI *plableFileName = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileName")));
		
		//3. 设置文件大小
		//CLabelUI *plableFileSize = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileSize")));
		double sz = double(lstUploadFiles[i].dwFileSize) / 1024;
		//sz = sz/1024;
		CDuiString strsz;
		strsz.Format("%.2f", sz);
		strsz += _T("KB");
		tstring strValue = lstUploadFiles[i].strFileName + "(" + tstring(strsz)+")";
		plableFileName->SetText(strValue.c_str());
		//plableFileSize->SetText(strsz);
		//4. 放入map容器
		int nSize = m_mapTaskFiles.size();
		m_mapTaskFiles.insert(make_pair(lstUploadFiles[i].strKey, lstUploadFiles[i]));
		nSize = m_mapTaskFiles.size();
		int abc=100;
	}
}


void mainDlg::InitWindow()
{
	SetIcon(IDI_IM_LOGO_ONLINE);
	InitVideoFrm();
	InitRecentTab();
	InitInSetTabLayoutInRecentPage();
	InitProjectTab();

	//设置自己的账户名
	m_pChatLst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
	if (NULL != m_pChatLst)
	{
		tstring strAccount = CController::Instance()->GetXmppClient()->jid().bare();
		m_pChatLst->SetMySelfAccount(strAccount);
	}	

	InitBussinessPartnerTab();
	InitInsetTabLayoutInBussinessPartnerPage();
	InitialWalletTab();
	InitContractTab();
	welcomeMsg();
}

void mainDlg::InitialWalletTab()
{
	CDialogBuilder m_DlgBuilder;
	CHorizontalLayoutUI *pWalletTab = NULL;
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pWalletTab = static_cast<CHorizontalLayoutUI*>(m_DlgBuilder.Create(_T("tab_money.xml"),
			(UINT)0, &cb, &m_PaintManager));
	}
	else
	{
		pWalletTab = static_cast<CHorizontalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	}

	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->Add(pWalletTab);	
	 pWebKitUI = static_cast<CWkeWebkitUI*>(m_PaintManager.FindControl(_T("ie")));
//	 pWebKitUI->SetURL(WEB_WALLET_URL);
	 pWebKitUI->SetInternVisible(false);

	 jsBindFunction("call", JsCall, 1);//这里绑定js函数，让js主动调用c++函数
	 jsBindFunction("chat", JsChat, 1);//这里绑定js函数，让js主动调用c++函数
}
void mainDlg::InitContractTab()
{
	if (NULL == m_pContractSelect)
	{
		m_pContractSelect = new JPContractSelect(&m_PaintManager);
		//m_pContractSelect->InitWindow();
		//m_pProjectPage->Initialize();
	}
}
//void mainDlg::updateDataList()
//{
//	DataListNodeInfo datainfo;
//	datainfo.m_strLogo = "Doc.png";
//	datainfo.m_strTime = "12:00";
//	datainfo.m_strType = "询报价";
//
//
//	CDataListUI *pContactsList = static_cast<CDataListUI*>(m_PaintManager.FindControl(ktreeDataListControlName));
//	
//	if (NULL != pContactsList)
//	{
//		for (int i=0; i< 3; i++)
//		{
//			pContactsList->AddItem(&datainfo);
//		}
//
//		pContactsList->AddOfferItem();
//		datainfo.m_strType = "技术咨询";
//		pContactsList->AddItem(&datainfo);
//		pContactsList->AddConsultationItem();
//	}
//	
//}

LRESULT mainDlg::ResponseDefaultKeyEvent(WPARAM wParam, LPARAM lPara, bool& bHandled)
{		
	if (wParam == VK_RETURN)
	{
		CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout));
		if (NULL != pLayout)
		{
			//如果当前的点-点聊天界面
			if (pLayout->GetCurSel() == CONST_RECENT_TAB_P2P_CHAT_LIST)
			{
				CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
				if (pEditInput->IsFocused() 
					&& (CChatAndFilePage::Instance()->IsEnterSendMode() && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0) 
					|| (!CChatAndFilePage::Instance()->IsEnterSendMode() && GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) >= 0))
				{
					OnBtnSendMessage();
					bHandled = TRUE;
					return TRUE;
				}	
			}
			//如果当前是群聊天界面
			else if(pLayout->GetCurSel() == CONST_RECENT_TAB_GROUPCHAT_PAGE)
			{
				CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl("inputEdit_groupchat"));
				if (pEditInput->IsFocused() 
					&& (m_pGroupChatPage->IsEnterSendMode() && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0) 
					|| (!m_pGroupChatPage->IsEnterSendMode() && GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) >= 0))
				{
					//OnBtnSendMessage();
					m_pGroupChatPage->SendGroupMessage();
					bHandled = TRUE;
					return TRUE;
				}	
			}
		}
		
		bHandled = FALSE;
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		bHandled = TRUE;
		return TRUE;
	}
	bHandled = FALSE;

	return FALSE;
}

UILIB_RESOURCETYPE mainDlg::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

LRESULT mainDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
	case WM_SEND_MSG:
		m_pPartnerInfopage->SendMsg();
		break;
	case WM_NET_PHONE:
		m_pPartnerInfopage->NetCall();
		break;
	case WM_MOBILE_PHONE:
		m_pPartnerInfopage->PhoneCall();
		break;
	case WM_BUSINESS:
		m_pPartnerInfopage->BusinessDetail();
		break;
	case WM_REMOVE_USER:
		OnDeleteFriend();
		break;
	case WM_REMOVE_CONTACT:
		OnDeleteContact();
		break;
	case WM_ROSTER_ARRIVED:
		{
			m_listAllFriends.clear();
			
			//GEt my room list，在这里请求roomlist,应答并不在这里
			CController::Instance()->GetRoomManager()->GetMyRoomList();

			//得到room list不是及时生效的，所以要设置一个定时器
			//			SetTimer(this->GetHWND(), WM_TIMER_UPDATE_ROOM_LIST, 100, NULL);

			//			RefreshContactListControl();//重新刷新一遍，去掉当前用户不在的群，by zyy

			//1.cache
			CacheFriendList();
			//2.refresh ui
			InitialFriendList();

			//do more thing..
			if (m_pGroupChatPage != NULL)
			{
				m_pGroupChatPage->InitMyDutys();
			}

			//本地数据库读取最近联系人
			CacheRecentChatList();
			//暴力：将好友聊天记录先查询cache
			InitChatMsgFromDB();

			//加载本地系统消息
			InitLocalSysMsg();

			//加载本地群组列表
			CacheRecentGroupChat();

			///刷新最近联系人列表-激活最上面的一条。
			RefreshContactListControl();			 
		}
		break;

		//再更新群名称的同时，需要更新群头像
	case WM_ROOM_NAME_UDPATED:
		{
			char* pBufroomId = (char*)wParam;
			char* pBufRoomName = (char*)lParam;
			tstring strRoomID(pBufroomId);
			tstring strRoomName(pBufRoomName);

			OnUpdateRoomName(strRoomID, strRoomName);
		}
		break;


	case WM_UPDATE_PRESENCE_STATUS:
		{
			char* pszAccount = (char*)wParam;
			tstring strAccount(pszAccount);
			EUsrState eUsrState = (EUsrState)lParam;

			//1.Update friend list cache + ui
			UpdateFriendListPresence(strAccount,eUsrState,  tstring(_T("")));

			//2.在聊天框的顶部用户昵称后显示在线状态
			UpdateUserInfoAtWndTop(strAccount, eUsrState, tstring(_T("")));

			//重新排序好友列表
			RefreshGroupList(strAccount);		

			delete []pszAccount;
		}
		break;
	case WM_NEW_VCARD_ARRIVED:
		{
			sUserVCard& oneUser = *((sUserVCard*)wParam);
			OnResponseNewVCard(oneUser);		
		}
		break;
	
	case WM_MEMO_UPDATE:
		{
			char* pBufAccount = (char*)wParam;
			char* pBufMemo = (char*)lParam;
			if (NULL != pBufAccount && pBufMemo)
			{
				tstring strAccount = pBufAccount;
				tstring strNewMemo = pBufMemo;
				OnUpdateMemoBtnClicked(strAccount, strNewMemo);
			}
		}
		break;
	case WM_FRIEND_REMOVED:
		{
			char* pBuf = (char*)wParam;
			tstring strKicked = pBuf;
			UpdateCacheAndUiAfterFriendDeleted(strKicked);
		}
		break;
	case WM_UPDATE_LASTGROUPMSG:
		{
			sGroupChatItem* pBuffer = (sGroupChatItem*)wParam;
			if (NULL != pBuffer)
			{
				//如果是命令类型的消息，就不显示
				if (pBuffer->strTextContent.find("#COMMAND#") == -1)
				{
					UpdateGroupLastMessage(pBuffer, true/*(bool)lParam*/);
				}	
				else
				{
					UpdateGroupLastMessage(pBuffer, false/*(bool)lParam*/);
				}
			}
		}
		break;
	case WM_PROJECT_WND_CLOSED:
		{
			OnProjectEditWndClosed();
		}
		break;
// 	case WM_GROUPMEM_CLOSED:
// 		{
// 			if (NULL != m_pGroupMemView)
// 			{
// 				m_pGroupMemView = NULL;
// 			}
// 		}
// 		break;
	case WM_SYS_CHANGE_PASSWORD:
		{
			HandleChangePassword();
		}
		break;
	case WM_SYS_CREATE_ORG:
		{
			HandleCreateOrg();
		}
		break;
	case WM_SYS_USER_INFO:
		{
			HandleUserInfo();
		}
		break;
	case WM_SYS_ORG_INFO:
		{
			HandleOrgInfo();
		}
		break;
	case WM_SYS_CHANGE_ORGANIZATION_INFO:
		{
			HandleChangeOrganization();
		}
		break;
	case WM_SYS_EMPLOYEE_MANAGE:
		{
			HandleEmployeeManage();
		}
		break;
	case WM_SYS_RESET_PASSWORD:
		{
			ResetPassWord();
		}
		break;
	case WM_SYS_ABOUT:
		{
			HandleSysAbout();
		}
		break;
	case WM_SYS_QUIT:
		{
			HandleSysQuit();
		}
		break;
	case WM_SYS_LOG_OFF:
		{
			LogOff();
		}
		break;
	case WM_SYS_QUIT_ORG:
		{
			HandleSysQuitOrg();
		}
		break;
	case WM_CREATE_ORG:
	case WM_EXIT_ORG:
	case WM_MODIFY_ORG_INFO:
		HandleOrgUpdateEvent(uMsg);
		break;
	case WM_GROUPDOWNLOAD_PACKFILE_DOWNLOADED: //下载了群资料包
		{			
			//#COMMAND#type=packdownloaded,{packid,roomname,from,packname,attribute,time}
			char* pMsg = new char[256];

			sPackReadReceipt* pPackReadReceipt = (sPackReadReceipt*)wParam;

			sprintf(pMsg,"#COMMAND#type=packdownloaded,{\"packid\":\"%s\",\"roomname\":\"%s\",\"from\":\"%s\",\"packname\":\"%s\",\"attribute\":\"%s\",\"time\":\"%s\"}",pPackReadReceipt->strPackID.c_str(),pPackReadReceipt->strRoomName.c_str(),pPackReadReceipt->strFrom.c_str(),pPackReadReceipt->strPackName.c_str(),pPackReadReceipt->strAttribute.c_str(),pPackReadReceipt->strTime.c_str());
			tstring sMsg = pMsg;

			m_pGroupChatPage->SendGroupMessage(sMsg);			
		}
		break;
	case WM_USER+912:
		{
			if (NULL != m_pProjectPage)
			{
				m_pProjectPage->RefreshPage();
			}
		}
		break;
	case WM_USER+199:
		{
			fileInfo* pTmp = (fileInfo*)(wParam);
			if (NULL != pTmp)
			{
				addFileMsg(*pTmp);
				OnProcess(pTmp->strKey, 100, pTmp->stropposite);
				delete pTmp;
				pTmp = NULL;
			}			
		}
		break;
	default:
		break;
	}
	
	return 0;
}



bool mainDlg::OnModifyPartnerMemo(tstring& strAccount)
{
	//先获取当钱的memo
	//先从cache中看是否存在
	bool bHaveCard = false;
	sUserVCard oneCard;
	if (NULL != m_pUsrMgrAdapter)
	{
		bHaveCard = m_pUsrMgrAdapter->GetUserVCard(strAccount, oneCard);
	}
	//不存在就从本地DB去找
	if (!bHaveCard)
	{
		bHaveCard = GetDBTaskModule()->LoadUserVCard(strAccount, oneCard);
	}

	oneCard.strRemark = m_pUsrMgrAdapter->GetMemo(strAccount);

	if (bHaveCard)
	{
		CModifyMemoWnd* pMemoModify = new CModifyMemoWnd;
		pMemoModify->InitMemo(strAccount, oneCard.strRemark);

		if (NULL != pMemoModify)
		{
			pMemoModify->ShowWindow(this->GetHWND());
		}
	}

	
	return true;
}

void mainDlg::OnUpdateMemoBtnClicked(tstring& strAccount, tstring& strNewMemo)
{
	//如果有修改了,更新到Server
	m_pUsrMgrAdapter->UpdateMemo(strAccount, strNewMemo);

	//同步更新界面：好友列表+最近联系人+好友信息页面
	UpdateMemoInFriendAndChatList(strAccount, strNewMemo);

	//更新本地数据库，一定在数据库，否则就是错误的。
	sUserVCard oneCard;
	bool bInDB = GetDBTaskModule()->LoadUserVCard(strAccount, oneCard);
	if (bInDB)
	{
		oneCard.strRemark = strNewMemo;
		GetDBTaskModule()->SaveUserVCard(strAccount, oneCard);
	}

	//优化排序，如果修改备注之后重新排序位置没有变化则不需要update UI
	tstring strOrgID;
	int pos = 0;
	vecFriendListItemIter iter = find_if(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper(strAccount, CSortHelper::eFoundField::eFoundAccount));
	if (m_listAllFriends.end() != iter) 
	{
			iter->strShowName = strNewMemo;
			strOrgID = iter->strOrgID;
			pos = std::distance(m_listAllFriends.begin(), iter);
	}
	
	vecFriendListItemIter firstIter, lastIter;
	CSortHelper::FindIf(m_listAllFriends.begin(), m_listAllFriends.end(), firstIter, lastIter, CSortHelper(strOrgID, CSortHelper::eFoundField::eFoundGroup));
	sort(firstIter,lastIter,CSortHelper::CompareFuncIntraGroup);

	if (pos != std::distance(m_listAllFriends.begin(),  find_if(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper(strAccount, CSortHelper::eFoundField::eFoundAccount))))
	{
		RefreshGroupList(strAccount, false);
	}	
}

void mainDlg::UpdateMemoInFriendAndChatList(tstring& strAccount, tstring& strNewMemo)
{
	//更新好友信息页面
	if (NULL != m_pPartnerInfopage)
	{
		m_pPartnerInfopage->UpdataMemo(strNewMemo);
	}

	//对于好友列表和最近联系人列表，如果memo为空的，就显示昵称，次之显示手机号
	tstring strShowName;
	if (strNewMemo.empty())
	{
		//先从cache中看是否存在
		bool bHaveCard = false;
		sUserVCard oneCard;
		bHaveCard = m_pUsrMgrAdapter->GetUserVCard(strAccount, oneCard);
		
		//不存在就从本地DB去找
		if (!bHaveCard)
		{
			bHaveCard = GetDBTaskModule()->LoadUserVCard(strAccount, oneCard);
		}

		if (bHaveCard)
		{
			strShowName = oneCard.strUserNickname;
		}
	}
	else
	{
		strShowName = strNewMemo;
	}


	//更新好友列表cache+ui
	if (m_listAllFriends.size()>0)
	{
		for (int i=0; i<m_listAllFriends.size(); i++)
		{
			sFriendListItem& oneFriend = m_listAllFriends[i];
			if (_tcsicmp(oneFriend.strAccount.c_str(), strAccount.c_str()) == 0)
			{
				oneFriend.strShowName = strShowName;
				break;
			}
		}

		UpdataShowNameInFriendList(strAccount, strShowName);
	}

	

	//跟新最近联系人cache+ui
	if (m_lstFriends.size()>0)
	{
		for (int i=0; i<m_lstFriends.size(); i++)
		{
			ContactListItemInfo& oneRecent = m_lstFriends[i];
			oneRecent.m_strNickName = strShowName.c_str();
			break;
		}

		UpdateShowNameInRecentChatList(strAccount, strShowName);
	}

}

void mainDlg::OnShowGroupChatWnd_Personal(tstring& strCellPhone, tstring& strRoomId)
{
	tstring strAccount = strCellPhone + "@test-d";

	//1.切换"消息"页面
	static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat))->Selected(true);

	//2.判断联系人页面是否有这个群了，如果有置顶，如果没有添加
	bool bGroupInRecentList = false;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); iter!=m_lstFriends.end(); iter++)
		{
			//个人账号，且，有Room信息
			if ( _tcsicmp(strAccount.c_str(), (*iter).m_strAccount.GetData()) == 0
				&& _tcsicmp(strRoomId.c_str(), iter->m_strRoomID.GetData() ) == 0)
			{
				ContactListItemInfo info = *iter;
				m_lstFriends.erase(iter);
				m_lstFriends.insert(m_lstFriends.begin(), info);
				bGroupInRecentList = true;
				break;
			}
		}
	}

	//将新建的群组加入到“最近联系人列表”数据库		
	if (!bGroupInRecentList)
	{
		//个人信息
		tstring strNickName;
		tstring strAvatarPath;
		sUserVCard oneUser;
		if (GetDBTaskModule()->LoadUserVCard(strAccount,  oneUser))
		{
			strNickName = oneUser.strUserNickname.c_str();
			strAvatarPath = oneUser.strPhotoPath;
		}
		sUserVCard oneNewVCard;
		if(CController::Instance()->GetUserInfoMgr()->GetUserVCard(strAccount, oneNewVCard))
		{
			strNickName = oneNewVCard.strUserNickname;
			strAvatarPath = oneNewVCard.strPhotoPath;
		}
		if (strNickName.empty())
		{
			strNickName = strCellPhone;
		}
		if (strAvatarPath.empty())
		{
			strAvatarPath = DEF_AVATAR_NOR;
		}

		//将新建的群组加入到“最近联系人列表”数据库		
		ContactListItemInfo info;
		info.m_strAccount = strAccount.c_str();
		info.m_strLogo = strAvatarPath.c_str();
		info.m_strNickName = strNickName.c_str();
		info.m_strOrgName  = strNickName.c_str();//没有机构，就用昵称
		info.m_strOrgID   =(_T(""));
		info.m_strRoomID   = strRoomId.c_str();
		info.m_strLastMsg =(_T(""));
		info.m_strLastTm   = GetFullTimeString(time(0)).c_str();//"2016-04-05 16:27:23";
		info.nItemID	   = GetNextItemID();

		//注意为什么这里要进行赋值，是因为这里的函数和 OnNewGroupMemberList 回调先后顺序是随机的
		ComposeGroupAvatar(info);
		m_lstFriends.insert(m_lstFriends.begin(), info);		

		RecentGroupItem oneGroup;
		oneGroup.strRoomID = strRoomId;
		oneGroup.strGroupNick=(_T(""));//OnRoomnameArrived回调会更新
		oneGroup.strOrgID=(_T(""));
		oneGroup.strOrgName = strNickName.c_str();
		oneGroup.strParticipant = strCellPhone;//对方的账号。

		GetDBTaskModule()->SaveRecentGroupItem(oneGroup);
	}

	//3. 刷新界面
	RefreshContactListControl();

	//4.给群聊天界面的文件包进行初始化
	UpdatePackListInGroupChat(strRoomId);
}

void mainDlg::OnShowGroupChatWnd(tstring& strOrgID, tstring&strOrgName, tstring& strRoomID)
{
	//1.切换"消息"页面
	static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat))->Selected(true);

	//2.判断联系人页面是否有这个群了，如果有置顶，如果没有添加
	bool bGroupInRecentList = false;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); 
			iter!=m_lstFriends.end(); iter++)
		{
			if ( SYS_GROUP_CHAT_ACCOUNT == tstring((*iter).m_strAccount))
			{
				//同一个机构，有可能ROOMID不同。???这是什么意思？？？by zyy
				//by zyy 认为roomid具有唯一性
				if (_tcsicmp(iter->m_strRoomID.GetData(), strRoomID.c_str()) == 0)
//				if (_tcsicmp(iter->m_strOrgID.GetData(), strOrgID.c_str()) == 0
//					&& _tcsicmp(iter->m_strRoomID.GetData(), strRoomID.c_str()) == 0)
				{
					ContactListItemInfo info = *iter;
					m_lstFriends.erase(iter);
					m_lstFriends.insert(m_lstFriends.begin(), info);
					bGroupInRecentList = true;
					break;
				}
			}
		}
	}

	//将新建的群组加入到“最近联系人列表”数据库	
	//群组头像用群组人员合并成后，这里的http获得的头像就没用了。
	if (!bGroupInRecentList)
	{
		//获取机构信息
		tstring strOrgID;
		tstring strOrgName;
		tstring strAvatarBase64;

		//个人信息
		tstring strDestPhone;
		tstring strNickName;

		int nTypeValue = 0;//1:org,2:person,3:error

		CHttpClient reqClient;
		tstring strResponse;
		tstring strToken = CController::Instance()->GetToken();
		tstring strUrlOrg(WEB_SERVER_BASE_URL);
		strUrlOrg += "business/get-pars-by-rid?rid=";

		strUrlOrg += strRoomID;
		tstring strOutUTF8Org;
		CChineseCode::GB2312ToUTF_8(strOutUTF8Org, strUrlOrg.c_str(), strUrlOrg.length());
		reqClient.Get(strOutUTF8Org, strToken, strResponse);

		bool bFail = false;
		if (!strResponse.empty())
		{
			//如果是机构
			if (strResponse.find("id") != -1 
				&& strResponse.find("name") != -1 
				&& strResponse.find("avatar_base64") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strResponse, root))
					{
						strOrgID = root["id"].asString();
						CChineseCode::UTF_8ToGB2312(strOrgName, root["name"].asString().c_str(), root["name"].asString().length());;
						strAvatarBase64 = root["avatar_base64"].asString();
						nTypeValue = 1;
					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
				}	
			}
			//如果是个人
			else if (strResponse.find("cell_phone") != -1 
				&& strResponse.find("id") != -1 
				&& strResponse.find("nick") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strResponse, root))
					{
						strDestPhone = root["cell_phone"].asString();
						CChineseCode::UTF_8ToGB2312(strNickName, root["nick"].asString().c_str(), root["nick"].asString().length());;
						nTypeValue = 2;
					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
				}	
			}
			else
			{
				nTypeValue = 3;//result=0
			}
		}

 		tstring strAvatarPath;
		
		//将新建的群组加入到“最近联系人列表”数据库		
		ContactListItemInfo info;
		info.m_strAccount = SYS_GROUP_CHAT_ACCOUNT;

		//为什么要在群名称回调的时候得到呢？群名称不可以传进来吗？
		info.m_strNickName = strOrgName.c_str();//初始化为机构名，在群名称回调的时候，更新这个名称
		info.m_strOrgName  = strOrgName.c_str();
		info.m_strOrgID    = strOrgID.c_str();
		info.m_strRoomID   = strRoomID.c_str();
		info.m_strLastMsg =(_T(""));
		info.m_strLastTm   = GetFullTimeString(time(0)).c_str();//"2016-04-05 16:27:23";
		info.nItemID	   = GetNextItemID();

		//注意为什么这里要进行赋值，是因为这里的函数和 OnNewGroupMemberList 回调先后顺序是随机的
		ComposeGroupAvatar(info);
		m_lstFriends.insert(m_lstFriends.begin(), info);

		RecentGroupItem oneGroup;
		oneGroup.strRoomID = strRoomID;
		oneGroup.strGroupNick = strOrgName;
		oneGroup.strOrgID = strOrgID;
		oneGroup.strOrgName = strOrgName;
		oneGroup.strParticipant = CController::Instance()->GetXmppClient()->jid().username();//自己进群。
		oneGroup.strAvatarPath = info.m_strLogo;
		GetDBTaskModule()->SaveRecentGroupItem(oneGroup);
	}

	//3. 刷新界面
	RefreshContactListControl();

	//4.给群聊天界面的文件包进行初始化
	UpdatePackListInGroupChat(strRoomID);
}

void mainDlg::InsertPackItemToChatRoom(tstring& strRoom, tstring& strPackID)
{
	if (NULL != m_pGroupChatPage)
	{
		m_pGroupChatPage->InsertFirstPack(strRoom, strPackID);
	}
}

void mainDlg::UpdatePackListInGroupChat(tstring& strRoomID)
{
	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();

	//获取当前的项目ID
	tstring strUrlNewProj(WEB_SERVER_BASE_URL);
	strUrlNewProj += "business/get-bid-by-rid?rid=";
	strUrlNewProj += strRoomID;
	tstring strOutUTF8Projname;
	CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlNewProj.c_str() , strUrlNewProj.length());
	int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);

	tstring strProjectID;
	if (!strResponse.empty() && strResponse.find("bid") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strResponse, root))
			{
				strProjectID = root["bid"].asString();
			}
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
		}	
	}
	else
	{
		return;
	}

	if (strProjectID.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("错误","获取项目ID失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	//初始化改room下的资料包列表
	if (NULL != m_pGroupChatPage)
	{
		m_pGroupChatPage->InitPackFiles(strProjectID, strRoomID);
	}
}


void mainDlg::OnShowChatWnd(tstring& strAccount)
{
	//1.切换主页面到联系人界面
	static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat))->Selected(true);

	//2.判断联系人页面是否有， 如果有置顶，如果没有添加
	bool bInRecentList = false;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); 
			iter!=m_lstFriends.end(); iter++)
		{
			if ( strAccount == tstring((*iter).m_strAccount) 
				&& iter->m_strRoomID.IsEmpty()) //并且这个人没有和我有群组聊天的项目。
			{
				ContactListItemInfo info = *iter;
				m_lstFriends.erase(iter);
				m_lstFriends.insert(m_lstFriends.begin(), info);
				bInRecentList = true;
				break;
			}
		}
	}

	//将新加的联系人加入到“最近联系人列表”数据库		
	if (!bInRecentList)
	{

		ContactListItemInfo info;
		GetContackItemByAccount(strAccount, info);
		m_lstFriends.insert(m_lstFriends.begin(), info);

		RECENTITEM oneItem;
		oneItem.strUid = strAccount;
		oneItem.strTime=(_T(""));
		GetDBTaskModule()->SaveRecentFriendList(oneItem);
	}

	//3. 刷新界面
	RefreshContactListControl();
}

void mainDlg::OnResponseNewVCard(sUserVCard& oneInfo)
{
	//save to database.
	GetDBTaskModule()->SaveUserVCard(oneInfo.strAccount, oneInfo);

	//1.更新最近联系人：头像，显示名
	int nSize = m_lstFriends.size();
	if (nSize>0)
	{
		for (int i=0; i<nSize; i++)
		{
			//找到这个人
			ContactListItemInfo& oneContact = m_lstFriends[i];
			if (_tcsicmp(oneContact.m_strAccount, oneInfo.strAccount.c_str()) == 0)
			{
				//1：更新头像。
				if (enmState_Offline == m_pUsrMgrAdapter->GetState(oneInfo.strAccount))
				{
					if (!oneInfo.strGrayAvatar.empty())
					{
						oneContact.m_strLogo = oneInfo.strGrayAvatar.c_str();
						UpdateAvatarInRecentList(oneInfo.strAccount, oneInfo.strGrayAvatar);
					}	
				}
				else if (enmState_Online == m_pUsrMgrAdapter->GetState(oneInfo.strAccount))
				{
					if (!oneInfo.strPhotoPath.empty())
					{
						oneContact.m_strLogo = oneInfo.strPhotoPath.c_str();
						UpdateAvatarInRecentList(oneInfo.strAccount, oneInfo.strPhotoPath);
					}	
				}
				else if (enmState_Busy == m_pUsrMgrAdapter->GetState(oneInfo.strAccount))
				{

				}

				//2：更改显示名称：备注>昵称>手机号
				tstring strMemo = m_pUsrMgrAdapter->GetMemo(oneInfo.strAccount);
				if (strMemo.empty())
				{
					oneContact.m_strNickName = oneInfo.strUserNickname.c_str();
					if (oneContact.m_strNickName.IsEmpty())
					{
						oneContact.m_strNickName = oneInfo.strPhone.c_str();
					}
				}
				else
				{
					oneContact.m_strNickName = strMemo.c_str();
				}

				UpdateShowNameInRecentChatList(oneInfo.strAccount, tstring(oneContact.m_strNickName.GetData()));

				break;
			}
		}	
	}
	
	//2.更新好友列表:头像，显示名
	int nFriendCount = m_listAllFriends.size();
	if (nFriendCount>0)
	{
		bool bUpdated = false;
		for (int i=0; i<nFriendCount; i++)
		{
			//找到这个人
			sFriendListItem& oneFriend = m_listAllFriends[i];
			if (_tcsicmp(oneFriend.strAccount.c_str(), oneInfo.strAccount.c_str()) == 0)
			{
				//1：更新头像
				if (enmState_Offline == m_pUsrMgrAdapter->GetState(oneInfo.strAccount))
				{
					if (!oneInfo.strGrayAvatar.empty())
					{
						oneFriend.strAvatar = oneInfo.strGrayAvatar.c_str();
						UpdateAvatarInFriendList(oneInfo.strAccount, oneInfo.strGrayAvatar);
						bUpdated = true;
					}
					else
					{
						oneInfo.strPhotoPath = DEF_AVATAR_GRAY;
						oneInfo.strGrayAvatar = DEF_AVATAR_GRAY;
						UpdateAvatarInFriendList(oneInfo.strAccount, oneInfo.strGrayAvatar);
						bUpdated = true;
					}
				}
				else if (enmState_Online == m_pUsrMgrAdapter->GetState(oneInfo.strAccount))
				{
					if (!oneInfo.strPhotoPath.empty())
					{
						oneFriend.strAvatar = oneInfo.strPhotoPath.c_str();
						UpdateAvatarInFriendList(oneInfo.strAccount, oneInfo.strPhotoPath);

						bUpdated = true;
					}	
					else
					{
						oneInfo.strPhotoPath = DEF_AVATAR_NOR;
						oneInfo.strGrayAvatar = DEF_AVATAR_NOR;
						UpdateAvatarInFriendList(oneInfo.strAccount, oneInfo.strPhotoPath);
						bUpdated = true;
					}
				}
				else if (enmState_Busy == m_pUsrMgrAdapter->GetState(oneInfo.strAccount))
				{

				}

				//2:更新显示名
				tstring strMemo = m_pUsrMgrAdapter->GetMemo(oneInfo.strAccount);
				if (strMemo.empty())
				{
					oneFriend.strShowName = oneInfo.strUserNickname.c_str();
					if (oneFriend.strShowName.empty())
					{
						oneFriend.strShowName = oneInfo.strPhone.c_str();
					}
				}
				else
				{
					oneFriend.strShowName = strMemo.c_str();
				}
				
				UpdataShowNameInFriendList(oneInfo.strAccount, oneFriend.strShowName);

				break;
			}
			/*if (bUpdated)
			{
			tstring strShowName;
			if (oneInfo.strRemark.empty())
			{
			if (oneInfo.strUserNickname.empty())
			{
			strShowName =  oneInfo.strPhone;
			}
			else
			{
			strShowName = oneInfo.strUserNickname;
			}
			}
			else
			{
			strShowName = oneInfo.strRemark;
			}

			UpdataShowNameInFriendList(oneInfo.strAccount, strShowName );

			break;
			}*/
		}
	}

	m_pPartnerInfopage->updateView(oneInfo);
}


//LRESULT mainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	return 0;
//}

LPCTSTR mainDlg::GetResourceID() const
{
	return MAKEINTRESOURCE(101);
}

LPCTSTR mainDlg::GetWindowClassName() const
{
	return _T("MMGui");
}

void mainDlg::Notify(TNotifyUI& msg)   //控件消息响应
{
	if (!isConnected())//断线了，直接屏蔽
	{
		return;
	}

	if (_tcsicmp(msg.sType, _T("dbclick")) == 0)
	{
		//不支持双击跳转到发送消息。会与click事件冲突。
		if (_tcsicmp(msg.pSender->GetName(), kListContactsFriendList) == 0)
		{
			OnDbClickFriendList(msg);
		}
		
	}
	else if (_tcsicmp(msg.sType, _T("rclick")) == 0)
	{	
		if (_tcsicmp(msg.pSender->GetName(), kListContactsControlName) == 0)
		{
			if (m_pRecentList->GetCurSel() == -1) return;

			//这里需要再次判断，系统消息，欢迎页面都不能出现右键菜单
			int nItemID = -1;//m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag();
			nItemID = m_pRecentList->GetItemAt(m_pRecentList->GetCurSel())->GetTag();

			tstring account;
			if (nItemID>-1)
			{
				for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
					it != m_lstFriends.end(); ++it)
				{
					if (nItemID == it->nItemID)
					{
						account = it->m_strAccount;
						break;
					}
				}
			}

			if (account == SYS_NOTIFY_ACCOUNT || account.empty())
			{
				return;
			}

			RECT rc = m_pRecentList->GetItemAt(m_pRecentList->GetCurSel())->GetPos();
			POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
			::ClientToScreen(*this, &pt);
			if (::PtInRect(&rc, pt))
			{
				CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_recentlist.xml"), eUIMenuDeleteFriend));
				pMenu->Init(*this, pt);
				pMenu->ShowWindow(TRUE);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kListContactsFriendList) == 0)
		{
			if (m_pFriendLst->GetCurSel() == -1) return;
			
			RECT rc = m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetPos();
			POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
			::ClientToScreen(*this, &pt);
			if (::PtInRect(&rc, pt))
			{
				if (!m_pFriendLst->IsParentNode((Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag()))
				{
					//by zyy 
					if (select_business_)
					{
						CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_business.xml"), eUIMenuDeleteFriend));
						pMenu->Init(*this, pt);
						pMenu->ShowWindow(TRUE);
					}
					else
					{
						int nItemID = -1;
						tstring account;
						Node* pNode = (Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag();
						nItemID = pNode->data().value;

						if (nItemID>-1)
						{
							for (int idx = 0; idx < m_listAllFriends.size(); idx++)
							{	
								if (m_listAllFriends[idx].nItemID == nItemID)
								{
									account = m_listAllFriends[idx].strAccount;
									break;
								}
							}
						}

						if (account != m_myselfInfo.strAccount)//不是本人
						{
							CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_friendlist.xml"), eUIMenuDeleteFriend));
							pMenu->Init(*this, pt);
							pMenu->ShowWindow(TRUE);
						}
					}
				}                
			}

// 				if (!m_pFriendLst->IsParentNode((Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag()))
// 				{
// 					int nItemID = -1;
// 					tstring account;
// 					tstring org;
// 
// 					Node* pNode = (Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag();
// 					nItemID = pNode->data().value;
// 
// //					m_pFriendLst->CanExpand(node)
// 
// 					if (nItemID>-1)
// 					{
// 						for (int idx = 0; idx < m_listAllFriends.size(); idx++)
// 						{	
// 							if (m_listAllFriends[idx].nItemID == nItemID)
// 							{
// 								account = m_listAllFriends[idx].strAccount;
// 								break;
// 							}
// 						}
// 					}
// 
// 					if (m_pFriendLst->CanExpand(pNode))//是机构
// 					{
// 						CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_business.xml"), eUIMenuDeleteFriend));
// 						pMenu->Init(*this, pt);
// 						pMenu->ShowWindow(TRUE);
// 					}
// 					else
// 					{
// 						if (account != m_myselfInfo.strAccount)//不是本人
// 						{
// 							CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_friendlist.xml"), eUIMenuDeleteFriend));
// 							pMenu->Init(*this, pt);
// 							pMenu->ShowWindow(TRUE);
// 						}
// 					}
// 				}
// 				if (!m_pFriendLst->IsParentNode((Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag()))
// 				{
// 					//by zyy 未能深入了解，暂时的做法。
// 					if (select_business_)
// 					{
// 						
// 					}
// 					else
// 					{
// 						//这里需要再次判断，本人不能出现右键菜单
// 						
// 
// 						//是我自己
// 						if (account == m_myselfInfo.strAccount)
// 						{
// 							return;
// 						}
// 
// 						
// 					}
// 				}				
// 			}
		}	
				
	}	
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		tstring strSender = msg.pSender->GetName();

		if (_tcsicmp(msg.pSender->GetName(), "ButtonAgreed") == 0)
		{
			INT_PTR pData = msg.pSender->GetParent()->GetParent()->GetTag();
			OnBnClickAddFriendReq(true, pData);
			CLogger::GetInstance()->PrintErrLog( "clicked agreed add friend button");	
		}
		else if (_tcsicmp(msg.pSender->GetName(), "ButtonAgreedOrg") == 0)
		{
			INT_PTR pData = msg.pSender->GetParent()->GetParent()->GetTag();
			OnBnClickAddFriendReq(true, pData);
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btnBusinessMaterial") == 0)
		{
			sProjectItem* pData = new sProjectItem;
			pData->strRoomID = m_CurItem.m_strRoomID;
			pData->strDestOrgID = m_CurItem.m_strOrgID;
			//pData->strDestOrgName = m_CurItem.m_strOrgName;

			if (CWebClient::getBidByRid(pData->strRoomID, pData->strProjID))
			{
				CFrameBusinessData::Instance()->ShowWindow(true);
				CFrameBusinessData::Instance()->setProjectItem(pData);
			}
			else
			{
				return;
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btnBusinessQuit") == 0)
		{
			//退出群聊 by zyy
			//退出群聊现在两步：先通过web 服务器检测当前用户是不是这个room的创建者，弹出相应的提示dialog;
			//然后调用web api 更新room 成员，最后调用xmpp的退出iq
			//
			tstring strCurRoomID = m_CurItem.m_strRoomID.GetData();

			//群主账号是#define  SYS_GROUP_CHAT_ACCOUNT			"10001@test-d"				//群组聊天的系统账号
			//也就是说没有人是群主？
			tstring strCurRoomAccount = m_CurItem.m_strAccount.GetData();
			if (strCurRoomAccount == m_myselfInfo.strAccount)
			{

			}
			else
			{
				CMsgBox* msg_box = new CMsgBox("","确定要退出该群？",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
				if (MSGBOX_IDYES == msg_box->ShowModal(NULL))
				{
					CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();

					vector<tstring> vecNewMem;
					vecNewMem.push_back(m_myselfInfo.strAccount);			
					pRoomMgr->ExitFromRoom(strCurRoomID, vecNewMem);
					pRoomMgr->ExitFromRoom(strCurRoomID, vecNewMem);//为什么要删除两次才能生效
					OnDeleteGroupFromMemberList(strCurRoomID);
					SetTimer(this->GetHWND(), WM_TIMER_DELETE_ROOM_LIST, 200, NULL);
				}	
			}			
		}
		//! 意见反馈
		else if(_tcsicmp(msg.pSender->GetName(), _T("BtnFeadbackSujection")) == 0)
		{
			ShellExecute(NULL,_T("open"),SUGGESTIONS_URL ,NULL,NULL,SW_SHOW);
		}

		else if (_tcsicmp(msg.pSender->GetName(), _T("btn_chose_business")) == 0)
		{
			CDuiString strNum = "0";
			m_pRecentList->setMsgNum(strNum, 10000);
			COptionUI *pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionBussinessPartner));
			pOption->Selected(true);
			
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("ButtonQueryFiles")) == 0)
		{
			CFrameBussinessData::Instance()->ShowWindow(true);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("btn_add_business")) == 0)
		{
			CDuiString strNum = "0";
			m_pRecentList->setMsgNum(strNum, 10000);
			COptionUI *pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionBussinessPartner));
			pOption->Selected(true);
			OnShowSearchPartnerPage();
		}
		else if(_tcsicmp(msg.pSender->GetName(), "ButtonDeny") == 0)
		{
			INT_PTR pData = msg.pSender->GetParent()->GetParent()->GetTag();
			OnBnClickAddFriendReq(false, pData);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kOptionCollection) == 0)
		{
			CHttpClient httpClient;
			tstring strUrl(WEB_SERVER_BASE_URL);
			strUrl += "user/get-key";

			tstring strToken = CController::Instance()->GetToken();
			tstring strHttpResponse;
			int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
			tstring strKey;
			if (strHttpResponse.find("\"result\":1") != -1 && strHttpResponse.find("key") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strHttpResponse, root))
					{
						
						strKey = root["key"].asString();
						

					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strHttpResponse.c_str());	
				}	
			}
			else
			{
//北京调试注销
//				return;
			}
			tstring strUrlColl(WEB_DOMAIN_URL);//  '998321c5c9960dfc880cf74234dd53e3'
			strUrlColl += "collection/index?key=";
			strUrlColl += strKey;
			LPCSTR lpURL = strUrlColl.c_str();
			
// 			ostringstream oss;
// 			oss<<"file:\\"<<CFileHelper::GetCurrentAppPath().GetData()<<"web.html";
// 			tstring str = oss.str();
// 
// 			tstring test_url = "file:\\\D:\\project\\mm-win\\bin\\web.html";

			ostringstream oss;
			oss<<"file:\\"<<CFileHelper::GetCurrentAppPath().GetData()<<"empty.html";
			tstring str = oss.str();

//			pWebKitUI->SetURL(str);
			pWebKitUI->SetURL(lpURL);
			pWebKitUI->SetInternVisible();

			jsBindFunction("msgBox", js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数

//			pWebKitUI->RunJS("window.scrollTo(0,document.body.scrollHeight)"); 
//			pWebKitUI->RunJS("document.body.style.overflow='hidden'");
			

//			ShellExecute(NULL, _T("open"), lpURL, "", NULL, SW_MINIMIZE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kOptionSysSetting) == 0)
		{
			POINT pt;  // = {50, 520};
			CDuiMenu *pMenu = NULL; // = new CDuiMenu(_T("menu_sys_setting.xml"), eUIMenuSysSetting);
			//m_iCurIdentity = USER_NO_ORG;
			m_iCurIdentity = apiGetUserIdentity();
			switch(m_iCurIdentity)
			{
			case USER_NO_ORG:
				pMenu = new CDuiMenu(_T("menu_sys_setting_no_org.xml"), eUIMenuSysSetting);			
				break;
			case USER_ORG_BOSS:
				pMenu = new CDuiMenu(_T("menu_sys_setting_org_boss.xml"), eUIMenuSysSetting);
				break;
			case USER_ORG_MEMBER:
				pMenu = new CDuiMenu(_T("menu_sys_setting_org_member.xml"), eUIMenuSysSetting);				
				break;
			default:
				return;
			}
			if (pMenu != NULL)
			{
				RECT rect;
				GetClientRect(this->GetHWND(), &rect);
				pt.x = 40;
				pt.y = rect.bottom;
				pMenu->Init(*this, pt);
				pMenu->ShowWindow(TRUE);
			}
		}
	/*	else if(_tcsicmp(msg.pSender->GetName(), kBtnHomePage) == 0)
		{
			ShowHomePage();
		}*/
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonMaxControlName) == 0)
		{
			if (IsZoomed(this->GetHWND()))
			{
				::ShowWindow(*this, SW_RESTORE);			
			}
			else
			{
				::ShowWindow(*this, SW_MAXIMIZE);			
			}			
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonMinControlName) == 0)
		{
			::ShowWindow(*this, SW_MINIMIZE);
		}	
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonCloseControlName) == 0)
		{
			//CVideoDlg::Instance()->Destroy();
// 			CMsgBox* pMsgBox = new CMsgBox("退出登录","是否确定退出登录",MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
// 			if(pMsgBox->ShowModal() == MSGBOX_IDYES)
// 			{
// 				OnExit(msg);
// 			}
			CFrameSysQuit::Instance()->ShowWindow(true);
		}
		else if (msg.pSender->GetName() == kBtnDialPhoneName || msg.pSender == m_pHomePageNetCall)
		{			
			/*if (!CUcPaasClient::Instance()->IsUcsConnected())
			{
				CMsgBox* pMsgBox = new CMsgBox("","无法连接语音通信服务器，不能进行语音通话",  MSGBOX_ICON_ERROR);
				pMsgBox->ShowModal();
				return;
			}			*/
			CCallFrame::Instance()->InitalFriendList(m_listAllFriends);
		}	
		else if (msg.pSender == m_pHomePageInvitePartner)
		{
			CInvitePartner* pInvitePartnerWnd = new CInvitePartner();
			pInvitePartnerWnd->CreateWnd(GetHWND());
		}
		else if (msg.pSender == m_pHomePageCreateOrg)
		{
			orgInfo org;
			CWebClient::GetMyOrg(org);
			if(!org.strId.empty() || _tcsicmp(org.strId.c_str(),_T("-1")) != 0)
			{
				tstring strMsg=(_T(""));
				strMsg += "您已经拥有机构(";
				strMsg += org.strName;
				strMsg += ")！";
				CMsgBox::MsgBox(GetHWND(), "创建机构", strMsg, MSGBOX_ICON_WARNING);
				return;
			}
			else
			{
				HandleCreateOrg();
			}
		}
		else if (msg.pSender == m_pHomePageBussiness)
		{
			pWebKitUI->SetInternVisible(false);
			static_cast<COptionUI*>(m_PaintManager.FindControl("TabProject"))->Selected(true);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonAddRosterControlName) == 0)
		{
			OnShowSearchPartnerPage();
		}
		else if (_tcsicmp(msg.pSender->GetName(), kButtonCreateNewProject) == 0)
		{
			OnShowCreateNewProjPage();
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose_tableview1")) == 0)
		{
			CTabLayoutUI* pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kBussinesPartnerTabLayout));
			if (NULL != pLayout)
			{
				pLayout->SelectItem(0);
			}
		}
		else if(msg.pSender->GetName() == _T("btnSystemSetting"))
		{
			OnClickSystemSettings();
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonOpenFile")) == 0)
		{
			
			CFileHelper fileHelpter;
		tstring strBasePath = fileHelpter.GetUserDir() +"User\\";  //+ m_MyselfItem.m_strAccount + "\\RecFiles\\";
		if (!fopen(strBasePath.c_str(), "w") )
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		strBasePath += mainDlg::getInstance()->getMyselfItem().m_strAccount + "\\";
		if (!fopen(strBasePath.c_str(), "w"))
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		strBasePath += "RecvFiles\\";
		if (!fopen(strBasePath.c_str(), "w"))
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		m_strFilePath = strBasePath;
			if (m_strFilePath != "")
				ShellExecute(NULL, "explore", m_strFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("button_element_send")) == 0)
		{
			tstring strKey = msg.pSender->GetParent()->GetParent()->GetName();
			int nSize = m_mapTaskFiles.size();
			map<tstring, fileInfo>::iterator iter = m_mapTaskFiles.find(strKey);
			//开始发送，隐藏发送和取消按钮
			CListContainerElementUI *pelement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());
			CButtonUI *pbtnSend = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pelement, _T("button_element_send")));
			CButtonUI *pbtnCancel = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pelement, _T("button_element_cancel")));
			if (pbtnSend != NULL && pbtnCancel != NULL)
			{
				pbtnSend->SetVisible(false);
				pbtnCancel->SetVisible(false);
			}


			m_FileControl.uploadFiles(iter->second, (IOssEventBase*)this);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("button_element_cancel")) == 0)
		{
			tstring strKey = msg.pSender->GetParent()->GetParent()->GetName();
			int nSize = m_mapTaskFiles.size();
			map<tstring, fileInfo>::iterator iter = m_mapTaskFiles.find(strKey);
			//1. UI清除
			CVerticalLayoutUI *pFileProcess = NULL;
			map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(tstring(m_CurItem.m_strAccount));
			if (it != m_mapFileTab.end())
			{
				CListContainerElementUI *pelement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());


				CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(it->second,"listfileProcess"));
				if (plstProcess!=NULL && pelement!=NULL)
					plstProcess->Remove(pelement);
				//2. map容器中清除
				m_mapTaskFiles.erase(iter);
				nSize = m_mapTaskFiles.size();
			}
			else
				return;

			
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("button_element_accept")) == 0)
		{
			CButtonUI* btn_reject = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("button_element_reject")));
			btn_reject->SetVisible(false);

			CButtonUI* btn_accept = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("button_element_accept")));
			btn_accept->SetVisible(false);

			tstring strKey = msg.pSender->GetParent()->GetParent()->GetName();
			map<tstring, fileInfo>::iterator iter = m_mapTaskFiles.find(strKey);
			//开始发送，隐藏发送和取消按钮
			CListContainerElementUI *pelement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());
			CButtonUI *pbtnSend = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pelement, _T("button_element_send")));
			CButtonUI *pbtnCancel = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pelement, _T("button_element_cancel")));
			if (pbtnSend != NULL && pbtnCancel != NULL)
			{
				pbtnSend->SetVisible(false);
				pbtnCancel->SetVisible(false);
			}
			bool bDownloaded = m_FileControl.downloadFiles(iter->second, (IOssEventBase*)this);
			if ( bDownloaded )
			{
				if (NULL != m_pMsgSessionMgr)
				{
					//check if empty.
					if (m_CurItem.m_strAccount.GetLength() == 0)
					{
						return;
					}

					SYSTEMTIME sysTime;
					::GetLocalTime(&sysTime);
					char* pTime = new char[128];
					sprintf(pTime,"%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds,sysTime.wDayOfWeek);

					tstring sTime = pTime;
					tstring sFileName = iter->second.strFileFullName;
					tstring sFrom = CController::Instance()->GetXmppClient()->jid().username();

					char* pReply = new char[256];

					sprintf(pReply,"#COMMAND#type=filedownloaded,{\"filename\":\"%s\",\"from\":\"%s\",\"time\":\"%s\"}",sFileName.c_str(),sFrom.c_str(),sTime.c_str());

					tstring sReply =	pReply;
					m_pMsgSessionMgr->SendMsg(tstring(m_CurItem.m_strAccount.GetData()), sReply);
				}
			}
			//int i = 0;
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("button_element_reject")) == 0)
		{
			CButtonUI* btn_reject = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("button_element_reject")));
			btn_reject->SetVisible(false);

			CButtonUI* btn_accept = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("button_element_accept")));
			btn_accept->SetVisible(false);


			tstring strKey = msg.pSender->GetParent()->GetParent()->GetName();
			map<tstring, fileInfo>::iterator iter = m_mapTaskFiles.find(strKey);

			//发送拒绝回执
			SYSTEMTIME sysTime;
			::GetLocalTime(&sysTime);
			char* pTime = new char[128];
			sprintf(pTime,"%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds,sysTime.wDayOfWeek);

			tstring sTime = pTime;
			tstring sFileName = iter->second.strFileFullName;
			tstring sFrom = CController::Instance()->GetXmppClient()->jid().username();

			char* pReply = new char[256];

			sprintf(pReply,"#COMMAND#type=filerejected,{\"filename\":\"%s\",\"from\":\"%s\",\"time\":\"%s\"}",sFileName.c_str(),sFrom.c_str(),sTime.c_str());

			tstring sReply =	pReply;
			m_pMsgSessionMgr->SendMsg(tstring(m_CurItem.m_strAccount.GetData()), sReply);

			
			//1. UI清除
			CVerticalLayoutUI *pFileProcess = NULL;
			map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(tstring(m_CurItem.m_strAccount));
			if (it != m_mapFileTab.end())
			{
				CListContainerElementUI *pelement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());


				CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(it->second,"listfileProcess"));
				if (plstProcess!=NULL && pelement!=NULL)
					plstProcess->Remove(pelement);
				//2. map容器中清除
				m_mapTaskFiles.erase(iter);
				if (NULL != m_pMsgSessionMgr)
				{
					//check if empty.
					if (m_CurItem.m_strAccount.GetLength() == 0)
					{
						return;
					}
				}
			}
			else
				return;
		}

		else if (_tcsicmp(msg.pSender->GetName(), kbuttonTabFile) == 0)
		{
			vector<tstring> lstFiles;
			if (getSelectedFiles(lstFiles))
			{
				addFilesForSend(lstFiles);
				// 准备上传文件
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonFile")) == 0)
		{
			if (m_CurItem.nItemID == -1)
			{
				MessageBox(NULL, _T("请先选择联系人！"), _T("提示"), MB_OK);
				return;
			}
			
			vector<tstring> lstFiles;
			if (getSelectedFiles(lstFiles))
			{
				addFilesForSend(lstFiles);
			}
			else
				return;
			//m_FileControl.uploadFiles(m_lstUploadFiles, (IOssEventBase *) this);
			refreshFilesProcess(m_lstUploadFiles);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("ButtonClip")) == 0)
		{
			//先屏蔽起来
			HMODULE hHandle = LoadLibrary("CameraDll.dll");
			if(NULL == hHandle)
			{
				MessageBox(NULL, _T("Error LoadLibrary <CameraDll.dll>！"), _T("提示"), MB_OK);
				return;
			}
			typedef int (WINAPI *CAMERASUBAREA)(int);
			CAMERASUBAREA pCameraSubArea = (CAMERASUBAREA)GetProcAddress(hHandle,"CameraSubArea");
			if(NULL == pCameraSubArea)
			{
				MessageBox(NULL, _T("Error LoadLibrary <CameraDll.dll> on Func <CameraSubArea>！"), _T("提示"), MB_OK);
				FreeLibrary(hHandle);
				return;
			}
			int nCapRes = pCameraSubArea(0);
			FreeLibrary(hHandle); 

			if(nCapRes != 1)
			{
				return;
			}

			//图片现在已经在剪贴板
			CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
			//CControlUI *pImage = new CControlUI();
			//pImage->SetName(_T("image"));
			if(!OpenClipboard(NULL))
			{
				return;
			}
			if(IsClipboardFormatAvailable(CF_BITMAP))
			{
				HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
				int e=GetLastError();
				Bitmap bmp(hBitmap,NULL);
				//pImage->SetFixedWidth(bmp.GetWidth());
				//pImage->SetFixedHeight(bmp.GetHeight());
				if (hBitmap != NULL) 
				{
					SaveBitmapToFile(hBitmap,L"d:\\test.png");
				}
				CloseClipboard ();
			}
			/*pImage->SetBkImage("d:\\test.png");
			RECT rt;
			rt.left = pEditInput->GetPos().left;
			rt.top = pEditInput->GetPos().top;

			rt.right = rt.left + pImage->GetFixedWidth();
			rt.bottom = rt.top + pImage->GetFixedHeight();

			pEditInput->SetPos(rt,true);*/
			
			//以下部分未完全成型且可能会在多处调用,后续需要进行封装
			ITextServices * pTextServices = pEditInput->GetTextServices();
			ITextHost * pTextHost = pEditInput->GetTextHost();
			if (pTextServices != NULL && pTextHost != NULL)
			{
				RichEdit_SetStartIndent(pTextServices, 300);
				RichEdit_InsertFace(pTextServices, pTextHost,"d:\\test.png", -1, -1, RGB(255,255,255), TRUE, 40);
			}
	
			if (pTextServices != NULL)
				pTextServices->Release();
			if (pTextHost != NULL)
				pTextHost->Release();

			//pEditInput->Add(pImage);
			//pEditInput->SetBkImage("d:\\test.png");
			
			//pEditInput->EnableScrollBar(true,false);
			//tstring sUserData=(_T(""));
			//CMagicKHelper::Instance()->GetPhotoBinval("d:\\test.png", sUserData);
			//pImage->SetUserData(sUserData.c_str());
		}
		else if(_tcsicmp(msg.pSender->GetName(), kbuttonFileSend) == 0)
		{
			//uploadfiles
			//CFileControl FileControl;
			//m_FileControl.uploadFiles(m_lstUploadFiles, (IOssEventBase *) this);
			refreshFilesProcess(m_lstUploadFiles);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(1);
		}
		else if (msg.pSender == m_pVoiceBtn) 
		{
			CVideoDlg::Instance()->OnLauchNetCall(m_CurItem.m_strAccount, WM_VOICE_AUTO);			
		}
		else if (msg.pSender == m_pVideoBtn) 
		{
			CVideoDlg::Instance()->OnLauchNetCall(m_CurItem.m_strAccount, WM_VIDEO_CALL);
		}		
		else if (msg.pSender->GetName() == kBtnCallTypeDropName) 
		{
			OnClickCallTypeDrop(msg.ptMouse);			
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonRejectFile")) == 0)
		{
			//todo 拒绝接受文件
			int i=0;
			return;
		}		
		else if (_tcsicmp(msg.pSender->GetName(), _T("btnHomePage")) == 0)
		{
			
			COptionUI* pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat));
			if (NULL != pOption)
			{
				pOption->Selected(true);
			}
			
			CTabLayoutUI* playout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout));
			if (NULL!= playout)
			{
				//记录上次的最近联系人的显示页面。
				if (playout->GetCurSel() == CONST_RECENT_TAB_P2P_CHAT_LIST)
				{
					//m_nLastSelectIndex = CONST_RECENT_TAB_P2P_CHAT_LIST;
					m_strSelectedIndex = "0";
				}
				else if(playout->GetCurSel() == CONST_RECENT_TAB_SYS_MSG_PAGE)
				{
					//m_nLastSelectIndex = CONST_RECENT_TAB_SYS_MSG_PAGE;
					m_strSelectedIndex = "1";
				}
				else if (playout->GetCurSel() == CONST_RECENT_TAB_HOME_PAGE)
				{
					//m_nLastSelectIndex = CONST_RECENT_TAB_HOME_PAGE;
					m_strSelectedIndex = "2";
				}
				else if (playout->GetCurSel() == CONST_RECENT_TAB_GROUPCHAT_PAGE)
				{
					//m_nLastSelectIndex = CONST_RECENT_TAB_GROUPCHAT_PAGE;
					m_strSelectedIndex = "3";
				}
				
				
				//设置当前为主页面
				playout->SelectItem(CONST_RECENT_TAB_HOME_PAGE/*2*/);	
				m_pBussinessMaterialBtn->SetVisible(false);
				m_pBussinessQuitBtn->SetVisible(false);
				SetTimer(this->GetHWND(), WM_TIEMER_UPDATE_HOMEPAGE_PIC, 2000, NULL);
			}
			
			
			return;
		}			
		else if (_tcsicmp(msg.pSender->GetName(), kOptionRecentChat) == 0 )
		{
			CTabLayoutUI* pLayouRecnet = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout));
			if (pLayouRecnet != NULL)
			{
				if(2 == pLayouRecnet->GetCurSel())
				{
					CTabLayoutUI* pLayoutBar =static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar));
					if (TAB_VIEW_RECENTCHAT_PAGE == pLayoutBar->GetCurSel())
					{
						pLayouRecnet->SelectItem(CONST_RECENT_TAB_P2P_CHAT_LIST/*0*/);	
						m_pBussinessMaterialBtn->SetVisible(false);
						m_pBussinessQuitBtn->SetVisible(false);
						KillTimer(this->GetHWND(), WM_TIEMER_UPDATE_HOMEPAGE_PIC);
						return;
					}
				}
			}
		}	
		else if (_tcsicmp(msg.pSender->GetName(), "btn_show_groupMember") == 0)
		{
			OnShowGroupMemberWnd();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_check_org_projectlist") == 0)
		{
			if (m_pPartnerInfopage)
			{
				m_pPartnerInfopage->ShowGroupProjectList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_add_orgfriend") == 0)
		{
//这里又是机构id的问题
//			int iOrgId = msg.pSender->GetTag();
// 			char * org_id = (char*)msg.pSender->GetTag();
// 			std::tstring strResult;
// 			strResult = org_id;
			if (m_pPartnerInfopage)
			{
				m_pPartnerInfopage->addOrgFriend();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_del_orgfriend") == 0)
		{
			int  iOrgId = msg.pSender->GetTag();
			if (m_pPartnerInfopage)
			{
				m_pPartnerInfopage->delOrgFriend(iOrgId);
			}
		}
	}
	else if(_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		//查看是不是侧边栏被点击了
		CTabLayoutUI* pSidebarTabs = static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar));
		if (NULL == pSidebarTabs) 
			return;

		//侧边栏“最近联系人”被选中		
		KillTimer(this->GetHWND(), WM_TIEMER_UPDATE_HOMEPAGE_PIC);

		if(msg.pSender->GetName() == kOptionRecentChat)
		{
			pWebKitUI->SetInternVisible(false);
			pSidebarTabs->SelectItem(TAB_VIEW_RECENTCHAT_PAGE);
			//lee：父级的TabLayout切换以后，子TabLayout要还原到上次的选中layout！
			int nSelected = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout))->GetCurSel();
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout))->SelectItem(nSelected/*CONST_RECENT_TAB_P2P_CHAT_LIST*/);	
			if (nSelected == CONST_RECENT_TAB_GROUPCHAT_PAGE)
			{
				m_pBussinessMaterialBtn->SetVisible(true);
				m_pBussinessQuitBtn->SetVisible(true);
			}
			else
			{
				m_pBussinessMaterialBtn->SetVisible(false);
				m_pBussinessQuitBtn->SetVisible(false);
			}
		}
		//'项目'
		else if (msg.pSender->GetName() == kOptionProject)
		{
			pWebKitUI->SetInternVisible(false);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->SelectItem(TAB_VIEW_PROJECT_PAGE);		
		}
		// 签约
		else if(msg.pSender->GetName() == RADIO_MAINDLG_PAGE_CONTRACT)
		{
			pWebKitUI->SetInternVisible(false);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->SelectItem(4);
			m_pContractSelect->InitData();
		}
		//侧边栏“商业伙伴”被选中了
		else if (msg.pSender->GetName() == kOptionBussinessPartner) 
		{
			pWebKitUI->SetInternVisible(false);
			if (NULL != pSidebarTabs)
			{
				pSidebarTabs->SelectItem(TAB_VIEW_BUSSINESS_PARTNER_PAGE);
			}
			CTabLayoutUI* pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kBussinesPartnerTabLayout));
			if (NULL != pLayout)
			{
				pLayout->SelectItem(0);
			}
		}
		//'钱包'
		else if (msg.pSender->GetName() == kOptionMoney) 
		{
			pWebKitUI->SetInternVisible(false);
  			tstring strUrl(WEB_DOMAIN_URL);
  			strUrl += "payment";
  			ShellExecute(NULL, _T("open"), _T("explorer.exe"),strUrl.c_str(), NULL, SW_SHOW);	
		}
		else if (msg.pSender->GetName() == kOptionExecute) 
		{
			tstring strUid, strKey;
//			GetUidAndKey(strUid, strKey);
//			tstring strUid, strKey;
			//			GetUidAndKey(strUid, strKey);
			CHttpClient httpClient;
			tstring strUrl(WEB_SERVER_BASE_URL);
			strUrl += "user/get-key";

			tstring strToken = CController::Instance()->GetToken();
			tstring strHttpResponse;
			int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
			//			tstring strUid, strKey;
			if (strHttpResponse.find("\"result\":1") != -1 && strHttpResponse.find("key") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strHttpResponse, root))
					{
						strKey = root["key"].asString();
						strUid = root["uid"].asString();
					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strHttpResponse.c_str());	
				}	
			}
			else
			{
				//北京调试注销
				//				return;
			}

//			http://192.168.8.14/user/csslogin/index?uid=111&sid=222&referUrl=/finance/index/index
//			tstring strUrlColl("http://192.168.8.14:80/");//  '998321c5c9960dfc880cf74234dd53e3'
			tstring strUrlColl(WEB_ROOT);
			strUrlColl += "user/csslogin/index?uid=";
			strUrlColl += strUid;
			strUrlColl += "&sid=";
			strUrlColl += strKey;
			strUrlColl += "&referUrl=/";//finance/index/index";
			strUrlColl += REFER_EXECUTE;
			ostringstream oss;
			oss<<"file:\\"<<CFileHelper::GetCurrentAppPath().GetData()<<"empty.html";
			tstring str = oss.str();

//			pWebKitUI->SetURL(str);
			pWebKitUI->SetURL(strUrlColl);
			pWebKitUI->SetInternVisible();

			jsBindFunction("bindFinance", JsFinance, 1);//这里绑定js函数，让js主动调用c++函数	
		}
		else if (msg.pSender->GetName() == kOptionFaceDeal) 
		{
			tstring strUid, strKey;
//			GetUidAndKey(strUid, strKey);
//			tstring strUid, strKey;
			//			GetUidAndKey(strUid, strKey);
			CHttpClient httpClient;
			tstring strUrl(WEB_SERVER_BASE_URL);
			strUrl += "user/get-key";

			tstring strToken = CController::Instance()->GetToken();
			tstring strHttpResponse;
			int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
			//			tstring strUid, strKey;
			if (strHttpResponse.find("\"result\":1") != -1 && strHttpResponse.find("key") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strHttpResponse, root))
					{
						strKey = root["key"].asString();
						strUid = root["uid"].asString();
					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strHttpResponse.c_str());	
				}	
			}
			else
			{
				//北京调试注销
				//				return;
			}

			//			http://192.168.8.14/user/csslogin/index?uid=111&sid=222&referUrl=/finance/index/index
			//			tstring strUrlColl("http://192.168.8.14:80/");//  '998321c5c9960dfc880cf74234dd53e3'
			tstring strUrlColl(WEB_ROOT);
			strUrlColl += "user/csslogin/index?uid=";
			strUrlColl += strUid;
			strUrlColl += "&sid=";
			strUrlColl += strKey;
			strUrlColl += "&referUrl=/";//finance/index/index";
			strUrlColl += REFER_FACE_DEAL;
			ostringstream oss;
			oss<<"file:\\"<<CFileHelper::GetCurrentAppPath().GetData()<<"empty.html";
			tstring str = oss.str();

//			pWebKitUI->SetURL(str);
			pWebKitUI->SetURL(strUrlColl);
			pWebKitUI->SetInternVisible();

			jsBindFunction("bindFinance", JsFinance, 1);//这里绑定js函数，让js主动调用c++函数	
		}
		else if (msg.pSender->GetName() == kOptionFinance) 
		{
			tstring strUid, strKey;
//			GetUidAndKey(strUid, strKey);
			CHttpClient httpClient;
			tstring strUrl(WEB_SERVER_BASE_URL);
			strUrl += "user/get-key";

			tstring strToken = CController::Instance()->GetToken();
			tstring strHttpResponse;
			int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
//			tstring strUid, strKey;
			if (strHttpResponse.find("\"result\":1") != -1 && strHttpResponse.find("key") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strHttpResponse, root))
					{
						strKey = root["key"].asString();
						strUid = root["uid"].asString();
					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strHttpResponse.c_str());	
				}	
			}
			else
			{
				//北京调试注销
				//				return;
			}

			//			http://192.168.8.14/user/csslogin/index?uid=111&sid=222&referUrl=/finance/index/index
			//			tstring strUrlColl("http://192.168.8.14:80/");//  '998321c5c9960dfc880cf74234dd53e3'
			tstring strUrlColl(WEB_ROOT);
			strUrlColl += "user/csslogin/index?uid=";
			strUrlColl += strUid;
			strUrlColl += "&sid=";
			strUrlColl += strKey;
			strUrlColl += "&referUrl=/";//finance/index/index";
			strUrlColl += REFER_FINANCE;
// 			ostringstream oss;
// 			oss<<"file:\\"<<CFileHelper::GetCurrentAppPath().GetData()<<"empty.html";
// 			tstring str = oss.str();

//			pWebKitUI->SetURL(str);
			pWebKitUI->SetURL(strUrlColl);
			pWebKitUI->SetInternVisible();

			jsBindFunction("bindFinance", JsFinance, 1);//这里绑定js函数，让js主动调用c++函数	
		}



		ShowNetCallBtn(pSidebarTabs->GetCurSel() == 0 
			&& !m_CurItem.m_strAccount.IsEmpty() 
			&& (CRegexHelper::IsValidMobileNo(ExtractPhoneNo(m_CurItem.m_strAccount).GetData())//如果是合法手机号才显示
				|| _tcsicmp(m_CurItem.m_strAccount.GetData(), SYS_GROUP_CHAT_ACCOUNT) == 0));//如果是群组账号也显示
	}
	else if (_tcsicmp(msg.sType, _T("itemactivate")) == 0)
	{
		int i=0;
		
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		//聊天记录列表被点击了。
		if (_tcsicmp(msg.pSender->GetName(), kListContactsControlName) == 0)
		{
			OnRecentChatListSelected();
		}
	}
	//如果好友列表被点击了。就切换显示：有个bug：右键点击的时候也会显示，to do...
	else if(_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		HandleItemClickEvent(msg);		
	}
	else if(_tcsicmp(msg.sType,  DUI_MSGTYPE_MOUSEENTER) == 0)
	{
		HandleMouseEnterEvent(msg);		
	}

}

void mainDlg::OnRecentChatListSelected()
{
	if (NULL == m_pRecentList)
	{
		return;
	}
	int iIndex = m_pRecentList->GetCurSel();

	//设置当前激活的好友
	tstring strAccount;
	bool bOrgPersonal=false;//是否是业务中的个人：有room信息，无机构信息，有个人信息
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout));
	if (NULL == pLayout)
	{
		return;
	}
	CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(iIndex));
	if (pItem != NULL)
	{
		if (pItem->GetTag() == 10000)
		{
			pLayout->SelectItem(CONST_RECENT_TAB_HOME_PAGE);
			m_pBussinessMaterialBtn->SetVisible(false);
			m_pBussinessQuitBtn->SetVisible(false);
			return;
		}
		for (int i=0; i<m_lstFriends.size(); i++)
		{
			if (pItem->GetTag() == m_lstFriends[i].nItemID)
			{
				m_CurItem = m_lstFriends[i];
				strAccount = m_lstFriends[i].m_strAccount;
				break;
			}
		}
	}

	//切换tab页签
	if (_tcsicmp(strAccount.c_str(), SYS_NOTIFY_ACCOUNT) == 0)
	{
		pLayout->SelectItem(CONST_RECENT_TAB_SYS_MSG_PAGE);
		m_pBussinessMaterialBtn->SetVisible(false);
		m_pBussinessQuitBtn->SetVisible(false);
		
		
		//隐藏下拉框，头像和相关按钮
		CButtonUI* pBtnShowMem = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_show_groupMember"));
		if (NULL != pBtnShowMem)
		{
			pBtnShowMem->SetVisible(false);
		}

		CLabelUI *pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName));
		if (NULL != pFriendNickname)
		{
			pFriendNickname->SetText(_T(""));
		}

		CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
		if (NULL != pFriendOrgname) 
		{
			pFriendOrgname->SetText(_T(""));
		}
		CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
		if (NULL != pBtnFriendAvatar)
		{
			pBtnFriendAvatar->SetNormalImage(_T(""));
		}
		CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
		if (NULL != pGroupMemCount)
		{
			pGroupMemCount->SetText(_T(""));
		}
		m_pVideoBtn->SetVisible(false);
		m_pVoiceBtn->SetVisible(false);

		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "lable_duty_" << i;
			CLabelUI *pLabelProperty = static_cast<CLabelUI*>(m_PaintManager.FindControl(ostr.str().c_str()));
			if (NULL != pLabelProperty)
			{				
				pLabelProperty->SetVisible(false);				
			}
		}




		CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kBtnCallTypeDropName));
		if (NULL != pControl )
		{
			pControl->SetVisible(false);
		}
		
		//处理系统消息和气泡
		//1. 更新系统消息气泡个数
		int nCount = GetMsgCenterModule()->GetUnreadMsgCount(SYS_NOTIFY_ACCOUNT);
		if (nCount>0)
		{
			CDuiString strCount;
			strCount.Format(_T("%d"), 0);
			m_pRecentList->setMsgNum(strCount, iIndex);
		}

		//2. 删除未读消息
		ListMsgs lstSys;
		//GetMsgCenterModule()->GetUnReadSysMsgs(lstSys);
		//GetMsgCenterModule()->ClearUnReadSysMsgs();
		
		//3. 
		//停止闪烁
		GetTrayTaskModule()->StopFlash(tstring(SYS_NOTIFY_ACCOUNT), enmMsgType_Sys);

		//4.  边栏
		m_bSysUnread = false;
		bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);

		//5.更新消息的状态
		for (ListMsgs::iterator it = lstSys.begin(); it != lstSys.end(); ++it)
		{
			CMsgBase* pMsg = (CMsgBase*)(*it);
			GetDBTaskModule()->UpdateSysMsgStatus((time_t)(pMsg->uTime), true);
			/*if (pMsg->strFromBare == SYS_NOTIFY_ACCOUNT)
			{
			listAllMsg.push_back(pMsg);
			}*/
		}
		

		return;
	}
	else if (_tcsicmp(strAccount.c_str(), SYS_GROUP_CHAT_ACCOUNT) == 0 
		|| (!m_CurItem.m_strRoomID.IsEmpty() && m_CurItem.m_strOrgID.IsEmpty()))//或者对方是个人的room聊天
	{
		//清空输入框的内容
		CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl("inputEdit_groupchat"));
		if (NULL == pEditInput)
		{
			return;
		}
		pEditInput->SetText(_T(""));

		//如果有切换，就刷新聊天记录。
		m_pGroupChatPage->SetCurrentRoom(tstring(m_CurItem.m_strRoomID.GetData()));

		//显示。退出。
		pLayout->SelectItem(CONST_RECENT_TAB_GROUPCHAT_PAGE);
		m_pBussinessMaterialBtn->SetVisible(true);
		m_pBussinessQuitBtn->SetVisible(true);
		//return;
	}
	else
	{
		pLayout->SelectItem(CONST_RECENT_TAB_P2P_CHAT_LIST);
		m_pBussinessMaterialBtn->SetVisible(false);
		m_pBussinessQuitBtn->SetVisible(false);
	}

	//更新窗口顶部的用户信息：昵称，Avatar，机构名。
	UpdateUserInfoAtWndTop(strAccount, m_CurItem.m_State, tstring(m_CurItem.m_strLogo.GetData()));
	ShowNetCallBtn(true);

	if (_tcsicmp(strAccount.c_str(), SYS_GROUP_CHAT_ACCOUNT) == 0 
		|| (!m_CurItem.m_strRoomID.IsEmpty() && m_CurItem.m_strOrgID.IsEmpty()))//如果对方是个人的群聊天
	{
		m_pVideoBtn->SetVisible(false);
		m_pVoiceBtn->SetVisible(false);
		CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kBtnCallTypeDropName));
		if (NULL != pControl )
		{
			pControl->SetVisible(false);
		}

		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "lable_duty_" << i;
			CLabelUI *pLabelProperty = static_cast<CLabelUI*>(m_PaintManager.FindControl(ostr.str().c_str()));
			if (NULL != pLabelProperty)
			{				
				pLabelProperty->SetVisible(false);				
			}
		}
	}
	else
	{
		m_pVideoBtn->SetVisible();
		m_pVoiceBtn->SetVisible();
		CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kBtnCallTypeDropName));
		if (NULL != pControl )
		{
			pControl->SetVisible();
		}

		vector<tstring> vproperty;
		//好友的OrgID是空的，因此需要再次去查询//by zyy
		const tstring id = m_CurItem.m_strID;//这也是空的，why？？？？？
		const tstring org_id = m_CurItem.m_strOrgID;
		const tstring account = m_CurItem.m_strAccount;
		const tstring org_name = m_CurItem.m_strOrgName;
		orgInfo info;
		tstring str;
		const tstring real_id = account.substr(0, account.length()-7);

		//根据账号id查询机构信息
		CWebClient::GetOrgByPhone(real_id, info, str);

		GetDuty(info.strId, account, vproperty);
		vector<CLabelUI*> vect_label;
		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "lable_duty_" << i;
			CLabelUI *pLabelProperty = static_cast<CLabelUI*>(m_PaintManager.FindControl(ostr.str().c_str()));
			if (NULL != pLabelProperty)
			{
				if (i>vproperty.size())
				{
					pLabelProperty->SetVisible(false);
				}
				else
				{
					pLabelProperty->SetText(vproperty[i-1].c_str());
					pLabelProperty->SetVisible(true);
					vect_label.push_back(pLabelProperty);
				}
			}
		}
	}
	

	if (_tcsicmp(strAccount.c_str(), SYS_GROUP_CHAT_ACCOUNT) != 0 && (m_CurItem.m_strRoomID.IsEmpty()))
	//if (m_CurItem.m_strRoomID.IsEmpty())//如果是个人
	{
		//清空和重新读取历史聊天消息。
		CChatListUI* pOutput = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
		CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
		if (NULL != pOutput)
		{
			pOutput->RemoveAll();
		}
		if (NULL != pEditInput)
		{
			pEditInput->SetText((_T("")));
		}

		if (NULL != pOutput)
		{
			pOutput->ReloadChatList(CDuiString(strAccount.c_str()));
		}

		//如果有未读消息==>读取消息，并告知flash停止。
		//to do...修改意见===>这个应该放在ItemClick的事件响应里面去做。跟itemselect响应区分开来。
		if (m_pRecentList->CheckIfBubbleExist(iIndex))
		{
			//1.清空气泡
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, iIndex);

			//2.插入消息。
			InsertUnReadMsg(strAccount);

			//3.Stop Flash.
			GetTrayTaskModule()->StopFlash(strAccount, enmMsgType_Chat);

			//4.如何左侧栏有消息提示 清空
			bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);
		}
	}

	//如果当前选中的是群，消息已经插入过了，只要清除气泡和停止闪烁即可。
	if (!m_CurItem.m_strRoomID.IsEmpty())
	{
		if (m_pRecentList->CheckIfBubbleExist(iIndex))
		{
			//1.清空气泡
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, iIndex);

			tstring strRoomID = m_CurItem.m_strRoomID.GetData();
			//2.清空这个房间的未读消息个数
			m_pGroupChatPage->GetGroupChatLogic()->ClearUnReadGroupMsg(strRoomID);
			//InsertUnReadMsg(strAccount);

			//3.Stop Flash.
			GetTrayTaskModule()->StopGroupFlash(strRoomID);

			//4.如何左侧栏有消息提示 清空
			bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);
		}
	}

	//lwt 文件页签同时选中
	map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(strAccount);
	if (it != m_mapFileTab.end())
	{
		static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(it->second);
		m_pCurFileTab = it->second;
	}
	else
	{
		//新建一个
		CDialogBuilder  m_DlgBuilder;
		CVerticalLayoutUI *pRecentChat = NULL;
		CDialogBuilderCallbackEx cb(&m_PaintManager);
		CVerticalLayoutUI *pFileProcess = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create(_T("tab_file_process.xml"),
				(UINT)0, &cb, &m_PaintManager));
		}
		else
		{
			pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
		}

		//追加到TabLayout Container容器内，索引值+1
		static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->Add(pFileProcess);
		static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(pFileProcess);
		static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
		m_mapFileTab.insert(make_pair(strAccount, pFileProcess));
		m_pCurFileTab = pFileProcess;
	}
	
}

static int sn_Sidebar_Width		= 40;
static int sn_FriendList_Width	= 215;
static int sn_CaptionBar_Height = 98;

void mainDlg::OnFriendListSelected(tstring& strAccount, bool bOrgItem, bool bInOrg)
{
	if (bOrgItem && NULL != m_pPartnerInfopage)
	{
		m_pPartnerInfopage->InitOrgInfoPage();
	}

	if (NULL != m_pPartnerInfopage)
	{
		//先从cache中看是否存在
		bool bHaveCard = false;
		sUserVCard oneCard;
		tstring strOrg;
		tstring strOid;

		//下面的code都是之前写的针对account，而不是机构的！！

		select_business_ = false;//by zyy
		if (!bOrgItem)
		{
			if (NULL != m_pUsrMgrAdapter)
			{
				bHaveCard = m_pUsrMgrAdapter->GetUserVCard(strAccount, oneCard);
			}

			//不存在就从本地DB去找 
			if (!bHaveCard)
			{
				bHaveCard = GetDBTaskModule()->LoadUserVCard(strAccount, oneCard);
			}

			//由于本地DB更新操作不work，暂时先更新下
			oneCard.strRemark = m_pUsrMgrAdapter->GetMemo(strAccount);

			//拿机构信息
			for (int i=0; i<m_listAllFriends.size(); i++)
			{
				if (strAccount == m_listAllFriends[i].strAccount)
				{
					strOrg = m_listAllFriends[i].strOrgnization;
					strOid = m_listAllFriends[i].strOrgID;
					break;
				}
			}

			//by zyy
			if (strOid == "-1")
			{
				select_business_ = true;
			}

			if (bHaveCard)
			{
				m_pPartnerInfopage->Initialize(strAccount, oneCard, strOrg, strOid);
			}
			else
			{
				m_pUsrMgrAdapter->FetchVCardByAccount(strAccount);
			}
		}
		
		
		if (bOrgItem && !bInOrg)
		{
			m_pPartnerInfopage->Initialize(strAccount, oneCard, strOrg, strOid);
		}
	}
}

void mainDlg::InsertUnReadMsg(tstring& strFromBare)
{
	ListMsgs listMsgs;
	GetMsgCenterModule()->GetUnReadTextMsgs(strFromBare, listMsgs);
	if (listMsgs.size()>0)
	{
		for (ListMsgs::iterator it = listMsgs.begin(); it != listMsgs.end(); ++it)
		{
			CTextMsg* pOneMsg = (CTextMsg*)(*it);
			if (pOneMsg != NULL)
			{
				CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));

				ChatInfo chatinfo;
				chatinfo.m_lstMsg.push_back(pOneMsg->strMsg.c_str());
				chatinfo.m_strAccount = pOneMsg->strFromBare.c_str();/*m_CurItem.m_strNickName;*/
				chatinfo.m_strRawMsg = pOneMsg->strMsg.c_str();
				chatinfo.uTime = pOneMsg->uTime;/*"12:30"*/;
				chatinfo.m_strLogo = (CFileHelper::GetHigtLightAvatar(pOneMsg->strFromBare)).c_str();		
				
				pChatlst->AddOppositeMsgItem(&chatinfo);
				pChatlst->CacheChatMessage(CDuiString(strFromBare.c_str()), &chatinfo);
				SetTimer(this->GetHWND(), WM_TIMER_CHATLISTENDDOWN, 50, NULL);
			}
		}
	}
	GetMsgCenterModule()->ClearUnReadTextMsgs(strFromBare);
}

// void mainDlg::OnReceiveCmdMsg(tstring& strFrom, ECmdType eType, tstring& strParas)
// {	
// 	//添加入右侧下载列表
// 	refreshFilesDownloadProcess(strParas, strFrom);
// }

void mainDlg::ProcessSysMsg_FileRequest(tstring& strFrom, tstring& strParas)
{
	refreshFilesDownloadProcess(strParas, strFrom);
}

void mainDlg::ProcessSysMsg_VoiceRequest(tstring& strFrom, tstring& strParas)
{
	fileInfo downLoadInfo; 
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strParas, root))
	{
		try
		{
			downLoadInfo.strDuration = root["duration"].asString();
			downLoadInfo.strFileFullName = root["file_name"].asString();
			downLoadInfo.strFileUrl = root["ali_file_url"].asString();
			tstring strSz = root["file_size"].asString();
			downLoadInfo.dwFileSize = atol(strSz.c_str());
			tstring strName = downLoadInfo.strFileFullName;
			tstring strfileName = strName.substr(0, strName.find_last_of("."));
			tstring strfileType = strName.substr(strName.find_last_of(".") + 1, strName.length());
			downLoadInfo.strFileName = strfileName;
			downLoadInfo.strFileType = strfileType;
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strFrom.c_str());	
		}	

	}
	//设置下载路径
	CFileHelper fileHelpter;
	tstring strBasePath = fileHelpter.GetUserDir() +"User\\";  //+ m_MyselfItem.m_strAccount + "\\RecFiles\\";
	if (!fopen(strBasePath.c_str(), "w") )
	{
		CreateDirectory(strBasePath.c_str(), NULL);
	}
	strBasePath += mainDlg::getInstance()->getMyselfItem().m_strAccount + "\\";
	if (!fopen(strBasePath.c_str(), "w"))
	{
		CreateDirectory(strBasePath.c_str(), NULL);
	}
	strBasePath += "RecvFiles\\";
	if (!fopen(strBasePath.c_str(), "w"))
	{
		CreateDirectory(strBasePath.c_str(), NULL);
	}
	strBasePath += "Voice\\";
	if (!fopen(strBasePath.c_str(), "w"))
	{
		CreateDirectory(strBasePath.c_str(), NULL);
	}
	m_strFilePath = strBasePath;
	strBasePath += downLoadInfo.strFileFullName;
	downLoadInfo.strFilePath = strBasePath;
	//downLoadInfo.strKey = strBasePath + "download" + strFrom;
	downLoadInfo.stropposite = strFrom;


	bool bRes = m_FileControl.downloadSignalFile(downLoadInfo, NULL);

	if (!bRes)
		return;


	//ui
	CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
	ChatInfo chatinfo;
	chatinfo.m_strAccount = m_myselfInfo.strAccount.c_str();/*m_CurItem.m_strNickName*/;
	chatinfo.uTime = time(0)/*"12:30"*/;
	chatinfo.m_strLogo = (CFileHelper::GetHigtLightAvatar(tstring(chatinfo.m_strAccount))).c_str();
	//chatinfo.m_lstFiles.push_back(info);
	//tstring strPack = info.strPackID;
	pChatlst->AddOppositeFileMsgItem(&chatinfo);       //todo lwt


	//CVerticalLayoutUI *pFileProcess = NULL;
	//map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(strFrom);
	//if (it != m_mapFileTab.end())
	//{
	//	//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(it->second);
	//	//m_pCurFileTab = it->second;
	//	pFileProcess = it->second;
	//}
	//else
	//{
	//	//新建一个
	//	CDialogBuilder  m_DlgBuilder;
	//	CVerticalLayoutUI *pRecentChat = NULL;
	//	CDialogBuilderCallbackEx cb(&m_PaintManager);
	//	//CVerticalLayoutUI *pFileProcess = NULL;
	//	if (!m_DlgBuilder.GetMarkup()->IsValid())
	//	{
	//		pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create(_T("tab_file_process.xml"),
	//			(UINT)0, &cb, &m_PaintManager));
	//	}
	//	else
	//	{
	//		pFileProcess = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	//	}

	//	//追加到TabLayout Container容器内，索引值+1
	//	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->Add(pFileProcess);
	//	//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(pFileProcess);
	//	//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
	//	m_mapFileTab.insert(make_pair(strFrom, pFileProcess));
	//}
	////CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(m_pCurFileTab,"listfileProcess"));
	//CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(pFileProcess,"listfileProcess"));
	//CDialogBuilder m_DlgBuilder;
	//CListContainerElementUI *pRecentChat = NULL;
	//tstring filePath = downLoadInfo.strFilePath;
	//CDialogBuilderCallbackEx cb(&m_PaintManager);
	//if (!m_DlgBuilder.GetMarkup()->IsValid())
	//{
	//	pRecentChat = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("element_file_download_progress.xml"),
	//		(UINT)0, &cb, &m_PaintManager));
	//}
	//else
	//{
	//	pRecentChat = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	//}
	//if (pRecentChat != NULL)
	//{
	//	pRecentChat->SetName(downLoadInfo.strKey.c_str());
	//	bool bRes = plstProcess->Add(pRecentChat);
	//}
	////1. 设置icon
	////todo
	////2. 设置名字
	//CLabelUI *plableFileName = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileName")));

	////3. 设置文件大小
	//double sz = double(downLoadInfo.dwFileSize) / 1024;
	////sz = sz/1024;
	//CDuiString strsz;
	//strsz.Format("%.2f", sz);
	//strsz += _T("KB");
	//tstring strValue = downLoadInfo.strFileName + "(" + tstring(strsz)+")";
	//plableFileName->SetText(strValue.c_str());
	////4. 放入map容器
	//m_mapTaskFiles.insert(make_pair(downLoadInfo.strKey, downLoadInfo));
	//int nSize = m_mapTaskFiles.size();

	////5. ui显示
	//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
}

void mainDlg::UpdateGroupLastMessage(sGroupChatItem* pOneGroupChat, bool bTextMsg)
{
	//1.判断联系人页面是否有这个群了，如果有置顶，如果没有添加
	bool bGroupInRecentList = false;
	int nItemId = -1;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); 
			iter!=m_lstFriends.end(); iter++)
		{
			//if ( SYS_GROUP_CHAT_ACCOUNT == tstring((*iter).m_strAccount))
			if(!iter->m_strRoomID.IsEmpty())
			{
				if (_tcsicmp(iter->m_strRoomID.GetData(), pOneGroupChat->strRoomID.c_str()) == 0)
				{
					ContactListItemInfo info = *iter;
					info.m_strLastMsg =  bTextMsg ? pOneGroupChat->strTextContent.c_str() : "";
					info.m_strLastTm  = (GetHMSTimeString(pOneGroupChat->tSendTime)).c_str();

					m_lstFriends.erase(iter);
					m_lstFriends.insert(m_lstFriends.begin(), info);
					bGroupInRecentList = true;
					nItemId = info.nItemID;
					break;
				}
			}
		}
	}

	//不在机构，就创建一个Item
	if (!bGroupInRecentList)
	{
		tstring room_id =  pOneGroupChat->strRoomID.c_str();

		if (1)//CController::Instance()->GetRoomManager()->IsInThisRoom(room_id, m_myselfInfo.strAccount))
		{
			//获取机构信息
			tstring strOrgID;
			tstring strOrgName;
			tstring strAvatarBase64;

			//个人信息
			tstring strDestPhone;
			tstring strDestNickName;
			tstring strDestAvatar;

			int nTypeValue = 0;//1:org,2:person,3:error

			CHttpClient reqClient;
			tstring strResponse;
			tstring strToken = CController::Instance()->GetToken();
			tstring strUrlOrg(WEB_SERVER_BASE_URL);
			strUrlOrg += "business/get-pars-by-rid?rid=";

			strUrlOrg += pOneGroupChat->strRoomID;
			tstring strOutUTF8Org;
			CChineseCode::GB2312ToUTF_8(strOutUTF8Org, strUrlOrg.c_str(), strUrlOrg.length());
			reqClient.Get(strOutUTF8Org, strToken, strResponse);

			bool bFail = false;
			if (!strResponse.empty())
			{
				//如果是机构
				if (strResponse.find("id") != -1 
					&& strResponse.find("name") != -1 
					&& strResponse.find("avatar_base64") != -1)
				{
					Json::Reader reader;
					Json::Value root;
					try
					{
						if (reader.parse(strResponse, root))
						{
							strOrgID = root["id"].asString();
							CChineseCode::UTF_8ToGB2312(strOrgName, root["name"].asString().c_str(), root["name"].asString().length()) ;
							strAvatarBase64 = root["avatar_base64"].asString();
							nTypeValue = 1;
						}
					}
					catch (...)
					{
						CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
					}	
				}
				//如果是个人
				else if (strResponse.find("cell_phone") != -1 
					&& strResponse.find("id") != -1 
					&& strResponse.find("nick") != -1)
				{
					Json::Reader reader;
					Json::Value root;
					try
					{
						if (reader.parse(strResponse, root))
						{
							strDestPhone = root["cell_phone"].asString();
							CChineseCode::UTF_8ToGB2312(strDestNickName, root["nick"].asString().c_str(), root["nick"].asString().length());
							nTypeValue = 2;
						}
					}
					catch (...)
					{
						CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
					}	
				}
				else
				{
					nTypeValue = 3;//result=0
				}
			}

			bool bOrgGroup = strOrgID.empty()? false : true;

			//如果是机构。
			tstring strOrgAvatarPath;
			if (bOrgGroup)
			{
				DuiLib::CDuiString strAvatar  = CFileHelper::GetHeadCacheDir();
				strAvatar += strOrgID.c_str();
				if (!strAvatarBase64.empty())
				{
					strAvatarBase64 = Base64::decode64(strAvatarBase64);
					try{
						if(CMagicKHelper::Instance()->GeneratePng(strAvatarBase64, tstring(strAvatar.GetData())))
						{
							tstring strPhotoPath = tstring(strAvatar+_T(".png"));
							strOrgAvatarPath = strPhotoPath;
						}
					}
					catch(...)
					{	
					}
				}

			}
			//如果是个人
			else
			{
				tstring strAccount = strDestPhone + MMPC_ACCOUNT;
				sUserVCard oneUser;
				if (GetDBTaskModule()->LoadUserVCard(strAccount, oneUser))
				{
					strDestNickName = oneUser.strUserNickname.c_str();
					strDestAvatar = oneUser.strPhotoPath;
				}
				sUserVCard oneNewVCard;
				if(CController::Instance()->GetUserInfoMgr()->GetUserVCard(strAccount, oneNewVCard))
				{
					strDestNickName = oneNewVCard.strUserNickname;
					strDestAvatar = oneNewVCard.strPhotoPath;
				}
				if (strDestNickName.empty())
				{
					strDestNickName = strDestPhone;
				}
				if (strDestAvatar.empty())
				{
					strDestAvatar = DEF_AVATAR_NOR;
				}
			}


			//将新建的群组加入到“最近联系人列表”数据库		
			ContactListItemInfo info;
			//判断是个人还是机构
			info.m_strAccount = bOrgGroup ? SYS_GROUP_CHAT_ACCOUNT : tstring(strDestPhone+"@test-d").c_str();

			//如果是机构
			if (nTypeValue == 1 || bOrgGroup)//实际上两个条件等价
			{
				info.m_strLogo = strOrgAvatarPath.empty()?  DEF_AVATAR_NOR : strOrgAvatarPath.c_str();
				info.m_strNickName=(_T(""));//strOrgName.c_str();
				info.m_strOrgName  = strOrgName.c_str();
				info.m_strOrgID    = strOrgID.c_str();
			}
			else if(nTypeValue == 2 || !bOrgGroup)//实际上两个条件等价
			{
				info.m_strLogo = DEF_AVATAR_NOR;
				info.m_strNickName=(_T(""));
				info.m_strOrgName  = strDestNickName.c_str();//"";
				info.m_strOrgID   =(_T(""));
			}

			info.m_strRoomID   = pOneGroupChat->strRoomID.c_str();
			info.m_strLastMsg  = bTextMsg ? pOneGroupChat->strTextContent.c_str() : "";
			info.m_strLastTm   = (GetHMSTimeString(pOneGroupChat->tSendTime)).c_str();
			info.nItemID	   = GetNextItemID();

			ComposeGroupAvatar(info);

			//添加到第一个。
			m_lstFriends.insert(m_lstFriends.begin(), info);

			//添加到最近联系人顶部.
			m_pRecentList->AddItem(&info, 0);

			//保存到数据库
			RecentGroupItem oneGroup;
			oneGroup.strRoomID = pOneGroupChat->strRoomID;
			oneGroup.strGroupNick=(_T(""));
			oneGroup.strOrgID = bOrgGroup ? strOrgID : "";
			oneGroup.strOrgName = bOrgGroup? strOrgName : strDestNickName;
			oneGroup.strParticipant = nTypeValue == 2 ? strDestPhone : CController::Instance()->GetXmppClient()->jid().username();
			oneGroup.strAvatarPath = info.m_strLogo;
			GetDBTaskModule()->SaveRecentGroupItem(oneGroup);
		}		
	}

	//2.如果这个人就是当前的选中的，就直接插入：这个应该在更新最后一条消息之前就做过了
	int nItemIndex = GetRoomItemIndexInChatList(pOneGroupChat->strRoomID);

	//3.如果这个人没有选中，就显示气泡，和，闪烁。
	if (m_pRecentList->GetCurSel() != nItemIndex)
	{
		int nUnReadCount = m_pGroupChatPage->GetGroupChatLogic()->GetUnReadGroupMsgCount(pOneGroupChat->strRoomID);
		if (nUnReadCount>0)
		{
			CDuiString strCount;
			strCount.Format(_T("%d"), nUnReadCount);
			m_pRecentList->setMsgNum(strCount, nItemIndex);

			//4.闪烁
			GetTrayTaskModule()->FlashGroup(pOneGroupChat->strRoomID);
		}
	}
	//如果是选中的，还是需要清除掉旧的统计数据哦。
	else
	{
		m_pGroupChatPage->GetGroupChatLogic()->ClearUnReadGroupMsg(pOneGroupChat->strRoomID);
	}

	//设置最后一条消息和时间
	UpdateLastGroupMessage(pOneGroupChat->strRoomID);

	//RefreshContactListControl();

}

void mainDlg::UpdateLastGroupMessage(tstring& strRoomID)
{
	if (NULL == m_pRecentList)
	{
		return;
	}
	//2.更新界面上
	int nIdx = GetRoomItemIndexInChatList(strRoomID);
	if (nIdx > -1)
	{		
		//更新m_lstFriends里面的缓存信息：最后一条消息，和时间
		if (m_lstFriends.size()>0)
		{
			for (int i=0; i< m_lstFriends.size(); i++)
			{
				ContactListItemInfo& oneItem = m_lstFriends[i];
				if (_tcsicmp(oneItem.m_strRoomID.GetData(), strRoomID.c_str()) == 0)
				{
					m_pRecentList->SetLastMessage(tstring(oneItem.m_strLastMsg.GetData()), tstring(oneItem.m_strLastTm.GetData()), nIdx);
					break;
				}
			}	
		}
	}
}

void mainDlg::UpdateLastChatMessage(tstring& strFromBare)
{
	if (NULL == m_pRecentList)
	{
		return;
	}
	//2.更新界面上
	int nIdx = GetAccountIndexInChatList(strFromBare);
	if (nIdx > -1)
	{
		ListMsgs listMsgs;
		GetMsgCenterModule()->GetUnReadTextMsgs(strFromBare, listMsgs);
		if (listMsgs.size()>0)
		{
			ListMsgs::reverse_iterator rit = listMsgs.rbegin();
			if (rit != listMsgs.rend())
			{
				CMsgBase* pMsg = (CMsgBase*)(*rit);
				if (NULL != pMsg)
				{
					m_pRecentList->SetLastMessage(pMsg->strMsg, GetHMSTimeString(pMsg->uTime), nIdx);
				}

				//更新m_lstFriends里面的缓存信息：最后一条消息，和时间
				if (m_lstFriends.size()>0)
				{
					for (int i=0; i< m_lstFriends.size(); i++)
					{
						ContactListItemInfo& oneItem = m_lstFriends[i];
						if (oneItem.m_strRoomID.IsEmpty() && //单人聊天，不是和对方的群。
							_tcsicmp(strFromBare.c_str(), oneItem.m_strAccount.GetData()) == 0)
						{
							oneItem.m_strLastMsg =  pMsg->strMsg.c_str();
							oneItem.m_strLastTm  = (GetHMSTimeString(pMsg->uTime)).c_str();
							break;
						}
					}	
				}
			}
		}
	}
}

//收到消息了：判断下，当前激活的联系人是不是strFromBare.
//1）如果是的==>直接将消息放入到消息列表中，并将strFrombare放在联系人列表的最上面。
//2）如果不是==>就气泡+闪烁提醒！
void mainDlg::OnReceiveMessage(CMsgBase* pRecvMsg, EMsgType eMsgType)
{
	if (NULL == m_pRecentList)
	{
		return;
	}

	//从List中找到这个人的节点索引。
	int nItemIndex = -1;
	int nItemID = -1;
	int nSize = m_lstFriends.size();
	if (nSize>0)
	{
		for(int i=0; i<nSize; i++)
		{
			ContactListItemInfo& oneRecentItem = m_lstFriends[i];
			if (eMsgType == enmMsgType_Group)
			{
				CGroupMsg* pGroupMsg = dynamic_cast<CGroupMsg*>(pRecvMsg);
				if (_tcsicmp(oneRecentItem.m_strRoomID.GetData(), pGroupMsg->strRoomID.c_str()) == 0)
				{
					nItemID = oneRecentItem.nItemID;
					break;
				}
			}
			else 
			{
				//避免里面既有个人，由于这个的群。
				if ( oneRecentItem.m_strRoomID.IsEmpty() //不是和这个人的群聊天item。
					&&_tcsicmp(oneRecentItem.m_strAccount.GetData(), pRecvMsg->strFromBare.c_str()) == 0)
				{
					nItemID = oneRecentItem.nItemID;
					break;
				}
			}
			
		}
		
		if (nItemID>=0)
		{
			nItemIndex = m_pRecentList->GetIndex(nItemID);
		}
	}

	//
	tstring strAccount = pRecvMsg->strFromBare;
	bool bProcessed = false;
	if (enmMsgType_Chat == eMsgType)
	{
		ProcessChatMsg(dynamic_cast<CTextMsg*>(pRecvMsg), nItemIndex, bProcessed);
	}
	else if (enmMsgType_File == eMsgType)
	{	
		//if (dynamic_cast<CSysMsg*>(pRecvMsg)->eSubType == enmSysMsgSubType_FileRequest)
		//{
			ProcessSysMsg_FileRequest(dynamic_cast<CSysMsg*>(pRecvMsg)->strFromBare, dynamic_cast<CSysMsg*>(pRecvMsg)->strMsg);
		//}
		
		
	}
	else if (enmMsgType_Voice == eMsgType)
	{
		ProcessSysMsg_VoiceRequest(dynamic_cast<CSysMsg*>(pRecvMsg)->strFromBare, dynamic_cast<CSysMsg*>(pRecvMsg)->strMsg);
	}
	else if (enmMsgType_Sys == eMsgType)
	{
		ProcessSysMsg(dynamic_cast<CSysMsg*>(pRecvMsg), nItemIndex, bProcessed);
	}
	else if (enmMsgType_Group == eMsgType)
	{
		ProcessOfflineGroupMsg(dynamic_cast<CGroupMsg*>(pRecvMsg), nItemIndex, bProcessed);
	}

	//先置顶
	m_pRecentList->topItem(nItemIndex);
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(this->m_hWnd,&wp);

	// showCmd = 1 表示窗口正常显示
	if(wp.showCmd != 1)
	{
		bProcessed = false;
	}
	//如果没有处理该消息，就闪烁提醒。
	if (!bProcessed)
	{
		int nIdx = GetAccountIndexInChatList(strAccount);
		if (nIdx > -1)
		{
			//找到了,设置未读消息个数
			int nCount = GetMsgCenterModule()->GetUnreadMsgCount(strAccount);
			CDuiString strCount;
			strCount.Format(_T("%d"), nCount);
			m_pRecentList->setMsgNum(strCount, nIdx);

			UpdateLastChatMessage(strAccount);
		}


		//边栏消息提示
		bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
		setOptionRecentChatListUnread(bunRead);
		//4.tray flash..
		GetTrayTaskModule()->Flash(strAccount, (EMsgType)eMsgType);
		//SetIcon(IDI_ICON_MESSAGE);
	}
	
}

void mainDlg::ProcessChatMsg(CTextMsg* pRecvMsg, int nItemIndex, bool& bProcessed)
{
	//如果存在
	if (nItemIndex != -1)
	{
		//当前激活的必须是strFrombare才可以有下面的行为：插入消息，清空未读消息！
		//这个地方直接获取当前激活的Item，判断ItemID是不是，如果有没有激活的(可能初始化的时候就会有没激活的)，就拿第一个。
		bool bSelectedItem = false;
		int nSelected = m_pRecentList->GetCurSel();
		if (nSelected == nItemIndex)//是激活的Item
		{
			//当前是不是strFromBare
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(nItemIndex));
			if (pItem != NULL)
			{
				UpdateLastChatMessage(pRecvMsg->strFromBare);
				InsertUnReadMsg(pRecvMsg->strFromBare);
				bProcessed = true;
				return;
			}
		}
	}
	else
	{
		ContactListItemInfo iteminfo;
		//从好友列表里面找到这个人，获取这个联系人的基本信息
		for (int i=0; i<m_listAllFriends.size(); i++)
		{
			sFriendListItem& oneFriend = m_listAllFriends[i];
			if (_tcsicmp(pRecvMsg->strFromBare.c_str(), oneFriend.strAccount.c_str()) == 0)
			{
				iteminfo.nItemID = GetNextItemID();
				iteminfo.m_strAccount = oneFriend.strAccount.c_str();
				iteminfo.m_strNickName = oneFriend.strShowName.c_str();
				iteminfo.m_strLastMsg = pRecvMsg->strMsg.c_str();
				iteminfo.m_State = m_pUsrMgrAdapter->GetState(pRecvMsg->strFromBare);
				iteminfo.m_strLogo = (CFileHelper::GetHigtLightAvatar(pRecvMsg->strFromBare)).c_str();
				iteminfo.m_strOrgName = oneFriend.strOrgnization.c_str();
				iteminfo.m_strOrgID = oneFriend.strOrgID.c_str();
				break;
			}
		}

		m_lstFriends.push_back(iteminfo);

		//add to List ctrl.
		m_pRecentList->AddItem(&iteminfo, 0);
			
		//缓存到本地数据库。
		RECENTITEM oneItem;
		oneItem.strUid = pRecvMsg->strFromBare;
		oneItem.strTime=(_T(""));
		GetDBTaskModule()->SaveRecentFriendList(oneItem);
	}
}

void mainDlg::ProcessSysMsg(CSysMsg* pRecvMsg, int nItemIndex, bool& bProcessed)
{
	CSysMsgAcceptAddFriend* pAddFriendReq = NULL;
	//CSysMsgAcceptAddFriend* pAcceptAddReq = NULL;
	//if (pRecvMsg->eSubType == enmSysMsgSubType_AddFriend)
	//{
		//pAddFriendReq = static_cast<CSysMsgAddFriend*>(pRecvMsg);
	//}
	//else if (pRecvMsg->eSubType == enmSysMsgSubType_AcceptAddFriend || pRecvMsg->eSubType == enmSysMsgSubType_OrgRequest)
	//{
		pAddFriendReq = static_cast<CSysMsgAcceptAddFriend*>(pRecvMsg);
		pAddFriendReq->eSubType = pRecvMsg->eSubType;
		// todo lwt add
	//}

	if (NULL == pAddFriendReq)
	{
		return;
	}


	//如果存在
	if (nItemIndex != -1)
	{
		bool bSelectedItem = false;
		int nSelected = m_pRecentList->GetCurSel();
		//if (nSelected == nItemIndex)
		{
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(nItemIndex));
			if (pItem != NULL)
			{
				//to do...
				CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
				if (NULL == pSysMsgList)
				{
					return;
				}
				//pSysMsgList->AddItem(pRecvMsg->strMsg, pRecvMsg->uTime, pRecvMsg->strFromBare, pRecvMsg->eSubType, 0);
				if (pAddFriendReq->eSubType == enmSysMsgSubType_AddFriend || pAddFriendReq->eSubType == enmSysMsgSubType_AcceptAddFriend )
					pSysMsgList->AddItem(pAddFriendReq->strMsg, pAddFriendReq->uTime, pAddFriendReq->strSender, pAddFriendReq->eSubType, 0);
				else
					pSysMsgList->AddOrgItem(pAddFriendReq->strMsg, pAddFriendReq->uTime, pAddFriendReq->strSender, pAddFriendReq->eSubType, 0);
				return;
			}
		}
	}
	else
	{		
		tstring strRecvTime = GetHMSTimeString(pRecvMsg->uTime);
		m_pRecentList->RemoveItem(10000);
		ContactListItemInfo iteminfo;
		iteminfo.nItemID = GetNextItemID();
		iteminfo.m_strAccount = pRecvMsg->strFromBare.c_str();
		iteminfo.m_strNickName = "系统消息";
		iteminfo.m_strLastMsg = pRecvMsg->strMsg.c_str();//os.str().c_str();
		iteminfo.m_strLogo = SYSNOTIFY_AVATAR;
		iteminfo.m_strLastTm = strRecvTime.c_str();

		//从好友列表里面找到这个人，获取这个联系人的基本信息
		m_lstFriends.push_back(iteminfo);

		//add to List ctrl.
		m_pRecentList->AddItem(&iteminfo, 0);

		CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
		if (NULL == pSysMsgList)
		{
			return;
		}
		//pSysMsgList->AddItem(pRecvMsg->strMsg, pRecvMsg->uTime, pRecvMsg->strFromBare, pRecvMsg->eSubType, 0);
		if (pAddFriendReq->eSubType == enmSysMsgSubType_AddFriend || pAddFriendReq->eSubType == enmSysMsgSubType_AcceptAddFriend )
			pSysMsgList->AddItem(pAddFriendReq->strMsg, pAddFriendReq->uTime, pAddFriendReq->strSender, pAddFriendReq->eSubType, 0);
		else
			pSysMsgList->AddOrgItem(pAddFriendReq->strMsg, pAddFriendReq->uTime, pAddFriendReq->strSender, pAddFriendReq->eSubType, 0);
		
		//缓存到本地数据库。
		RECENTITEM oneItem;
		oneItem.strUid = pRecvMsg->strFromBare;
		oneItem.strTime = strRecvTime.c_str();
		GetDBTaskModule()->SaveRecentFriendList(oneItem);
	}
}

void mainDlg::ProcessOfflineGroupMsg(CGroupMsg* pGroupMsg, int nItemIndex, bool& bProcessed)
{
	//写入到数据库,便于接下来的界面显示的是，去数据库读取。
	_tGroupChatMessage oneGroupMsg;
	oneGroupMsg.strMsg = pGroupMsg->strMsg;
	oneGroupMsg.m_timeChat = pGroupMsg->uTime;
	oneGroupMsg.strRoomID = pGroupMsg->strRoomID;
	oneGroupMsg.strSenderPhone = CFileHelper::GetUserName(pGroupMsg->strFromBare);
	oneGroupMsg.strSenderAccount = pGroupMsg->strFromBare;

	GetDBTaskModule()->SaveGroupChatMsg(pGroupMsg->strRoomID, oneGroupMsg);

	//Join room.
	tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
	CController::Instance()->GetRoomManager()->EnterRoom(pGroupMsg->strRoomID, strUser);

	ContactListItemInfo info;
	info.m_strRoomID = pGroupMsg->strRoomID.c_str();
	
	bProcessed = true;

	//初始化群本地消息记录
	if (NULL != m_pGroupChatPage)
	{
		m_pGroupChatPage->InitGroupMsg(pGroupMsg->strRoomID);
		m_pGroupChatPage->GetGroupChatLogic()->AddOneUnReadCount(pGroupMsg->strRoomID);
		//第一次拉起来的群聊天，需要初始化pack列表。
		UpdatePackListInGroupChat(pGroupMsg->strRoomID);
	}
}

void mainDlg::OnDbClickGroupMsg(tstring strRoom)
{
	//选中聊天记录Option,如果当前是好友列表的话。
	COptionUI* pChatList = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat));
	if (NULL != pChatList)
	{
		pChatList->Selected(true);
	}

	//1.去除气泡
	int indx = GetRoomItemIndexInChatList(strRoom);
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.清空气泡
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, indx);	

			//2.放置到List顶部
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(indx));
			pItem->Select();
			m_pRecentList->SetItemIndex(pItem, 0);

		}
	}
}

void mainDlg::OnDbClickChatMsg(tstring strAccount)
{
	//选中聊天记录Option,如果当前是好友列表的话。
	COptionUI* pChatList = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat));
	if (NULL != pChatList)
	{
		pChatList->Selected(true);
	}

	//1.去除气泡
	int indx = GetAccountIndexInChatList(strAccount);
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.清空气泡
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, indx);	

			//2.放置到List顶部
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(indx));
			pItem->Select();
			m_pRecentList->SetItemIndex(pItem, 0);
			
		}
	}

	//2.加载消息
	InsertUnReadMsg(strAccount);
}

void mainDlg::OnBnClickAddFriendReq(bool bAgree, INT_PTR ptrData)
{
	//一次只会去除1个气泡个数
	int indx = GetAccountIndexInChatList(tstring(SYS_NOTIFY_ACCOUNT));
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.更新系统消息气泡个数
			//int nCount = GetMsgCenterModule()->GetUnreadMsgCount(SYS_NOTIFY_ACCOUNT);
			//if (nCount>0)
			//{
			//	CDuiString strCount;
			//	strCount.Format(_T("%d"), nCount-1);
			//	m_pRecentList->setMsgNum(strCount, indx);
			//}

			//2.放置到List顶部
			//CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(indx));
			//pItem->Select();
			//m_pRecentList->SetItemIndex(pItem, 0);

		}
	}


	CMsgBase* pMsg = NULL;
	if(GetMsgCenterModule()->GetLatestUnReadMsg(tstring(SYS_NOTIFY_ACCOUNT), pMsg) && pMsg != NULL)
	{
		if(pMsg->eType == enmMsgType_Sys)
		{
			CSysMsg* pSysMsg = dynamic_cast<CSysMsg*>(pMsg);
			if (pSysMsg != NULL)
			{
				if (pSysMsg->eSubType == enmSysMsgSubType_AddFriend)
				{
					CSysMsgAddFriend* pAddFriendReq = (CSysMsgAddFriend*)(pSysMsg);
					RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
					if (NULL != pRosterMgr)
					{
						gloox::StringList group;
						JID jid(pAddFriendReq->strSender);
						pRosterMgr->SendAckSubscriptionRequest(jid, bAgree, CChineseCode::EncodeUTF8(tstring(m_MyselfItem.m_strNickName.GetData())));
						CLogger::GetInstance()->PrintErrLog( "send agree add friend msg");	
						//if (bAgree)
						//{
						//	//pRosterMgr->ackSubscriptionRequest(jid, true);
						//	pRosterMgr->SendAckSubscriptionRequest(jid, true, CChineseCode::EncodeUTF8(tstring(m_MyselfItem.m_strNickName.GetData())));
						//}
						//else
						//{
						//	//pRosterMgr->ackSubscriptionRequest(jid, false);
						//	pRosterMgr->SendAckSubscriptionRequest(jid, false, CChineseCode::EncodeUTF8(tstring(m_MyselfItem.m_strNickName.GetData())));
						//}	
					}
				}
				else if (pSysMsg->eSubType == enmSysMsgSubType_RmvFriend)
				{
					CSysMsgRemvFriend* pRmvFriend = (CSysMsgRemvFriend*)(pSysMsg);
					ProcessSysMsg_RemoveFriend(pRmvFriend->strRemoved);
				}
				else if (pSysMsg->eSubType == enmSysMsgSubType_AddOrgFriend)
				{
					CSysMsgAcceptAddFriend* pAddFriendReq = (CSysMsgAcceptAddFriend*)(pSysMsg);
					tstring strReqId = pAddFriendReq->strSender;
					CWebClient::approveOrgBuddyRequest(strReqId,bAgree);
				}
			}

			GetMsgCenterModule()->RemoveLatestUnReadMsg(tstring(SYS_NOTIFY_ACCOUNT), pMsg->uTime);

			//停止闪烁
			GetTrayTaskModule()->StopFlash(tstring(SYS_NOTIFY_ACCOUNT), enmMsgType_Sys);

			//更新消息的状态
			GetDBTaskModule()->UpdateSysMsgStatus((time_t)(pMsg->uTime), true);

			////禁止button
			//CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
			//if (NULL == pSysMsgList)
			//{
			//	return;
			//}
			//pSysMsgList->DisableButton((sSysMsg*)ptrData);
		}
	}

	//禁止button
	CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
	if (NULL == pSysMsgList)
	{
		return;
	}
	pSysMsgList->DisableButton((sSysMsg*)ptrData);
}
void mainDlg::OnDbClickSysMessage(tstring strNewUser)
{

	//一次只会去除1个气泡个数
	int indx = GetAccountIndexInChatList(strNewUser);
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.更新系统消息气泡个数
			int nCount = GetMsgCenterModule()->GetUnreadMsgCount(SYS_NOTIFY_ACCOUNT);
			if (nCount>0)
			{
				CDuiString strCount;
				strCount.Format(_T("%d"), nCount-1);
				m_pRecentList->setMsgNum(strCount, indx);
			}

			//2.放置到List顶部
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(indx));
			pItem->Select();
			m_pRecentList->SetItemIndex(pItem, 0);

		}
	}


	CMsgBase* pMsg = NULL;
	if(GetMsgCenterModule()->GetLatestUnReadMsg(strNewUser, pMsg) && pMsg != NULL)
	{
		if(pMsg->eType == enmMsgType_Sys)
		{
			CSysMsg* pSysMsg = dynamic_cast<CSysMsg*>(pMsg);
			if (pSysMsg != NULL)
			{
				if (pSysMsg->eSubType == enmSysMsgSubType_AddFriend)
				{
					CSysMsgAddFriend* pAddFriendReq = (CSysMsgAddFriend*)(pSysMsg);
					ProcessSysMsg_AddFriendReq(pAddFriendReq->strSender, pAddFriendReq->strMsg);
				}
				else if (pSysMsg->eSubType == enmSysMsgSubType_RmvFriend)
				{
					CSysMsgRemvFriend* pRmvFriend = (CSysMsgRemvFriend*)(pSysMsg);
					ProcessSysMsg_RemoveFriend(pRmvFriend->strRemoved);
				}
				else if(pSysMsg->eSubType == enmSysMsgSubType_AddOrgFriend)
				{
					CSysMsgAcceptAddFriend* pAddFriendReq = (CSysMsgAcceptAddFriend*)(pSysMsg);
					CMsgBox* pMsgBox = new CMsgBox("加好友请求", pAddFriendReq->strMsg.c_str(),MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
					bool bAgree = false;
					if (MSGBOX_IDYES == pMsgBox->ShowModal(NULL))	
						bAgree = true;
					tstring strReqId = pAddFriendReq->strSender;
					CWebClient::approveOrgBuddyRequest(strReqId,bAgree);
				}
			
				GetMsgCenterModule()->RemoveLatestUnReadMsg(strNewUser, pMsg->uTime);

				//更新消息的状态
				GetDBTaskModule()->UpdateSysMsgStatus((time_t)(pMsg->uTime), true);

				//同步更新主界面系统消息列表里面的按钮状态
				CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
				if (NULL == pSysMsgList)
				{
					return;
				}
				sSysMsg oneSys;
				if (pSysMsg->eSubType == enmSysMsgSubType_AddFriend || pSysMsg->eSubType == enmSysMsgSubType_AddOrgFriend)
				{
					CSysMsgAddFriend* pAddFR = static_cast<CSysMsgAddFriend*>(pSysMsg);
					oneSys.eSubType = pSysMsg->eSubType;
					oneSys.strMsg = pSysMsg->strMsg;
					oneSys.strSender = pAddFR->strSender;
					oneSys.uTime = pSysMsg->uTime;
					pSysMsgList->DisableButton(&oneSys);
				}
			}	
		}
	}
}

void mainDlg::ProcessSysMsg_AddFriendReq(tstring& strSender, tstring& strReqMsg)
{
	StringList groups;
	CMsgBox* pMsgBox = new CMsgBox("加好友请求", strReqMsg.c_str(),MSGBOX_ICON_ASK, MSGBOX_IDYESNO);

	if (MSGBOX_IDYES == pMsgBox->ShowModal(NULL))	
	{
		RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
		if (NULL != pRosterMgr)
		{
			gloox::StringList group;
			JID jid(strSender);
			//pRosterMgr->ackSubscriptionRequest(jid, true);
			pRosterMgr->SendAckSubscriptionRequest(jid, true, CChineseCode::EncodeUTF8(tstring(m_MyselfItem.m_strNickName.GetData())));
		}
	}
	else
	{
		RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
		if (NULL != pRosterMgr)
		{
			gloox::StringList group;
			JID jid(strSender);
			//pRosterMgr->ackSubscriptionRequest(jid, false);
			pRosterMgr->SendAckSubscriptionRequest(jid, false, CChineseCode::EncodeUTF8(tstring(m_MyselfItem.m_strNickName.GetData())));
		}
	}
}

void mainDlg::ProcessSysMsg_OrgRequest(CSysMsgOrgRequest* pOrgMsg, int nItemIndex, bool& bProcessed)
{
	int i = 0;
}

void mainDlg::ProcessSysMsg_RemoveFriend(tstring& strRemoved)
{

}

void mainDlg::OnMySelfCardInfo(sUserVCard& oneInfo)
{
	m_myselfInfo = oneInfo;
	m_MyselfItem.m_strAccount = oneInfo.strAccount.c_str();
	m_MyselfItem.m_strNickName = oneInfo.strUserNickname.c_str();
	m_MyselfItem.m_strLogo = oneInfo.strPhotoPath.empty() ? DEF_AVATAR_NOR :oneInfo.strPhotoPath.c_str();
	tstring strMyOrg, strMyOrgId;
	m_pUsrMgrAdapter->GetMyOrganization(strMyOrg, strMyOrgId);
	m_MyselfItem.m_strOrgID = strMyOrgId.empty()? DEF_ORGID_MYFRIEND : strMyOrgId.c_str() ;
	m_MyselfItem.m_strOrgName = strMyOrg.c_str();
	//更新我的头像和昵称
	CButtonUI* pBtnMySelfAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonMySelfAvatar));
	if (NULL != pBtnMySelfAvatar)
	{
		pBtnMySelfAvatar->SetNormalImage(m_MyselfItem.m_strLogo);
	}
	CLabelUI* pLabelMyNickname = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLabelMySelfNickname));
	if (NULL != pLabelMyNickname)
	{
		pLabelMyNickname->SetText(m_MyselfItem.m_strNickName);
	}

	CLabelUI* pLabelMyOrg = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLabelMySelfCompany));
	if (NULL != pLabelMyOrg)
	{
		CDuiString strOrg = m_MyselfItem.m_strOrgName.IsEmpty()? "您还没有机构": m_MyselfItem.m_strOrgName;
		pLabelMyOrg->SetText(strOrg);
	}
		
	

	if (this->GetHWND())
	{
		SendMessage(WM_NEW_VCARD_ARRIVED, (WPARAM)(&oneInfo));
	}

	//SendMessage(WM_NEW_VCARD_ARRIVED);

}
void mainDlg::OnMySelfStateUpdate(tstring& strAccount, EUsrState eState, 
	tstring strRes, tstring strBusyInfo /*= ""*/)
{
	m_MyselfItem.m_strAccount = strAccount.c_str();
	m_MyselfItem.m_State = eState;
}

void mainDlg::UpdateRecentListPresence(tstring& strAccount, EUsrState eState, tstring& strBusyInfo)
{
	DuiLib::CDuiString strAvatarPath = CFileHelper::GetHeadCacheDir();
	tstring strOnlineAvatar = strAvatarPath.GetData();
	strOnlineAvatar += strAccount;
	strOnlineAvatar += ".png";

	//有些用户没有设置用户头像，就是空的
	bool bExist = PathFileExists(strOnlineAvatar.c_str());
	if (bExist)
	{
		int nSize = m_lstFriends.size();
		for (int i=0; i<nSize; i++)
		{
			ContactListItemInfo& oneItem = m_lstFriends[i];
			if (_tcsicmp(oneItem.m_strAccount.GetData(), strAccount.c_str()) ==0)
			{
				oneItem.m_State = eState;

				//更新图标
				if (eState == enmState_Online ||
					eState == enmState_Busy )
				{
					oneItem.m_strLogo = strOnlineAvatar.c_str();

					UpdateAvatarInRecentList(strAccount, strOnlineAvatar);
				}
				else if (eState == enmState_Offline)
				{
					tstring strOfflineAvatar = CFileHelper::GetGrayImageName(strOnlineAvatar);
					oneItem.m_strLogo = strOfflineAvatar.c_str();

					UpdateAvatarInRecentList(strAccount, strOfflineAvatar);
				}

				break;
			}
		}
	}
}

void mainDlg::UpdateFriendListPresence(tstring& strAccount, EUsrState eState, tstring& strBusyInfo)
{
	DuiLib::CDuiString strAvatarPath = CFileHelper::GetHeadCacheDir();
	tstring strOnlineAvatar = strAvatarPath.GetData();
	strOnlineAvatar += strAccount;
	strOnlineAvatar += ".png";

	//有些用户没有设置用户头像，就是空的
	bool bExist = PathFileExists(strOnlineAvatar.c_str());
	//if (bExist)
	{
		int nSize = m_listAllFriends.size();
		for (int i=0; i<nSize; i++)
		{
			sFriendListItem& oneItem = m_listAllFriends[i];
			if (_tcsicmp(oneItem.strAccount.c_str(), strAccount.c_str()) ==0)
			{
				oneItem.eState = eState;

				//更新图标
				if (eState == enmState_Online ||
					eState == enmState_Busy )
				{
					//用户没有头像就使用默认头像
					if (!bExist)
					{
						strOnlineAvatar = DEF_AVATAR_NOR;
					}

					oneItem.strAvatar = strOnlineAvatar.c_str();

					//UpdateAvatarInFriendList(strAccount, strOnlineAvatar);
				}
				else if (eState == enmState_Offline)
				{
					tstring strOfflineAvatar;
					if (!bExist)
					{
						strOfflineAvatar = DEF_AVATAR_GRAY;
					}
					else
					{
						strOfflineAvatar = CFileHelper::GetGrayImageName(strOnlineAvatar);
					}
					
					oneItem.strAvatar= strOfflineAvatar.c_str();

					//UpdateAvatarInFriendList(strAccount, strOfflineAvatar);
				}

				break;
			}
		}
	}
}

void mainDlg::OnUserStateUpdated(tstring& strAccount, EUsrState eState, 
	tstring strRes, tstring strBusyInfo/*=(_T(""))*/)
{
	//1.Update recent cache + ui===>取消了，只有联系人列表会更新状态信息
	//UpdateRecentListPresence(strAccount,eState, strBusyInfo);	

	char* pszAccount = new char[strAccount.length() + 1];
	memset(pszAccount, 0, strAccount.length() +1);
	memcpy(pszAccount, strAccount.c_str(), strAccount.length());
	PostMessage(WM_UPDATE_PRESENCE_STATUS, (WPARAM)pszAccount, (LPARAM)eState);
}

void mainDlg::UpdateUserInfoAtWndTop(tstring& strAccount, EUsrState eState, tstring& strFriendAvatar)
{
//	
	if (_tcsicmp(m_CurItem.m_strAccount.GetData(), strAccount.c_str())==0)
	{
		CLabelUI *pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName));

		if (NULL != pFriendNickname)
		{
			pFriendNickname->SetText(m_CurItem.m_strNickName.GetData());
		}

		CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
		if (NULL != pFriendOrgname) 
		{
			pFriendOrgname->SetText(m_CurItem.m_strOrgName);
		}
		CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
		if (NULL != pBtnFriendAvatar && !strFriendAvatar.empty())
		{
			pBtnFriendAvatar->SetNormalImage(strFriendAvatar.c_str());
		}

		//如果当前不是群聊天
		if (m_CurItem.m_strRoomID.IsEmpty())
		{
			CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
			if (NULL != pGroupMemCount)
			{
				pGroupMemCount->SetText(_T(""));
			}
			CButtonUI* pBtnShowMem = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_show_groupMember"));
			if (NULL != pBtnShowMem)
			{
				pBtnShowMem->SetVisible(false);
			}
			
		}
		//如果当前是群，显示群人数
		else
		{
			CButtonUI* pBtnShowMem = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_show_groupMember"));
			if (NULL != pBtnShowMem)
			{
				pBtnShowMem->SetVisible(true);
			}
			

			int nCount = CController::Instance()->GetRoomManager()->GetRoomMemberCount(tstring(m_CurItem.m_strRoomID.GetData()));
			char chBuf[10]={0};
			itoa(nCount, (char*)(&chBuf), 10);
			tstring strCount = "(";
			strCount += chBuf;
			strCount += "人)";
			CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
			if (NULL != pGroupMemCount)
			{
				pGroupMemCount->SetText(strCount.c_str());
			}
		}
	}


}
//切换到主线程去做这个事情
//联系人都拉取了
//展示好友列表：先去本地数据库去读信息，如果没读到就到server去fetch vcard。
void mainDlg::OnRosterAvalible(MapAllRosters& listUserInfos)
{
	if (this->GetHWND())
	{
		PostMessage(WM_ROSTER_ARRIVED);
	}
}

//VCard信息回来了，就更新头像信息，需要判断在线状态，根据状态不同绘制头像
//同步更新最近联系人+好友列表：先跟新cache，再更新UI
//有可能用户压根就没有头像===>使用默认头像
void mainDlg::OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo)
{
	if (this->GetHWND())
	{
		SendMessage(WM_NEW_VCARD_ARRIVED, (WPARAM)(&oneInfo));
	}
}

//目前只是查找的地方用到了,更新查找页面
void mainDlg::OnStrangerVCard(tstring& strAccount, sUserVCard& oneCard)
{


}

void mainDlg::UpdateShowNameInRecentChatList(tstring& strAccount, tstring& strShowName)
{
	//1.更新最近联系人，如果有的话。
	if (NULL == m_pRecentList)
	{
		return;
	}
	int nIdx = GetAccountIndexInChatList(strAccount);
	if (nIdx > -1)
	{
		m_pRecentList->UpdateShowName(strShowName, nIdx);	
	}
}

void mainDlg::UpdateAvatarInRecentList(tstring& strAccount, tstring& strAvatarPath)
{
	//1.更新最近联系人，如果有的话。
	if (NULL == m_pRecentList)
	{
		return;
	}
	int nIdx = GetAccountIndexInChatList(strAccount);
	if (nIdx > -1)
	{
		m_pRecentList->UpdateHeadImage(strAvatarPath, nIdx);	
	}
}

void mainDlg::UpdateAvatarInRecentList(const tstring& room_id, const tstring& avatar_path, int item_id )
{
	//更新显示
	if (NULL != m_pRecentList)
	{
		int nCount = m_pRecentList->GetCount();
		if (nCount>0)
		{
			for (int i=0; i<nCount; i++)
			{
				CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(i));
				if (pItem != NULL)
				{
					if (pItem->GetTag() == item_id)
					{
						m_pRecentList->UpdateHeadImage(avatar_path, i);	
					}
				}

			}
		}
	}
}	

void mainDlg::UpdataShowNameInFriendList(tstring& strAccount, tstring& strShowName)
{
	int nIndex = GetAccountIndexInFriendList(strAccount);
	if (nIndex > -1)
	{
		m_pFriendLst->UpdateShowName(strShowName, nIndex);
	}
}

void mainDlg::UpdateDutyInFriendList(const tstring& account, const vector<tstring>& duty)
{
	tstring _account = account;
	int nIndex = GetAccountIndexInFriendList(_account);
	if (nIndex > -1)
	{
		m_pFriendLst->UpdateDuty(duty, nIndex);
	}
}

void mainDlg::UpdateAvatarInFriendList(tstring& strAccount, tstring& strAvatarPath)
{
	//2.更新好友列表里面的头像。
	int nIndex = GetAccountIndexInFriendList(strAccount);
	if (nIndex > -1)
	{
		m_pFriendLst->UpdataAvatar(strAvatarPath, nIndex);
	}
}

void mainDlg::OnStoreVCardResult(tstring& strAccount, bool bSucc)
{

}

void mainDlg::MMsendMessage()
{
	ChatInfo chatinfo;
	//get input text.
	CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
	//空内容不发送
	if(pEditInput != NULL && pEditInput->GetTextLength() == 0)
		return;
	//ITextServices * pTextServices = pEditInput->GetTextServices();

	//tstring strText;
	//RichEdit_GetText(pTextServices, strText);

	//pTextServices->Release();
	/*if(pEditInput != NULL && pEditInput->GetTextLength() == 0)
		return;*/
// 	ITextServices * pTextServices = pEditInput->GetTextServices();
// 
// 	tstring strText;
// 	RichEdit_GetText(pTextServices, strText);

//	pTextServices->Release();

	bool bIsEmptyMsg = CRegexHelper::IsEmptyString(pEditInput->GetText().GetData());
	if (bIsEmptyMsg || pEditInput->GetTextLength() > MAX_SEND_MSG_LEN)
	{
		if (bIsEmptyMsg) pEditInput->SetText(_T(""));
		RECT rect;
		GetClientRect(this->GetHWND(), &rect);
		POINT pt;
		pt.x = rect.right - 450;
		pt.y = rect.bottom - 80;

		CDuiMenu *pMenu = new CDuiMenu(_T("emptyMsgReminder.xml"), bIsEmptyMsg? eUIMenuMsgEmpty: eUIMenuMsgTooLog);
		pMenu->Init(mainDlg::getInstance()->GetHWND(), pt);
		pMenu->ShowToolTipBox();		
		return;
	}
	//todo 需要分行获取 
	int iLineCount = pEditInput->GetLineCount();
	CDuiString strLine;
	for (int i=0; i<iLineCount; i++)
	{
		CDuiString strTmp = pEditInput->GetLine(i, 99999);
		if (!strTmp.IsEmpty())
		{
			if (strTmp[strTmp.GetLength()-1] != 13)
			{
				strLine += strTmp;
				if (i==iLineCount-1)
					chatinfo.m_lstMsg.push_back(strLine);
				continue;
			}
			else
			{
				strLine += strTmp;
			}
		}

		chatinfo.m_lstMsg.push_back(strLine);
		strLine.Empty();
	}
	CDuiString strValue = pEditInput->GetText();
	//clear input.
	pEditInput->SetText((_T("")));
	CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));

	chatinfo.m_strRawMsg = strValue;
	chatinfo.m_strAccount = m_MyselfItem.m_strAccount;
	chatinfo.uTime = time(0);
	chatinfo.m_strLogo = m_MyselfItem.m_strLogo.GetData();
	pChatlst->AddMyMsgItem(&chatinfo);
	SetTimer(this->GetHWND(), WM_TIMER_CHATLISTENDDOWN, 50, NULL);
	//send mesage
	if (NULL != m_pMsgSessionMgr)
	{
		//check if empty.
		if (m_CurItem.m_strAccount.GetLength() == 0 || strValue.GetLength() == 0)
		{
			return;
		}

		m_pMsgSessionMgr->SendMsg(tstring(m_CurItem.m_strAccount.GetData()), tstring(strValue.GetData()));
	}

	//缓存本地聊天记录
	pChatlst->CacheChatMessage(m_CurItem.m_strAccount,&chatinfo);

	//保存消息到本地DB
	_tChatMessage oneText;
	oneText.strDestUser = chatinfo.m_strAccount.GetData();
	oneText.strChatText = strValue;
	oneText.m_timeChat = chatinfo.uTime;
	GetDBTaskModule()->SaveChatMessage(tstring(m_CurItem.m_strAccount.GetData()), oneText);

	//更新最后一条消息内容和发送时间
	int nIdx = GetAccountIndexInChatList(tstring(m_CurItem.m_strAccount.GetData()));
	if (nIdx > -1)
	{
		if (NULL == m_pRecentList)
		{
			return;
		}

		m_pRecentList->SetLastMessage(oneText.strChatText, GetHMSTimeString(oneText.m_timeChat), nIdx);

		//更新m_lstFriends里面的缓存信息：最后一条消息，和时间
		if (m_lstFriends.size()>0)
		{
			for (int i=0; i< m_lstFriends.size(); i++)
			{
				ContactListItemInfo& oneItem = m_lstFriends[i];
				if (_tcsicmp(m_CurItem.m_strAccount.GetData(), oneItem.m_strAccount.GetData()) == 0)
				{
					oneItem.m_strLastMsg =  oneText.strChatText.c_str();
					oneItem.m_strLastTm  = (GetHMSTimeString(oneText.m_timeChat)).c_str();
					break;
				}
			}	
		}

	}
}

int mainDlg::GetAccountIndexInFriendList(tstring& strAccount)
{
	//从cache中找到ItemID
	int nItemID = -1;
	for (int i=0; i<m_listAllFriends.size(); i++)
	{
		sFriendListItem& oneFriend = m_listAllFriends[i];
		if (_tcsicmp(oneFriend.strAccount.c_str(), strAccount.c_str()) == 0)
		{
			nItemID = oneFriend.nItemID;
			break;
		}
	}

	if (nItemID>=0)
	{
		//3.拿到strFromBare的索引，设置消息的气泡数
		if (NULL != m_pFriendLst)
		{
			int nCount = m_pFriendLst->GetCount();
			if (nCount>0)
			{
				for (int i=0; i<nCount; i++)
				{
					CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pFriendLst->GetItemAt(i));
					if (pItem != NULL)
					{
						Node* node  = (Node*)pItem->GetTag();
						if (node->data().value == nItemID)
						{
							return i;
						}
					}

				}
			}
		}
	}
	

	return -1;
}

int mainDlg::GetRoomItemIndexInChatList(tstring& strRoomId)
{
	//从Cache中找到ItemID,如果对方是一个群组的，就需要判断room信息是否为空
	int nItemID = -1;
	for (int i=0; i< m_lstFriends.size(); i++)
	{
		ContactListItemInfo& oneItem = m_lstFriends[i];	
		if ( _tcsicmp(strRoomId.c_str(), oneItem.m_strRoomID.GetData()) == 0)
		{
			nItemID = oneItem.nItemID;
			break;
		}
	}

	if (nItemID>=0)
	{
		//3.拿到strFromBare的索引，设置消息的气泡数
		if (NULL != m_pRecentList)
		{
			int nCount = m_pRecentList->GetCount();
			if (nCount>0)
			{
				for (int i=0; i<nCount; i++)
				{
					CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(i));
					if (pItem != NULL)
					{
						if (pItem->GetTag() == nItemID)
						{
							return i;
						}
					}

				}
			}
		}
	}


	return -1;
}
//根据用户名在List Control找到对应的item 索引值。
int mainDlg::GetAccountIndexInChatList(tstring& strFromBare, bool bInGroup /*= false*/)
{
	//从Cache中找到ItemID,如果对方是一个群组的，就需要判断room信息是否为空
	int nItemID = -1;
	for (int i=0; i< m_lstFriends.size(); i++)
	{
		ContactListItemInfo& oneItem = m_lstFriends[i];
		if (!bInGroup)
		{
			if ( oneItem.m_strRoomID.IsEmpty() &&//群必须为空
				_tcsicmp(strFromBare.c_str(), oneItem.m_strAccount.GetData()) == 0)
			{
				nItemID = oneItem.nItemID;
				break;
			}
		}
		else
		{
			//对方有群信息
			if ( !oneItem.m_strRoomID.IsEmpty() && //群不能为空
				_tcsicmp(strFromBare.c_str(), oneItem.m_strAccount.GetData()) == 0)
			{
				nItemID = oneItem.nItemID;				
				break;
			}
		}
	}
	if (nItemID>=0)
	{
		//3.拿到strFromBare的索引，设置消息的气泡数
		if (NULL != m_pRecentList)
		{
			int nCount = m_pRecentList->GetCount();
			if (nCount>0)
			{
				for (int i=0; i<nCount; i++)
				{
					CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(i));
					if (pItem != NULL)
					{
						if (pItem->GetTag() == nItemID)
						{
							return i;
						}
					}

				}
			}
		}
	}
	

	return -1;
}


void mainDlg::InitBussinessPartnerTab()
{
	CDialogBuilder m_DlgBuilder;
	CVerticalLayoutUI *pTabBussinessPartner = NULL;
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pTabBussinessPartner = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create(_T("tab_friend_list.xml"),
			(UINT)0, &cb, &m_PaintManager));
	}
	else
	{
		pTabBussinessPartner = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	}

	if (NULL == pTabBussinessPartner)
	{
		return;
	}

	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->Add(pTabBussinessPartner);
	m_pFriendLst = static_cast<CUIFriendList*>(m_PaintManager.FindSubControlByName(pTabBussinessPartner, kListContactsFriendList));
}

//添加好友成功了:更新好友列表,用户点开这个人，就切换到这个联系人，并且在聊天窗口顶部显示“您已经添加了xxx，现在可以聊天了。”
void mainDlg::OnAddFriendSucc(tstring& strFriend)
{
	//初始化好友列表
	if (NULL != m_pFriendLst)
	{
		//cache.
		sRosterItemLocal oneRoster;
		if (m_pUsrMgrAdapter->GetOneUserRoster(strFriend, oneRoster))
		{
			sFriendListItem oneFriend;
			oneFriend.nItemID	 = GetNextItemID();
			oneFriend.strAccount = strFriend.c_str();
			oneFriend.strAvatar = DEF_AVATAR_NOR;
			oneFriend.strSignatrue=(_T(""));
			oneFriend.strShowName  = m_pUsrMgrAdapter->GetMemo(strFriend);//"";
			oneFriend.strOrgnization = oneRoster.strOrgnization;
			oneFriend.strOrgID = oneRoster.strOrgID;

			//m_listAllFriends.push_back(oneFriend); 
			m_listAllFriends.insert( find_if(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper(oneFriend.strOrgnization, CSortHelper::eFoundField::eFoundGroup)), oneFriend);
			//如果所属机构不存在，就创建一个机构节点。
			map<tstring, Node*>::iterator itFind = m_mapOrg2Node.find(oneRoster.strOrgID);
			int nSize = m_mapOrg2Node.size();
			DuiLib::Node* pOrgNode = NULL;
			if (itFind == m_mapOrg2Node.end())
			{
				FriendListItemInfo item;
				item.folder = true;
				item.empty = false;
				item.strOrgName = oneRoster.strOrgnization;
				item.strOrgID   = oneRoster.strOrgID;
				item.itemId = GetNextItemID();
				pOrgNode = m_pFriendLst->AddNode(item, NULL);	
				m_mapOrg2Node.insert(make_pair(oneRoster.strOrgID, pOrgNode));
			}
			else
			{
				//也许是排序什么地方那个导致的删除map没有删除干净？！此处的pOrgNode居然为NULL！！
				pOrgNode = itFind->second;
				if (pOrgNode == NULL)
				{
					FriendListItemInfo item;
					item.folder = true;
					item.empty = false;
					item.strOrgName = oneRoster.strOrgnization;
					item.strOrgID   = oneRoster.strOrgID;
					item.itemId = GetNextItemID();
					pOrgNode = m_pFriendLst->AddNode(item, NULL);	
					m_mapOrg2Node[oneRoster.strOrgID] = pOrgNode;//重新赋值一个新的Node节点！！！
				}
			}

			//添加到界面
			FriendListItemInfo friendNode;
			friendNode.folder = false;
			friendNode.empty = false;
			friendNode.strOrgName = oneFriend.strOrgnization;//_T("我的好友");
			friendNode.itemId = oneFriend.nItemID;
			friendNode.nick_name = oneFriend.strShowName;
			friendNode.logo = oneFriend.strAvatar;
			friendNode.account = oneFriend.strAccount;
			//add to list control
			m_pFriendLst->AddNode(friendNode,  pOrgNode);
		}
	}
}
//更新好友列表和聊天记录列表
void mainDlg::OnFriendRemoved(tstring& strKick,bool bActive)
{
	char* pBuf = new char[256];
	memset(pBuf, 0, 256);
	strcpy(pBuf, strKick.c_str());
	PostMessage(WM_FRIEND_REMOVED, (WPARAM)pBuf, 0);
	//UpdateCacheAndUiAfterFriendDeleted(strKick);
}

//最近联系人只存在本地，因此不涉及到与服务器的通讯
void mainDlg::OnDeleteContact()
{
	CMsgBox* pMsgBox = new CMsgBox("删除聊天","是否删除该聊天记录？",MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
	if(pMsgBox->ShowModal() == MSGBOX_IDNO)
	{
		return;
	}


	if (NULL == m_pRecentList)
	{
		return;
	}
	//获取激活Item的Item ID。
	tstring strAccount;
	tstring strNickName;
	tstring strRoomId;
	int nItemID = -1;//m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag();
	nItemID = m_pRecentList->GetItemAt(m_pRecentList->GetCurSel())->GetTag();


	if (nItemID>-1)
	{
		for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
			it != m_lstFriends.end(); ++it)
		{
			if (nItemID == it->nItemID)
			{
				strAccount = it->m_strAccount;
				strNickName = it->m_strNickName;
				strRoomId = it->m_strRoomID;
				m_lstFriends.erase(it);
				break;
			}
		}		
	}


	//删除本地DB的聊天记录和最近联系人列表
	if (strRoomId.empty())//通过roomid来判断是个人还是群聊天
	{
		GetDBTaskModule()->RemoveRecentItem(strAccount);
		GetDBTaskModule()->RemoveChatMessage(strAccount);
	}
	else
	{
		GetDBTaskModule()->RemoveFromRecentGroup(strRoomId);
		GetDBTaskModule()->RemoveGroupChatMessage(strRoomId);
	}
	

	//如果聊天框内正好是这个人，就需要清除聊天记录
	CLabelUI* pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName/*kbuttonContactNameControlName*/));
	if (NULL != pFriendNickname)
	{
		CDuiString strNickStatus = pFriendNickname->GetText();
		int nIdex = strNickStatus.Find(strNickName.c_str(), 0);
		if(nIdex>=0)
		{
			//清空？还是选择最近联系人里面的最上面的一个联系人信息呢？===to do...
			pFriendNickname->SetText(_T(""));
			//清空用户头像
			CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
			if (NULL != pBtnFriendAvatar)
			{
				pBtnFriendAvatar->SetNormalImage(_T(""));
			}
			//清空用户机构信息
			CLabelUI* pLabelFriendCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
			if (NULL != pLabelFriendCompany)
			{
				pLabelFriendCompany->SetText(_T(""));
			}

			CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
			if (NULL != pFriendOrgname) pFriendOrgname->SetText(_T(""));

			//清空聊天记录和输入框内容
			CChatListUI* pOutput = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
			CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
			if (NULL != pOutput)
			{
				pOutput->RemoveAll();
			}
			if (NULL != pEditInput)
			{
				pEditInput->SetText((_T("")));
			}

			CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
			if (NULL != pGroupMemCount)
			{
				pGroupMemCount->SetText(_T(""));
			}

			CButtonUI* pBtnShowMem = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_show_groupMember"));
			if (NULL != pBtnShowMem)
			{
				pBtnShowMem->SetVisible(false);
			}
		}
	}

	if (nItemID>-1)
	{
		CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
		pChatlst->ClearMessage(strAccount.c_str());

		m_pRecentList->RemoveItem(nItemID);	
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// void mainDlg::UpdateCacheAndUiAfterFriendDeleted(tstring& strDeletedAccount)
// {
// 	int nItemIDInFriendList = -1;
// 	tstring strOrgId;
// 	for (vector<sFriendListItem>::iterator it = m_listAllFriends.begin();
// 		it != m_listAllFriends.end(); ++it)
// 	{
// 		if (_tcsicmp(it->strAccount.c_str(), strDeletedAccount.c_str()) == 0)
// 		{
// 			nItemIDInFriendList = it->nItemID;
// 			strOrgId = it->strOrgID;
// 			//从好友列表Cache删除掉。
// 			m_listAllFriends.erase(it);
// 			break;
// 		}
// 	}
// 
// 	//从好友列表ui+LocalDB删除    todo 由于不一定能调用到CListUI::SelectItem, 故暂时使用m_listAllFirends中的位置来删除
// 	if(NULL != m_pFriendLst)
// 	{
// 		int nCount = m_pFriendLst->GetCount();
// 		if (nCount>0)
// 		{
// 			for (int i=0; i<nCount; i++)
// 			{
// 				CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pFriendLst->GetItemAt(i));
// 				if (pItem != NULL)
// 				{
// 					Node* node  = (Node*)pItem->GetTag();
// 					Node* pParentNode = node->parent();
// 					if (node->data().value == nItemIDInFriendList)
// 					{
// 						m_pFriendLst->RemoveNode(node);
// 						//如果这个父亲节点是空的了，那么这个组织机构应该也要删除掉，--llzhou
// 						if (!pParentNode->has_children())
// 						{
// 							//m_pFriendLst->RemoveNode(pParentNode);//如果是“我的好友”是不可以删除的--lee
// 							if (DEF_ORGID_MYFRIEND != strOrgId)
// 							{
// 								m_pFriendLst->RemoveNode(pParentNode);
// 								int nSize = m_mapOrg2Node.size();
// 								map<tstring, DuiLib::Node*>::iterator it2 = m_mapOrg2Node.find(strOrgId);
// 								if (it2 != m_mapOrg2Node.end())
// 								{
// 									m_mapOrg2Node.erase(it2);
// 								}
// 
// 								nSize = m_mapOrg2Node.size();
// 								int ncout =100;
// 							}
// 						}
// 
// 						break;
// 					}
// 				}
// 			}
// 		}
// 	}
// 
// 	//删除本地DB的聊天记录和最近联系人列表
// 	GetDBTaskModule()->RemoveRecentItem(strDeletedAccount);
// 	//GetDBTaskModule()->RemoveChatList(strAccount);
// 
// 	//清空最近联系人cache+ui+LocalDB
// 	if (m_lstFriends.size()>0)
// 	{
// 		int nItemID=-1;
// 		tstring strNickName;
// 		for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
// 			it != m_lstFriends.end(); ++it)
// 		{
// 			ContactListItemInfo& oneFriend = *it;
// 			//找到这个人了，从Cache中删除
// 			if (_tcsicmp(oneFriend.m_strAccount.GetData(), strDeletedAccount.c_str())==0)
// 			{
// 				nItemID = it->nItemID;
// 				strNickName = it->m_strNickName.GetData();
// 				m_lstFriends.erase(it);
// 				break;
// 			}
// 		}
// 
// 		//从界面删除这个人
// 		if (nItemID>=0)
// 		{
// 			if (NULL != m_pRecentList)
// 			{
// 				m_pRecentList->RemoveItem(nItemID);
// 			}
// 		}
// 
// 		//如果聊天框内正好是这个人，就需要清除聊天记录
// 		CLabelUI* pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName/*kbuttonContactNameControlName*/));
// 		if (NULL != pFriendNickname)
// 		{
// 			CDuiString strNickStatus = pFriendNickname->GetText();
// 			int nIdex = strNickStatus.Find(strNickName.c_str(), 0);
// 			if(nIdex>=0)
// 			{
// 				//清空？还是选择最近联系人里面的最上面的一个联系人信息呢？===to do...
// 				pFriendNickname->SetText(_T(""));
// 				//清空用户头像
// 				CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
// 				if (NULL != pBtnFriendAvatar)
// 				{
// 					pBtnFriendAvatar->SetNormalImage(_T(""));
// 				}
// 				//清空用户机构信息
// 				CLabelUI* pLabelFriendCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
// 				if (NULL != pLabelFriendCompany)
// 				{
// 					pLabelFriendCompany->SetText(_T(""));
// 				}
// 
// 				CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
// 				if (NULL != pFriendOrgname) pFriendOrgname->SetText(_T(""));
// 
// 				//清空聊天记录和输入框内容
// 				CChatListUI* pOutput = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
// 				CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
// 				if (NULL != pOutput)
// 				{
// 					pOutput->RemoveAll();
// 				}
// 				if (NULL != pEditInput)
// 				{
// 					pEditInput->SetText((_T("")));
// 				}
// 			}
// 		}
// 	}
// 
// 	////如果新加好友列表中有这个人，就删除掉吧：我刚加的，又删除了。
// 	//map<tstring, time_t>::iterator itFind = m_mapAccount2Time.find(strDeletedAccount);
// 	//if (itFind != m_mapAccount2Time.end())
// 	//{
// 	//	m_mapAccount2Time.erase(itFind);
// 	//}
// }
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mainDlg::OnDeleteFriend()
{
	CMsgBox* pMsgBox = new CMsgBox("删除好友","是否删除该好友？",MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
 	if(pMsgBox->ShowModal() == MSGBOX_IDNO)
 	{
 		return;
 	}


	if (NULL == m_pFriendLst)
	{
		return;
	}
	//获取激活Item的Item ID。
	tstring strAccount;
	int nItemID = -1;//m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag();
	Node* pNodeData = (Node*)(m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag());
	if(pNodeData != NULL)
	{
		nItemID = pNodeData->data().value;
	}

	if (nItemID>-1)
	{
		for (vector<sFriendListItem>::iterator it = m_listAllFriends.begin();
			it != m_listAllFriends.end(); ++it)
		{
			if (nItemID == it->nItemID)
			{
				strAccount = it->strAccount;

				tstring strMyAccount = CController::Instance()->GetXmppClient()->jid().bare();
				if (_tcsicmp(strAccount.c_str(), strMyAccount.c_str()) == 0)
				{
					CMsgBox* pMsgBox = new CMsgBox("删除好友","不能删除自己 ！",MSGBOX_ICON_WARNING, MSGBOX_IDOK);
					//不给删除自己
					pMsgBox->ShowModal();
					return;
				}
				tstring strOrgnization = it->strOrgnization;
				tstring strMyOrg, strMyOrgId;
				m_pUsrMgrAdapter->GetMyOrganization(strMyOrg, strMyOrgId);
				if (_tcsicmp(strOrgnization.c_str(), strMyOrg.c_str()) == 0)
				{
					CMsgBox* pMsgBox = new CMsgBox("删除好友","不能删除同一机构的好友 ！",MSGBOX_ICON_WARNING, MSGBOX_IDOK);
					//不给删除同一机构成员
					pMsgBox->ShowModal();
					return;
				}
				//1.从server删除
				if (NULL != m_pUsrMgrAdapter)
				{
					m_pUsrMgrAdapter->RemoveUser(it->strAccount);
				}

				break;
			}
		}

		UpdateCacheAndUiAfterFriendDeleted(strAccount);
	}
	
}

void mainDlg::InitInsetTabLayoutInBussinessPartnerPage()
{
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kBussinesPartnerTabLayout));
	if (NULL == pLayout)
	{
		return;
	}

	if (NULL == m_pPartnerInfopage)
	{
		m_pPartnerInfopage = new CPartnerInfoPage(&m_PaintManager);
	}

	//CDialogBuilder DlgBuilder2;
	//CDialogBuilderCallbackEx cb2(&m_PaintManager);
	////2.加载添加联系人page页面到
	//CContainerUI *pTabSearchPage = NULL;
	//if (!DlgBuilder2.GetMarkup()->IsValid())
	//{
	//	pTabSearchPage = static_cast<CContainerUI*>(DlgBuilder2.Create(_T("tab_searchtoadd_friend.xml"),
	//		(UINT)0, &cb2, &m_PaintManager));
	//}
	//else
	//{
	//	pTabSearchPage = static_cast<CContainerUI*>(DlgBuilder2.Create((UINT)0, &m_PaintManager));
	//}

	//if (NULL != pTabSearchPage)
	//{
	//	pLayout->AddAt(pTabSearchPage, 1);
	//}
	
	//激活当前页面为商家信息页面
	if (pLayout->GetCount()>0)
	{
		pLayout->SelectItem(0);
	}
	
}

mainDlg* mainDlg::getInstance()
{
	return m_pInstance;
}

bool mainDlg::addFileMsg(fileInfo &info)
{
	CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
	ChatInfo chatinfo;
	chatinfo.m_strAccount = m_CurItem.m_strAccount;/*m_CurItem.m_strNickName*/;
	chatinfo.uTime = time(0)/*"12:30"*/;
	chatinfo.m_strLogo = m_MyselfItem.m_strLogo.GetData();
	chatinfo.m_lstFiles.push_back(info);
	tstring strPack = info.strPackID;
	pChatlst->AddMyFileMsgItem(&chatinfo);       //todo lwt
	ostringstream os;
	CFileHelper helper;
	os<<"#COMMAND#"<<"type="<<helper.GetDescription(helper.getFileType(info.strFileType)).c_str()<<",";
	os<<"{"
	<<"\"file_name\":\""<<info.strFileFullName<<"\","
	<<"\"file_type\":\""<<info.strFileType<<"\","
	<<"\"file_size\":\""<<info.strFileSz<<"\","
	<<"\"ali_file_url\":\""<<info.strFileUrl<<"\","
	<<"\"file_path\":\""<<info.strFilePath<<"\""
	<<"}";
	MMSendFileMsg(os.str());


	//缓存本地聊天记录
	chatinfo.m_strRawMsg = os.str().c_str();
	pChatlst->CacheChatMessage(m_CurItem.m_strAccount,&chatinfo);

	//保存消息到本地DB
	_tChatMessage oneText;
	oneText.strDestUser = chatinfo.m_strAccount.GetData();
	oneText.strChatText = os.str().c_str();
	oneText.m_timeChat = chatinfo.uTime;
	GetDBTaskModule()->SaveChatMessage(tstring(m_CurItem.m_strAccount.GetData()), oneText);


	//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(false);
	return 1;
}

bool mainDlg::addDownloadedFileMsg(fileInfo &info)
{
	CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
	ChatInfo chatinfo;
	chatinfo.m_strAccount = m_myselfInfo.strAccount.c_str();/*m_CurItem.m_strNickName*/;
	chatinfo.uTime = time(0)/*"12:30"*/;
	chatinfo.m_strLogo = (CFileHelper::GetHigtLightAvatar(tstring(chatinfo.m_strAccount))).c_str();
	chatinfo.m_lstFiles.push_back(info);
	tstring strPack = info.strPackID;
	pChatlst->AddOppositeFileMsgItem(&chatinfo);       //todo lwt


	//CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
	//ChatInfo chatinfo;
	//chatinfo.m_strAccount = m_CurItem.m_strAccount;/*m_CurItem.m_strNickName*/;
	//chatinfo.uTime = time(0)/*"12:30"*/;
	//chatinfo.m_strLogo = m_MyselfItem.m_strLogo.GetData();
	//chatinfo.m_lstFiles.push_back(info);
	//tstring strPack = info.strPackID;
	//pChatlst->AddMyFileMsgItem(&chatinfo);       //todo lwt
	ostringstream os;
	CFileHelper helper;
	os<<"#COMMAND#"<<"type="<<helper.GetDescription(ECmdType::enmCmd_FileReq).c_str()<<",";
	os<<"{"
		<<"\"file_name\":\""<<info.strFileFullName<<"\","
		<<"\"file_type\":\""<<info.strFileType<<"\","
		<<"\"file_size\":\""<<info.strFileSz<<"\","
		<<"\"ali_file_url\":\""<<info.strFileUrl<<"\","
		<<"\"file_path\":\""<<info.strFilePath<<"\""
		<<"}";
	//MMSendFileMsg(os.str());


	//缓存本地聊天记录
	chatinfo.m_strRawMsg = os.str().c_str();
	pChatlst->CacheChatMessage(m_CurItem.m_strAccount,&chatinfo);

	//保存消息到本地DB
	_tChatMessage oneText;
	oneText.strDestUser = chatinfo.m_strAccount.GetData();
	oneText.strChatText = os.str().c_str();
	oneText.m_timeChat = chatinfo.uTime;
	GetDBTaskModule()->SaveChatMessage(tstring(m_CurItem.m_strAccount.GetData()), oneText);


	////static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(false);
	//return 1;



	return 1;
}

std::tstring mainDlg::getCurIdentifier()
{
	tstring strMyselfValue = tstring(m_MyselfItem.m_strAccount).substr(0, tstring(m_MyselfItem.m_strAccount).find("@"));
	tstring strCurValue = tstring(m_CurItem.m_strAccount).substr(0, tstring(m_CurItem.m_strAccount).find("@"));
	if (atol(strMyselfValue.c_str()) <= atol(strCurValue.c_str()))
		return strMyselfValue + strCurValue;
	else
		return strCurValue + strMyselfValue;
}

void mainDlg::MMSendFileMsg(const tstring& strParas )
{
	if (NULL != m_pMsgSessionMgr)
	{
		if (m_CurItem.m_strAccount.GetLength() == 0 || strParas.length() == 0)
		{
			return;
		}

		tstring strUTF8;
		CChineseCode::GB2312ToUTF_8(strUTF8, strParas.c_str(), strParas.length());
		m_pMsgSessionMgr->SendCmd(tstring(m_CurItem.m_strAccount.GetData()), ECmdType::enmCmd_FileReq, strUTF8);
	}
}

DuiLib::ContactListItemInfo mainDlg::getMyselfItem()
{
	return m_MyselfItem;
}

void mainDlg::setLimitChoseFile( bool bLimit)
{
	m_bChoseFile = bLimit;
}


DuiLib::ContactListItemInfo mainDlg::getCurItem()
{
	return m_CurItem;
}


void mainDlg::OnShowCreateNewProjPage()
{
	if (NULL == m_pCreateNewProjPage)
	{
		m_pCreateNewProjPage = new CCreateNewProjStep1(&m_PaintManager);
		if (NULL != m_pCreateNewProjPage)
		{
			m_pCreateNewProjPage->SetMainWndHWND(m_hWnd);
			m_pCreateNewProjPage->CreateWnd();
			m_pCreateNewProjPage->Initialize();
			return;
		}
	}
	if (::IsWindow(m_pCreateNewProjPage->GetHWND()))
	{
		m_pCreateNewProjPage->ShowWindow(true);
	}	
}

void mainDlg::OnProjectEditWndClosed()
{
	if (NULL != m_pCreateNewProjPage)
	{
		m_pCreateNewProjPage = NULL;
	}
}

void mainDlg::OnShowSearchPartnerPage()
{
	if (NULL == m_pSearchPartnerPage)
	{
		m_pSearchPartnerPage = new CSearchBussinessPartnerPage();
		if (NULL != m_pSearchPartnerPage)
		{
			m_pSearchPartnerPage->SetMainWndHWND(m_hWnd);
			m_pSearchPartnerPage->CreateWnd(/*m_hWnd*/);
			m_pSearchPartnerPage->Initialize();
			m_pSearchPartnerPage->SetForegroundWnd();
			return;
		}
	}
	m_pSearchPartnerPage->SetForegroundWnd();
	m_pSearchPartnerPage->ShowWindow(true);
}


void mainDlg::OnClickSystemSettings()
{
	CSystemSetDlg *pSysSetDlg = new CSystemSetDlg(m_MyselfItem.m_strAccount);
	if(NULL == pSysSetDlg)  return;
	pSysSetDlg->CreateDlg();
}

void mainDlg::InitLocalSysMsg()
{
	CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
	if (NULL == pSysMsgList)
	{
		return;
	}

	//mem leak...
	vector<sSysMsg>* pVec = new vector<sSysMsg>;// vecSysMsgs;
	if(GetDBTaskModule()->LoadSysMessage(*pVec))
	{
		int nSize = pVec->size();
		for (int i=0; i<nSize; i++)
		{
			sSysMsg oneSysMsg = (*pVec)[i];
			//pSysMsgList->CacheSysMsg(oneSysMsg);
			pSysMsgList->AddItem(oneSysMsg.strMsg, oneSysMsg.uTime, oneSysMsg.strExInfo/*oneSysMsg.strSender*/, oneSysMsg.eSubType, 0);
			if (oneSysMsg.bProcessed)
			{
				pSysMsgList->DisableButton(&oneSysMsg);
			}
		}
		//pSysMsgList->AddWelcomeItem();
	}
}
void mainDlg::InitChatMsgFromDB()
{
	m_pChatLst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
	if (NULL != m_pChatLst)
	{
		int nSize = m_lstFriends.size();
		if (nSize>0)
		{
			for (int i=0; i<nSize; i++)
			{
				ContactListItemInfo& oneRecent = m_lstFriends[i];
				
				//1.先从数据库加载
				vector<_tChatMessage> vecOldMsgs;
				if(GetDBTaskModule()->LoadChatMessage(tstring(oneRecent.m_strAccount.GetData()), vecOldMsgs))
				{
					for (int i=0; i<vecOldMsgs.size(); i++)
					{
						_tChatMessage& oneMsg = vecOldMsgs[i];

						ChatInfo chatinfo;
						tstring strAccount = oneMsg.strDestUser + _T("@test-d");
						chatinfo.m_lstMsg.push_back(oneMsg.strChatText.c_str());
						chatinfo.m_strAccount = strAccount.c_str();
						chatinfo.m_strRawMsg = oneMsg.strChatText.c_str();
						//chatinfo.uTime = 
						
						chatinfo.m_strLogo = (CFileHelper::GetHigtLightAvatar(strAccount)).c_str();
						m_pChatLst->CacheChatMessage(oneRecent.m_strAccount, &chatinfo);
					}
				}
			}
		}
	}	
}

void mainDlg::OnDbClickFriendList(TNotifyUI& msg)
{
	if (m_pFriendLst->GetCurSel() == -1) return;

	RECT rc = m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetPos();
	POINT pt = {msg.ptMouse.x, msg.ptMouse.y};

	if (::PtInRect(&rc, pt))
	{
		Node* pNode = (Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag();
		if (!m_pFriendLst->IsParentNode(pNode))
		{
			for (int idx = 0; idx < m_listAllFriends.size(); idx++)
			{	
				if (m_listAllFriends[idx].nItemID == pNode->data().value)
				{
					OnShowChatWnd(m_listAllFriends[idx].strAccount);	
					return;
				}
			}
		}				
	}
}

void mainDlg::OnBtnSendMessage()
{
	if (m_CurItem.nItemID == -1)
	{
		MessageBox(NULL, _T("请先选择联系人！"), _T("提示"), MB_OK);
		return;
	}

	MMsendMessage();
}

void mainDlg::UpdateCacheAndUiAfterFriendDeleted(tstring& strDeletedAccount)
{
	int nItemIDInFriendList = -1;
	tstring strOrgId;
	for (vector<sFriendListItem>::iterator it = m_listAllFriends.begin();
		it != m_listAllFriends.end(); ++it)
	{
		if (_tcsicmp(it->strAccount.c_str(), strDeletedAccount.c_str()) == 0)
		{
			nItemIDInFriendList = it->nItemID;
			strOrgId = it->strOrgID;
			//从好友列表Cache删除掉。
			m_listAllFriends.erase(it);
			break;
		}
	}

 	//从好友列表ui+LocalDB删除    todo 由于不一定能调用到CListUI::SelectItem, 故暂时使用m_listAllFirends中的位置来删除
 	if(NULL != m_pFriendLst)
 	{
 		int nCount = m_pFriendLst->GetCount();
 		if (nCount>0)
 		{
 			for (int i=0; i<nCount; i++)
 			{
 				CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pFriendLst->GetItemAt(i));
 				if (pItem != NULL)
 				{
 					Node* node  = (Node*)pItem->GetTag();
					Node* pParentNode = node->parent();
 					if (node->data().value == nItemIDInFriendList)
 					{
 						m_pFriendLst->RemoveNode(node);
						//如果这个父亲节点是空的了，那么这个组织机构应该也要删除掉，--llzhou
						if (!pParentNode->has_children())
						{
							//m_pFriendLst->RemoveNode(pParentNode);//如果是“我的好友”是不可以删除的--lee
							if (DEF_ORGID_MYFRIEND != strOrgId)
							{
								m_pFriendLst->RemoveNode(pParentNode);
								int nSize = m_mapOrg2Node.size();
								map<tstring, DuiLib::Node*>::iterator it2 = m_mapOrg2Node.find(strOrgId);
								if (it2 != m_mapOrg2Node.end())
								{
									m_mapOrg2Node.erase(it2);
								}
								
								nSize = m_mapOrg2Node.size();
								int ncout =100;
							}
						}
						
 						break;
 					}
 				}
 			}
 		}
 	}

	//删除本地DB的聊天记录和最近联系人列表
	GetDBTaskModule()->RemoveRecentItem(strDeletedAccount);
	//GetDBTaskModule()->RemoveChatList(strAccount);

	//清空最近联系人cache+ui+LocalDB
	if (m_lstFriends.size()>0)
	{
		int nItemID=-1;
		tstring strNickName;
		for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
			it != m_lstFriends.end(); ++it)
		{
			ContactListItemInfo& oneFriend = *it;
			//找到这个人了，从Cache中删除
			if (_tcsicmp(oneFriend.m_strAccount.GetData(), strDeletedAccount.c_str())==0)
			{
				nItemID = it->nItemID;
				strNickName = it->m_strNickName.GetData();
				m_lstFriends.erase(it);
				break;
			}
		}

		//从界面删除这个人
		if (nItemID>=0)
		{
			if (NULL != m_pRecentList)
			{
				m_pRecentList->RemoveItem(nItemID);
			}
		}

		//如果聊天框内正好是这个人，就需要清除聊天记录
		CLabelUI* pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName/*kbuttonContactNameControlName*/));
		if (NULL != pFriendNickname)
		{
			CDuiString strNickStatus = pFriendNickname->GetText();
			int nIdex = strNickStatus.Find(strNickName.c_str(), 0);
			if(nIdex>=0)
			{
				//清空？还是选择最近联系人里面的最上面的一个联系人信息呢？===to do...
				pFriendNickname->SetText(_T(""));
				//清空用户头像
				CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
				if (NULL != pBtnFriendAvatar)
				{
					pBtnFriendAvatar->SetNormalImage(_T(""));
				}
				//清空用户机构信息
				CLabelUI* pLabelFriendCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
				if (NULL != pLabelFriendCompany)
				{
					pLabelFriendCompany->SetText(_T(""));
				}

				CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
				if (NULL != pFriendOrgname) pFriendOrgname->SetText(_T(""));

				//清空聊天记录和输入框内容
				CChatListUI* pOutput = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
				CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(krichEditInputEditControlName));
				if (NULL != pOutput)
				{
					pOutput->RemoveAll();
				}
				if (NULL != pEditInput)
				{
					pEditInput->SetText((_T("")));
				}
			}
		}
	}

	////如果新加好友列表中有这个人，就删除掉吧：我刚加的，又删除了。
	//map<tstring, time_t>::iterator itFind = m_mapAccount2Time.find(strDeletedAccount);
	//if (itFind != m_mapAccount2Time.end())
	//{
	//	m_mapAccount2Time.erase(itFind);
	//}
}



void mainDlg::OnClickCallTypeDrop(POINT pt)
{
	CDuiMenu *pMenu = new CDuiMenu(_T("Call\\callTypeDropUISkin.xml"), eUIMenuCallTypeDrop);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

void mainDlg::OnProcess( tstring& fileName, int iValue, tstring& strKey )
{
	if (fileName.empty())
		return;
	CVerticalLayoutUI *pFileProcess = NULL;
	map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(strKey);
	if (it != m_mapFileTab.end())
	{
		CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(it->second,"listfileProcess"));
		CListContainerElementUI* pelement = static_cast<CListContainerElementUI*>(m_PaintManager.FindSubControlByName(plstProcess, fileName.c_str()));
		CProgressUI *pProcess = static_cast<CProgressUI*>(m_PaintManager.FindSubControlByName(pelement, _T("progress_file_upload")));
		pProcess->SetValue(iValue);
		if (iValue == 100)
		{
			tstring strKey = fileName;
			//map<tstring, fileInfo>::iterator iter = m_mapTaskFiles.find(strKey);
			
			//1. UI清除
			if (plstProcess!=NULL && pelement!=NULL)
				plstProcess->Remove(pelement);
			//2. map容器中清除
			/*if (iter!=m_mapTaskFiles.end())
				m_mapTaskFiles.erase(iter);*/
			int nSize = m_mapTaskFiles.size();
			for (map<tstring, fileInfo>::iterator it = m_mapTaskFiles.begin(); it != m_mapTaskFiles.end();)
			{
				if (it->first == strKey)
				{
					m_mapTaskFiles.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}

			nSize = m_mapTaskFiles.size();
		}
		else if (iValue == -1)	
		{
			tstring strKey = fileName;
			map<tstring, fileInfo>::iterator iter = m_mapTaskFiles.find(strKey);
			//发送，或者下载失败
			if ( iter != m_mapTaskFiles.end())
			{
				//1. 系统消息
				tstring strSys;
				strSys = iter->second.strFileFullName + " " + "传输失败。";
				CDuiString duistrSys = strSys.c_str();
				CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
				if (pChatlst != NULL)
					pChatlst->AddSysItem(duistrSys);
				//2. map容器清除
				m_mapTaskFiles.erase(iter);
			}
			
			//3. ui清除
			if (plstProcess!=NULL && pelement!=NULL)
				plstProcess->Remove(pelement);
			
		}
	}
	else
		return;
}

bool mainDlg::GetCallInFriendItem(tstring strPhoneNo, sFriendListItem& friendItem)
{
	for(std::vector<sFriendListItem>::iterator iter = m_listAllFriends.begin(); iter != m_listAllFriends.end(); ++iter)
	{
		if (tstring::npos != iter->strAccount.find(strPhoneNo))
		{
			friendItem = *iter;
			return true;
		}
	}
	return false;
}


void mainDlg::HandleItemClickEvent( TNotifyUI& msg )
{
	tstring strName = msg.pSender->GetName();
	//
	if (_tcsicmp(strName.c_str(), "System_WelCome_Item") == 0)
	{
		int nCount = m_pRecentList->GetCount();
		int nRecentCur = m_pRecentList->GetCurSel();
		if (-1 == nRecentCur && nCount == 1)
		{
			CDuiString strZero = "0";
			m_pRecentList->setMsgNum(strZero, nRecentCur);
			m_bSysUnread = false;
			bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);
		}

		//如果有离线消息，可能nCount不止一个。
		if (-1 != nRecentCur)
		{
			CDuiString strZero = "0";
			m_pRecentList->setMsgNum(strZero, nRecentCur);
			m_bSysUnread = false;
			bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);
		}
	}
	if (_tcsicmp(strName.c_str(), "10000@test-d") == 0)
	{
		int nCount = m_pRecentList->GetCount();
		int nRecentCur = m_pRecentList->GetCurSel();
		if (-1 == nRecentCur && nCount == 1)
		{
			CDuiString strZero = "0";
			m_pRecentList->setMsgNum(strZero, nRecentCur);
			m_bSysUnread = false;
		
		}

		//如果有离线消息，可能nCount不止一个。
		if (-1 != nRecentCur)
		{
			CDuiString strZero = "0";
			m_pRecentList->setMsgNum(strZero, nRecentCur);
			m_bSysUnread = false;
		}

		//GetMsgCenterModule()->ClearUnReadSysMsgs();
		//bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
		//setOptionRecentChatListUnread(bunRead);
	}
	//当前在最近联系人 & 是主页显示====>
	if (TAB_VIEW_RECENTCHAT_PAGE == static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->GetCurSel()
			&& CONST_RECENT_TAB_HOME_PAGE == static_cast<CTabLayoutUI*>(m_PaintManager.FindSubControlByName(static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar)), kRecentChatLayout))->GetCurSel())
	{
		//如果当前展示的主页，就显示上次最近联系人被选中的tab page：
		//if(m_nLastSelectIndex != -1)
		if(!m_strSelectedIndex.empty())
		{
			int nSelected = _tcstoul(m_strSelectedIndex.c_str(), NULL, 10);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout))->SelectItem(nSelected/*CONST_RECENT_TAB_P2P_CHAT_LIST*/);	
		}
		m_pBussinessMaterialBtn->SetVisible(false);
		m_pBussinessQuitBtn->SetVisible(false);
		KillTimer(this->GetHWND(), WM_TIEMER_UPDATE_HOMEPAGE_PIC);
		return;
	}			

	

	if (0 == msg.pSender->GetName().Find(STR_FRIENDLIST_ITEM)
				&& (m_pFriendLst != NULL) 
				&&  m_pFriendLst->GetItemIndex(msg.pSender) != -1)
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			Node* node = (Node*)msg.pSender->GetTag();
			if (m_pFriendLst->CanExpand(node))
			{
				tstring strOrgId = node->data().strOrgID;
				OnFriendListSelected(strOrgId,true); //将来可以在partner_page页面把机构介绍展现出来，暂时先空白处理吧！ llzhou
				m_pFriendLst->SetChildVisible(node, !node->data().child_visible_);
			}
			else 
			{
				for (int idx = 0; idx < m_listAllFriends.size(); idx++)
				{	
					if (m_listAllFriends[idx].nItemID == node->data().value)
					{
						OnFriendListSelected(m_listAllFriends[idx].strAccount);
						break;
					}
				}
			}
		}
	}
	else if (0 == msg.pSender->GetName().Find("ResListItem_")
				&& (NULL != m_pGroupChatPage->m_pFilePacketList)
				&& m_pGroupChatPage->m_pFilePacketList->GetItemIndex(msg.pSender) != -1)
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			Node* node = (Node*)msg.pSender->GetTag();
			if (m_pGroupChatPage->m_pFilePacketList->CanExpand(node))
			{
				//tstring str;
				//OnFriendListSelected(str,true); //将来可以在partner_page页面把机构介绍展现出来，暂时先空白处理吧！ llzhou
				m_pGroupChatPage->m_pFilePacketList->SetChildVisible(node, !node->data().child_visible_);
			}
			else 
			{
				for (int idx = 0; idx < m_listAllFriends.size(); idx++)
				{	
					if (m_listAllFriends[idx].nItemID == node->data().value)
					{
						//OnFriendListSelected(m_listAllFriends[idx].strAccount);
						break;
					}
				}
			}
		}
	}
}


void mainDlg::ShowNetCallBtn(bool bVisible)
{
	CHorizontalLayoutUI* pVoiceHLayouUI = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(kHLayoutNetCallName));	
	CHorizontalLayoutUI* pContactHLayouUI = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(kHLayoutContactInfo));	
	
	if (NULL != pVoiceHLayouUI) pVoiceHLayouUI->SetVisible(bVisible);	
	if (NULL != pContactHLayouUI) pContactHLayouUI->SetVisible(bVisible);
}

void mainDlg::HandleChangePassword()
{
	tstring strUrl(WEB_DOMAIN_URL);
	strUrl += "site/reset-pwd";
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), strUrl.c_str(), NULL, SW_SHOW);
}

void mainDlg::HandleChangeOrganization()
{
	CFrameChangeOrganization::Instance()->ShowWindow(true);
	CFrameChangeOrganization::Instance()->showFocus();
}

void mainDlg::HandleEmployeeManage()
{
	CFrameEmployeeManage1::Instance()->ShowWindow(true);
	//CFrameEmployeeManage::Instance()->showFocus();
}


sUserVCard mainDlg::getMyUserInfo()
{
	return m_myselfInfo;
}

void mainDlg::HandleSysAbout()
{
	CFrameSysAbout::Instance()->ShowWindow(true);
}

void mainDlg::ResetPassWord()
{	
	CRegisterAccountDlg* pRegisterDlg = new CRegisterAccountDlg(DLG_TYPE_RESET_PASSWORD);
	pRegisterDlg->CreateWnd(this->GetHWND());
}

void mainDlg::HandleSysQuit()
{
	CFrameSysQuit::Instance()->ShowWindow(true);
}

void mainDlg::InitVideoFrm()
{
	CVideoDlg::Instance()->ActiveVoiceDlg(false);
	m_pVoiceBtn =  static_cast<CButtonUI*>(m_PaintManager.FindControl("ButtonNetCall"));
	m_pVideoBtn =  static_cast<CButtonUI*>(m_PaintManager.FindControl("ButtonVideo"));
	m_pBussinessMaterialBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnBusinessMaterial"));
	m_pBussinessQuitBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl("btnBusinessQuit"));
}

void mainDlg::setUserIdentity( int iIdentity )
{
	m_iCurIdentity = iIdentity;
}

int mainDlg::getUserIdentity()
{
	return m_iCurIdentity;
}

void mainDlg::HandleCreateOrg()
{
	CFrameCreateOrg::Instance()->ShowWindow(true);
}

void mainDlg::HandleUserInfo()
{
	CFrameUserInfo::Instance()->ShowWindow(true);
}

int mainDlg::apiGetUserIdentity()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-status?");
	tstring strPost = (_T(""));
	strPost += _T("cell_phone=");
	strPost += m_myselfInfo.strPhone;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return USER_NO_ORG;
	}
	else if(strHttpResponse.find("is_admin") != -1)
	{
		tstring strRes = strHttpResponse.substr(strHttpResponse.find("is_admin") + 10,1);
		if (strRes == "1")
			return USER_ORG_BOSS;
		else if (strRes == "0")
			return USER_ORG_MEMBER;
		else
			return -1;
	}
	else 
	{
		return -1;
	}
}

void mainDlg::HandleSysQuitOrg()
{
	CFrameSysQuitOrg::Instance()->ShowWindow(true);
}

LRESULT mainDlg::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (pWebKitUI)
	{
		RECT rect;
		GetClientRect(this->GetHWND(), &rect);
		rect.left += 40;
		rect.top += 98;	
		rect.bottom -= 5;
		rect.right -= 5;
		pWebKitUI->SetPos(rect);
	}
	
	return __super::OnSize(uMsg, wParam, lParam, bHandled);
}


bool mainDlg::GetGroupMembersOfMyself(vector<sFriendListItem*>& vecGroupMembsers)
{
	bool bInOrg = false;
	//先判断自己是不是有组织
	if (NULL != m_pUsrMgrAdapter)
	{
		tstring strMyOrgID;
		tstring strMyOrgName;
		if(m_pUsrMgrAdapter->GetMyOrganization(strMyOrgName, strMyOrgID))
		{
			bInOrg = true;
			for (vector<sFriendListItem>::iterator it = m_listAllFriends.begin();
				it != m_listAllFriends.end(); ++it)
			{
				sFriendListItem& oneFriend = *it;
				if (_tcsicmp(oneFriend.strOrgID.c_str(), strMyOrgID.c_str()) == 0)
				{
					sFriendListItem* pNewFriend = new sFriendListItem;
					pNewFriend->strAccount = oneFriend.strAccount;
					pNewFriend->strAvatar  = oneFriend.strAvatar;
					pNewFriend->strOrgID   = oneFriend.strOrgID;
					pNewFriend->strOrgnization = oneFriend.strOrgnization;
					pNewFriend->strShowName = oneFriend.strShowName;
		
					vecGroupMembsers.push_back(pNewFriend);
				}
			}
		}
		//不在任何组织里面
		else
		{
			sFriendListItem* pMySelf = new sFriendListItem;
			pMySelf->strAccount = CController::Instance()->GetXmppClient()->jid().bare();
			pMySelf->strAvatar = m_MyselfItem.m_strLogo.GetData();
			pMySelf->strShowName = m_MyselfItem.m_strNickName.GetData();
			vecGroupMembsers.push_back(pMySelf);
		}
	}

	return bInOrg;
}



bool mainDlg::GetMembersByOrgID(tstring& strOrgID, vector<sFriendListItem>& vecMembers)
{
	for (vector<sFriendListItem>::iterator it = m_listAllFriends.begin();
		it != m_listAllFriends.end(); ++it)
	{
		sFriendListItem& oneFriend = *it;
		if (_tcsicmp(oneFriend.strOrgID.c_str(), strOrgID.c_str()) == 0)
		{
			sFriendListItem oneMember;
			oneMember.strAccount = oneFriend.strAccount;
			oneMember.strAvatar  = oneFriend.strAvatar;
			oneMember.strOrgID   = oneFriend.strOrgID;
			oneMember.strOrgnization = oneFriend.strOrgnization;
			oneMember.strShowName = oneFriend.strShowName;
			oneMember.eState = oneFriend.eState;
			oneMember.nItemID = oneFriend.nItemID;
			oneMember.strSignatrue = oneFriend.strSignatrue;

			vecMembers.push_back(oneMember);
		}
	}

	return vecMembers.size()>0;
}

void mainDlg::RefreshGroupList(const tstring strAccount, bool bDelayUpdate /*= true*/)
{
	vecFriendListItemIter iterAccount = find_if(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper(strAccount, CSortHelper::eFoundField::eFoundAccount));
	if (m_listAllFriends.end() == iterAccount) return;

	tstring strGroup = iterAccount->strOrgnization.empty()? ORGANIZATION_MYFRIEND: iterAccount->strOrgnization;
	tstring strOrgId = iterAccount->strOrgID.empty()? DEF_ORGID_MYFRIEND: iterAccount->strOrgID;
	if (bDelayUpdate)
	{
		CDelaySortMrg::Instance()->AddRefreshGroup(strOrgId, strGroup);
	}
	else 
	{
		int nCurSel = m_pFriendLst->GetCurSel();
		vecFriendListItemIter firstIter, lastIter;
		CSortHelper::FindIf(m_listAllFriends.begin(), m_listAllFriends.end(), firstIter, lastIter, CSortHelper(strOrgId, CSortHelper::eFoundField::eFoundGroup));
		sort(firstIter,lastIter,CSortHelper::CompareFuncIntraGroup);

		m_pFriendLst->RemoveChildNode(m_mapOrg2Node[strOrgId]);
		while (firstIter != lastIter)	
		{
			FriendListItemInfo item;	
			item.strOrgName = strGroup;
			item.itemId = firstIter->nItemID;
			item.logo = firstIter->strAvatar;
			item.nick_name =firstIter->strShowName;
			m_pFriendLst->AddNode(item,m_mapOrg2Node[strOrgId]);
			++firstIter;
		}
		m_pFriendLst->SelectItem(nCurSel);
	}		
}

LRESULT mainDlg::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (WM_TIMER == uMsg )
	{
		if (wParam == WM_TIEMER_UPDATE_FRIEND_LIST && CDelaySortMrg::Instance()->WaitDelaySort())
		{
			SortFriendList();
		}
		if (wParam == WM_TIEMER_UPDATE_HOMEPAGE_PIC )
		{
			RefreshHomePagePic();
		}
		if (wParam == WM_TIMER_UPDATE_ROOM_LIST)
		{
			if (CController::Instance()->GetRoomManager()->GetListUpdateState())
			{
				KillTimer(this->GetHWND(), WM_TIMER_UPDATE_ROOM_LIST);
				RefreshContactListControl();
			}
			
		}

		if (wParam == WM_TIMER_DELETE_ROOM_LIST)
		{
			if (CController::Instance()->GetRoomManager()->GetListDeleteState())
			{
				KillTimer(this->GetHWND(), WM_TIMER_DELETE_ROOM_LIST);
				RefreshContactListControl();
			}

		}
		if (wParam == WM_TIMER_CHATLISTENDDOWN )
		{
			CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
			pChatlst->EndDown();
			KillTimer(this->GetHWND(), WM_TIMER_CHATLISTENDDOWN);
		}
		if (wParam == WM_TIMER_GROUPCHATPAGEENDDOWN )
		{
			CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl("listgroupchat"));
			pChatlst->EndDown();
			KillTimer(this->GetHWND(), WM_TIMER_GROUPCHATPAGEENDDOWN);
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void mainDlg::SortFriendList()
{
	int nCurSel = m_pFriendLst->GetCurSel();
	t_mapStr2Str mapOrgId2Group;
	CDelaySortMrg::Instance()->CopyOrgId2GroupMap(mapOrgId2Group);

	sort(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper::CompareFunc);//修正接受好友请求排序后好友消失的bug by HQ 20160520

	if (mapOrgId2Group.size() >0)
	{
		m_pFriendLst->RemoveAll();	
		map<tstring, DuiLib::Node*> mapBK;
		mapBK = m_mapOrg2Node;
		m_mapOrg2Node.clear();

		set<t_strstrPair> tOrgIdGroupPairSet;
		for (std::vector<sFriendListItem>::iterator iter = m_listAllFriends.begin(); 
			iter != m_listAllFriends.end(); ++iter)
		{
			tOrgIdGroupPairSet.insert(t_strstrPair(iter->strOrgID, iter->strOrgnization));		
		}
		tOrgIdGroupPairSet.insert( make_pair(DEF_ORGID_MYFRIEND,ORGANIZATION_MYFRIEND));		

		vector<t_strstrPair> vecOrgIdGroupPairVector;
		copy(tOrgIdGroupPairSet.begin(), tOrgIdGroupPairSet.end(), back_inserter(vecOrgIdGroupPairVector));
		sort(vecOrgIdGroupPairVector.begin(), vecOrgIdGroupPairVector.end(), CSortHelper::CompareGroupFunc);

		for (vector<t_strstrPair>::iterator iter = vecOrgIdGroupPairVector.begin(); iter != vecOrgIdGroupPairVector.end(); ++iter)
		{
			FriendListItemInfo rootItem;
			rootItem.folder = true;
			rootItem.strOrgName =iter->second;
			rootItem.strOrgID = iter->first;
			rootItem.itemId = GetNextItemID();
			DuiLib::Node* pOrgNode = m_pFriendLst->AddNode(rootItem, NULL);	
			m_mapOrg2Node.insert(make_pair(rootItem.strOrgID, pOrgNode));
		}

		for (std::vector<sFriendListItem>::iterator iter = m_listAllFriends.begin(); 
			iter != m_listAllFriends.end(); ++iter)
		{
			FriendListItemInfo item;
			item.empty = false;
			item.folder = false;
			item.strOrgName = iter->strOrgnization.empty() ? ORGANIZATION_MYFRIEND : iter->strOrgnization;
			item.strOrgID = iter->strOrgID;
			item.itemId = iter->nItemID;
			item.logo = iter->strAvatar;
			item.nick_name =iter->strShowName;
			item.account = iter->strAccount;
	
			m_pFriendLst->AddNode(item,m_mapOrg2Node[item.strOrgID]);
			
			map<tstring, DuiLib::Node*>::iterator iterFind = mapBK.find(item.strOrgID);

			if(iterFind != mapBK.end())
			{
				//这句话会导致开机崩溃，暂时屏蔽，by zyy
				//m_pFriendLst->SetChildVisible(m_mapOrg2Node[item.strOrgID],iterFind->second->data().child_visible_);
				//m_mapOrg2Node[item.strOrgID]->data().child_visible_ = iterFind->second->data().child_visible_;
			}
		}
	}
	m_pFriendLst->SelectItem(nCurSel);
}

void mainDlg::welcomeMsg()
{   
	CFileHelper::GetHeadCacheDir();
	//GetDBTaskModule()->LoadLastMessage(strAccount, oneChatMsg)
	bool bFirst = GetDBTaskModule()->IsFirstLogin();
	if (bFirst)
	{
		// 添加欢迎信息
		ContactListItemInfo info;
		info.nItemID = 10000;//?
		info.m_strAccount = "10000@test-d";
		info.m_strNickName = "系统消息";
		info.m_strMsgCount = "1";
		info.m_strLastMsg = " 欢迎使用MM!";
		info.m_strLastTm = (GetHMSTimeString(time(0))).c_str();  
		info.m_strLogo = "default_sys_notify.png";
		m_pRecentList->AddItem(&info);
		m_bSysUnread = true;
		bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
		setOptionRecentChatListUnread(bunRead);
		CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kRecentChatLayout));
		if (NULL == pLayout)
		{
			return;
		}

		CFrameGuideUser::Instance()->ShowWindow(true);
		CFrameGuideOrg::Instance()->ShowWindow(false);
	}
}

void mainDlg::showGuideOrg()
{
	CFrameGuideOrg::Instance()->ShowWindow(true);
}

void mainDlg::HandleOrgUpdateEvent( UINT uMsg )
{
	tstring strAccount = CController::Instance()->GetXmppClient()->jid().bare();
	Node* pParentNode = NULL;

	vecFriendListItemIter iterAccount = find_if(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper(strAccount, CSortHelper::eFoundField::eFoundAccount));
	if (m_listAllFriends.end() != iterAccount)
	{
		map<tstring, Node*>::iterator iter  = m_mapOrg2Node.find(iterAccount->strOrgID);
		if (m_mapOrg2Node.end() != iter) pParentNode = iter->second;
	}	
	
	
	CLabelUI* pLabelMyOrg = static_cast<CButtonUI*>(m_PaintManager.FindControl(kLabelMySelfCompany));	

	if (WM_EXIT_ORG == uMsg && NULL != pParentNode)
	{		
		Node* pChild = m_pFriendLst->GetChildNode(pParentNode, iterAccount->nItemID);
		if (NULL == pChild) return;
		
		m_pFriendLst->RemoveNode(pChild);

		if (!pParentNode->has_children())
		{
				m_pFriendLst->RemoveNode(pParentNode);
		}		
		pLabelMyOrg->SetText("您还没有机构");
	}


	orgInfo org;
	if (!CWebClient::GetMyOrg(org))
	{
		return;
	}

	if (WM_MODIFY_ORG_INFO == uMsg && NULL != pParentNode)
	{
		m_pFriendLst->ModifyOrgName(pParentNode, org.strName);
		pLabelMyOrg->SetText(org.strName.c_str());
	}
	else if (WM_CREATE_ORG == uMsg)
	{
		sFriendListItem oneFriend;
		oneFriend.nItemID	 = GetNextItemID();
		oneFriend.strAccount = strAccount;
		oneFriend.strAvatar = m_MyselfItem.m_strLogo;
		oneFriend.strShowName  = m_MyselfItem.m_strNickName.IsEmpty()? ExtractPhoneNo(strAccount): m_MyselfItem.m_strNickName.GetData();
		oneFriend.strOrgnization = org.strName;
		oneFriend.strOrgID = org.strId;
		m_listAllFriends.insert( find_if(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper(oneFriend.strOrgnization, CSortHelper::eFoundField::eFoundGroup)), oneFriend);


		FriendListItemInfo item;
		item.folder = true;
		item.strOrgName = org.strName;
		item.strOrgID = org.strId;
		item.itemId = GetNextItemID();
		Node* pParentNode = m_pFriendLst->AddNode(item, NULL);
		m_mapOrg2Node.insert(make_pair(org.strId, pParentNode));

		item.folder = false;
		item.itemId = oneFriend.nItemID;
		item.logo = oneFriend.strAvatar;
		item.nick_name = oneFriend.strShowName;

		m_pFriendLst->AddNode(item,pParentNode);		
		pLabelMyOrg->SetText(org.strName.c_str());
	}	
	CWebClient::ClearOrgCache();
}


void mainDlg::InitialHomePage()
{
	CDialogBuilder m_DlgBuilder;
	CHorizontalLayoutUI *pHomePageTab = NULL;
	CDialogBuilderCallbackEx cb(&m_PaintManager);
	if (!m_DlgBuilder.GetMarkup()->IsValid())
	{
		pHomePageTab = static_cast<CHorizontalLayoutUI*>(m_DlgBuilder.Create(_T("homePage\\homepage.xml"),
			(UINT)0, &cb, &m_PaintManager));
	}
	else
	{
		pHomePageTab = static_cast<CHorizontalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
	}	
	static_cast<CTabLayoutUI*>(m_PaintManager.FindControl("RecentChatTabLayout"))->AddAt(pHomePageTab, CONST_RECENT_TAB_HOME_PAGE/*2*/);	

	m_pHomePageHelpText = static_cast<CTextUI*>(m_PaintManager.FindSubControlByName(pHomePageTab, "txthomePageTip"));
	m_pHomePageVLBkImg = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(pHomePageTab, "homepageBkImg"));
	m_pHomePageInvitePartner = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pHomePageTab, "btnhomePageAddPartner"));
	m_pHomePageNetCall = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pHomePageTab, "btnhomePageNetCall"));
	m_pHomePageCreateOrg = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pHomePageTab, "btnhomePageCreateOrg"));
	m_pHomePageBussiness = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pHomePageTab, "btnhomePageBusiness"));
}


void mainDlg::HandleMouseEnterEvent(TNotifyUI& msg)
{	
	if (NULL == m_pHomePageHelpText) return;

	if (_tcsicmp(msg.pSender->GetName(), "btnhomePageAddPartner") == 0)
	{
		m_pHomePageHelpText->SetText("商业伙伴是您进行买卖的对象，分为机构和个人两种，您可以使用MM Android版轻松的将手机联系人添加入商业伙伴。");
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btnhomePageCreateOrg") == 0)
	{
		m_pHomePageHelpText->SetText("机构是商业伙伴的一种，每个人同时只可以存在一个机构中，创建机构者默认成为机构管理员，管理员可进行管理权限转让，机构成员管理等操作。");
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btnhomePageBusiness") == 0)
	{			
		m_pHomePageHelpText->SetText("创建新业务按钮帮助您快速的建立业务，一个业务最多可发给三家机构询报价，在业务沟通中您可以发送资料、音视频聊天。这一切都能够在业务面板方便的进行管理。");
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btnhomePageNetCall") == 0)
	{
		m_pHomePageHelpText->SetText("我们提供免费电话服务，帮助您毫无障碍地与商业伙伴进行沟通。与每个月高额的话费单说再见吧！");
	}
}

void mainDlg::RefreshHomePagePic()
{
	static int idx = 0;
	CDuiString strPic;
	strPic.Format("homepage\\bg%d.PNG", idx++%5);
	m_pHomePageVLBkImg->SetBkImage(strPic);
}


//如果当前选中的room刚好就是strRoomId，就更新room成员个数！
//注意这个函数的命名，实际上新建room，添加room成员都会调用此函数
//对于新建room，群头像相当就刷新了两次，后面再看是否有修改的必要  by zyy
void mainDlg::OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<tstring>& vecMemList)
{
	if (_tcsicmp(m_CurItem.m_strRoomID.GetData(), strRoomID.c_str()) == 0)
	{
		char chBuf[10]={0};
		itoa(vecMemList.size(), (char*)(&chBuf), 10);
		tstring strCount = "(";
		strCount += chBuf;
		strCount += "人)";
		CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
		if (NULL != pGroupMemCount)
		{
			pGroupMemCount->SetText(strCount.c_str());
		}

		////群头像要需要更新了,如果是新建群组，头像已经更新过了，如果是添加成员这里群组头像就需要再次更新，所以强制再更新一次

		OutputDebugString("进OnNewGroupMemberList 相等\r\n");
		ComposeGroupAvatar(m_CurItem);
		OutputDebugString(m_CurItem.m_strLogo.GetData());
		OutputDebugString("\r\n");

		//路径最原始的存放是在 m_lstFriends 里面，所以最关键的是要更新m_lstFriends
		for (int i=0; i<m_lstFriends.size(); i++)
		{
			if (m_CurItem.nItemID == m_lstFriends[i].nItemID)
			{	
				//不相等说明是群组人员个数发生了变化，同理群组成员个数如果大于9，头像未
				//发生变化，则不需要更新
				if (m_lstFriends[i].m_strLogo != m_CurItem.m_strLogo)
				{
					OutputDebugString("赋值logo\r\n");
					m_lstFriends[i].m_strLogo = m_CurItem.m_strLogo;

					UpdateAvatarInRecentList(m_CurItem.m_strRoomID.GetData(), m_CurItem.m_strLogo.GetData(),
						m_CurItem.nItemID);

					GetDBTaskModule()->UpdateAvatarInRecentGroup(m_CurItem.m_strRoomID.GetData(), m_CurItem.m_strLogo.GetData());
				}
				break;
			}
		}	

		//只要路径不改，头像就无法刷新，因此群组头像路径现在与成员个数相关联
 		CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
 		if (NULL != pBtnFriendAvatar)
 		{
			pBtnFriendAvatar->SetNormalImage(m_CurItem.m_strLogo);
 		}	
	}
	else
	{
		////群头像要需要更新了,如果是新建群组，头像已经更新过了，如果是添加成员这里群组头像就需要再次更新，所以强制再更新一次
		ContactListItemInfo info;
		info.m_strRoomID = strRoomID.c_str();

		OutputDebugString("进OnNewGroupMemberList 不相等\r\n");
		ComposeGroupAvatar(info);
		OutputDebugString(info.m_strLogo.GetData());
		OutputDebugString("\r\n");

		//路径最原始的存放是在 m_lstFriends 里面，所以最关键的是要更新m_lstFriends
		for (int i=0; i<m_lstFriends.size(); i++)
		{
			if (strRoomID == m_lstFriends[i].m_strRoomID.GetData())
			{	
				//不相等说明是群组人员个数发生了变化，同理群组成员个数如果大于9，头像未
				//发生变化，则不需要更新
				if (m_lstFriends[i].m_strLogo != info.m_strLogo)
				{
					OutputDebugString("赋值logo\r\n");
					m_lstFriends[i].m_strLogo = info.m_strLogo;

					UpdateAvatarInRecentList(info.m_strRoomID.GetData(), info.m_strLogo.GetData(),
						m_lstFriends[i].nItemID);

					GetDBTaskModule()->UpdateAvatarInRecentGroup(info.m_strRoomID.GetData(), info.m_strLogo.GetData());
				}
				break;
			}
		}	
	}
}

//从m_lstFriends中删除
//m_pRecentList里面的删除通过RefreshContactListControl()函数的调用完成
//从本地数据库中删除
void mainDlg::OnDeleteGroupFromMemberList(const std::tstring& strRoomID)
{
	if (_tcsicmp(m_CurItem.m_strRoomID.GetData(), strRoomID.c_str()) == 0)
	{
		for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
			it != m_lstFriends.end(); ++it)
		{
			if (_tcsicmp(it->m_strRoomID.GetData(), strRoomID.c_str()) == 0)
			{
				m_lstFriends.erase(it);
				break;
			}
		}
	}

	GetDBTaskModule()->RemoveFromRecentGroup(strRoomID);
}

//为了更新群昵称
void mainDlg::OnRoomNameArrived(const std::tstring& strRoomID, tstring& strRoomNickName)
{
	char* pBufRoomId = new char[strRoomID.length()+1];
	char* pBufRoomName = new char[strRoomNickName.length()+1];
	memset(pBufRoomId, 0, strRoomID.length()+1);
	memset(pBufRoomName, 0, strRoomNickName.length()+1);

	strcpy(pBufRoomId, strRoomID.c_str());
	strcpy(pBufRoomName, strRoomNickName.c_str());

	PostMessage(WM_ROOM_NAME_UDPATED, (WPARAM)pBufRoomId, (LPARAM)pBufRoomName);
}

//by zyy
//在roomname到达时。roomlist似乎才真正更新，需要继续理解
//这时候更新群组头像才能正确的获取群组成员从而更新头像。
void mainDlg::OnUpdateRoomName(tstring& strRoomID, tstring& strRoomNickName)
{
	tstring avatar_path;
	bool bUpdate = false;
	int nItemIDTobeupdate = -1;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); 
			iter!=m_lstFriends.end(); iter++)
		{
			if ( SYS_GROUP_CHAT_ACCOUNT == tstring((*iter).m_strAccount)
				|| (!iter->m_strRoomID.IsEmpty() && iter->m_strOrgID.IsEmpty()))//如果对方是个人的群聊天。
			{
				if ( _tcsicmp(iter->m_strRoomID.GetData(), strRoomID.c_str()) == 0)
				{
					//如果是机构群聊天。更新nickname的同时，更新头像
					if ( !iter->m_strOrgID.IsEmpty() )
					{
						ContactListItemInfo& info = *iter;
						info.m_strNickName = strRoomNickName.c_str();
						avatar_path = info.m_strLogo;
						bUpdate = true;
						nItemIDTobeupdate = iter->nItemID;
						break;
					}
					//如果是个人群聊天
					else
					{
						ContactListItemInfo& info = *iter;
						info.m_strNickName = strRoomNickName.c_str();
						avatar_path = info.m_strLogo;
						bUpdate = true;
						nItemIDTobeupdate = iter->nItemID;
						break;
					}
				}			
			}
		}
	}

	//最近联系人的群名称修改下：最初始的m_lstFriends也需要更新，不然不能对应，
	if (bUpdate && nItemIDTobeupdate != -1)
	{
		if (NULL != m_pRecentList)
		{
			int nCount = m_pRecentList->GetCount();
			for (int i=0; i<nCount; i++)
			{
				int nItemID = m_pRecentList->GetItemAt(i)->GetTag();
				if (nItemID == nItemIDTobeupdate)
				{
					m_pRecentList->UpdateShowName(strRoomNickName, i/*nItemIDTobeupdate*/);
//					m_pRecentList->UpdateHeadImage(avatar_path, i);
					//顶部信息也需要更新。
					if (m_CurItem.nItemID == nItemID)
					{
						m_CurItem.m_strNickName = strRoomNickName.c_str();
						//这个函数对群的图标处理有问题，单独做一遍，先不管以后再改 by zyy
						UpdateUserInfoAtWndTop(tstring(m_CurItem.m_strAccount.GetData()), m_CurItem.m_State, tstring(m_CurItem.m_strLogo.GetData()));
					
						CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
						if (NULL != pBtnFriendAvatar)
						{
							pBtnFriendAvatar->SetNormalImage(avatar_path.c_str());
						}	
					}					
				}
			}
		}
	}
	else//有可能是收到离线群消息后，新建起来的room，还没有在最近联系人里面有Item，就新建一个
	{
		//同样要判断本人是否在该群内
		//本地群管理这时候还没有添加该群内容
		if (1)//CController::Instance()->GetRoomManager()->IsInThisRoom(strRoomID, m_myselfInfo.strAccount))
		{
			//将新建的群组加入到“最近联系人列表”数据库		
			ContactListItemInfo info;
			info.m_strAccount = SYS_GROUP_CHAT_ACCOUNT;
			info.m_strLogo = DEF_AVATAR_NOR;
			info.m_strNickName = strRoomNickName.c_str();//strOrgName.c_str();
			info.m_strOrgName =(_T(""));//strOrgName.c_str();
			info.m_strOrgID   =(_T(""));//strOrgID.c_str();
			info.m_strRoomID   = strRoomID.c_str();
			info.m_strLastMsg =(_T(""));
			info.m_strLastTm   = GetFullTimeString(time(0)).c_str();
			info.nItemID	   = GetNextItemID();

     		m_lstFriends.insert(m_lstFriends.begin(), info);

			RecentGroupItem oneGroup;
			oneGroup.strRoomID = strRoomID;
			oneGroup.strGroupNick = strRoomNickName;
			oneGroup.strOrgID=(_T(""));//strOrgID;
			oneGroup.strOrgName=(_T(""));//strOrgName;
			oneGroup.strParticipant = CController::Instance()->GetXmppClient()->jid().username();
			oneGroup.strAvatarPath = info.m_strLogo;
			GetDBTaskModule()->SaveRecentGroupItem(oneGroup);

			//3. 增加到最近联系人的顶部.
			m_pRecentList->AddItem(&info, 0);
			//RefreshContactListControl();

			//4.给群聊天界面的文件包进行初始化
			UpdatePackListInGroupChat(oneGroup.strRoomID);
		}		
	}
}

void mainDlg::OnShowGroupMemberWnd()
{
	tstring strCurRoomID = m_CurItem.m_strRoomID.GetData();
	map<tstring, CGroupMemberView*>::iterator itFind = m_mapRoom2Memberview.find(strCurRoomID);
	if (itFind != m_mapRoom2Memberview.end())
	{
		//更新最新的成员列表。
		itFind->second->Initialize(strCurRoomID);
		itFind->second->ShowWindow();
	}
	else
	{
		CGroupMemberView* pGroupMemView = new CGroupMemberView(&m_PaintManager);
		if (NULL != pGroupMemView)
		{
			pGroupMemView->SetMainWndHwnd(m_hWnd);
			pGroupMemView->CreateWnd();
			pGroupMemView->Initialize(strCurRoomID);
			
			pGroupMemView->ShowWindow(true);

			m_mapRoom2Memberview.insert(make_pair(strCurRoomID, pGroupMemView));
		}
	}
}

bool mainDlg::GetCurrGroupRoomID(tstring& strRoomID)
{
	if (!m_CurItem.m_strRoomID.IsEmpty())
	{
		strRoomID = m_CurItem.m_strRoomID.GetData();
		return true;
	}

	return false;
}

bool mainDlg::GetCurrGroupOrgID(tstring& strOrgID)
{
	if (!m_CurItem.m_strOrgID.IsEmpty())
	{
		strOrgID = m_CurItem.m_strOrgID.GetData();
		return true;
	}

	return false;
}

bool mainDlg::GetFriendItem(tstring& strAccount, sFriendListItem& OneFriend)
{
	//如果是自己
	if (_tcsicmp(m_MyselfItem.m_strAccount.GetData(), strAccount.c_str()) == 0)
	{
		OneFriend.eState = m_MyselfItem.m_State;
		OneFriend.nItemID = m_MyselfItem.nItemID;
		OneFriend.strAccount = strAccount;
		OneFriend.strAvatar = m_MyselfItem.m_strLogo;
		OneFriend.strOrgID = m_MyselfItem.m_strOrgID;
		OneFriend.strOrgnization = m_MyselfItem.m_strOrgName;
		OneFriend.strShowName = m_MyselfItem.m_strNickName;
		return true;
	}
	//如果是自己的好友
	else
	{
		for (int i=0; i<m_listAllFriends.size(); i++)
		{
			sFriendListItem& oneItem = m_listAllFriends[i];
			if (_tcsicmp(oneItem.strAccount.c_str(), strAccount.c_str()) == 0)
			{
				OneFriend = oneItem;
				return true;
			}
		}
	}
	

	return false;
}

void mainDlg::OnEnterRoomFailed(tstring& strErrorDes)
{

}

void mainDlg::OnEnterRoomSucess()
{
	
}

tstring mainDlg::GetMyAvatar()
{
	return tstring(m_MyselfItem.m_strLogo.GetData());
}

void mainDlg::HandleOrgInfo()
{
	CFrameEmployee::Instance()->ShowWindow(true);
}

void mainDlg::setOptionRecentChatListUnread( bool bUnread)
{
	COptionUI* pOption = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat));
	if (pOption == NULL)
		return;
	if (bUnread || m_bSysUnread)
		pOption->SetForeImage("main_dialog_sidebar_recent_chatlist_newmessage.png");
	else
		pOption->SetForeImage("main_dialog_sidebar_recent_chatlist.png");
	
}

void mainDlg::ComposeGroupAvatar( ContactListItemInfo& info )
{
	EnterCriticalSection(&compose_vatar_cs_);
	vector<tstring> member_list;
	vector<sFriendListItem> member_detail;

	tstring group_id = info.m_strRoomID;
	tstring out_path;

	if(CController::Instance()->GetRoomManager()->GetRoomMemberList(group_id, member_list))
//	if(CController::Instance()->GetRoomManager()->GetRoomMemberListFromRoot(group_id, member_list))
	{
//		OutputDebugString("进ComposeGroupAvatar\r\n");
//		Sleep(10);
		for (int i=0; i<member_list.size(); i++)
		{
			sFriendListItem oneFriend;
			//先从好友里面去找。
			if (mainDlg::getInstance()->GetFriendItem(member_list[i], oneFriend))
			{

			}
			else
			{
				//从陌生人里面去找。
				sUserVCard oneVCard;
				CController::Instance()->GetUserInfoMgr()->GetUserVCard(member_list[i], oneVCard);
				oneFriend.strAccount = oneVCard.strAccount;
				oneFriend.strAvatar = oneVCard.strPhotoPath.empty()? DEF_AVATAR_NOR : oneVCard.strPhotoPath;
				oneFriend.strOrgID = oneVCard.strOrganization;
				oneFriend.strOrgnization = oneVCard.strOrganization;
				oneFriend.strShowName = oneVCard.strUserNickname;
			}

			//这里不设计到duty。
			// 				vector<tstring> vecProperty;
			// 				if(GetDuty(oneFriend.strOrgID, oneFriend.strAccount, vecProperty))
			// 				{
			// 
			// 				}

			member_detail.push_back(oneFriend);
		}

		//混合所有图片,注意这里取到的logo是有在线和离线效果的。我们需要将离线图片替换成在线的。
		tstring app_path = CFileHelper::GetCurrentAppPath();
		tstring avatar_path = CFileHelper::GetHeadCacheDir();

		//只要路径不改，头像就无法刷新，因此群组头像路径现在与成员个数相关联
		std::ostringstream ostr;
		ostr << avatar_path << group_id << "-" << member_detail.size() << ".png";
		avatar_path = ostr.str();

		int pos = app_path.find("bin");
		if (pos < 0)
		{
			MessageBox(NULL, _T("路径不包含bin，程序寻址有错！"), _T("提示"), MB_OK);
		}
		tstring path = app_path.substr(0, pos);

		vector<tstring> vec_avatar;
		for (int i=0;i!=member_detail.size();i++)
		{
			tstring avatar(member_detail[i].strAvatar);
			//注意这里default全部拼写错误，一直错下去吧 by zyy
			if (avatar == "defalut_avatar_gray.png" || avatar == "defalut_avatar_nor.png")
			{
				avatar = path + "MM\\res\\defalut_avatar_nor.png";
			}
			else
			{
				//注意这里图片的格式 18133619498@test-d_Gray.png，18133619498@test-d.png
				tstring feature = "@test-d_Gray.png";
				int pos = avatar.find("@test-d_Gray.png");
				if (pos > 0)
				{
					avatar.replace(pos, pos+feature.length(), "@test-d.png");
				}

				//如果还没有下载下来
				if (!CFileHelper::IsExistFile(avatar.c_str()))
				{
					avatar = path + "MM\\res\\defalut_avatar_nor.png";
				}
			}
			vec_avatar.push_back(avatar);
		}

		CMagicKHelper::Instance()->Composite(vec_avatar, avatar_path);

		info.m_strLogo = avatar_path.c_str();
	}	
	LeaveCriticalSection(&compose_vatar_cs_);
}

void mainDlg::UpdateTopPosition(vector<CLabelUI*> vect_label)
{
	//位置移动。。。。。。。。。。。。by zyy
	//duilib无法获取控件位置，可以设置控件位置，因此，这里的位置是从xml里面得到
	//得到名字占用的位置
	RECT rect_origin, rect_total;
	rect_origin.left = 260;
	rect_origin.right = 380;
	rect_origin.top = 43;
	rect_origin.bottom = 75;

	rect_total.left = 260;
	rect_total.right = 560;
	rect_total.top = 43;
	rect_total.bottom = 75;

	//得到名字字符串的长度
	HDC hdc = m_PaintManager.GetPaintDC();
	CLabelUI *pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName));
	SIZE sz = CRenderEngine::GetTextSize(hdc, &m_PaintManager, pFriendNickname->GetText(), -1,0);
	sz.cx += 12;

	//	nick_name->SetPos(rect_origin);
	MoveControllers(pFriendNickname, rect_origin, sz, vect_label, rect_total);
}

void mainDlg::ReConnect()
{
	//remove icon in taskbar.	
// 	FreeUCSNetServiceCallBackInterface();
// 	GetMsgCenterModule()->Release();
// 	GetTrayTaskModule()->Release();

// 	m_pGroupChatPage = new CGroupChatPage(&m_PaintManager);
// 	m_pGroupChatPage->SetMainHwnd(this->GetHWND());

 	CController::Instance()->GetRoomManager()->RemoveHandler(this);
//	CController::DestroyInstance();
// 	m_pUsrMgrAdapter = CController::Instance()->GetUserInfoMgr();
// 	m_pUsrMgrAdapter->RemoveEventHandler(this); 
// 	m_pUsrMgrAdapter->AddEventHandler(this);

	//数据库重新连接
	GetDBTaskModule()->CloseDB();
	GetDBTaskModule()->Initialize();

 	m_pMsgSessionMgr = NULL;
 	CMsgSessionManager* pSessionMgr = CController::Instance()->GetMsgSessionMgr();
 	if (NULL != pSessionMgr)
 	{
 		m_pMsgSessionMgr = pSessionMgr;
 	}
 
 	//注册接收聊天消息。
  	GetMsgCenterModule()->AddEventHandler(this);
  	GetTrayTaskModule()->AddEventHandler((ITrayTaskModuleEvent*)this);
  	m_pInstance = this;
  
  	CController::Instance()->GetRoomManager()->AddHandler(this);
	m_pGroupChatPage->GetGroupChatLogic()->RegisterToRoom();

//	CController::Instance()->GetRoomManager()->AddHandler(dynamic_cast<IRoomManagerEvent*>(m_pGroupChatPage));
//   	CWebClient::api_getRegions();

	//重新连接上之，设置判断值
	
	CController::Instance()->GetRoomManager()->GetMyRoomList();

//	m_pGroupChatPage->Initialize()
// 	if (NULL != m_pGroupChatPage)
// 	{
// 		delete m_pGroupChatPage;
// 		m_pGroupChatPage = NULL;
// 	}
	

	vector<RecentGroupItem> vecGroupList;
	if(GetDBTaskModule()->LoadRecentGroupItem(vecGroupList))
	{
		int nSize = vecGroupList.size();
		if (nSize>0)
		{
			for (int i=0; i<nSize; i++)
			{
				RecentGroupItem& oneGroup = vecGroupList[i];

				//Join room.
				tstring strUser = CController::Instance()->GetXmppClient()->jid().username();
				CController::Instance()->GetRoomManager()->EnterRoom(oneGroup.strRoomID, strUser);				
			}
		}

	}	
//	CacheRecentGroupChat();
	SetConnection();
}

void mainDlg::playMP3( tstring strPath )
{
	//char str[128]={0};
	//int i = 0;
	//char buf[128]={0};

	//MCI_OPEN_PARMS mciOpen;
	//MCIERROR mciError;
	//mciOpen.lpstrDeviceType = "mpegvideo";
	//mciOpen.lpstrElementName = "c://1.mp3";

	//mciError = mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,(DWORD)&mciOpen);
	//if(mciError)
	//{
	//	mciGetErrorString(mciError,buf,128);
	//	printf("%s/n",buf);
	//	goto Error;
	//}
	//UINT DeviceID = mciOpen.wDeviceID ;
	//MCI_PLAY_PARMS mciPlay;

	//mciError = mciSendCommand(DeviceID,MCI_PLAY,0 ,(DWORD)&mciPlay);
	//if(mciError)
	//{
	//	printf("send MCI_PLAY command failed/n");
	//	goto Error;
	//}

	////WinExec("sndvol32.exe",SW_SHOWNORMAL);
	////这个可以打开音量控制不过可以用编程实现。

	char str[128]={0};
	int i = 0;
	char buf[128]={0};

	MCI_OPEN_PARMS mciOpen;
	MCIERROR mciError;
	mciOpen.lpstrDeviceType = "mpegvideo";
	mciOpen.lpstrElementName = "c://1.mp3";

	mciError = mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,(DWORD)&mciOpen);
	if(mciError)
	{
		mciGetErrorString(mciError,buf,128);
		printf("%s/n",buf);
		//goto Error;
	}
	UINT DeviceID = mciOpen.wDeviceID ;
	MCI_PLAY_PARMS mciPlay;

	mciError = mciSendCommand(DeviceID,MCI_PLAY,0 ,(DWORD)&mciPlay);
	if(mciError)
	{
		printf("send MCI_PLAY command failed/n");
		//goto Error;
	}

	//WinExec("sndvol32.exe",SW_SHOWNORMAL);
	//这个可以打开音量控制不过可以用编程实现。

	while(1)
	{
		sprintf(str,"播放时间:/t%d",i);
		printf("%s/r",str);
		i++;
		Sleep(1000);
	}
}

void mainDlg::LogOff()
{
	CMsgBox* pMsgBox = new CMsgBox("","确定注销当前账号？",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
	if (MSGBOX_IDNO == pMsgBox->ShowModal(NULL))
	{			
		return;
	}		
	char szModule[MAX_PATH];
	memset(szModule, 0, MAX_PATH);
	::GetModuleFileName(NULL, szModule, MAX_PATH);
	ShellExecute(NULL, NULL,  szModule, NULL, NULL, SW_SHOWNORMAL);
	GetTrayTaskModule()->Release();
	exit(0);
}

void mainDlg::GetUidAndKey(tstring& key, tstring& uid)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += "user/get-key";

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	tstring strUid, strKey;
	if (strHttpResponse.find("\"result\":1") != -1 && strHttpResponse.find("key") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strHttpResponse, root))
			{
				key = root["key"].asString();
				uid = root["uid"].asString();
			}
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strHttpResponse.c_str());	
		}	
	}
	else
	{
		//北京调试注销
		//				return;
	}
}



void mainDlg::initOrgProperty()
{
	
}
