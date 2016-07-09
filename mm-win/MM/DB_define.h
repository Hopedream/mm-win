#pragma once

#include <string>

using namespace std;

typedef /*int*/tstring IM_UID /*uin*/;		// 用户标识数字,64bit
typedef int GroupId /*GroupId*/;	// 群id
typedef unsigned int IM_uint32 ;
typedef unsigned char IM_uint8;
typedef int IM_int32;
typedef char IM_int8;


//
//字体结构信息 todo 待优化
//
#pragma pack(push,1)

typedef struct 
{
	IM_uint32 cbSize;
	IM_uint32 uFontSize;
	IM_uint32 uEffect;
	IM_int32 uColor;
	CHAR pFontName[32];
}FontInfo;

struct _tUserOption
{
	IM_uint8 auth_type;
	char     question[128];
	char     answer[128];
};

struct _tDetailedUserInfo
{
	IM_uint8 age;
	IM_uint8 birthday_type;
	IM_uint8 birthday_month;
	IM_uint8 birthday_day;
	char     home_region[50];
	char     home_city[50];
	char     home_province[50];
	char     local_region[50];
	char     local_city[50];
	char     local_province[50];
	char     college[64];
	char     company[64];
	char     carrier[64];
	char     home_page[128];
	char     e_mail[32];
	char     mobile_num[11];
	char     telphone[13];
	_tUserOption userOption;
};

struct _tUserInfo
{
	IM_uint32 good_uid;
	char      name[50];
	IM_uint8  gender;
	IM_uint32 pic_id;
	char      signature[128];
	IM_uint8  save_msg;
	IM_uint32 option_mask;
};


struct _tChatMessage
{
	IM_UID strDestUser;
	tstring strChatText;
	IM_uint32 m_timeChat;
	tstring strTimeStrFormat;
	_tChatMessage()
	{
		m_timeChat = 0;
	}
};

struct _tGroupChatMessage
{
	tstring strRoomID;
	tstring strSenderPhone;
	tstring strSenderAccount;
	tstring strMsg;
	IM_uint32 m_timeChat;
	tstring strTimeString;
	_tGroupChatMessage()
	{
		m_timeChat = 0;
	}
};

struct sLoginUser
{
	tstring strPhoneNum;
	tstring strPwd;
	__int64 timeLogin;
	bool bRemPwd;
	bool bAutoLogin;
	sLoginUser()
	{
		strPhoneNum=(_T(""));
		strPwd=(_T(""));
		timeLogin=0;
		bRemPwd = false;
		bAutoLogin = false;
	}
};