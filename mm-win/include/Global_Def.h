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


#define  MMPC_DOMAIN					_T("@test-d/mmpc")
#define  MMPC_MUC_DOMAIN				_T("@muc.test-d")
#define  MMPC_EMPTYSTR					_T("")
#define  MMPC_ACCOUNT					_T("@test-d")
#define  DEF_AVATAR_NOR					_T("defalut_avatar_nor.png")
#define  DEF_AVATAR_GRAY				_T("default_avatar_gray.png")
#define  ORGANIZATION_MYFRIEND			_T("�ҵĺ���")
#define  DEF_ORGID_MYFRIEND				_T("-1")						//�ҵĺ��ѵ�Ĭ�ϵ�ID�˺�
#define  SYS_NOTIFY_ACONT_NUM			_T("10000")						//ϵͳ֪ͨ���˺�
#define  SYS_NOTIFY_ACCOUNT				_T("10000@test-d")				//ϵͳ֪ͨ��Ĭ���˺�
#define  SYS_GROUP_CHAT_NUM				_T("10001")						//Ⱥ�������˺�
#define  SYS_GROUP_CHAT_ACCOUNT			_T("10001@test-d")				//Ⱥ�������ϵͳ�˺�
#define  SYSNOTIFY_AVATAR				_T("default_sys_notify.png")	//ϵͳ֪ͨ��Ĭ��ͼ��
#define  DEF_PACK_ICON					_T("PackList\\pack.png")		//Ĭ�ϵ����ϰ�ͼ��

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


//���������ϵ��tab��page����ֵ:layout =  "RecentChatTabLayout"
const int CONST_RECENT_TAB_P2P_CHAT_LIST = 0;		//һ��һ����ҳ��	
const int CONST_RECENT_TAB_SYS_MSG_PAGE	 = 1;		//ϵͳ��Ϣҳ��
const int CONST_RECENT_TAB_HOME_PAGE	 = 2;		//��ҳҳ��
const int CONST_RECENT_TAB_GROUPCHAT_PAGE= 3;		//Ⱥ����ҳ��


//Ⱥ����ҳ�棬�ұߵ�����page������ֵ�� layout= "GroupChatFileLayout"
const int CONST_GROUPCHAT_FILE_EDIT_PAGE = 0;		//�༭����ҳ��
const int CONST_GROUPCHAT_FILE_LIST_PAGE = 1;		//���ϰ��б�ҳ��
const int CONST_GROUPCHAT_CONTRACT_PAGE  = 2;       //��Լ�༭ҳ��
#define CONTRACT_IDENTITY_PAYEE			 _T("1")       //�տ���
#define CONTRACT_IDENTITY_DRAWEE         _T("2")       //������
#define CONTRACT_PAY_TYPE_NONE			 _T("1")		 //��ί��ƽ̨����
#define CONTRACT_PAY_TYPE_SURE           _T("2")       //��������
#define CONTRACT_PAY_TYPE_AGREE			 _T("3")      //Э��֧��
#define CONTRACT_PAY_TYPE_LOGISTICS      _T("4")


#define ORG_PROPERTY_LENGTH 4
const TCHAR g_OrgProperty[ORG_PROPERTY_LENGTH][MAX_PATH] = {_T("������"), _T("����"), _T("������"), _T("������")};