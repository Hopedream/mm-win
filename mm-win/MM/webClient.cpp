#include "StdAfx.h"
#include "webClient.h"
#include "HttpClient.h"
#include "../json/json.h"
#include "../include/Global_Def.h"
#include "Controller.h"
#include "../Common/Logger.h"
#include "Utility.h"
#include "StringHelper.h"
#include "FileHelper.h"
#include "MagicKHelper.h"
#include "../gloox-1.0.14/src/base64.h"

bool CWebClient::IsValidUser( const tstring& strPhoneNo )
{
	tstring strhttpUrl(WEB_SERVER_SITE_URL);
	strhttpUrl += "is-user-existed?cell_phone=";
	strhttpUrl += strPhoneNo;

	tstring strResponse;
	CHttpClient reqClient;
	reqClient.Get(strhttpUrl.c_str(), strResponse);
	if(strResponse.find("result") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strResponse, root))
			{
				return root["result"].asInt() == 1;
			}
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
		}	
	}
	

	return false;
}


bool CWebClient::GetMyOrg(orgInfo& org)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost; // = _T("oid=");

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("name") != -1 && strHttpResponse.find("detail_address") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(CChineseCode::DecodeUTF8(strHttpResponse), root) && root["result"].isNull())
			{
				org.strId =  root["id"].asString();
				org.strMemberId = root["mem_id"].asString();
				org.strName =  root["name"].asString();
				org.strAddress =  root["address"].asString();
				org.strDetailAddr =  root["detail_address"].asString();
				org.strTelephone =  root["telephone"].asString();
				return true;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strHttpResponse.c_str());
		}
	}
	
	return false;
}


bool CWebClient::InviteBussinessParter(const tstring& strPhoneNo, tstring& strErrDescription)
{
	tstring strhttpUrl(WEB_SERVER_BASE_URL);
	strhttpUrl += "user/invite";

	tstring strPost = "phones=[\"";
	strPost += strPhoneNo;
	strPost += "\"]";


	tstring strResponse;
	CHttpClient reqClient;
	int nResult = reqClient.Post(strhttpUrl, strPost, CController::Instance()->GetToken(),strResponse);

	if (strResponse.find("result") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(strResponse, root))
			{
				return root["result"].asInt() == 1;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strResponse.c_str());
		}
	}
	
	

	return false;
}

bool CWebClient::GetOrgByPhone( tstring strPhone, orgInfo& _orgInfo, tstring strOrgId)
{
	t_orgInfoMapIter iter = m_orgMap.find(strOrgId);
	if (m_orgMap.end() != iter)
	{
		_orgInfo = iter->second;
		return true;
	}

	if (tstring::npos != strPhone.find("@"))
	{
		strPhone = ExtractPhoneNo(strPhone);
	}
	tstring strhttpUrl(WEB_SERVER_SITE_URL);
	strhttpUrl += "get-org-by-cell?cell_phone=";
	strhttpUrl += strPhone;

	tstring strResponse;
	CHttpClient reqClient;
	reqClient.Get(strhttpUrl.c_str(), strResponse);
	if (strResponse.find("name") != -1 && strResponse.find("telephone") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(CChineseCode::DecodeUTF8(strResponse), root) && root["result"].isNull())
			{
				_orgInfo.strName = root["name"].asString();
				_orgInfo.strTelephone = root["telephone"].asString();
				_orgInfo.strId = root["id"].asString();
				_orgInfo.strMemberId = root["mem_id"].asString();
				_orgInfo.strDetailAddr = root["detail_address"].asString();
			}
			if (_orgInfo.strId != "")
				m_orgMap.insert(make_pair(_orgInfo.strId, _orgInfo));
			return true;
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
		}	
	}
	

	return false;
}

void CWebClient::ClearOrgCache()
{
	m_orgMap.clear();
}

bool CWebClient::api_getRegions()
{
	m_strRegions.clear();
	tstring strhttpUrl(WEB_SERVER_SITE_URL);
	strhttpUrl += "get-regions";

	//tstring strResponse;
	CHttpClient reqClient;
	int nResult = reqClient.Get(strhttpUrl,m_strRegions);
	if (m_strRegions.find("result") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(m_strRegions, root))
			{
				return !root["result"].asInt() == 0;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", m_strRegions.c_str());
		}
	}
	
	return true;
}

std::tstring CWebClient::getRegions()
{
	return m_strRegions;
}

std::tstring CWebClient::api_getPacks( tstring& strBid, tstring& strStatus )
{
	tstring strhttpUrl(WEB_FILE_URL);
	strhttpUrl += "get-packs-by-status?bid=";
	strhttpUrl += strBid;
	strhttpUrl += "&status=";
	strhttpUrl += strStatus;
	tstring strResponse;
	CHttpClient reqClient;
	tstring strToken = CController::Instance()->GetToken();
	int nResult = reqClient.Get(strhttpUrl, strToken, strResponse);
	return CChineseCode::DecodeUTF8(strResponse);
}

bool CWebClient::getBidByRid(tstring& strRid, tstring& strBid)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("business/get-bid-by-rid?");
	strUrl += _T("rid=");
	strUrl += strRid;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("bid") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(strHttpResponse, root))
			{
				strBid =  root["bid"].asString();
				return true;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strHttpResponse.c_str());
			return false;
		}
	}
	else
		return false;

}

bool CWebClient::GetOrgById( tstring strId, orgInfo& org )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org-by-id?");
	strUrl += _T("oid=");
	strUrl += strId;

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("name") != -1 && strHttpResponse.find("detail_address") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(CChineseCode::DecodeUTF8(strHttpResponse), root) && root["result"].isNull())
			{
				org.strId =  root["id"].asString();
				org.strMemberId = root["mem_id"].asString();
				org.strName =  root["name"].asString();
				org.strAddress =  root["address"].asString();
				org.strCellPhone = root["phone"].asString();
				org.strDetailAddr =  root["detail_address"].asString();
				org.strTelephone =  root["telephone"].asString();
				return true;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strHttpResponse.c_str());
		}
	}

	
	return false;
}

bool CWebClient::updatePackStatus( tstring& strPackId, tstring &strStatus )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("file/update-pack-status?");
	strUrl += _T("pack_id=");
	strUrl += strPackId;
	strUrl += _T("&status=");
	strUrl += strStatus;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);

	if (strHttpResponse.find("result") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(strHttpResponse, root))
			{
				root["result"].asInt() == 1;
				return true;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strHttpResponse.c_str());
		}
	}
	
	return false;
}

bool CWebClient::getFileByUrl( tstring& strUrl, tstring& strFilePath )
{
	
	return true;
	//CInternetSession session;  
	//CHttpFile *httpFile = (CHttpFile *)session.OpenURL(URL);  
	//CStdioFile imgFile;  
	//char buff[1024];    // 缓存  

	//imgFile.Open("图片名字.png", CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);  

	//DWORD dwStatusCode;  
	//httpFile->QueryInfoStatusCode(dwStatusCode);  

	//if(dwStatusCode == HTTP_STATUS_OK) {  
	//	int size=0;  
	//	do {  
	//		size = httpFile->Read(buff,1024);    // 读取图片  
	//		imgFile.Write(buff,size);  
	//	}while(size > 0);  
	//}  

	//httpFile->Close();  
	//session.Close();   
}

bool CWebClient::getOrgAvatar( tstring strOrgId, tstring& strOutFilePath )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org-avatar?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	tstring strNameDefault = strOrgId;
	strPost += strNameDefault;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		tstring strAva;
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strHttpResponse, root) && strHttpResponse.find("avatar") != -1)
		{
			strAva = root["avatar"].asString();
		}
		else
		{
			return false;
		}
		tstring strResponse;
		//int res = httpClient.Get(strAva, strResponse);

		DuiLib::CDuiString strAvatar  = CFileHelper::GetHeadCacheDir();
		strAvatar += strOrgId.c_str();
		strAvatar += getTmpFileId().c_str();
		strAva = Base64::decode64(strAva);
		try{
			if(CMagicKHelper::Instance()->GeneratePng(/*strResponse*/strAva, tstring(strAvatar.GetData())))
			{
				tstring strPhotoPath = tstring(strAvatar+_T(".png"));
				strOutFilePath = strPhotoPath;
				//m_pBtnSelect->SetBkImage(strPhotoPath.c_str());
			}
			return true;
		}
		catch(...)
		{
			return false;
		}
		
	}
}

bool CWebClient::addOrgFriend(const tstring& strOrgId)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org-buddy/buddy-request?");
	tstring strPost = _T("");
	strPost += _T("to_org_id=");
	strPost += strOrgId;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		if (strHttpResponse.find("\"result\":1") != -1)
			return true;
		else
			return false;
	}
}

bool CWebClient::delOrgFriend( tstring strOrgId )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org-buddy/remove-org-buddy?");
	tstring strPost = _T("");
	strPost += _T("to_oid=");
	strPost += strOrgId;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		if (strHttpResponse.find("\"result\":1") != -1)
			return true;
		else
			return false;
	}
}

bool CWebClient::isOrgBuddy( tstring strOrgId, tstring strOrgidDest )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("site/is-org-buddy?");
	tstring strPost = _T("");
	strPost += _T("oid1=");
	strPost += strOrgId;
	strPost += "&oid2=";
	strPost += strOrgidDest;
	strUrl += strPost;
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		if (strHttpResponse.find("\"result\":1") != -1)
			return true;
		else
			return false;
	}
}

bool CWebClient::approveOrgBuddyRequest( tstring strReqId,bool isAgree)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org-buddy/approve-request?");
	tstring strPost = _T("");
	strPost += _T("request_id=");
	strPost += strReqId;
	strPost += _T("&approve_result=");
	strPost += isAgree ? _T("1") : _T("0");
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		if (strHttpResponse.find("\"result\":1") != -1)
			return true;
		else
			return false;
	}
}

//获取uid，sid
bool CWebClient::GetUidAndSid(tstring& uid, tstring& sid)
{
	bool ret(false);
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += "user/get-key";

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") != -1 && strHttpResponse.find("uid") != -1 
		&& strHttpResponse.find("key") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strHttpResponse, root))
			{
				sid = root["key"].asString();
				uid = root["uid"].asString();
				ret = true;
			}
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strHttpResponse.c_str());	
			ret = false;
		}	
	}
	else
	{
		//北京调试注销
		//				return;
		ret = false;
	}
	return ret;
}

bool CWebClient::SetVerison(tstring strVersion)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("site/save-version");
	//tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strVersion,strHttpResponse);
	if (retCode == 1)
	{
		return true;
	}
	
	return false;
}
bool CWebClient::getContractFiles(tstring& strRespons,const tstring& strPackId,const tstring& strOrgId)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("file/get-signed-pack-files?");
	strUrl += _T("pid=");
	strUrl += strPackId;
	strUrl += _T("&oid=");
	strUrl += strOrgId;

	tstring strToken = CController::Instance()->GetToken();

	tstring strUTF8Url = CChineseCode::EncodeUTF8(strUrl);
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUTF8Url, strToken, strHttpResponse);
	strRespons = CChineseCode::DecodeUTF8(strHttpResponse);

	return retCode;
}
bool CWebClient::getContractInfo(tstring& strResponsInfo,const tstring& status /* = 1 */,const tstring& type /* = "" */,const tstring& stime/* ="" */,const tstring& etime/* ="" */,const tstring& key/* ="" */)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("file/get-signed-list?");
	strUrl += "status=";
	strUrl += status;

	if(!type.empty())
	{
		strUrl += "&pay_type=";
		strUrl += type;
	}

	if(!stime.empty())
	{
		strUrl += "&stime=";
		strUrl += stime;
	}

	if(!etime.empty())
	{
		strUrl += "&etime=";
		strUrl += etime;
	}

	if(!key.empty())
	{
		strUrl += "&key=";
		strUrl += key;
	}

	tstring strToken = CController::Instance()->GetToken();

	tstring strUTF8Url = CChineseCode::EncodeUTF8(strUrl);
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUTF8Url, strToken, strHttpResponse);
	strResponsInfo = CChineseCode::DecodeUTF8(strHttpResponse);

	return retCode;
}

tstring CWebClient::GetVersion()
{
	tstring strhttpUrl(WEB_SERVER_BASE_URL);
	strhttpUrl += "site/get-version";
	tstring strResponse;
	CHttpClient reqClient;
	int nResult = reqClient.Get(strhttpUrl, strResponse);
	return CChineseCode::DecodeUTF8(strResponse);
}

tstring CWebClient::GetVerifyCode(const tstring& phone)
{
	tstring strhttpUrl(WEB_SERVER_BASE_URL);
	strhttpUrl += "site/get-vcode-by-cell?cell=";
	strhttpUrl += phone;
	tstring strResponse;
	CHttpClient reqClient;
	int nResult = reqClient.Get(strhttpUrl, strResponse);
	return CChineseCode::DecodeUTF8(strResponse);
}

bool CWebClient::GetTheOtherSideOrgId( const vector<tstring>& room_member, const tstring& my_account, tstring& other_side_id )
{
	orgInfo info;
	GetOrgByPhone(my_account, info);
	tstring my_org_id = info.strId;
	for (int i=0;i!=room_member.size();i++)
	{
		orgInfo info;
		GetOrgByPhone(room_member[i], info);
		if (info.strId != my_org_id)
		{
			other_side_id = info.strId;
			return true;
		}
	}
	return false;
}
bool CWebClient::addContract(const vector<tstring>& vecInfo,tstring& strRespons)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("file/create-agr-pack?");

	tstring tmp;
	strUrl += "name=";
	strUrl += vecInfo[0];

	strUrl += "&status=";
	strUrl += vecInfo[1];

	strUrl += "&money=";
	strUrl += vecInfo[2];

	strUrl += "&pay_type=";
	strUrl += vecInfo[3];

	strUrl += "&rec_oid=";
	strUrl += vecInfo[4];

	tmp = vecInfo[5];
	if(!tmp.empty())
	{
		strUrl += "&desc=";
		strUrl += vecInfo[5];
	}

	tmp = vecInfo[6];
	if(!tmp.empty())
	{
		strUrl += "&bid=";
		strUrl += vecInfo[6];
	}

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;

	tstring strUTF8Url = CChineseCode::EncodeUTF8(strUrl);
	int retCode = httpClient.Get(strUTF8Url, strToken, strHttpResponse);

	strRespons = CChineseCode::DecodeUTF8(strHttpResponse);
	return true;
}
tstring CWebClient::getFindFiles( tstring strFileName, tstring strbid, tstring strSender, tstring strSendTime, 
	tstring strSendEtime, tstring strRecv, tstring strRecvTime, tstring strRecvEtime )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("file/find-files?");
	tstring strPost;
	bool bfirst = true;
	if (strFileName != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "name=";
		strPost += strFileName;
	}
	if (strbid != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "bid=";
		strPost += strbid;
	}
	if (strSender != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "sender_uname=";
		strPost += strSender;
	}
	if (strSendTime != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "send_stime=";
		strPost += strSendTime;
	}
	if (strSendEtime != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "send_etime=";
		strPost += strSendEtime;
	}
	if (strRecv != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "receiver_oname=";
		strPost += strRecv;
	}
	if (strRecvTime != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "receive_stime=";
		strPost += strRecvTime;
	}
	if (strRecvEtime != "")
	{
		if (!bfirst)
		{
			strPost+="&";
		}
		bfirst = false;
		strPost += "receive_etime=";
		strPost += strRecvEtime;
	}
	//strPost = "";
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost,CController::Instance()->GetToken(),strHttpResponse);
	if (retCode == 0)
	{
		return strHttpResponse;
	}

	return "";
}

t_orgInfoMap CWebClient::m_orgMap;
tstring CWebClient::m_strRegions;

