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

const TCHAR* const kListContactsControlName			= _T("listRecentChat");			//�����ϵ���б�List
const TCHAR* const kListContactsFriendList			= _T("allFriendList");			//�ҵĺ����б�List			
const TCHAR* const kcomboTimeControlName			= _T("combTime");
const TCHAR* const kcomboTypeControlName			= _T("combType");
const TCHAR* const kcomboGroupControlName			= _T("combGroup");
const TCHAR* const ktreeDataListControlName			= _T("treeDataList");
//const TCHAR* const kbuttonContactNameControlName	= _T("buttonContactName");
const TCHAR* const kbuttonSendButtonControlName		= _T("buttonSend");				//������Ϣ��ť
const TCHAR* const krichEditInputEditControlName	= _T("inputEdit_maindlg");
const TCHAR* const krichEditOutputEditControlName	= _T("outputEdit");
const TCHAR* const kbuttonMinControlName			= _T("ButtonMinize");			//��������С����ť
const TCHAR* const kbuttonMaxControlName			= _T("ButtonMaxmize");			//��������󻯰�ť
const TCHAR* const kbuttonCloseControlName			= _T("ButtonClose");			//������رհ�ť
const TCHAR* const kListChatControlName				= _T("listChatControl");		//�����¼�б�List
//const TCHAR* const koptionRosterControlName			= _T("optionRoster");
//const TCHAR* const koptionFriendsterControlName		= _T("optionFriendster");
//const TCHAR* const kTabRecentChat					= _T("tabRecentChat");			//�����ϵ��Tabҳǩ
const TCHAR* const kTabChatAndFileList				= _T("tabChatAndFileList");		//�������ļ��б�Tabҳǩ
const TCHAR* const kbuttonAddRosterControlName		= _T("btnAddBPartner");			//"�����ҵ���"
const TCHAR* const kbuttonFileControlName			= _T("buttonFile");
const TCHAR* const kBtnDialPhoneName			= _T("ButtonDialPhone");
const TCHAR* const kHLayoutNetCallName			= _T("HLayoutNetCall");
const TCHAR* const kHLayoutContactInfo			= _T("VLayoutContactInfo");
const TCHAR* const kBtnCallTypeDropName			= _T("btnCallTypeDrop");
const TCHAR* const kBtnHomePage			= _T("btnHomePage");

const TCHAR* const kTabLayoutFileSend               = _T("TabFileSend_roster");               //������Ҳ����"�����ļ�"ҳǩ �����������Ӧÿ�������ϵ�ˣ�
//tabFileSend�Ŀؼ�
const TCHAR* const kbuttonTabFile                   = _T("btn_tab_file");             //ѡ�񱾵��ļ���ť
const TCHAR* const kTileFilesforSend                = _T("tile_FilesforSend");        //չʾ�������ļ�TileLayout
const TCHAR* const kbuttonFileSend                  = _T("buttonFileSend");           //�����ļ���ť

const TCHAR* const kTabLayoutSidebar				= _T("SidebarTabContainer");		//������Ĳ����Layout�ؼ���Ŀǰ�����ˣ�����Ϣ��+����Ŀ��+����ҵ��顱...��6��Tabҳǩ
const TCHAR* const kBussinesPartnerTabLayout		= _T("BussinessPartnerTabLayout");	//����ҵ��顱ҳǩ�ڲ���TabLayout������Ŀǰ�����ˣ��Ӻ���ҳ�棬 �̼���Ϣҳ��
const TCHAR* const kOptionRecentChat				= _T("TabRecentChatList");			//���������ϵ�˵�ҳǩ��Option��ť	_T("optionChat");
const TCHAR* const kOptionProject					= _T("TabProject");					//������Ŀ��ҳǩ��Option��ť
const TCHAR* const kOptionBussinessPartner			= _T("TabBusinessPartner");			//������ҵ���ҳǩ��Option��ť
const TCHAR* const kOptionMoney						= _T("TabMoney");					//����Ǯ��ҳǩ��Option��ť
const TCHAR* const kOptionExecute					= _T("TabExecute");					//����ִ�е�Option��ť
const TCHAR* const kOptionFaceDeal					= _T("TabFaceDeal");				//�������潻�׵�Option��ť
const TCHAR* const kOptionFinance					= _T("TabFinance");					//���������Option��ť
const TCHAR* const kOptionSysSetting				= _T("TabSysSetting");				//����ϵͳ����ҳǩ��Option��ť
const TCHAR* const kOptionCollection				= _T("TabCollection");				//�����ղ�ҳǩ��Option��ť
const TCHAR* const kButtonMySelfAvatar				= _T("ButtonMySelfAvatar");			//�Լ���ͷ��Avatar
const TCHAR* const kLabelMySelfNickname				= _T("LabelMySelfNickName");		//�Լ����ǳ�
const TCHAR* const kLabelMySelfCompany				= _T("LabelMySelfCompany");			//�Լ��Ļ�����

const TCHAR* const kButtonShowGroupMember			= _T("btn_show_groupMember");		//��ʾȺ����Ϣ
const TCHAR* const kLabelFriendNickName				= _T("LabelFriendNickname");		//����(��ҵ���)���ǳ�
const TCHAR* const kButtonFriendAvatar				= _T("ButtonFriendAvatar");			//����(��ҵ���)��ͷ��Avatar
const TCHAR* const kLabelFriendCompany				= _T("LabelFriendCompany");			//����(��ҵ���)�Ļ�����
const TCHAR* const kLabelGroupMemCount				= _T("lable_groupmem_count");		//Ⱥ�������

const TCHAR* const kRecentChatLayout				= _T("RecentChatTabLayout");		//�����ϵ���ڲ���Tab�����������¼tab��ϵͳ��Ϣtab��Ⱥ����Ϣtab...
const TCHAR* const kSysMsgListCtrl					= _T("invitefriendListctrl");		//ϵͳ��Ϣҳ���Listctrl�ؼ�

const TCHAR* const kButtonCreateNewProject			= _T("ButtonCreateNewProject");		//�½���Ŀ

jsValue JS_CALL js_msgBox(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
	const wchar_t* title = jsToStringW(es, jsArg(es, 1));

	HWND hwnd = mainDlg::getInstance()->GetHWND();
	if (::IsWindow(hwnd) )
	{
//		::PostMessage(hwnd, WM_TIMER_CHATLISTENDDOWN, (WPARAM)text, (LPARAM)title);
		CMsgBox* pMsgBox = new CMsgBox("����","���س����ȡ��ҳ���ȡ����ť��Ӧ���رձ�ҳ��");
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

	CMsgBox* pMsgBox = new CMsgBox("����", nowchar);
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

	CMsgBox* pMsgBox = new CMsgBox("����", nowchar);
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

	CMsgBox* pMsgBox = new CMsgBox("����", nowchar);
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

	//ע�����������Ϣ��
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

				//room��Ա���server�˻�ȡ��������ﲻ�ܰѳ�Ա�����ж���Ϊ�ܲ�����ӵı�־���������һ������ʱ�����ػ�ȡ��list
				//����ʱroom��Ա��û���õ�
				//���listֱ��add������Աͼ����Ҫ��ó�Ա����ܺϲ�
				m_pRecentList->AddItem(&info);		
				
			}
			m_pRecentList->SelectItem(0);			
		}
	}
}

void mainDlg::InitialFriendList()
{	
	sort(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper::CompareFunc);

	//1���Ȼ�ȡ��������
	//���ҵĺ��ѡ�����û����Ա������£���֤����������ȷ��
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

	//������Ϣ�Ĳ�ͬ��������ط���m_pFriend����û�г�ʼ����
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
	//��ʼ����������Ϣ
	oneContactItem.nItemID = GetNextItemID();
	oneContactItem.m_strAccount = strAccount.c_str();

	//��ѯ���һ�ε��������ݺ�ʱ��
	_tChatMessage oneChatMsg;
	if (GetDBTaskModule()->LoadLastMessage(strAccount, oneChatMsg))
	{
		//tstring strOut;
		//CChineseCode::UTF_8ToGB2312(strOut, (char*)oneChatMsg.strChatText.c_str(), oneChatMsg.strChatText.size());
		//oneContactItem.m_strLastMsg = strOut.c_str();
		
		oneContactItem.m_strLastMsg = oneChatMsg.strChatText.c_str();
		oneContactItem.m_strLastTm  = oneChatMsg.strTimeStrFormat.c_str();
	}

	//��ȡ�û���״̬�����ʱ���õĵ��𣿣�
	EUsrState eState = m_pUsrMgrAdapter->GetState(strAccount);
	oneContactItem.m_State = eState;

	oneContactItem.m_strLogo = (CFileHelper::GetHigtLightAvatar(strAccount)).c_str();
	

	//�����޸ģ�ȡ�����û��ı�ע��
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
			//���������
			oneContactItem.m_strNickName = oneVCard.strUserNickname.c_str();
		}
	}

	oneContactItem.m_strOrgName = m_pUsrMgrAdapter->GetOrgName(strAccount).c_str();
}

void mainDlg::CacheRecentChatList()
{
	//1.to do...�ӱ��ص�DB��ȡ�����ϵ����Ϣ��
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
				//�������ݿ�ɾ��������ʧ�����Լ��˸��жϡ�
				if (m_pUsrMgrAdapter->IsMyFriend(strAccount))
				{
					GetContackItemByAccount(strAccount, oneContact);
					m_lstFriends.push_back(oneContact);
				}
				//�����ϵͳ��Ϣ��ǩ
				else if (_tcsicmp(strAccount.c_str(), SYS_NOTIFY_ACCOUNT) == 0)
				{
					if (NULL != m_pRecentList)
					{
						m_pRecentList->RemoveItem(10000);
					}
					ContactListItemInfo iteminfo;
					iteminfo.nItemID = GetNextItemID();
					iteminfo.m_strAccount = SYS_NOTIFY_ACCOUNT;
					iteminfo.m_strNickName = "ϵͳ��Ϣ";
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
	//1.to do...�ӱ��ص�DB��ȡ�����ϵ����Ϣ��
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
				
				//���������Ҫ��������by zyy ����֤
				//�����·���ֱ�Ӳ���m_lstFriends

				//����Է��Ǹ���
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
				
				//��ʼ��Ⱥ������Ϣ��¼
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
	
	//1.cache�����б����Ϣ��
	for (MapAllRosters::iterator iter = listUserInfos.begin(); iter !=listUserInfos.end(); iter++)
	{
		sFriendListItem oneFriend;
		oneFriend.nItemID	 = GetNextItemID();
		oneFriend.strAccount = iter->second.strAccount;
		oneFriend.strShowName = iter->second.strMemo;//��ע��
		oneFriend.strOrgnization = iter->second.strOrgnization;
		oneFriend.strOrgID = iter->second.strOrgID;


		//�ӱ������ݿ�ȡͷ����ǳƣ����û�У�����ȡvcard
		sUserVCard oneVcard;
		if (GetDBTaskModule()->LoadUserVCard(oneFriend.strAccount, oneVcard))
		{
			//��ע�ǿյľ���ʾ�ǳ�
			if (oneFriend.strShowName.empty())
			{
				oneFriend.strShowName = oneVcard.strUserNickname;
			}
			//�ǳ��ǿյģ�����ʾ�ֻ��˺�
			if (oneFriend.strShowName.empty())
			{
				oneFriend.strShowName = oneVcard.strPhone;
			}

			//Ĭ���������ߵ�ͷ��
			if (!oneVcard.strGrayAvatar.empty())
			{
				oneFriend.strAvatar = oneVcard.strGrayAvatar;
			}
			else
			{
				oneFriend.strAvatar = DEF_AVATAR_GRAY;
			}
		}
		//û��VCard
		else
		{
			//���û�б�ע����ֱ����ʾ�ֻ���
			if (oneFriend.strShowName.empty())
			{
				oneFriend.strShowName = CFileHelper::GetUserName(oneFriend.strAccount);
			}

			oneFriend.strAvatar = DEF_AVATAR_GRAY;
			
			//��ȡVCard��
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

//lee��������������ع���һ��CRecentTabPage:public CHorizontalLayout����ȥ
//���ڸ����������InitInSetTabLayoutInRecentPage�ĳ�ʼ������
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
	
	//׷�ӵ�TabLayout Container�����ڣ�����ֵ+1
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

	//1.���ظ��������Page��pLayout�� index=0
	CChatAndFilePage::Instance()->InitialChatPage(&m_PaintManager);

	//2.����ϵͳ��Ϣ��Page��pLayout, index=1
	if (NULL == m_pSysMsgPage)
	{
		m_pSysMsgPage = new CSysMsgPage(&m_PaintManager);
	}

	//3.��ʼ��Homepage
	InitialHomePage();

	//4.����Ⱥ�����Page��pLayout,  index=2
	if (NULL == m_pGroupChatPage)
	{
		m_pGroupChatPage = new CGroupChatPage(&m_PaintManager);
		m_pGroupChatPage->SetMainHwnd(this->GetHWND());
	}

	//���ǰҳ�������¼��ҳ��
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

	//׷�ӵ�TabLayout Container�����ڣ�����ֵ+1
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
		CMsgBox* pMsgBox = new CMsgBox("��ʾ","��ѡ����ļ�����10������ɾ�������ļ����ϴ���", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	else
	{
		for (int i=0; i<lstfiles.size(); i++)
		{
			tstring filePath = lstfiles[i];
			tstring strValue = filePath.c_str();
			//��ȡ�ļ���
			//1. ȥ���ļ���
			int iPos=strValue.find_last_of("\\");
			strValue = strValue.substr(iPos + 1); //strValue.Mid(i, strValue.GetLength());
			CDuiString strFileFullName = strValue.c_str();
			//2. ȥ���ļ����ͺ�׺
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
			//3. ��ȡ�ļ���С
			infoValue.dwFileSize = getFileSize(infoValue.strFilePath);
			if (infoValue.dwFileSize <= 0)
			{
				CMsgBox* pMsgBox = new CMsgBox("����","���ļ��޷����ͣ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return;
			}
				
			if (m_mapTaskFiles.find(infoValue.strKey) != m_mapTaskFiles.end())   //�Ѵ��ڣ����ظ��ϴ�
				return;
			m_lstUploadFiles.push_back(infoValue);
		}	
	}
}

void mainDlg::refreshFilesDownloadProcess(tstring & strPack, tstring& strFrom)
{
	//���ﲻ֪��Ϊɶjosn���ܽ���
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


	//��������·��
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
			//�Ѵ��ڣ�������������
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
			//�½�һ��
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

			//׷�ӵ�TabLayout Container�����ڣ�����ֵ+1
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
	//1. ����icon
	//todo
	CControlUI *pCtrlLogo = static_cast<CControlUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("controlFileLogo")));
	pCtrlLogo->SetBkImage(GetPackFileIcon(downLoadInfo.strFileType).c_str());
	//2. ��������
	CLabelUI *plableFileName = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileName")));
	
	//3. �����ļ���С
	double sz = double(downLoadInfo.dwFileSize) / 1024;
	//sz = sz/1024;
	CDuiString strsz;
	strsz.Format("%.2f", sz);
	strsz += _T("KB");
	tstring strValue = downLoadInfo.strFileName + "(" + tstring(strsz)+")";
	plableFileName->SetText(strValue.c_str());
	//4. ����map����
	m_mapTaskFiles.insert(make_pair(downLoadInfo.strKey, downLoadInfo));
	int nSize = m_mapTaskFiles.size();

	//5. ui��ʾ
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
		//1. ����icon
		CControlUI *pCtrlLogo = static_cast<CControlUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("controlFileLogo")));
		pCtrlLogo->SetBkImage(GetPackFileIcon(lstUploadFiles[i].strFileType).c_str());
			//todo
		//2. ��������
		CLabelUI *plableFileName = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileName")));
		
		//3. �����ļ���С
		//CLabelUI *plableFileSize = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileSize")));
		double sz = double(lstUploadFiles[i].dwFileSize) / 1024;
		//sz = sz/1024;
		CDuiString strsz;
		strsz.Format("%.2f", sz);
		strsz += _T("KB");
		tstring strValue = lstUploadFiles[i].strFileName + "(" + tstring(strsz)+")";
		plableFileName->SetText(strValue.c_str());
		//plableFileSize->SetText(strsz);
		//4. ����map����
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

	//�����Լ����˻���
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

	 jsBindFunction("call", JsCall, 1);//�����js��������js��������c++����
	 jsBindFunction("chat", JsChat, 1);//�����js��������js��������c++����
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
//	datainfo.m_strType = "ѯ����";
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
//		datainfo.m_strType = "������ѯ";
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
			//�����ǰ�ĵ�-���������
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
			//�����ǰ��Ⱥ�������
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
			
			//GEt my room list������������roomlist,Ӧ�𲢲�������
			CController::Instance()->GetRoomManager()->GetMyRoomList();

			//�õ�room list���Ǽ�ʱ��Ч�ģ�����Ҫ����һ����ʱ��
			//			SetTimer(this->GetHWND(), WM_TIMER_UPDATE_ROOM_LIST, 100, NULL);

			//			RefreshContactListControl();//����ˢ��һ�飬ȥ����ǰ�û����ڵ�Ⱥ��by zyy

			//1.cache
			CacheFriendList();
			//2.refresh ui
			InitialFriendList();

			//do more thing..
			if (m_pGroupChatPage != NULL)
			{
				m_pGroupChatPage->InitMyDutys();
			}

			//�������ݿ��ȡ�����ϵ��
			CacheRecentChatList();
			//�����������������¼�Ȳ�ѯcache
			InitChatMsgFromDB();

			//���ر���ϵͳ��Ϣ
			InitLocalSysMsg();

			//���ر���Ⱥ���б�
			CacheRecentGroupChat();

			///ˢ�������ϵ���б�-�����������һ����
			RefreshContactListControl();			 
		}
		break;

		//�ٸ���Ⱥ���Ƶ�ͬʱ����Ҫ����Ⱥͷ��
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

			//2.�������Ķ����û��ǳƺ���ʾ����״̬
			UpdateUserInfoAtWndTop(strAccount, eUsrState, tstring(_T("")));

			//������������б�
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
				//������������͵���Ϣ���Ͳ���ʾ
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
	case WM_GROUPDOWNLOAD_PACKFILE_DOWNLOADED: //������Ⱥ���ϰ�
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
	//�Ȼ�ȡ��Ǯ��memo
	//�ȴ�cache�п��Ƿ����
	bool bHaveCard = false;
	sUserVCard oneCard;
	if (NULL != m_pUsrMgrAdapter)
	{
		bHaveCard = m_pUsrMgrAdapter->GetUserVCard(strAccount, oneCard);
	}
	//�����ھʹӱ���DBȥ��
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
	//������޸���,���µ�Server
	m_pUsrMgrAdapter->UpdateMemo(strAccount, strNewMemo);

	//ͬ�����½��棺�����б�+�����ϵ��+������Ϣҳ��
	UpdateMemoInFriendAndChatList(strAccount, strNewMemo);

	//���±������ݿ⣬һ�������ݿ⣬������Ǵ���ġ�
	sUserVCard oneCard;
	bool bInDB = GetDBTaskModule()->LoadUserVCard(strAccount, oneCard);
	if (bInDB)
	{
		oneCard.strRemark = strNewMemo;
		GetDBTaskModule()->SaveUserVCard(strAccount, oneCard);
	}

	//�Ż���������޸ı�ע֮����������λ��û�б仯����Ҫupdate UI
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
	//���º�����Ϣҳ��
	if (NULL != m_pPartnerInfopage)
	{
		m_pPartnerInfopage->UpdataMemo(strNewMemo);
	}

	//���ں����б�������ϵ���б����memoΪ�յģ�����ʾ�ǳƣ���֮��ʾ�ֻ���
	tstring strShowName;
	if (strNewMemo.empty())
	{
		//�ȴ�cache�п��Ƿ����
		bool bHaveCard = false;
		sUserVCard oneCard;
		bHaveCard = m_pUsrMgrAdapter->GetUserVCard(strAccount, oneCard);
		
		//�����ھʹӱ���DBȥ��
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


	//���º����б�cache+ui
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

	

	//���������ϵ��cache+ui
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

	//1.�л�"��Ϣ"ҳ��
	static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat))->Selected(true);

	//2.�ж���ϵ��ҳ���Ƿ������Ⱥ�ˣ�������ö������û�����
	bool bGroupInRecentList = false;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); iter!=m_lstFriends.end(); iter++)
		{
			//�����˺ţ��ң���Room��Ϣ
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

	//���½���Ⱥ����뵽�������ϵ���б����ݿ�		
	if (!bGroupInRecentList)
	{
		//������Ϣ
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

		//���½���Ⱥ����뵽�������ϵ���б����ݿ�		
		ContactListItemInfo info;
		info.m_strAccount = strAccount.c_str();
		info.m_strLogo = strAvatarPath.c_str();
		info.m_strNickName = strNickName.c_str();
		info.m_strOrgName  = strNickName.c_str();//û�л����������ǳ�
		info.m_strOrgID   =(_T(""));
		info.m_strRoomID   = strRoomId.c_str();
		info.m_strLastMsg =(_T(""));
		info.m_strLastTm   = GetFullTimeString(time(0)).c_str();//"2016-04-05 16:27:23";
		info.nItemID	   = GetNextItemID();

		//ע��Ϊʲô����Ҫ���и�ֵ������Ϊ����ĺ����� OnNewGroupMemberList �ص��Ⱥ�˳���������
		ComposeGroupAvatar(info);
		m_lstFriends.insert(m_lstFriends.begin(), info);		

		RecentGroupItem oneGroup;
		oneGroup.strRoomID = strRoomId;
		oneGroup.strGroupNick=(_T(""));//OnRoomnameArrived�ص������
		oneGroup.strOrgID=(_T(""));
		oneGroup.strOrgName = strNickName.c_str();
		oneGroup.strParticipant = strCellPhone;//�Է����˺š�

		GetDBTaskModule()->SaveRecentGroupItem(oneGroup);
	}

	//3. ˢ�½���
	RefreshContactListControl();

	//4.��Ⱥ���������ļ������г�ʼ��
	UpdatePackListInGroupChat(strRoomId);
}

void mainDlg::OnShowGroupChatWnd(tstring& strOrgID, tstring&strOrgName, tstring& strRoomID)
{
	//1.�л�"��Ϣ"ҳ��
	static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat))->Selected(true);

	//2.�ж���ϵ��ҳ���Ƿ������Ⱥ�ˣ�������ö������û�����
	bool bGroupInRecentList = false;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); 
			iter!=m_lstFriends.end(); iter++)
		{
			if ( SYS_GROUP_CHAT_ACCOUNT == tstring((*iter).m_strAccount))
			{
				//ͬһ���������п���ROOMID��ͬ��???����ʲô��˼������by zyy
				//by zyy ��Ϊroomid����Ψһ��
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

	//���½���Ⱥ����뵽�������ϵ���б����ݿ�	
	//Ⱥ��ͷ����Ⱥ����Ա�ϲ��ɺ������http��õ�ͷ���û���ˡ�
	if (!bGroupInRecentList)
	{
		//��ȡ������Ϣ
		tstring strOrgID;
		tstring strOrgName;
		tstring strAvatarBase64;

		//������Ϣ
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
			//����ǻ���
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
			//����Ǹ���
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
		
		//���½���Ⱥ����뵽�������ϵ���б����ݿ�		
		ContactListItemInfo info;
		info.m_strAccount = SYS_GROUP_CHAT_ACCOUNT;

		//ΪʲôҪ��Ⱥ���ƻص���ʱ��õ��أ�Ⱥ���Ʋ����Դ�������
		info.m_strNickName = strOrgName.c_str();//��ʼ��Ϊ����������Ⱥ���ƻص���ʱ�򣬸����������
		info.m_strOrgName  = strOrgName.c_str();
		info.m_strOrgID    = strOrgID.c_str();
		info.m_strRoomID   = strRoomID.c_str();
		info.m_strLastMsg =(_T(""));
		info.m_strLastTm   = GetFullTimeString(time(0)).c_str();//"2016-04-05 16:27:23";
		info.nItemID	   = GetNextItemID();

		//ע��Ϊʲô����Ҫ���и�ֵ������Ϊ����ĺ����� OnNewGroupMemberList �ص��Ⱥ�˳���������
		ComposeGroupAvatar(info);
		m_lstFriends.insert(m_lstFriends.begin(), info);

		RecentGroupItem oneGroup;
		oneGroup.strRoomID = strRoomID;
		oneGroup.strGroupNick = strOrgName;
		oneGroup.strOrgID = strOrgID;
		oneGroup.strOrgName = strOrgName;
		oneGroup.strParticipant = CController::Instance()->GetXmppClient()->jid().username();//�Լ���Ⱥ��
		oneGroup.strAvatarPath = info.m_strLogo;
		GetDBTaskModule()->SaveRecentGroupItem(oneGroup);
	}

	//3. ˢ�½���
	RefreshContactListControl();

	//4.��Ⱥ���������ļ������г�ʼ��
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

	//��ȡ��ǰ����ĿID
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
		CMsgBox* pMsgBox = new CMsgBox("����","��ȡ��ĿIDʧ�ܣ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	//��ʼ����room�µ����ϰ��б�
	if (NULL != m_pGroupChatPage)
	{
		m_pGroupChatPage->InitPackFiles(strProjectID, strRoomID);
	}
}


void mainDlg::OnShowChatWnd(tstring& strAccount)
{
	//1.�л���ҳ�浽��ϵ�˽���
	static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat))->Selected(true);

	//2.�ж���ϵ��ҳ���Ƿ��У� ������ö������û�����
	bool bInRecentList = false;
	if (m_lstFriends.size() >0)
	{
		for(vector<ContactListItemInfo>::iterator iter = m_lstFriends.begin(); 
			iter!=m_lstFriends.end(); iter++)
		{
			if ( strAccount == tstring((*iter).m_strAccount) 
				&& iter->m_strRoomID.IsEmpty()) //���������û�к�����Ⱥ���������Ŀ��
			{
				ContactListItemInfo info = *iter;
				m_lstFriends.erase(iter);
				m_lstFriends.insert(m_lstFriends.begin(), info);
				bInRecentList = true;
				break;
			}
		}
	}

	//���¼ӵ���ϵ�˼��뵽�������ϵ���б����ݿ�		
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

	//3. ˢ�½���
	RefreshContactListControl();
}

void mainDlg::OnResponseNewVCard(sUserVCard& oneInfo)
{
	//save to database.
	GetDBTaskModule()->SaveUserVCard(oneInfo.strAccount, oneInfo);

	//1.���������ϵ�ˣ�ͷ����ʾ��
	int nSize = m_lstFriends.size();
	if (nSize>0)
	{
		for (int i=0; i<nSize; i++)
		{
			//�ҵ������
			ContactListItemInfo& oneContact = m_lstFriends[i];
			if (_tcsicmp(oneContact.m_strAccount, oneInfo.strAccount.c_str()) == 0)
			{
				//1������ͷ��
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

				//2��������ʾ���ƣ���ע>�ǳ�>�ֻ���
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
	
	//2.���º����б�:ͷ����ʾ��
	int nFriendCount = m_listAllFriends.size();
	if (nFriendCount>0)
	{
		bool bUpdated = false;
		for (int i=0; i<nFriendCount; i++)
		{
			//�ҵ������
			sFriendListItem& oneFriend = m_listAllFriends[i];
			if (_tcsicmp(oneFriend.strAccount.c_str(), oneInfo.strAccount.c_str()) == 0)
			{
				//1������ͷ��
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

				//2:������ʾ��
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

void mainDlg::Notify(TNotifyUI& msg)   //�ؼ���Ϣ��Ӧ
{
	if (!isConnected())//�����ˣ�ֱ������
	{
		return;
	}

	if (_tcsicmp(msg.sType, _T("dbclick")) == 0)
	{
		//��֧��˫����ת��������Ϣ������click�¼���ͻ��
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

			//������Ҫ�ٴ��жϣ�ϵͳ��Ϣ����ӭҳ�涼���ܳ����Ҽ��˵�
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

						if (account != m_myselfInfo.strAccount)//���Ǳ���
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
// 					if (m_pFriendLst->CanExpand(pNode))//�ǻ���
// 					{
// 						CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_business.xml"), eUIMenuDeleteFriend));
// 						pMenu->Init(*this, pt);
// 						pMenu->ShowWindow(TRUE);
// 					}
// 					else
// 					{
// 						if (account != m_myselfInfo.strAccount)//���Ǳ���
// 						{
// 							CDuiMenu *pMenu(new CDuiMenu(_T("menu_in_friendlist.xml"), eUIMenuDeleteFriend));
// 							pMenu->Init(*this, pt);
// 							pMenu->ShowWindow(TRUE);
// 						}
// 					}
// 				}
// 				if (!m_pFriendLst->IsParentNode((Node*)m_pFriendLst->GetItemAt(m_pFriendLst->GetCurSel())->GetTag()))
// 				{
// 					//by zyy δ�������˽⣬��ʱ��������
// 					if (select_business_)
// 					{
// 						
// 					}
// 					else
// 					{
// 						//������Ҫ�ٴ��жϣ����˲��ܳ����Ҽ��˵�
// 						
// 
// 						//�����Լ�
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
			//�˳�Ⱥ�� by zyy
			//�˳�Ⱥ��������������ͨ��web ��������⵱ǰ�û��ǲ������room�Ĵ����ߣ�������Ӧ����ʾdialog;
			//Ȼ�����web api ����room ��Ա��������xmpp���˳�iq
			//
			tstring strCurRoomID = m_CurItem.m_strRoomID.GetData();

			//Ⱥ���˺���#define  SYS_GROUP_CHAT_ACCOUNT			"10001@test-d"				//Ⱥ�������ϵͳ�˺�
			//Ҳ����˵û������Ⱥ����
			tstring strCurRoomAccount = m_CurItem.m_strAccount.GetData();
			if (strCurRoomAccount == m_myselfInfo.strAccount)
			{

			}
			else
			{
				CMsgBox* msg_box = new CMsgBox("","ȷ��Ҫ�˳���Ⱥ��",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
				if (MSGBOX_IDYES == msg_box->ShowModal(NULL))
				{
					CMUCRoomManager* pRoomMgr = CController::Instance()->GetRoomManager();

					vector<tstring> vecNewMem;
					vecNewMem.push_back(m_myselfInfo.strAccount);			
					pRoomMgr->ExitFromRoom(strCurRoomID, vecNewMem);
					pRoomMgr->ExitFromRoom(strCurRoomID, vecNewMem);//ΪʲôҪɾ�����β�����Ч
					OnDeleteGroupFromMemberList(strCurRoomID);
					SetTimer(this->GetHWND(), WM_TIMER_DELETE_ROOM_LIST, 200, NULL);
				}	
			}			
		}
		//! �������
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
//��������ע��
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

			jsBindFunction("msgBox", js_msgBox, 2);//�����js��������js��������c++����

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
// 			CMsgBox* pMsgBox = new CMsgBox("�˳���¼","�Ƿ�ȷ���˳���¼",MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
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
				CMsgBox* pMsgBox = new CMsgBox("","�޷���������ͨ�ŷ����������ܽ�������ͨ��",  MSGBOX_ICON_ERROR);
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
				strMsg += "���Ѿ�ӵ�л���(";
				strMsg += org.strName;
				strMsg += ")��";
				CMsgBox::MsgBox(GetHWND(), "��������", strMsg, MSGBOX_ICON_WARNING);
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
			//��ʼ���ͣ����ط��ͺ�ȡ����ť
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
			//1. UI���
			CVerticalLayoutUI *pFileProcess = NULL;
			map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(tstring(m_CurItem.m_strAccount));
			if (it != m_mapFileTab.end())
			{
				CListContainerElementUI *pelement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());


				CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(it->second,"listfileProcess"));
				if (plstProcess!=NULL && pelement!=NULL)
					plstProcess->Remove(pelement);
				//2. map���������
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
			//��ʼ���ͣ����ط��ͺ�ȡ����ť
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
					sprintf(pTime,"%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds,sysTime.wDayOfWeek);

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

			//���;ܾ���ִ
			SYSTEMTIME sysTime;
			::GetLocalTime(&sysTime);
			char* pTime = new char[128];
			sprintf(pTime,"%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds,sysTime.wDayOfWeek);

			tstring sTime = pTime;
			tstring sFileName = iter->second.strFileFullName;
			tstring sFrom = CController::Instance()->GetXmppClient()->jid().username();

			char* pReply = new char[256];

			sprintf(pReply,"#COMMAND#type=filerejected,{\"filename\":\"%s\",\"from\":\"%s\",\"time\":\"%s\"}",sFileName.c_str(),sFrom.c_str(),sTime.c_str());

			tstring sReply =	pReply;
			m_pMsgSessionMgr->SendMsg(tstring(m_CurItem.m_strAccount.GetData()), sReply);

			
			//1. UI���
			CVerticalLayoutUI *pFileProcess = NULL;
			map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(tstring(m_CurItem.m_strAccount));
			if (it != m_mapFileTab.end())
			{
				CListContainerElementUI *pelement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());


				CListUI *plstProcess = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(it->second,"listfileProcess"));
				if (plstProcess!=NULL && pelement!=NULL)
					plstProcess->Remove(pelement);
				//2. map���������
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
				// ׼���ϴ��ļ�
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonFile")) == 0)
		{
			if (m_CurItem.nItemID == -1)
			{
				MessageBox(NULL, _T("����ѡ����ϵ�ˣ�"), _T("��ʾ"), MB_OK);
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
			//����������
			HMODULE hHandle = LoadLibrary("CameraDll.dll");
			if(NULL == hHandle)
			{
				MessageBox(NULL, _T("Error LoadLibrary <CameraDll.dll>��"), _T("��ʾ"), MB_OK);
				return;
			}
			typedef int (WINAPI *CAMERASUBAREA)(int);
			CAMERASUBAREA pCameraSubArea = (CAMERASUBAREA)GetProcAddress(hHandle,"CameraSubArea");
			if(NULL == pCameraSubArea)
			{
				MessageBox(NULL, _T("Error LoadLibrary <CameraDll.dll> on Func <CameraSubArea>��"), _T("��ʾ"), MB_OK);
				FreeLibrary(hHandle);
				return;
			}
			int nCapRes = pCameraSubArea(0);
			FreeLibrary(hHandle); 

			if(nCapRes != 1)
			{
				return;
			}

			//ͼƬ�����Ѿ��ڼ�����
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
			
			//���²���δ��ȫ�����ҿ��ܻ��ڶദ����,������Ҫ���з�װ
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
			//todo �ܾ������ļ�
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
				//��¼�ϴε������ϵ�˵���ʾҳ�档
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
				
				
				//���õ�ǰΪ��ҳ��
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
//�������ǻ���id������
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
		//�鿴�ǲ��ǲ�����������
		CTabLayoutUI* pSidebarTabs = static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar));
		if (NULL == pSidebarTabs) 
			return;

		//������������ϵ�ˡ���ѡ��		
		KillTimer(this->GetHWND(), WM_TIEMER_UPDATE_HOMEPAGE_PIC);

		if(msg.pSender->GetName() == kOptionRecentChat)
		{
			pWebKitUI->SetInternVisible(false);
			pSidebarTabs->SelectItem(TAB_VIEW_RECENTCHAT_PAGE);
			//lee��������TabLayout�л��Ժ���TabLayoutҪ��ԭ���ϴε�ѡ��layout��
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
		//'��Ŀ'
		else if (msg.pSender->GetName() == kOptionProject)
		{
			pWebKitUI->SetInternVisible(false);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->SelectItem(TAB_VIEW_PROJECT_PAGE);		
		}
		// ǩԼ
		else if(msg.pSender->GetName() == RADIO_MAINDLG_PAGE_CONTRACT)
		{
			pWebKitUI->SetInternVisible(false);
			static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->SelectItem(4);
			m_pContractSelect->InitData();
		}
		//���������ҵ��顱��ѡ����
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
		//'Ǯ��'
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
				//��������ע��
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

			jsBindFunction("bindFinance", JsFinance, 1);//�����js��������js��������c++����	
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
				//��������ע��
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

			jsBindFunction("bindFinance", JsFinance, 1);//�����js��������js��������c++����	
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
				//��������ע��
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

			jsBindFunction("bindFinance", JsFinance, 1);//�����js��������js��������c++����	
		}



		ShowNetCallBtn(pSidebarTabs->GetCurSel() == 0 
			&& !m_CurItem.m_strAccount.IsEmpty() 
			&& (CRegexHelper::IsValidMobileNo(ExtractPhoneNo(m_CurItem.m_strAccount).GetData())//����ǺϷ��ֻ��Ų���ʾ
				|| _tcsicmp(m_CurItem.m_strAccount.GetData(), SYS_GROUP_CHAT_ACCOUNT) == 0));//�����Ⱥ���˺�Ҳ��ʾ
	}
	else if (_tcsicmp(msg.sType, _T("itemactivate")) == 0)
	{
		int i=0;
		
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		//�����¼�б�����ˡ�
		if (_tcsicmp(msg.pSender->GetName(), kListContactsControlName) == 0)
		{
			OnRecentChatListSelected();
		}
	}
	//��������б�����ˡ����л���ʾ���и�bug���Ҽ������ʱ��Ҳ����ʾ��to do...
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

	//���õ�ǰ����ĺ���
	tstring strAccount;
	bool bOrgPersonal=false;//�Ƿ���ҵ���еĸ��ˣ���room��Ϣ���޻�����Ϣ���и�����Ϣ
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

	//�л�tabҳǩ
	if (_tcsicmp(strAccount.c_str(), SYS_NOTIFY_ACCOUNT) == 0)
	{
		pLayout->SelectItem(CONST_RECENT_TAB_SYS_MSG_PAGE);
		m_pBussinessMaterialBtn->SetVisible(false);
		m_pBussinessQuitBtn->SetVisible(false);
		
		
		//����������ͷ�����ذ�ť
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
		
		//����ϵͳ��Ϣ������
		//1. ����ϵͳ��Ϣ���ݸ���
		int nCount = GetMsgCenterModule()->GetUnreadMsgCount(SYS_NOTIFY_ACCOUNT);
		if (nCount>0)
		{
			CDuiString strCount;
			strCount.Format(_T("%d"), 0);
			m_pRecentList->setMsgNum(strCount, iIndex);
		}

		//2. ɾ��δ����Ϣ
		ListMsgs lstSys;
		//GetMsgCenterModule()->GetUnReadSysMsgs(lstSys);
		//GetMsgCenterModule()->ClearUnReadSysMsgs();
		
		//3. 
		//ֹͣ��˸
		GetTrayTaskModule()->StopFlash(tstring(SYS_NOTIFY_ACCOUNT), enmMsgType_Sys);

		//4.  ����
		m_bSysUnread = false;
		bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);

		//5.������Ϣ��״̬
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
		|| (!m_CurItem.m_strRoomID.IsEmpty() && m_CurItem.m_strOrgID.IsEmpty()))//���߶Է��Ǹ��˵�room����
	{
		//�������������
		CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl("inputEdit_groupchat"));
		if (NULL == pEditInput)
		{
			return;
		}
		pEditInput->SetText(_T(""));

		//������л�����ˢ�������¼��
		m_pGroupChatPage->SetCurrentRoom(tstring(m_CurItem.m_strRoomID.GetData()));

		//��ʾ���˳���
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

	//���´��ڶ������û���Ϣ���ǳƣ�Avatar����������
	UpdateUserInfoAtWndTop(strAccount, m_CurItem.m_State, tstring(m_CurItem.m_strLogo.GetData()));
	ShowNetCallBtn(true);

	if (_tcsicmp(strAccount.c_str(), SYS_GROUP_CHAT_ACCOUNT) == 0 
		|| (!m_CurItem.m_strRoomID.IsEmpty() && m_CurItem.m_strOrgID.IsEmpty()))//����Է��Ǹ��˵�Ⱥ����
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
		//���ѵ�OrgID�ǿյģ������Ҫ�ٴ�ȥ��ѯ//by zyy
		const tstring id = m_CurItem.m_strID;//��Ҳ�ǿյģ�why����������
		const tstring org_id = m_CurItem.m_strOrgID;
		const tstring account = m_CurItem.m_strAccount;
		const tstring org_name = m_CurItem.m_strOrgName;
		orgInfo info;
		tstring str;
		const tstring real_id = account.substr(0, account.length()-7);

		//�����˺�id��ѯ������Ϣ
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
	//if (m_CurItem.m_strRoomID.IsEmpty())//����Ǹ���
	{
		//��պ����¶�ȡ��ʷ������Ϣ��
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

		//�����δ����Ϣ==>��ȡ��Ϣ������֪flashֹͣ��
		//to do...�޸����===>���Ӧ�÷���ItemClick���¼���Ӧ����ȥ������itemselect��Ӧ���ֿ�����
		if (m_pRecentList->CheckIfBubbleExist(iIndex))
		{
			//1.�������
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, iIndex);

			//2.������Ϣ��
			InsertUnReadMsg(strAccount);

			//3.Stop Flash.
			GetTrayTaskModule()->StopFlash(strAccount, enmMsgType_Chat);

			//4.������������Ϣ��ʾ ���
			bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);
		}
	}

	//�����ǰѡ�е���Ⱥ����Ϣ�Ѿ�������ˣ�ֻҪ������ݺ�ֹͣ��˸���ɡ�
	if (!m_CurItem.m_strRoomID.IsEmpty())
	{
		if (m_pRecentList->CheckIfBubbleExist(iIndex))
		{
			//1.�������
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, iIndex);

			tstring strRoomID = m_CurItem.m_strRoomID.GetData();
			//2.�����������δ����Ϣ����
			m_pGroupChatPage->GetGroupChatLogic()->ClearUnReadGroupMsg(strRoomID);
			//InsertUnReadMsg(strAccount);

			//3.Stop Flash.
			GetTrayTaskModule()->StopGroupFlash(strRoomID);

			//4.������������Ϣ��ʾ ���
			bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
			setOptionRecentChatListUnread(bunRead);
		}
	}

	//lwt �ļ�ҳǩͬʱѡ��
	map<tstring, CVerticalLayoutUI*>::iterator it = m_mapFileTab.find(strAccount);
	if (it != m_mapFileTab.end())
	{
		static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SelectItem(it->second);
		m_pCurFileTab = it->second;
	}
	else
	{
		//�½�һ��
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

		//׷�ӵ�TabLayout Container�����ڣ�����ֵ+1
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
		//�ȴ�cache�п��Ƿ����
		bool bHaveCard = false;
		sUserVCard oneCard;
		tstring strOrg;
		tstring strOid;

		//�����code����֮ǰд�����account�������ǻ����ģ���

		select_business_ = false;//by zyy
		if (!bOrgItem)
		{
			if (NULL != m_pUsrMgrAdapter)
			{
				bHaveCard = m_pUsrMgrAdapter->GetUserVCard(strAccount, oneCard);
			}

			//�����ھʹӱ���DBȥ�� 
			if (!bHaveCard)
			{
				bHaveCard = GetDBTaskModule()->LoadUserVCard(strAccount, oneCard);
			}

			//���ڱ���DB���²�����work����ʱ�ȸ�����
			oneCard.strRemark = m_pUsrMgrAdapter->GetMemo(strAccount);

			//�û�����Ϣ
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
// 	//������Ҳ������б�
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
	//��������·��
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
	//	//�½�һ��
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

	//	//׷�ӵ�TabLayout Container�����ڣ�����ֵ+1
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
	////1. ����icon
	////todo
	////2. ��������
	//CLabelUI *plableFileName = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pRecentChat, _T("labelFileName")));

	////3. �����ļ���С
	//double sz = double(downLoadInfo.dwFileSize) / 1024;
	////sz = sz/1024;
	//CDuiString strsz;
	//strsz.Format("%.2f", sz);
	//strsz += _T("KB");
	//tstring strValue = downLoadInfo.strFileName + "(" + tstring(strsz)+")";
	//plableFileName->SetText(strValue.c_str());
	////4. ����map����
	//m_mapTaskFiles.insert(make_pair(downLoadInfo.strKey, downLoadInfo));
	//int nSize = m_mapTaskFiles.size();

	////5. ui��ʾ
	//static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutFileSend))->SetVisible(true);
}

void mainDlg::UpdateGroupLastMessage(sGroupChatItem* pOneGroupChat, bool bTextMsg)
{
	//1.�ж���ϵ��ҳ���Ƿ������Ⱥ�ˣ�������ö������û�����
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

	//���ڻ������ʹ���һ��Item
	if (!bGroupInRecentList)
	{
		tstring room_id =  pOneGroupChat->strRoomID.c_str();

		if (1)//CController::Instance()->GetRoomManager()->IsInThisRoom(room_id, m_myselfInfo.strAccount))
		{
			//��ȡ������Ϣ
			tstring strOrgID;
			tstring strOrgName;
			tstring strAvatarBase64;

			//������Ϣ
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
				//����ǻ���
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
				//����Ǹ���
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

			//����ǻ�����
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
			//����Ǹ���
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


			//���½���Ⱥ����뵽�������ϵ���б����ݿ�		
			ContactListItemInfo info;
			//�ж��Ǹ��˻��ǻ���
			info.m_strAccount = bOrgGroup ? SYS_GROUP_CHAT_ACCOUNT : tstring(strDestPhone+"@test-d").c_str();

			//����ǻ���
			if (nTypeValue == 1 || bOrgGroup)//ʵ�������������ȼ�
			{
				info.m_strLogo = strOrgAvatarPath.empty()?  DEF_AVATAR_NOR : strOrgAvatarPath.c_str();
				info.m_strNickName=(_T(""));//strOrgName.c_str();
				info.m_strOrgName  = strOrgName.c_str();
				info.m_strOrgID    = strOrgID.c_str();
			}
			else if(nTypeValue == 2 || !bOrgGroup)//ʵ�������������ȼ�
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

			//��ӵ���һ����
			m_lstFriends.insert(m_lstFriends.begin(), info);

			//��ӵ������ϵ�˶���.
			m_pRecentList->AddItem(&info, 0);

			//���浽���ݿ�
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

	//2.�������˾��ǵ�ǰ��ѡ�еģ���ֱ�Ӳ��룺���Ӧ���ڸ������һ����Ϣ֮ǰ��������
	int nItemIndex = GetRoomItemIndexInChatList(pOneGroupChat->strRoomID);

	//3.��������û��ѡ�У�����ʾ���ݣ��ͣ���˸��
	if (m_pRecentList->GetCurSel() != nItemIndex)
	{
		int nUnReadCount = m_pGroupChatPage->GetGroupChatLogic()->GetUnReadGroupMsgCount(pOneGroupChat->strRoomID);
		if (nUnReadCount>0)
		{
			CDuiString strCount;
			strCount.Format(_T("%d"), nUnReadCount);
			m_pRecentList->setMsgNum(strCount, nItemIndex);

			//4.��˸
			GetTrayTaskModule()->FlashGroup(pOneGroupChat->strRoomID);
		}
	}
	//�����ѡ�еģ�������Ҫ������ɵ�ͳ������Ŷ��
	else
	{
		m_pGroupChatPage->GetGroupChatLogic()->ClearUnReadGroupMsg(pOneGroupChat->strRoomID);
	}

	//�������һ����Ϣ��ʱ��
	UpdateLastGroupMessage(pOneGroupChat->strRoomID);

	//RefreshContactListControl();

}

void mainDlg::UpdateLastGroupMessage(tstring& strRoomID)
{
	if (NULL == m_pRecentList)
	{
		return;
	}
	//2.���½�����
	int nIdx = GetRoomItemIndexInChatList(strRoomID);
	if (nIdx > -1)
	{		
		//����m_lstFriends����Ļ�����Ϣ�����һ����Ϣ����ʱ��
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
	//2.���½�����
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

				//����m_lstFriends����Ļ�����Ϣ�����һ����Ϣ����ʱ��
				if (m_lstFriends.size()>0)
				{
					for (int i=0; i< m_lstFriends.size(); i++)
					{
						ContactListItemInfo& oneItem = m_lstFriends[i];
						if (oneItem.m_strRoomID.IsEmpty() && //�������죬���ǺͶԷ���Ⱥ��
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

//�յ���Ϣ�ˣ��ж��£���ǰ�������ϵ���ǲ���strFromBare.
//1������ǵ�==>ֱ�ӽ���Ϣ���뵽��Ϣ�б��У�����strFrombare������ϵ���б�������档
//2���������==>������+��˸���ѣ�
void mainDlg::OnReceiveMessage(CMsgBase* pRecvMsg, EMsgType eMsgType)
{
	if (NULL == m_pRecentList)
	{
		return;
	}

	//��List���ҵ�����˵Ľڵ�������
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
				//����������и��ˣ����������Ⱥ��
				if ( oneRecentItem.m_strRoomID.IsEmpty() //���Ǻ�����˵�Ⱥ����item��
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

	//���ö�
	m_pRecentList->topItem(nItemIndex);
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(this->m_hWnd,&wp);

	// showCmd = 1 ��ʾ����������ʾ
	if(wp.showCmd != 1)
	{
		bProcessed = false;
	}
	//���û�д������Ϣ������˸���ѡ�
	if (!bProcessed)
	{
		int nIdx = GetAccountIndexInChatList(strAccount);
		if (nIdx > -1)
		{
			//�ҵ���,����δ����Ϣ����
			int nCount = GetMsgCenterModule()->GetUnreadMsgCount(strAccount);
			CDuiString strCount;
			strCount.Format(_T("%d"), nCount);
			m_pRecentList->setMsgNum(strCount, nIdx);

			UpdateLastChatMessage(strAccount);
		}


		//������Ϣ��ʾ
		bool bunRead = GetMsgCenterModule()->doHaveUnReadMsg();
		setOptionRecentChatListUnread(bunRead);
		//4.tray flash..
		GetTrayTaskModule()->Flash(strAccount, (EMsgType)eMsgType);
		//SetIcon(IDI_ICON_MESSAGE);
	}
	
}

void mainDlg::ProcessChatMsg(CTextMsg* pRecvMsg, int nItemIndex, bool& bProcessed)
{
	//�������
	if (nItemIndex != -1)
	{
		//��ǰ����ı�����strFrombare�ſ������������Ϊ��������Ϣ�����δ����Ϣ��
		//����ط�ֱ�ӻ�ȡ��ǰ�����Item���ж�ItemID�ǲ��ǣ������û�м����(���ܳ�ʼ����ʱ��ͻ���û�����)�����õ�һ����
		bool bSelectedItem = false;
		int nSelected = m_pRecentList->GetCurSel();
		if (nSelected == nItemIndex)//�Ǽ����Item
		{
			//��ǰ�ǲ���strFromBare
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
		//�Ӻ����б������ҵ�����ˣ���ȡ�����ϵ�˵Ļ�����Ϣ
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
			
		//���浽�������ݿ⡣
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


	//�������
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
		iteminfo.m_strNickName = "ϵͳ��Ϣ";
		iteminfo.m_strLastMsg = pRecvMsg->strMsg.c_str();//os.str().c_str();
		iteminfo.m_strLogo = SYSNOTIFY_AVATAR;
		iteminfo.m_strLastTm = strRecvTime.c_str();

		//�Ӻ����б������ҵ�����ˣ���ȡ�����ϵ�˵Ļ�����Ϣ
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
		
		//���浽�������ݿ⡣
		RECENTITEM oneItem;
		oneItem.strUid = pRecvMsg->strFromBare;
		oneItem.strTime = strRecvTime.c_str();
		GetDBTaskModule()->SaveRecentFriendList(oneItem);
	}
}

void mainDlg::ProcessOfflineGroupMsg(CGroupMsg* pGroupMsg, int nItemIndex, bool& bProcessed)
{
	//д�뵽���ݿ�,���ڽ������Ľ�����ʾ���ǣ�ȥ���ݿ��ȡ��
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

	//��ʼ��Ⱥ������Ϣ��¼
	if (NULL != m_pGroupChatPage)
	{
		m_pGroupChatPage->InitGroupMsg(pGroupMsg->strRoomID);
		m_pGroupChatPage->GetGroupChatLogic()->AddOneUnReadCount(pGroupMsg->strRoomID);
		//��һ����������Ⱥ���죬��Ҫ��ʼ��pack�б�
		UpdatePackListInGroupChat(pGroupMsg->strRoomID);
	}
}

void mainDlg::OnDbClickGroupMsg(tstring strRoom)
{
	//ѡ�������¼Option,�����ǰ�Ǻ����б�Ļ���
	COptionUI* pChatList = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat));
	if (NULL != pChatList)
	{
		pChatList->Selected(true);
	}

	//1.ȥ������
	int indx = GetRoomItemIndexInChatList(strRoom);
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.�������
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, indx);	

			//2.���õ�List����
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(indx));
			pItem->Select();
			m_pRecentList->SetItemIndex(pItem, 0);

		}
	}
}

void mainDlg::OnDbClickChatMsg(tstring strAccount)
{
	//ѡ�������¼Option,�����ǰ�Ǻ����б�Ļ���
	COptionUI* pChatList = static_cast<COptionUI*>(m_PaintManager.FindControl(kOptionRecentChat));
	if (NULL != pChatList)
	{
		pChatList->Selected(true);
	}

	//1.ȥ������
	int indx = GetAccountIndexInChatList(strAccount);
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.�������
			CDuiString strEmpty(_T("0"));
			m_pRecentList->setMsgNum(strEmpty, indx);	

			//2.���õ�List����
			CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(m_pRecentList->GetItemAt(indx));
			pItem->Select();
			m_pRecentList->SetItemIndex(pItem, 0);
			
		}
	}

	//2.������Ϣ
	InsertUnReadMsg(strAccount);
}

void mainDlg::OnBnClickAddFriendReq(bool bAgree, INT_PTR ptrData)
{
	//һ��ֻ��ȥ��1�����ݸ���
	int indx = GetAccountIndexInChatList(tstring(SYS_NOTIFY_ACCOUNT));
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.����ϵͳ��Ϣ���ݸ���
			//int nCount = GetMsgCenterModule()->GetUnreadMsgCount(SYS_NOTIFY_ACCOUNT);
			//if (nCount>0)
			//{
			//	CDuiString strCount;
			//	strCount.Format(_T("%d"), nCount-1);
			//	m_pRecentList->setMsgNum(strCount, indx);
			//}

			//2.���õ�List����
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

			//ֹͣ��˸
			GetTrayTaskModule()->StopFlash(tstring(SYS_NOTIFY_ACCOUNT), enmMsgType_Sys);

			//������Ϣ��״̬
			GetDBTaskModule()->UpdateSysMsgStatus((time_t)(pMsg->uTime), true);

			////��ֹbutton
			//CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
			//if (NULL == pSysMsgList)
			//{
			//	return;
			//}
			//pSysMsgList->DisableButton((sSysMsg*)ptrData);
		}
	}

	//��ֹbutton
	CListSysMsg* pSysMsgList = static_cast<CListSysMsg*>(m_PaintManager.FindControl(kSysMsgListCtrl));
	if (NULL == pSysMsgList)
	{
		return;
	}
	pSysMsgList->DisableButton((sSysMsg*)ptrData);
}
void mainDlg::OnDbClickSysMessage(tstring strNewUser)
{

	//һ��ֻ��ȥ��1�����ݸ���
	int indx = GetAccountIndexInChatList(strNewUser);
	if (indx>-1)
	{
		if (NULL != m_pRecentList)
		{
			//1.����ϵͳ��Ϣ���ݸ���
			int nCount = GetMsgCenterModule()->GetUnreadMsgCount(SYS_NOTIFY_ACCOUNT);
			if (nCount>0)
			{
				CDuiString strCount;
				strCount.Format(_T("%d"), nCount-1);
				m_pRecentList->setMsgNum(strCount, indx);
			}

			//2.���õ�List����
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
					CMsgBox* pMsgBox = new CMsgBox("�Ӻ�������", pAddFriendReq->strMsg.c_str(),MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
					bool bAgree = false;
					if (MSGBOX_IDYES == pMsgBox->ShowModal(NULL))	
						bAgree = true;
					tstring strReqId = pAddFriendReq->strSender;
					CWebClient::approveOrgBuddyRequest(strReqId,bAgree);
				}
			
				GetMsgCenterModule()->RemoveLatestUnReadMsg(strNewUser, pMsg->uTime);

				//������Ϣ��״̬
				GetDBTaskModule()->UpdateSysMsgStatus((time_t)(pMsg->uTime), true);

				//ͬ������������ϵͳ��Ϣ�б�����İ�ť״̬
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
	CMsgBox* pMsgBox = new CMsgBox("�Ӻ�������", strReqMsg.c_str(),MSGBOX_ICON_ASK, MSGBOX_IDYESNO);

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
	//�����ҵ�ͷ����ǳ�
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
		CDuiString strOrg = m_MyselfItem.m_strOrgName.IsEmpty()? "����û�л���": m_MyselfItem.m_strOrgName;
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

	//��Щ�û�û�������û�ͷ�񣬾��ǿյ�
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

				//����ͼ��
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

	//��Щ�û�û�������û�ͷ�񣬾��ǿյ�
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

				//����ͼ��
				if (eState == enmState_Online ||
					eState == enmState_Busy )
				{
					//�û�û��ͷ���ʹ��Ĭ��ͷ��
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
	//1.Update recent cache + ui===>ȡ���ˣ�ֻ����ϵ���б�����״̬��Ϣ
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

		//�����ǰ����Ⱥ����
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
		//�����ǰ��Ⱥ����ʾȺ����
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
			strCount += "��)";
			CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
			if (NULL != pGroupMemCount)
			{
				pGroupMemCount->SetText(strCount.c_str());
			}
		}
	}


}
//�л������߳�ȥ���������
//��ϵ�˶���ȡ��
//չʾ�����б���ȥ�������ݿ�ȥ����Ϣ�����û�����͵�serverȥfetch vcard��
void mainDlg::OnRosterAvalible(MapAllRosters& listUserInfos)
{
	if (this->GetHWND())
	{
		PostMessage(WM_ROSTER_ARRIVED);
	}
}

//VCard��Ϣ�����ˣ��͸���ͷ����Ϣ����Ҫ�ж�����״̬������״̬��ͬ����ͷ��
//ͬ�����������ϵ��+�����б��ȸ���cache���ٸ���UI
//�п����û�ѹ����û��ͷ��===>ʹ��Ĭ��ͷ��
void mainDlg::OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo)
{
	if (this->GetHWND())
	{
		SendMessage(WM_NEW_VCARD_ARRIVED, (WPARAM)(&oneInfo));
	}
}

//Ŀǰֻ�ǲ��ҵĵط��õ���,���²���ҳ��
void mainDlg::OnStrangerVCard(tstring& strAccount, sUserVCard& oneCard)
{


}

void mainDlg::UpdateShowNameInRecentChatList(tstring& strAccount, tstring& strShowName)
{
	//1.���������ϵ�ˣ�����еĻ���
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
	//1.���������ϵ�ˣ�����еĻ���
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
	//������ʾ
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
	//2.���º����б������ͷ��
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
	//�����ݲ�����
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
	//todo ��Ҫ���л�ȡ 
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

	//���汾�������¼
	pChatlst->CacheChatMessage(m_CurItem.m_strAccount,&chatinfo);

	//������Ϣ������DB
	_tChatMessage oneText;
	oneText.strDestUser = chatinfo.m_strAccount.GetData();
	oneText.strChatText = strValue;
	oneText.m_timeChat = chatinfo.uTime;
	GetDBTaskModule()->SaveChatMessage(tstring(m_CurItem.m_strAccount.GetData()), oneText);

	//�������һ����Ϣ���ݺͷ���ʱ��
	int nIdx = GetAccountIndexInChatList(tstring(m_CurItem.m_strAccount.GetData()));
	if (nIdx > -1)
	{
		if (NULL == m_pRecentList)
		{
			return;
		}

		m_pRecentList->SetLastMessage(oneText.strChatText, GetHMSTimeString(oneText.m_timeChat), nIdx);

		//����m_lstFriends����Ļ�����Ϣ�����һ����Ϣ����ʱ��
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
	//��cache���ҵ�ItemID
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
		//3.�õ�strFromBare��������������Ϣ��������
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
	//��Cache���ҵ�ItemID,����Է���һ��Ⱥ��ģ�����Ҫ�ж�room��Ϣ�Ƿ�Ϊ��
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
		//3.�õ�strFromBare��������������Ϣ��������
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
//�����û�����List Control�ҵ���Ӧ��item ����ֵ��
int mainDlg::GetAccountIndexInChatList(tstring& strFromBare, bool bInGroup /*= false*/)
{
	//��Cache���ҵ�ItemID,����Է���һ��Ⱥ��ģ�����Ҫ�ж�room��Ϣ�Ƿ�Ϊ��
	int nItemID = -1;
	for (int i=0; i< m_lstFriends.size(); i++)
	{
		ContactListItemInfo& oneItem = m_lstFriends[i];
		if (!bInGroup)
		{
			if ( oneItem.m_strRoomID.IsEmpty() &&//Ⱥ����Ϊ��
				_tcsicmp(strFromBare.c_str(), oneItem.m_strAccount.GetData()) == 0)
			{
				nItemID = oneItem.nItemID;
				break;
			}
		}
		else
		{
			//�Է���Ⱥ��Ϣ
			if ( !oneItem.m_strRoomID.IsEmpty() && //Ⱥ����Ϊ��
				_tcsicmp(strFromBare.c_str(), oneItem.m_strAccount.GetData()) == 0)
			{
				nItemID = oneItem.nItemID;				
				break;
			}
		}
	}
	if (nItemID>=0)
	{
		//3.�õ�strFromBare��������������Ϣ��������
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

//��Ӻ��ѳɹ���:���º����б�,�û��㿪����ˣ����л��������ϵ�ˣ����������촰�ڶ�����ʾ�����Ѿ������xxx�����ڿ��������ˡ���
void mainDlg::OnAddFriendSucc(tstring& strFriend)
{
	//��ʼ�������б�
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
			//����������������ڣ��ʹ���һ�������ڵ㡣
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
				//Ҳ��������ʲô�ط��Ǹ����µ�ɾ��mapû��ɾ���ɾ������˴���pOrgNode��ȻΪNULL����
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
					m_mapOrg2Node[oneRoster.strOrgID] = pOrgNode;//���¸�ֵһ���µ�Node�ڵ㣡����
				}
			}

			//��ӵ�����
			FriendListItemInfo friendNode;
			friendNode.folder = false;
			friendNode.empty = false;
			friendNode.strOrgName = oneFriend.strOrgnization;//_T("�ҵĺ���");
			friendNode.itemId = oneFriend.nItemID;
			friendNode.nick_name = oneFriend.strShowName;
			friendNode.logo = oneFriend.strAvatar;
			friendNode.account = oneFriend.strAccount;
			//add to list control
			m_pFriendLst->AddNode(friendNode,  pOrgNode);
		}
	}
}
//���º����б�������¼�б�
void mainDlg::OnFriendRemoved(tstring& strKick,bool bActive)
{
	char* pBuf = new char[256];
	memset(pBuf, 0, 256);
	strcpy(pBuf, strKick.c_str());
	PostMessage(WM_FRIEND_REMOVED, (WPARAM)pBuf, 0);
	//UpdateCacheAndUiAfterFriendDeleted(strKick);
}

//�����ϵ��ֻ���ڱ��أ���˲��漰�����������ͨѶ
void mainDlg::OnDeleteContact()
{
	CMsgBox* pMsgBox = new CMsgBox("ɾ������","�Ƿ�ɾ���������¼��",MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
	if(pMsgBox->ShowModal() == MSGBOX_IDNO)
	{
		return;
	}


	if (NULL == m_pRecentList)
	{
		return;
	}
	//��ȡ����Item��Item ID��
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


	//ɾ������DB�������¼�������ϵ���б�
	if (strRoomId.empty())//ͨ��roomid���ж��Ǹ��˻���Ⱥ����
	{
		GetDBTaskModule()->RemoveRecentItem(strAccount);
		GetDBTaskModule()->RemoveChatMessage(strAccount);
	}
	else
	{
		GetDBTaskModule()->RemoveFromRecentGroup(strRoomId);
		GetDBTaskModule()->RemoveGroupChatMessage(strRoomId);
	}
	

	//��������������������ˣ�����Ҫ��������¼
	CLabelUI* pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName/*kbuttonContactNameControlName*/));
	if (NULL != pFriendNickname)
	{
		CDuiString strNickStatus = pFriendNickname->GetText();
		int nIdex = strNickStatus.Find(strNickName.c_str(), 0);
		if(nIdex>=0)
		{
			//��գ�����ѡ�������ϵ��������������һ����ϵ����Ϣ�أ�===to do...
			pFriendNickname->SetText(_T(""));
			//����û�ͷ��
			CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
			if (NULL != pBtnFriendAvatar)
			{
				pBtnFriendAvatar->SetNormalImage(_T(""));
			}
			//����û�������Ϣ
			CLabelUI* pLabelFriendCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
			if (NULL != pLabelFriendCompany)
			{
				pLabelFriendCompany->SetText(_T(""));
			}

			CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
			if (NULL != pFriendOrgname) pFriendOrgname->SetText(_T(""));

			//��������¼�����������
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
// 			//�Ӻ����б�Cacheɾ������
// 			m_listAllFriends.erase(it);
// 			break;
// 		}
// 	}
// 
// 	//�Ӻ����б�ui+LocalDBɾ��    todo ���ڲ�һ���ܵ��õ�CListUI::SelectItem, ����ʱʹ��m_listAllFirends�е�λ����ɾ��
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
// 						//���������׽ڵ��ǿյ��ˣ���ô�����֯����Ӧ��ҲҪɾ������--llzhou
// 						if (!pParentNode->has_children())
// 						{
// 							//m_pFriendLst->RemoveNode(pParentNode);//����ǡ��ҵĺ��ѡ��ǲ�����ɾ����--lee
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
// 	//ɾ������DB�������¼�������ϵ���б�
// 	GetDBTaskModule()->RemoveRecentItem(strDeletedAccount);
// 	//GetDBTaskModule()->RemoveChatList(strAccount);
// 
// 	//��������ϵ��cache+ui+LocalDB
// 	if (m_lstFriends.size()>0)
// 	{
// 		int nItemID=-1;
// 		tstring strNickName;
// 		for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
// 			it != m_lstFriends.end(); ++it)
// 		{
// 			ContactListItemInfo& oneFriend = *it;
// 			//�ҵ�������ˣ���Cache��ɾ��
// 			if (_tcsicmp(oneFriend.m_strAccount.GetData(), strDeletedAccount.c_str())==0)
// 			{
// 				nItemID = it->nItemID;
// 				strNickName = it->m_strNickName.GetData();
// 				m_lstFriends.erase(it);
// 				break;
// 			}
// 		}
// 
// 		//�ӽ���ɾ�������
// 		if (nItemID>=0)
// 		{
// 			if (NULL != m_pRecentList)
// 			{
// 				m_pRecentList->RemoveItem(nItemID);
// 			}
// 		}
// 
// 		//��������������������ˣ�����Ҫ��������¼
// 		CLabelUI* pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName/*kbuttonContactNameControlName*/));
// 		if (NULL != pFriendNickname)
// 		{
// 			CDuiString strNickStatus = pFriendNickname->GetText();
// 			int nIdex = strNickStatus.Find(strNickName.c_str(), 0);
// 			if(nIdex>=0)
// 			{
// 				//��գ�����ѡ�������ϵ��������������һ����ϵ����Ϣ�أ�===to do...
// 				pFriendNickname->SetText(_T(""));
// 				//����û�ͷ��
// 				CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
// 				if (NULL != pBtnFriendAvatar)
// 				{
// 					pBtnFriendAvatar->SetNormalImage(_T(""));
// 				}
// 				//����û�������Ϣ
// 				CLabelUI* pLabelFriendCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
// 				if (NULL != pLabelFriendCompany)
// 				{
// 					pLabelFriendCompany->SetText(_T(""));
// 				}
// 
// 				CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
// 				if (NULL != pFriendOrgname) pFriendOrgname->SetText(_T(""));
// 
// 				//��������¼�����������
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
// 	////����¼Ӻ����б���������ˣ���ɾ�����ɣ��Ҹռӵģ���ɾ���ˡ�
// 	//map<tstring, time_t>::iterator itFind = m_mapAccount2Time.find(strDeletedAccount);
// 	//if (itFind != m_mapAccount2Time.end())
// 	//{
// 	//	m_mapAccount2Time.erase(itFind);
// 	//}
// }
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mainDlg::OnDeleteFriend()
{
	CMsgBox* pMsgBox = new CMsgBox("ɾ������","�Ƿ�ɾ���ú��ѣ�",MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
 	if(pMsgBox->ShowModal() == MSGBOX_IDNO)
 	{
 		return;
 	}


	if (NULL == m_pFriendLst)
	{
		return;
	}
	//��ȡ����Item��Item ID��
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
					CMsgBox* pMsgBox = new CMsgBox("ɾ������","����ɾ���Լ� ��",MSGBOX_ICON_WARNING, MSGBOX_IDOK);
					//����ɾ���Լ�
					pMsgBox->ShowModal();
					return;
				}
				tstring strOrgnization = it->strOrgnization;
				tstring strMyOrg, strMyOrgId;
				m_pUsrMgrAdapter->GetMyOrganization(strMyOrg, strMyOrgId);
				if (_tcsicmp(strOrgnization.c_str(), strMyOrg.c_str()) == 0)
				{
					CMsgBox* pMsgBox = new CMsgBox("ɾ������","����ɾ��ͬһ�����ĺ��� ��",MSGBOX_ICON_WARNING, MSGBOX_IDOK);
					//����ɾ��ͬһ������Ա
					pMsgBox->ShowModal();
					return;
				}
				//1.��serverɾ��
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
	////2.���������ϵ��pageҳ�浽
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
	
	//���ǰҳ��Ϊ�̼���Ϣҳ��
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


	//���汾�������¼
	chatinfo.m_strRawMsg = os.str().c_str();
	pChatlst->CacheChatMessage(m_CurItem.m_strAccount,&chatinfo);

	//������Ϣ������DB
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


	//���汾�������¼
	chatinfo.m_strRawMsg = os.str().c_str();
	pChatlst->CacheChatMessage(m_CurItem.m_strAccount,&chatinfo);

	//������Ϣ������DB
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
				
				//1.�ȴ����ݿ����
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
		MessageBox(NULL, _T("����ѡ����ϵ�ˣ�"), _T("��ʾ"), MB_OK);
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
			//�Ӻ����б�Cacheɾ������
			m_listAllFriends.erase(it);
			break;
		}
	}

 	//�Ӻ����б�ui+LocalDBɾ��    todo ���ڲ�һ���ܵ��õ�CListUI::SelectItem, ����ʱʹ��m_listAllFirends�е�λ����ɾ��
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
						//���������׽ڵ��ǿյ��ˣ���ô�����֯����Ӧ��ҲҪɾ������--llzhou
						if (!pParentNode->has_children())
						{
							//m_pFriendLst->RemoveNode(pParentNode);//����ǡ��ҵĺ��ѡ��ǲ�����ɾ����--lee
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

	//ɾ������DB�������¼�������ϵ���б�
	GetDBTaskModule()->RemoveRecentItem(strDeletedAccount);
	//GetDBTaskModule()->RemoveChatList(strAccount);

	//��������ϵ��cache+ui+LocalDB
	if (m_lstFriends.size()>0)
	{
		int nItemID=-1;
		tstring strNickName;
		for (vector<ContactListItemInfo>::iterator it = m_lstFriends.begin();
			it != m_lstFriends.end(); ++it)
		{
			ContactListItemInfo& oneFriend = *it;
			//�ҵ�������ˣ���Cache��ɾ��
			if (_tcsicmp(oneFriend.m_strAccount.GetData(), strDeletedAccount.c_str())==0)
			{
				nItemID = it->nItemID;
				strNickName = it->m_strNickName.GetData();
				m_lstFriends.erase(it);
				break;
			}
		}

		//�ӽ���ɾ�������
		if (nItemID>=0)
		{
			if (NULL != m_pRecentList)
			{
				m_pRecentList->RemoveItem(nItemID);
			}
		}

		//��������������������ˣ�����Ҫ��������¼
		CLabelUI* pFriendNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendNickName/*kbuttonContactNameControlName*/));
		if (NULL != pFriendNickname)
		{
			CDuiString strNickStatus = pFriendNickname->GetText();
			int nIdex = strNickStatus.Find(strNickName.c_str(), 0);
			if(nIdex>=0)
			{
				//��գ�����ѡ�������ϵ��������������һ����ϵ����Ϣ�أ�===to do...
				pFriendNickname->SetText(_T(""));
				//����û�ͷ��
				CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
				if (NULL != pBtnFriendAvatar)
				{
					pBtnFriendAvatar->SetNormalImage(_T(""));
				}
				//����û�������Ϣ
				CLabelUI* pLabelFriendCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
				if (NULL != pLabelFriendCompany)
				{
					pLabelFriendCompany->SetText(_T(""));
				}

				CLabelUI *pFriendOrgname = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelFriendCompany));
				if (NULL != pFriendOrgname) pFriendOrgname->SetText(_T(""));

				//��������¼�����������
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

	////����¼Ӻ����б���������ˣ���ɾ�����ɣ��Ҹռӵģ���ɾ���ˡ�
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
			
			//1. UI���
			if (plstProcess!=NULL && pelement!=NULL)
				plstProcess->Remove(pelement);
			//2. map���������
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
			//���ͣ���������ʧ��
			if ( iter != m_mapTaskFiles.end())
			{
				//1. ϵͳ��Ϣ
				tstring strSys;
				strSys = iter->second.strFileFullName + " " + "����ʧ�ܡ�";
				CDuiString duistrSys = strSys.c_str();
				CChatListUI *pChatlst = static_cast<CChatListUI*>(m_PaintManager.FindControl(kListChatControlName));
				if (pChatlst != NULL)
					pChatlst->AddSysItem(duistrSys);
				//2. map�������
				m_mapTaskFiles.erase(iter);
			}
			
			//3. ui���
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

		//�����������Ϣ������nCount��ֹһ����
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

		//�����������Ϣ������nCount��ֹһ����
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
	//��ǰ�������ϵ�� & ����ҳ��ʾ====>
	if (TAB_VIEW_RECENTCHAT_PAGE == static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar))->GetCurSel()
			&& CONST_RECENT_TAB_HOME_PAGE == static_cast<CTabLayoutUI*>(m_PaintManager.FindSubControlByName(static_cast <CTabLayoutUI*>(m_PaintManager.FindControl(kTabLayoutSidebar)), kRecentChatLayout))->GetCurSel())
	{
		//�����ǰչʾ����ҳ������ʾ�ϴ������ϵ�˱�ѡ�е�tab page��
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
				OnFriendListSelected(strOrgId,true); //����������partner_pageҳ��ѻ�������չ�ֳ�������ʱ�ȿհ״���ɣ� llzhou
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
				//OnFriendListSelected(str,true); //����������partner_pageҳ��ѻ�������չ�ֳ�������ʱ�ȿհ״���ɣ� llzhou
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
	//���ж��Լ��ǲ�������֯
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
		//�����κ���֯����
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

	sort(m_listAllFriends.begin(), m_listAllFriends.end(), CSortHelper::CompareFunc);//�������ܺ�����������������ʧ��bug by HQ 20160520

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
				//��仰�ᵼ�¿�����������ʱ���Σ�by zyy
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
		// ��ӻ�ӭ��Ϣ
		ContactListItemInfo info;
		info.nItemID = 10000;//?
		info.m_strAccount = "10000@test-d";
		info.m_strNickName = "ϵͳ��Ϣ";
		info.m_strMsgCount = "1";
		info.m_strLastMsg = " ��ӭʹ��MM!";
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
		pLabelMyOrg->SetText("����û�л���");
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
		m_pHomePageHelpText->SetText("��ҵ����������������Ķ��󣬷�Ϊ�����͸������֣�������ʹ��MM Android�����ɵĽ��ֻ���ϵ���������ҵ��顣");
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btnhomePageCreateOrg") == 0)
	{
		m_pHomePageHelpText->SetText("��������ҵ����һ�֣�ÿ����ͬʱֻ���Դ���һ�������У�����������Ĭ�ϳ�Ϊ��������Ա������Ա�ɽ��й���Ȩ��ת�ã�������Ա����Ȳ�����");
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btnhomePageBusiness") == 0)
	{			
		m_pHomePageHelpText->SetText("������ҵ��ť���������ٵĽ���ҵ��һ��ҵ�����ɷ������һ���ѯ���ۣ���ҵ��ͨ�������Է������ϡ�����Ƶ���졣��һ�ж��ܹ���ҵ����巽��Ľ��й���");
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btnhomePageNetCall") == 0)
	{
		m_pHomePageHelpText->SetText("�����ṩ��ѵ绰���񣬰����������ϰ�������ҵ�����й�ͨ����ÿ���¸߶�Ļ��ѵ�˵�ټ��ɣ�");
	}
}

void mainDlg::RefreshHomePagePic()
{
	static int idx = 0;
	CDuiString strPic;
	strPic.Format("homepage\\bg%d.PNG", idx++%5);
	m_pHomePageVLBkImg->SetBkImage(strPic);
}


//�����ǰѡ�е�room�պþ���strRoomId���͸���room��Ա������
//ע�����������������ʵ�����½�room�����room��Ա������ô˺���
//�����½�room��Ⱥͷ���൱��ˢ�������Σ������ٿ��Ƿ����޸ĵı�Ҫ  by zyy
void mainDlg::OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<tstring>& vecMemList)
{
	if (_tcsicmp(m_CurItem.m_strRoomID.GetData(), strRoomID.c_str()) == 0)
	{
		char chBuf[10]={0};
		itoa(vecMemList.size(), (char*)(&chBuf), 10);
		tstring strCount = "(";
		strCount += chBuf;
		strCount += "��)";
		CLabelUI *pGroupMemCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(kLabelGroupMemCount));
		if (NULL != pGroupMemCount)
		{
			pGroupMemCount->SetText(strCount.c_str());
		}

		////Ⱥͷ��Ҫ��Ҫ������,������½�Ⱥ�飬ͷ���Ѿ����¹��ˣ��������ӳ�Ա����Ⱥ��ͷ�����Ҫ�ٴθ��£�����ǿ���ٸ���һ��

		OutputDebugString("��OnNewGroupMemberList ���\r\n");
		ComposeGroupAvatar(m_CurItem);
		OutputDebugString(m_CurItem.m_strLogo.GetData());
		OutputDebugString("\r\n");

		//·����ԭʼ�Ĵ������ m_lstFriends ���棬������ؼ�����Ҫ����m_lstFriends
		for (int i=0; i<m_lstFriends.size(); i++)
		{
			if (m_CurItem.nItemID == m_lstFriends[i].nItemID)
			{	
				//�����˵����Ⱥ����Ա���������˱仯��ͬ��Ⱥ���Ա�����������9��ͷ��δ
				//�����仯������Ҫ����
				if (m_lstFriends[i].m_strLogo != m_CurItem.m_strLogo)
				{
					OutputDebugString("��ֵlogo\r\n");
					m_lstFriends[i].m_strLogo = m_CurItem.m_strLogo;

					UpdateAvatarInRecentList(m_CurItem.m_strRoomID.GetData(), m_CurItem.m_strLogo.GetData(),
						m_CurItem.nItemID);

					GetDBTaskModule()->UpdateAvatarInRecentGroup(m_CurItem.m_strRoomID.GetData(), m_CurItem.m_strLogo.GetData());
				}
				break;
			}
		}	

		//ֻҪ·�����ģ�ͷ����޷�ˢ�£����Ⱥ��ͷ��·���������Ա���������
 		CButtonUI* pBtnFriendAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(kButtonFriendAvatar));
 		if (NULL != pBtnFriendAvatar)
 		{
			pBtnFriendAvatar->SetNormalImage(m_CurItem.m_strLogo);
 		}	
	}
	else
	{
		////Ⱥͷ��Ҫ��Ҫ������,������½�Ⱥ�飬ͷ���Ѿ����¹��ˣ��������ӳ�Ա����Ⱥ��ͷ�����Ҫ�ٴθ��£�����ǿ���ٸ���һ��
		ContactListItemInfo info;
		info.m_strRoomID = strRoomID.c_str();

		OutputDebugString("��OnNewGroupMemberList �����\r\n");
		ComposeGroupAvatar(info);
		OutputDebugString(info.m_strLogo.GetData());
		OutputDebugString("\r\n");

		//·����ԭʼ�Ĵ������ m_lstFriends ���棬������ؼ�����Ҫ����m_lstFriends
		for (int i=0; i<m_lstFriends.size(); i++)
		{
			if (strRoomID == m_lstFriends[i].m_strRoomID.GetData())
			{	
				//�����˵����Ⱥ����Ա���������˱仯��ͬ��Ⱥ���Ա�����������9��ͷ��δ
				//�����仯������Ҫ����
				if (m_lstFriends[i].m_strLogo != info.m_strLogo)
				{
					OutputDebugString("��ֵlogo\r\n");
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

//��m_lstFriends��ɾ��
//m_pRecentList�����ɾ��ͨ��RefreshContactListControl()�����ĵ������
//�ӱ������ݿ���ɾ��
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

//Ϊ�˸���Ⱥ�ǳ�
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
//��roomname����ʱ��roomlist�ƺ����������£���Ҫ�������
//��ʱ�����Ⱥ��ͷ�������ȷ�Ļ�ȡȺ���Ա�Ӷ�����ͷ��
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
				|| (!iter->m_strRoomID.IsEmpty() && iter->m_strOrgID.IsEmpty()))//����Է��Ǹ��˵�Ⱥ���졣
			{
				if ( _tcsicmp(iter->m_strRoomID.GetData(), strRoomID.c_str()) == 0)
				{
					//����ǻ���Ⱥ���졣����nickname��ͬʱ������ͷ��
					if ( !iter->m_strOrgID.IsEmpty() )
					{
						ContactListItemInfo& info = *iter;
						info.m_strNickName = strRoomNickName.c_str();
						avatar_path = info.m_strLogo;
						bUpdate = true;
						nItemIDTobeupdate = iter->nItemID;
						break;
					}
					//����Ǹ���Ⱥ����
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

	//�����ϵ�˵�Ⱥ�����޸��£����ʼ��m_lstFriendsҲ��Ҫ���£���Ȼ���ܶ�Ӧ��
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
					//������ϢҲ��Ҫ���¡�
					if (m_CurItem.nItemID == nItemID)
					{
						m_CurItem.m_strNickName = strRoomNickName.c_str();
						//���������Ⱥ��ͼ�괦�������⣬������һ�飬�Ȳ����Ժ��ٸ� by zyy
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
	else//�п������յ�����Ⱥ��Ϣ���½�������room����û���������ϵ��������Item�����½�һ��
	{
		//ͬ��Ҫ�жϱ����Ƿ��ڸ�Ⱥ��
		//����Ⱥ������ʱ��û����Ӹ�Ⱥ����
		if (1)//CController::Instance()->GetRoomManager()->IsInThisRoom(strRoomID, m_myselfInfo.strAccount))
		{
			//���½���Ⱥ����뵽�������ϵ���б����ݿ�		
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

			//3. ���ӵ������ϵ�˵Ķ���.
			m_pRecentList->AddItem(&info, 0);
			//RefreshContactListControl();

			//4.��Ⱥ���������ļ������г�ʼ��
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
		//�������µĳ�Ա�б�
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
	//������Լ�
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
	//������Լ��ĺ���
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
//		OutputDebugString("��ComposeGroupAvatar\r\n");
//		Sleep(10);
		for (int i=0; i<member_list.size(); i++)
		{
			sFriendListItem oneFriend;
			//�ȴӺ�������ȥ�ҡ�
			if (mainDlg::getInstance()->GetFriendItem(member_list[i], oneFriend))
			{

			}
			else
			{
				//��İ��������ȥ�ҡ�
				sUserVCard oneVCard;
				CController::Instance()->GetUserInfoMgr()->GetUserVCard(member_list[i], oneVCard);
				oneFriend.strAccount = oneVCard.strAccount;
				oneFriend.strAvatar = oneVCard.strPhotoPath.empty()? DEF_AVATAR_NOR : oneVCard.strPhotoPath;
				oneFriend.strOrgID = oneVCard.strOrganization;
				oneFriend.strOrgnization = oneVCard.strOrganization;
				oneFriend.strShowName = oneVCard.strUserNickname;
			}

			//���ﲻ��Ƶ�duty��
			// 				vector<tstring> vecProperty;
			// 				if(GetDuty(oneFriend.strOrgID, oneFriend.strAccount, vecProperty))
			// 				{
			// 
			// 				}

			member_detail.push_back(oneFriend);
		}

		//�������ͼƬ,ע������ȡ����logo�������ߺ�����Ч���ġ�������Ҫ������ͼƬ�滻�����ߵġ�
		tstring app_path = CFileHelper::GetCurrentAppPath();
		tstring avatar_path = CFileHelper::GetHeadCacheDir();

		//ֻҪ·�����ģ�ͷ����޷�ˢ�£����Ⱥ��ͷ��·���������Ա���������
		std::ostringstream ostr;
		ostr << avatar_path << group_id << "-" << member_detail.size() << ".png";
		avatar_path = ostr.str();

		int pos = app_path.find("bin");
		if (pos < 0)
		{
			MessageBox(NULL, _T("·��������bin������Ѱַ�д�"), _T("��ʾ"), MB_OK);
		}
		tstring path = app_path.substr(0, pos);

		vector<tstring> vec_avatar;
		for (int i=0;i!=member_detail.size();i++)
		{
			tstring avatar(member_detail[i].strAvatar);
			//ע������defaultȫ��ƴд����һֱ����ȥ�� by zyy
			if (avatar == "defalut_avatar_gray.png" || avatar == "defalut_avatar_nor.png")
			{
				avatar = path + "MM\\res\\defalut_avatar_nor.png";
			}
			else
			{
				//ע������ͼƬ�ĸ�ʽ 18133619498@test-d_Gray.png��18133619498@test-d.png
				tstring feature = "@test-d_Gray.png";
				int pos = avatar.find("@test-d_Gray.png");
				if (pos > 0)
				{
					avatar.replace(pos, pos+feature.length(), "@test-d.png");
				}

				//�����û����������
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
	//λ���ƶ�������������������������by zyy
	//duilib�޷���ȡ�ؼ�λ�ã��������ÿؼ�λ�ã���ˣ������λ���Ǵ�xml����õ�
	//�õ�����ռ�õ�λ��
	RECT rect_origin, rect_total;
	rect_origin.left = 260;
	rect_origin.right = 380;
	rect_origin.top = 43;
	rect_origin.bottom = 75;

	rect_total.left = 260;
	rect_total.right = 560;
	rect_total.top = 43;
	rect_total.bottom = 75;

	//�õ������ַ����ĳ���
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

	//���ݿ���������
	GetDBTaskModule()->CloseDB();
	GetDBTaskModule()->Initialize();

 	m_pMsgSessionMgr = NULL;
 	CMsgSessionManager* pSessionMgr = CController::Instance()->GetMsgSessionMgr();
 	if (NULL != pSessionMgr)
 	{
 		m_pMsgSessionMgr = pSessionMgr;
 	}
 
 	//ע�����������Ϣ��
  	GetMsgCenterModule()->AddEventHandler(this);
  	GetTrayTaskModule()->AddEventHandler((ITrayTaskModuleEvent*)this);
  	m_pInstance = this;
  
  	CController::Instance()->GetRoomManager()->AddHandler(this);
	m_pGroupChatPage->GetGroupChatLogic()->RegisterToRoom();

//	CController::Instance()->GetRoomManager()->AddHandler(dynamic_cast<IRoomManagerEvent*>(m_pGroupChatPage));
//   	CWebClient::api_getRegions();

	//����������֮�������ж�ֵ
	
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
	////������Դ��������Ʋ��������ñ��ʵ�֡�

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
	//������Դ��������Ʋ��������ñ��ʵ�֡�

	while(1)
	{
		sprintf(str,"����ʱ��:/t%d",i);
		printf("%s/r",str);
		i++;
		Sleep(1000);
	}
}

void mainDlg::LogOff()
{
	CMsgBox* pMsgBox = new CMsgBox("","ȷ��ע����ǰ�˺ţ�",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
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
		//��������ע��
		//				return;
	}
}



void mainDlg::initOrgProperty()
{
	
}
