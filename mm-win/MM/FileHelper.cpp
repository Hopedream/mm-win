#include <io.h>
#include "FileHelper.h"
#include "Controller.h"
#include <shlobj.h>  
#include "..\include\Global_Def.h"
#include <Shlwapi.h>
#include <algorithm>


DuiLib::CDuiString CFileHelper::GetCurrentAppPath()
{
	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(NULL, tszModule, MAX_PATH);
	DuiLib::CDuiString sInstancePath = tszModule;
	int pos = sInstancePath.ReverseFind(_T('\\'));
	if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
	return sInstancePath;
}

tstring CFileHelper::GetPackDir(tstring& strPackName, tstring&strPackID)
{
	tstring strPackPath;

	strPackPath = GetFileRecvDir().GetData();

	//对每个文件包，加上Packet
	strPackPath += strPackName;
	strPackPath += "-";
	strPackPath += strPackID;
	strPackPath += "\\";
	if(!PathFileExists(strPackPath.c_str()))
	{
		CreateDirectory(strPackPath.c_str(), NULL);
	}

	return strPackPath;
}

DuiLib::CDuiString CFileHelper::GetFileRecvDir()
{
	tstring strMyCount = CController::Instance()->GetXmppClient()->jid().username();

	DuiLib::CDuiString strAppPath = GetUserDir();
	strAppPath += _T("User\\");
	if (!IsExistFolder(strAppPath.GetData()) )
	{
		CreateDirectory(strAppPath.GetData(), NULL);
	}
	strAppPath += strMyCount.c_str();
	strAppPath += _T("\\");

	if (!IsExistFolder(strAppPath.GetData()))
	{
		CreateDirectory(strAppPath.GetData(), NULL);
	}
	strAppPath += _T("RecvFiles\\");
	if(!IsExistFolder(strAppPath.GetData()))
	{
		CreateDirectory(strAppPath.GetData(), NULL);
	}

	return strAppPath;

}
DuiLib::CDuiString CFileHelper::GetHeadCacheDir()
{
	tstring strMyCount = CController::Instance()->GetXmppClient()->jid().username();

	DuiLib::CDuiString strAppPath = GetUserDir();
	strAppPath += _T("User\\");
	if (!IsExistFolder(strAppPath.GetData()) )
	{
		CreateDirectory(strAppPath.GetData(), NULL);
	}
	strAppPath += strMyCount.c_str();
	strAppPath += _T("\\");

	if (!IsExistFolder(strAppPath.GetData()))
	{
		CreateDirectory(strAppPath.GetData(), NULL);
	}
	strAppPath += _T("Avatar\\");
	if(!IsExistFolder(strAppPath.GetData()))
	{
		CreateDirectory(strAppPath.GetData(), NULL);
	}

	return strAppPath;

} 

tstring CFileHelper::GetDescription(ECmdType eType)
{
	tstring strDes;
	switch (eType)
	{
	case enmCmd_FileReq:
		strDes = "fileRequest";
		break;
	case enmCmd_GroupPackFile:
		strDes = "packRequest";
		break;
	case enmCmd_GroupQianyuePack:
		strDes = "groupcontract";
		break;
	case enmCmd_Undefied:
		strDes = "undefied";
		break;
	case enmCmd_ImgReq:
		strDes = "imgRequest";
		break;
	}

	return strDes;
}

tstring CFileHelper::GetGrayImageName(tstring& strHigtImageName)
{
	int index = strHigtImageName.find(".png");
	tstring strGrayName = strHigtImageName.substr(0, strHigtImageName.length()-4);
	strGrayName += _T("_Gray.png");

	return strGrayName;
}

bool CFileHelper::IsExistFile( const DuiLib::CDuiString& strFileName )
{
	BOOL bR = true;
	DWORD dwAttribute = ::GetFileAttributes(strFileName.GetData());
	if (dwAttribute != -1)
	{
		bR = !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY);
	}
	else
	{
		bR = false;
	}
	return bR; 
}

bool CFileHelper::IsExistFolder( const DuiLib::CDuiString& strFileName )
{
	DWORD dwAttribute = ::GetFileAttributes(strFileName.GetData());
	if (-1 == dwAttribute)
	{
		return false;
	}
	return (dwAttribute & FILE_ATTRIBUTE_DIRECTORY);
}

bool CFileHelper::CreateFolder(const TCHAR* pszFolderName)
{	
	if(access(pszFolderName,0)==0)  //文件或文件夹存在
	{
		if (GetFileAttributes(pszFolderName)&FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}		
	} 
	tstring strFolder = pszFolderName;
	list<tstring> lsFolderList;
	
	if (strFolder.length()-1 == strFolder.find_last_of(_T("\\"))) //delete the last '\'
	{
		strFolder.substr(0,strFolder.length()-1);
	}
	lsFolderList.push_front(strFolder);

	tstring::size_type npos;
	while ((npos = strFolder.find_last_of(_T("\\"))) != tstring::npos)
	{
		strFolder = strFolder.substr(0,npos);
		if (access(strFolder.c_str(),0) != 0)
		{
			lsFolderList.push_front(strFolder);
		}
		else
		{
			break;
		}
	}

	for (list<tstring>::iterator ite = lsFolderList.begin(); ite != lsFolderList.end(); ++ite)
	{
		if (!CreateDirectory(ite->c_str(), NULL)) 
		{ 			
			return false;
		}
	}

	return access(pszFolderName,0)==0;
}

DuiLib::CDuiString CFileHelper::GetUserDir()
{
	TCHAR szPath[MAX_PATH];
	memset(szPath, 0, sizeof(TCHAR)*MAX_PATH);
	SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS, NULL, SHGFP_TYPE_CURRENT, szPath);
	DuiLib::CDuiString strUserDocFolder;// = CDuiString(szPath) + "\\jiaparts\\";
	strUserDocFolder.Format("%s\\jiaparts\\", szPath);
	if (!IsExistFolder(strUserDocFolder.GetData()))
	{
		CreateDirectory(strUserDocFolder.GetData(), NULL);
	}
	return strUserDocFolder;
}


tstring CFileHelper::GetHigtLightAvatar(tstring& strAccount)
{
	tstring strLocalAvatar;
	//头像：从本地cache找
	DuiLib::CDuiString strAvatarPath = GetHeadCacheDir();
	tstring strOnlineAvatar = strAvatarPath.GetData();
	strOnlineAvatar += strAccount;
	strOnlineAvatar += ".png";

	//有些用户没有设置用户头像，就是空的
	bool bExist = PathFileExists(strOnlineAvatar.c_str());
	if (bExist)
	{
		strLocalAvatar = strOnlineAvatar.c_str();
	}
	else
	{
		strLocalAvatar = DEF_AVATAR_NOR;
	}	

	return strLocalAvatar;
}

ECmdType CFileHelper::getFileType( tstring strType )
{
	std::transform(strType.begin(), strType.end(),strType.begin(), ::tolower);
	if(strType.find("png") != -1)
	{
		return ECmdType::enmCmd_ImgReq;
	}
	else if (strType.find("bmp") != -1)
	{
		return ECmdType::enmCmd_ImgReq;
	}
	else if (strType.find("jpg") != -1)
	{
		return ECmdType::enmCmd_ImgReq;
	}
	else
	{
		return ECmdType::enmCmd_FileReq;
	}
}
