#pragma once

#include "..\DuiLib\UIlib.h"
#include "..\include\MsgSessionMgrEvent_i.h"



class CFileHelper
{
public:
	CFileHelper(){}
	~CFileHelper(){}

	/**
	* ��ȡ��ǰ��������Ŀ¼
	*/
	static DuiLib::CDuiString GetCurrentAppPath();
	

	/**
	* @strUsername : �����Լ��ı���Ŀ¼
	* ��ȡͷ��cache��·����û�оʹ���
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
	/** �����ļ��У�֧�ִ����༶�ļ���                                                                     */
	/************************************************************************/
	static bool CreateFolder(const TCHAR* pszFolderName);
	static bool IsExistFolder( const DuiLib::CDuiString& strFileName );
	static DuiLib::CDuiString GetUserDir();       
};
