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
	

	/**
	* @strUsername : 创建自己的本地目录
	* 获取头像cache的路径，没有就创建
	*/
	static DuiLib::CDuiString GetHeadCacheDir();


	static DuiLib::CDuiString GetFileRecvDir();

	static tstring GetPackDir(tstring& strPackName, tstring& strPackID);

	static tstring GetHigtLightAvatar(tstring& strAccount);


	static bool IsExistFile(const DuiLib::CDuiString& strFileName);	

	static ECmdType getFileType(tstring strType);

	/**
	* 
	*/
	static tstring GetGrayImageName(tstring& strHigtImageName);


	tstring GetDescription(ECmdType eType);

	/**
	* @uid : eg:189@test-d ---> 189
	*/
	static tstring GetUserName(tstring uid)
	{
		tstring strPhone;
		int index = uid.find('@');
		if (index > 0)
		{
			strPhone = uid.substr(0, index);
		}
		else
		{
			strPhone = uid;
		}

		return strPhone;
	}

	/************************************************************************/
	/** 创建文件夹，支持创建多级文件夹                                                                     */
	/************************************************************************/
	static bool CreateFolder(const TCHAR* pszFolderName);
	static bool IsExistFolder( const DuiLib::CDuiString& strFileName );
	static DuiLib::CDuiString GetUserDir();       
};
