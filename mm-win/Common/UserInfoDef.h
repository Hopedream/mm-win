
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
//gloox���涨���VCard��Ϣ�ܷḻ��Ϊ�˲����û�ֱ������VCard�������˲����û����ĵ�����

typedef struct
{
	tstring strAccount;
	tstring strUserNickname;
	tstring strRemark;		//��ע
	tstring strGender;		//�Ա�
	tstring strSignature;	//ǩ��
	tstring strPhone;
	tstring strEmail;
	tstring strAddr;
	tstring strPhotoType;	//ͷ������
	tstring strPhotoBinary;	//ͷ��base64
	tstring strUrl;			//ͷ���Url��ַ
	tstring strPhotoPath;	//����ͷ�񱾵�·��
	tstring strGrayAvatar;	//����ͷ�񱾵�·��
	tstring strAge;
	tstring strOrganization; //��λ 
	tstring strVocation;     //��ҵ
	tstring strTelephone;    //�绰
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
	tstring strMemo;			//��ע��
	list<tstring> ListResources;	//��Դ��
	tstring strOrgID;			//����ID����ʾΨһ�Ļ���
	tstring strOrgnization;		//������,ĿǰMMԼ��һ���û�ֻ������һ������
	EUsrState eStatus;			//����״̬��Ϣ
	tstring strBusyTips;			//æµ״̬��ǩ��
	tagRosterItem()
	{
		eStatus = enmState_Offline;
	}
} sRosterItemLocal;
typedef map<tstring, sRosterItemLocal> MapAllRosters;


struct sFriendListItem
{
	int	   nItemID;				//��ʶItem��
	tstring strAccount;			//�û��˺�
	tstring strOrgnization;		//������
	tstring strOrgID;			//����ID
	tstring strShowName;			//��ʾԭ�򣺱�ע>�ǳ�>�˺�
	tstring strAvatar;			//�û�ͷ��
	tstring strSignatrue;		//�û�ǩ��/�ֻ���/��˾����
	EUsrState eState;			//�û����ѵ�״̬��Ϣ
	sFriendListItem()
	{
		nItemID = -1;
		eState = enmState_Offline;
	}
};

typedef std::vector<sFriendListItem> vecFriendListItem;
typedef std::vector<sFriendListItem>::iterator vecFriendListItemIter;