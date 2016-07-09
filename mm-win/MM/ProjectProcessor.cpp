#include "ProjectProcessor.h"
#include "HttpClient.h"
#include "Controller.h"
#include "StringHelper.h"
#include "..\json\json.h"
#include <sstream>
#include "MsgBox.h"
#include "..\Include\Global_Def.h"
#include "MUCRoomMgr.h"

using namespace std;

CProjectProcessor::CProjectProcessor()
{

}

CProjectProcessor::~CProjectProcessor()
{

}

//注意，按照目前的逻辑 from_time 只有当天一个选择，所以默认不加
//并且，加上from_time之后，向服务器查询是有问题的。是否from_time 要写成 2016-06-23 00:00:00才能正确，目前没有验证
//by zyy

//注意，最终原因发现url不能识别空格，需要加空格转换成+
bool CProjectProcessor::GetProject(const std::tstring& strTime, std::vector<sProjectItem>& vecProjs,const std::tstring& strSearch)
{
	vecProjs.clear();

	tstring endtime("");
	while(true)
	{
		
		tstring strUrlNewProj(WEB_SERVER_BASE_URL);
		strUrlNewProj += "business/get-business-and-orgs";

// 		if (!endtime.empty())
// 		{
// 			int pos = endtime.find(" ");
// 			endtime.replace(pos, 1, "+");
// 			strUrlNewProj += "?end_time=";
// 			strUrlNewProj += endtime;			
// 		}


		//注意，这里是有from_time格式的写法
		if (!strTime.empty())
		{
			strUrlNewProj += "?from_time=";
			strUrlNewProj += strTime;
		}
		if (!endtime.empty())
		{
			int pos = endtime.find(" ");
			endtime.replace(pos, 1, "+");
			if (strTime.empty())
			{
				strUrlNewProj += "?end_time=";
				strUrlNewProj += endtime;
			}
			else
			{
				strUrlNewProj += "&end_time=";
				strUrlNewProj += endtime;
			}
		}


		CHttpClient reqClient;
		tstring strResponse;
		tstring strToken = CController::Instance()->GetToken();
//		tstring sReplace;
//		sReplace = strUrlNewProj.replace(strUrlNewProj.find(" "), 1, "+");
//		tstring strInputUtf8;
//		CChineseCode::GB2312ToUTF_8(strInputUtf8, strUrlNewProj.c_str(), strUrlNewProj.length());

		int nSucc = reqClient.Get(strUrlNewProj, strToken, strResponse);

		tstring strOut;
		CChineseCode::UTF_8ToGB2312(strOut, (char*)strResponse.c_str(), strResponse.size());

		if (!strOut.empty())
		{
			//弱判断：是否是正确的结果字符串
			if (strOut.find("id") != -1 
				&& strOut.find("name") != -1 
				&& strOut.find("create_time") != -1 
				&& strOut.find("update_time") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				if (reader.parse(strOut, root))
				{
					int nSize = root.size();
					for (int i=0; i<nSize; i++)
					{
						sProjectItem oneProj;
//						endtime = root[i]["update_time"].asString();
						oneProj.strProjID = root[i]["id"].asString();
						oneProj.strProjName = root[i]["name"].asString();
						oneProj.strCreateTime = root[i]["create_time"].asString();
						endtime = oneProj.strCreateTime;


						//机构信息：目前一个项目下基本上只挂了一个room/机构
						if (root[i]["org"].size()>0)
						{
							for (int j=0; j<root[i]["org"].size(); j++)
							{
								oneProj.strDestOrgID = root[i]["org"][j]["id"].asString();
								oneProj.strDestOrgName = root[i]["org"][j]["name"].asString();
								oneProj.strRoomID = root[i]["org"][j]["room_id"].asString();
							}
						}
						//个人信息
						if (root[i]["participants"].size()>0)
						{
							for (int z=0; z<root[i]["participants"].size(); z++)
							{
								oneProj.vecDestParticipants.push_back(root[i]["participants"][z]["cell_phone"].asString());
								oneProj.strRoomID = root[i]["participants"][z]["room_id"].asString();
							}
						}

						if (strSearch == ""||oneProj.strProjName.find(strSearch) != tstring::npos||oneProj.strDestOrgName.find(strSearch) != tstring::npos)
						{

							vecProjs.push_back(oneProj);

							//cache..
							m_mapID2Projects.insert(make_pair(oneProj.strProjID, oneProj));
						}
					}
				}
			}	
			else if(_tcsicmp(strOut.c_str(), "[]") != 0)
			{
				CMsgBox* pMsgBox = new CMsgBox("错误",_T("获取项目列表失败！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal(NULL);	
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}



	

	//
	GetRoomMemList(vecProjs);

	return vecProjs.size()>0;
}

//根据OrgID和Cell Phone获取业务往来。
bool CProjectProcessor::GetProjectsByOrgId(const std::tstring& strOrgId, std::vector<sProjectItem>& vecProjs)
{
	vecProjs.clear();

	tstring strUrlNewProj(WEB_SERVER_BASE_URL);
	strUrlNewProj += "business/get-bus-contacts";
	if (!strOrgId.empty())
	{
		strUrlNewProj += "?oid_cell=";
		strUrlNewProj += strOrgId;
	}

	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();
	tstring strInputUtf8;
	int nSucc = reqClient.Get(strUrlNewProj, strToken, strResponse);

	tstring strOut;
	CChineseCode::UTF_8ToGB2312(strOut, (char*)strResponse.c_str(), strResponse.size());

	if (!strOut.empty())
	{
		//弱判断：是否是正确的结果字符串
		if (strOut.find("id") != -1 
			&& strOut.find("name") != -1 
			&& strOut.find("create_time") != -1 
			&& strOut.find("create_by") != -1
			&& strOut.find("org") != -1 )
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(strOut, root))
			{
				int nSize = root.size();
				for (int i=0; i<nSize; i++)
				{
					sProjectItem oneProj;
					oneProj.strProjID = root[i]["id"].asString();
					oneProj.strProjName = root[i]["name"].asString();
					oneProj.strCreateTime = root[i]["create_time"].asString();
					if (root[i]["org"].size()>0)
					{
						for (int j=0; j<root[i]["org"].size(); j++)
						{
							oneProj.strDestOrgID = root[i]["org"][j]["id"].asString();
							oneProj.strDestOrgName = root[i]["org"][j]["name"].asString();
							oneProj.strRoomID = root[i]["org"][j]["room_id"].asString();
						}
					}

					vecProjs.push_back(oneProj);

					//cache..
					m_mapID2Projects.insert(make_pair(oneProj.strProjID, oneProj));
				}
			}
		}	
		else if(_tcsicmp(strOut.c_str(), "[]") != 0)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("获取项目列表失败！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);	
		}
	}

	GetRoomMemList(vecProjs);

	return vecProjs.size()>0;
}

bool CProjectProcessor::GetProjectsByCellPhone(const std::tstring& strCellPhone, std::vector<sProjectItem>& vecProjs)
{
	vecProjs.clear();

	tstring strUrlNewProj(WEB_SERVER_BASE_URL);
	strUrlNewProj += "business/get-bus-contacts";

	if (!strCellPhone.empty())
	{
		strUrlNewProj += "?oid_cell=";
		strUrlNewProj += strCellPhone;
	}

	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();
	tstring strInputUtf8;
	int nSucc = reqClient.Get(strUrlNewProj, strToken, strResponse);

	tstring strOut;
	CChineseCode::UTF_8ToGB2312(strOut, (char*)strResponse.c_str(), strResponse.size());

	if (!strOut.empty())
	{
		//弱判断：是否是正确的结果字符串
		if (strOut.find("id") != -1 
			&& strOut.find("name") != -1 
			&& strOut.find("create_time") != -1 
			&& strOut.find("create_by") != -1
			&& strOut.find("participant") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(strOut, root))
			{
				int nSize = root.size();
				for (int i=0; i<nSize; i++)
				{
					sProjectItem oneProj;
					oneProj.strProjID = root[i]["id"].asString();
					oneProj.strProjName = root[i]["name"].asString();
					oneProj.strCreateTime = root[i]["create_time"].asString();

					if (root[i]["participant"].size()>0)
					{
						for (int z=0; z<root[i]["participant"].size(); z++)
						{
							oneProj.vecDestParticipants.push_back(root[i]["participant"][z]["cell_phone"].asString());
							oneProj.strRoomID = root[i]["participant"][z]["room_id"].asString();
						}
					}

					vecProjs.push_back(oneProj);

					//cache..
					m_mapID2Projects.insert(make_pair(oneProj.strProjID, oneProj));
				}
			}
		}	
		else if(_tcsicmp(strOut.c_str(), "[]") != 0)
		{
			CMsgBox* pMsgBox = new CMsgBox("错误",_T("获取项目列表失败！"),MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);	
		}
	}

	//
	GetRoomMemList(vecProjs);

	return vecProjs.size()>0;
}

void CProjectProcessor::GetRoomMemList(std::vector<sProjectItem>& vecProjs)
{
	int nSize = vecProjs.size();
	for (int i=0; i<nSize; i++)
	{
		sProjectItem& oneProj = vecProjs[i];
		if (!oneProj.strRoomID.empty())
		{
			CController::Instance()->GetRoomManager()->GetMembersByRoomID(oneProj.strRoomID);
		}
	}
}