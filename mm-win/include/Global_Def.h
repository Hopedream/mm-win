#pragma once


extern TCHAR  WEB_DOMAIN_URL[MAX_PATH];
extern TCHAR  WEB_SERVER_BASE_URL[MAX_PATH];
extern TCHAR  WEB_SERVER_SITE_URL[MAX_PATH];
extern TCHAR  WEB_GET_PASSWORD_URL[MAX_PATH];
extern TCHAR  WEB_WALLET_URL[MAX_PATH];
extern TCHAR  WEB_FILE_URL[MAX_PATH];
extern TCHAR  WEB_ROOT[MAX_PATH];
extern TCHAR  REFER_EXECUTE[MAX_PATH];
extern TCHAR  REFER_FACE_DEAL[MAX_PATH];
extern TCHAR  REFER_FINANCE[MAX_PATH];
extern TCHAR  REFER_MOTOR_TYPE[MAX_PATH];
extern TCHAR  REFER_VIN[MAX_PATH];
extern TCHAR  SUGGESTIONS_URL[MAX_PATH];
//#define  WEB_DOMAIN_URL					"http://60.173.242.117:9001/"
//#define  WEB_SERVER_BASE_URL			"http://60.173.242.117:9001/api/"
//#define  WEB_SERVER_SITE_URL			"http://60.173.242.117:9001/api/site/"
//#define  WEB_GET_PASSWORD_URL			"http://60.173.242.117:9001/site/reset-pwd"
//#define  WEB_WALLET_URL					"http://60.173.242.117:9001/payment"
//#define  WEB_FILE_URL                   "http://60.173.242.117:9001/api/file/"
//
//#define  WEB_DOMAIN_URL					"http://10.0.9.7/"
//#define  WEB_SERVER_BASE_URL			"http://10.0.9.7/api/"
//#define  WEB_SERVER_SITE_URL			"http://10.0.9.7/api/site/"
//#define  WEB_GET_PASSWORD_URL			"http://10.0.9.7/site/reset-pwd"
//#define  WEB_WALLET_URL					"http://10.0.9.7/payment"
//#define  WEB_FILE_URL                   "http://10.0.9.7/api/file/"


#define  MMPC_DOMAIN					"@test-d/mmpc"
#define  MMPC_MUC_DOMAIN				"@muc.test-d"
#define  MMPC_EMPTYSTR					""
#define  MMPC_ACCOUNT					"@test-d"
#define  DEF_AVATAR_NOR					"defalut_avatar_nor.png"
#define  DEF_AVATAR_GRAY				"default_avatar_gray.png"
#define  ORGANIZATION_MYFRIEND			_T("我的好友")
#define  DEF_ORGID_MYFRIEND				"-1"						//我的好友的默认的ID账号
#define  SYS_NOTIFY_ACONT_NUM			"10000"						//系统通知的账号
#define  SYS_NOTIFY_ACCOUNT				"10000@test-d"				//系统通知的默认账号
#define  SYS_GROUP_CHAT_NUM				"10001"						//群组聊天账号
#define  SYS_GROUP_CHAT_ACCOUNT			"10001@test-d"				//群组聊天的系统账号
#define  SYSNOTIFY_AVATAR				"default_sys_notify.png"	//系统通知的默认图标
#define  DEF_PACK_ICON					"PackList\\pack.png"		//默认的资料包图标

#define MAX_SEND_MSG_LEN 1024

#define GRAY_TEXT_COLOR 0x00B4B4B4
#define BLACK_TEXT_COLOR 0xFF000000
#define MAX_AUTH_MSG_LEN 24

enum eUserMsg{	 
	WM_REMOVE_USER = WM_USER+103,
	WM_SEND_MSG,
	WM_NET_PHONE,
	WM_MOBILE_PHONE,
	WM_BUSINESS,
	WM_UCPAAS_NOTIFY,
	WM_CALL_IP_PHONE,
	WM_CALL_SMART_PHONE,
	WM_CALL_BACK,
	WM_VOICE_AUTO,
	WM_VIDEO_CALL,
	WM_SYS_CHANGE_PASSWORD,
	WM_SYS_CREATE_ORG,
	WM_SYS_USER_INFO,
	WM_SYS_ORG_INFO,
	WM_SYS_CHANGE_ORGANIZATION_INFO,
	WM_SYS_EMPLOYEE_MANAGE,
	WM_SYS_ABOUT,
	WM_SYS_RESET_PASSWORD,
	WM_SYS_LOG_OFF,
	WM_SYS_QUIT,
	WM_SYS_QUIT_ORG,
	WM_UPDATE_PRESENCE_STATUS,
	WM_TIEMER_UPDATE_FRIEND_LIST,
	WM_TIMER_TOPWND,
	WM_TIMER_TOOLTIP_BOX_HIDE,
	WM_MODIFY_ORG_INFO,
	WM_CREATE_ORG,
	WM_EXIT_ORG,
	WM_TIEMER_UPDATE_HOMEPAGE_PIC,
	WM_TIMER_UPDATE_ROOM_LIST,
	WM_TIMER_DELETE_ROOM_LIST,
	WM_TIMER_CHATLISTENDDOWN,
	WM_TIMER_GROUPCHATPAGEENDDOWN,
	WM_REMOVE_CONTACT,
	WM_GROUPDOWNLOAD_PACKFILE_DOWNLOADED,
};


//定义最近联系人tab下page索引值:layout =  "RecentChatTabLayout"
const int CONST_RECENT_TAB_P2P_CHAT_LIST = 0;		//一对一聊天页面	
const int CONST_RECENT_TAB_SYS_MSG_PAGE	 = 1;		//系统消息页面
const int CONST_RECENT_TAB_HOME_PAGE	 = 2;		//主页页面
const int CONST_RECENT_TAB_GROUPCHAT_PAGE= 3;		//群聊天页面


//群聊天页面，右边的两个page的索引值： layout= "GroupChatFileLayout"
const int CONST_GROUPCHAT_FILE_EDIT_PAGE = 0;		//编辑资料页面
const int CONST_GROUPCHAT_FILE_LIST_PAGE = 1;		//资料包列表页面
const int CONST_GROUPCHAT_CONTRACT_PAGE  = 2;       //合约编辑页面
#define CONTRACT_IDENTITY_PAYEE			 _T("1")       //收款人
#define CONTRACT_IDENTITY_DRAWEE         _T("2")       //付款人
#define CONTRACT_PAY_TYPE_NONE			 _T("1")		 //不委托平台记账
#define CONTRACT_PAY_TYPE_SURE           _T("2")       //担保交易
#define CONTRACT_PAY_TYPE_AGREE			 _T("3")      //协议支付
#define CONTRACT_PAY_TYPE_LOGISTICS      _T("4")


#define ORG_PROPERTY_LENGTH 4
const TCHAR g_OrgProperty[ORG_PROPERTY_LENGTH][MAX_PATH] = {_T("物流商"), _T("修理厂"), _T("经销商"), _T("批发商")};