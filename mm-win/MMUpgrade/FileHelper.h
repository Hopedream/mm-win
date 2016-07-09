#pragma once

#include "..\DuiLib\UIlib.h"
#include "..\include\MsgSessionMgrEvent_i.h"



class CFileHelper
{
public:
	CFileHelper(){}
	~CFileHelper(){}

	/**
	* 获取当前进程运行目录
	*/
	static DuiLib::CDuiString GetCurrentAppPath();
	

	///**
	//* @strUsername : 创建自己的本地目录
	//* 获取头像cache的路径，没有就创建
	//*/
	//static DuiLib::CDuiString GetHeadCacheDir();


	//static DuiLib::CDuiString GetFileRecvDir();

	//static string GetPackDir(string& strPackName, string& strPackID);

	//static string GetHigtLightAvatar(string& strAccount);


	static bool IsExistFile(const DuiLib::CDuiString& strFileName);	

	//static ECmdType getFileType(string strType);

	///**
	//* 
	//*/
	//static string GetGrayImageName(string& strHigtImageName);


	//string GetDescription(ECmdType eType);

	///**
	//* @uid : eg:189@test-d ---> 189
	//*/
	//static string GetUserName(string uid)
	//{
	//	string strPhone;
	//	int index = uid.find('@');
	//	if (index > 0)
	//	{
	//		strPhone = uid.substr(0, index);
	//	}
	//	else
	//	{
	//		strPhone = uid;
	//	}

	//	return strPhone;
	//}

	/************************************************************************/
	/** 创建文件夹，支持创建多级文件夹                                                                     */
	/************************************************************************/
	static bool CreateFolder(const TCHAR* pszFolderName);
	static bool IsExistFolder( const DuiLib::CDuiString& strFileName );
	static DuiLib::CDuiString GetUserDir();    
	static int getFileSize(const string& strPath);
	static bool getSelectedFiles(vector<string> &, char* lpFilter = "All\0*.*\0Text\0*.TXT\0", bool bMulti=true);
	static bool getPathIniFile(string&);
};
