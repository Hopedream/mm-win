
#pragma once
#include <string>
#include <list>
#include <vector>
using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
//gloox里面定义的VCard信息很丰富，为了不让用户直接依赖VCard，定义了部分用户关心的数据

typedef struct
{
	tstring strAccount;
	tstring strUserNickname;
	tstring strRemark;		//备注
	tstring strGender;		//性别
	tstring strSignature;	//签名
	tstring strPhone;
	tstring strEmail;
	tstring strAddr;
	tstring strPhotoType;	//头像类型
	tstring strPhotoBinary;	//头像base64
	tstring strUrl;			//头像的Url地址
	tstring strPhotoPath;	//在线头像本地路径
	tstring strGrayAvatar;	//离线头像本地路径
	tstring strAge;
	tstring strOrganization; //单位 
	tstring strVocation;     //行业
	tstring strTelephone;    //电话
}sUserVCard;

typedef map<tstring, sUserVCard> MapAccount2VCards;


enum EUsrState
{
	enmState_Online		= 0,
	enmState_Offline	= 1,
	enmState_Busy		= 2,
};

typedef struct tagRosterItem 
{
	tstring strAccount;
	tstring strMemo;			//备注名
	list<tstring> ListResources;	//资源名
	tstring strOrgID;			//机构ID，标示唯一的机构
	tstring strOrgnization;		//机构名,目前MM约定一个用户只能属于一个机构
	EUsrState eStatus;			//好友状态信息
	tstring strBusyTips;			//忙碌状态的签名
	tagRosterItem()
	{
		eStatus = enmState_Offline;
	}
} sRosterItemLocal;
typedef map<tstring, sRosterItemLocal> MapAllRosters;


struct sFriendListItem
{
	int	   nItemID;				//标识Item的
	tstring strAccount;			//用户账号
	tstring strOrgnization;		//机构名
	tstring strOrgID;			//机构ID
	tstring strShowName;			//显示原则：备注>昵称>账号
	tstring strAvatar;			//用户头像
	tstring strSignatrue;		//用户签名/手机号/公司名称
	EUsrState eState;			//用户好友的状态信息
	sFriendListItem()
	{
		nItemID = -1;
		eState = enmState_Offline;
	}
};

typedef std::vector<sFriendListItem> vecFriendListItem;
typedef std::vector<sFriendListItem>::iterator vecFriendListItemIter;