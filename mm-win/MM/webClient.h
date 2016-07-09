#pragma once
#include <string>
#include <map>
using namespace std;
#include "UIDefine.h"


typedef map<tstring, orgInfo> t_orgInfoMap;
typedef map<tstring, orgInfo>::iterator t_orgInfoMapIter;

class CWebClient{
public:
	static bool IsValidUser(const tstring& strPhoneNo);
	static bool InviteBussinessParter(const tstring& strPhoneNo, tstring& strErrDescription);
	static bool GetOrgByPhone( tstring strPhone, orgInfo& _orgInfo,  tstring strOrgId = (_T("")));
	static bool GetOrgById(tstring strId, orgInfo& org);
	static bool GetMyOrg(orgInfo& org);
	static void ClearOrgCache();
	static bool api_getRegions();
	static tstring getRegions();
	static tstring api_getPacks(tstring& strBid, tstring& strStatus);
	static bool updatePackStatus(tstring& strPackId, tstring &strStatus);
	static bool getFileByUrl(tstring& strUrl, tstring& strFilePath);
	static bool getOrgAvatar(tstring strOrgId, tstring& strOutFilePath);
	static bool getBidByRid(tstring& strRid, tstring& strBid);
	static bool addOrgFriend(const tstring& strOrgId);
	static bool delOrgFriend(tstring strOrgId);
	static bool isOrgBuddy(tstring strOrgId, tstring strOrgidDest);
	static bool approveOrgBuddyRequest(tstring strReqId,bool isAgree);
	static bool GetUidAndSid(tstring& uid, tstring& sid);
	static bool SetVerison(tstring strVersion);
	static tstring GetVersion();
	static tstring GetVerifyCode(const tstring& phone);
	static bool GetTheOtherSideOrgId(const vector<tstring>& room_member, const tstring& my_account,tstring& other_side_id);
	static bool getContractInfo(tstring& strResponsInfo,const tstring& status = _T("1"),const tstring& type = (_T("")),const tstring& stime=(_T("")),const tstring& etime=(_T("")),const tstring& key=(_T("")));
	static bool getContractFiles(tstring& strRespons,const tstring& strPackId,const tstring& strOrgId);
	static bool addContract(const vector<tstring>& vecInfo,tstring& strRespons);

	//����ҵ�����ϣ�֧��ģ����ѯ��
	static tstring getFindFiles(tstring strFileName, tstring strbid, tstring strSender, tstring strSendTime,
	tstring strSendEtime, tstring strRecv, tstring strRecvTime, tstring strRecvEtime);
private:
	static t_orgInfoMap m_orgMap;
	static tstring m_strRegions;
};