#include <io.h>
#include "FileHelper.h"
//#include "Controller.h"
#include <shlobj.h>  
//#include "..\include\Global_Def.h"
#include <Shlwapi.h>
#include <algorithm>


int CFileHelper::getFileSize( const string& strPath )
{
	DWORD sz = 0;
	HANDLE handle = CreateFile(_T(strPath.c_str()), FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (handle != INVALID_HANDLE_VALUE)
	{
		sz = GetFileSize(handle, NULL);
		//cout<<size<<endl;
		CloseHandle(handle);
	}
	return sz;
}

bool CFileHelper::getSelectedFiles( vector<string> & lstFiles, char* lpFilter, bool bMulti)
{
	OPENFILENAME ofn;
	TCHAR szOpenFileNames[80*MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szFileName[80*MAX_PATH];

	TCHAR* p;
	int nLen = 0;
	ZeroMemory( &ofn, sizeof(ofn) );
	//// open a file name
	//ZeroMemory( &ofn , sizeof( ofn));
	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrFile = szOpenFileNames ;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szOpenFileNames );
	ofn.lpstrFilter = lpFilter;
	ofn.lpstrTitle = "请选择要上传的文件";
	//ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	//ofn.nFilterIndex =1;
	//ofn.lpstrFileTitle = NULL ;
	//ofn.nMaxFileTitle = 0 ;
	//ofn.lpstrInitialDir=NULL ;
	if (bMulti)
		ofn.Flags = OFN_EXPLORER |OFN_ALLOWMULTISELECT ;
	else
	{
		ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}
	//ofn.Flags = OFN_EXPLORER;// |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if( GetOpenFileName( &ofn ) )

	{  

		//把第一个文件名前的复制到szPath,即:
		//如果只选了一个文件,就复制到最后一个'/'
		//如果选了多个文件,就复制到第一个NULL字符
		lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset );

		//当只选了一个文件时,下面这个NULL字符是必需的.
		//这里不区别对待选了一个和多个文件的情况

		szPath[ ofn.nFileOffset ] = '/0';
		nLen = lstrlen(szPath);

		if( szPath[nLen-1] != '\\' )   //如果选了多个文件,则必须加上'//'

		{
			lstrcat(szPath, TEXT("\\"));
		}


		p = szOpenFileNames + ofn.nFileOffset; //把指针移到第一个文件
		ZeroMemory(szFileName, sizeof(szFileName));

		while( *p )
		{   
			lstrcat(szFileName, szPath);  //给文件名加上路径  
			lstrcat(szFileName, p);    //加上文件名  
			//lstrcat(szFileName, TEXT("/n")); //换行   

			lstFiles.push_back(szFileName);
			szFileName[0] = '\0';
			p += lstrlen(p) +1;     //移至下一个文件
		}
		//MessageBox(NULL, szFileName, TEXT("MultiSelect"), MB_OK);
	}
	else 
		return 0;
	return 1;
}

bool CFileHelper::getPathIniFile( string& strPathIniFile)
{
	OPENFILENAME ofn;
	TCHAR szOpenFileNames[80*MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szFileName[80*MAX_PATH];

	TCHAR* p;
	int nLen = 0;
	ZeroMemory( &ofn, sizeof(ofn) );
	//// open a file name
	//ZeroMemory( &ofn , sizeof( ofn));
	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrFile = szOpenFileNames ;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szOpenFileNames );
	ofn.lpstrFilter = "ini\0*.ini";
	ofn.lpstrTitle = "请选择路径指向ini文件";
	//ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	//ofn.nFilterIndex =1;
	//ofn.lpstrFileTitle = NULL ;
	//ofn.nMaxFileTitle = 0 ;
	//ofn.lpstrInitialDir=NULL ;
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if( GetOpenFileName( &ofn ) )

	{  

		//把第一个文件名前的复制到szPath,即:
		//如果只选了一个文件,就复制到最后一个'/'
		//如果选了多个文件,就复制到第一个NULL字符
		lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset );

		//当只选了一个文件时,下面这个NULL字符是必需的.
		//这里不区别对待选了一个和多个文件的情况

		szPath[ ofn.nFileOffset ] = '/0';
		nLen = lstrlen(szPath);

		if( szPath[nLen-1] != '\\' )   //如果选了多个文件,则必须加上'//'

		{
			lstrcat(szPath, TEXT("\\"));
		}


		p = szOpenFileNames + ofn.nFileOffset; //把指针移到第一个文件
		ZeroMemory(szFileName, sizeof(szFileName));

		if( *p )
		{   
			lstrcat(szFileName, szPath);  //给文件名加上路径  
			lstrcat(szFileName, p);    //加上文件名  
			//lstrcat(szFileName, TEXT("/n")); //换行   

			strPathIniFile = szFileName;
		}
		//MessageBox(NULL, szFileName, TEXT("MultiSelect"), MB_OK);
	}
	else 
		return 0;
	return 1;
}

DuiLib::CDuiString CFileHelper::GetCurrentAppPath()
{
	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(NULL, tszModule, MAX_PATH);
	DuiLib::CDuiString sInstancePath = tszModule;
	int pos = sInstancePath.ReverseFind(_T('\\'));
	if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
	return sInstancePath;
}

//string CFileHelper::GetPackDir(string& strPackName, string&strPackID)
//{
//	string strPackPath;
//
//	strPackPath = GetFileRecvDir().GetData();
//
//	//对每个文件包，加上Packet
//	strPackPath += strPackName;
//	strPackPath += "-";
//	strPackPath += strPackID;
//	strPackPath += "\\";
//	if(!PathFileExists(strPackPath.c_str()))
//	{
//		CreateDirectory(strPackPath.c_str(), NULL);
//	}
//
//	return strPackPath;
//}

//DuiLib::CDuiString CFileHelper::GetFileRecvDir()
//{
//	string strMyCount = CController::Instance()->GetXmppClient()->jid().username();
//
//	DuiLib::CDuiString strAppPath = GetUserDir();
//	strAppPath += _T("User\\");
//	if (!IsExistFolder(strAppPath.GetData()) )
//	{
//		CreateDirectory(strAppPath.GetData(), NULL);
//	}
//	strAppPath += strMyCount.c_str();
//	strAppPath += _T("\\");
//
//	if (!IsExistFolder(strAppPath.GetData()))
//	{
//		CreateDirectory(strAppPath.GetData(), NULL);
//	}
//	strAppPath += _T("RecvFiles\\");
//	if(!IsExistFolder(strAppPath.GetData()))
//	{
//		CreateDirectory(strAppPath.GetData(), NULL);
//	}
//
//	return strAppPath;
//
//}
//DuiLib::CDuiString CFileHelper::GetHeadCacheDir()
//{
//	string strMyCount = CController::Instance()->GetXmppClient()->jid().username();
//
//	DuiLib::CDuiString strAppPath = GetUserDir();
//	strAppPath += _T("User\\");
//	if (!IsExistFolder(strAppPath.GetData()) )
//	{
//		CreateDirectory(strAppPath.GetData(), NULL);
//	}
//	strAppPath += strMyCount.c_str();
//	strAppPath += _T("\\");
//
//	if (!IsExistFolder(strAppPath.GetData()))
//	{
//		CreateDirectory(strAppPath.GetData(), NULL);
//	}
//	strAppPath += _T("Avatar\\");
//	if(!IsExistFolder(strAppPath.GetData()))
//	{
//		CreateDirectory(strAppPath.GetData(), NULL);
//	}
//
//	return strAppPath;
//
//} 

//string CFileHelper::GetDescription(ECmdType eType)
//{
//	string strDes;
//	switch (eType)
//	{
//	case enmCmd_FileReq:
//		strDes = "fileRequest";
//		break;
//	case enmCmd_GroupPackFile:
//		strDes = "packRequest";
//		break;
//	case enmCmd_GroupQianyuePack:
//		strDes = "groupcontract";
//		break;
//	case enmCmd_Undefied:
//		strDes = "undefied";
//		break;
//	case enmCmd_ImgReq:
//		strDes = "imgRequest";
//		break;
//	}
//
//	return strDes;
//}

//string CFileHelper::GetGrayImageName(string& strHigtImageName)
//{
//	int index = strHigtImageName.find(".png");
//	string strGrayName = strHigtImageName.substr(0, strHigtImageName.length()-4);
//	strGrayName += _T("_Gray.png");
//
//	return strGrayName;
//}

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
	string strFolder = pszFolderName;
	list<string> lsFolderList;
	
	if (strFolder.length()-1 == strFolder.find_last_of(_T("\\"))) //delete the last '\'
	{
		strFolder.substr(0,strFolder.length()-1);
	}
	lsFolderList.push_front(strFolder);

	string::size_type npos;
	while ((npos = strFolder.find_last_of(_T("\\"))) != string::npos)
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

	for (list<string>::iterator ite = lsFolderList.begin(); ite != lsFolderList.end(); ++ite)
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


//string CFileHelper::GetHigtLightAvatar(string& strAccount)
//{
//	string strLocalAvatar;
//	//头像：从本地cache找
//	DuiLib::CDuiString strAvatarPath = GetHeadCacheDir();
//	string strOnlineAvatar = strAvatarPath.GetData();
//	strOnlineAvatar += strAccount;
//	strOnlineAvatar += ".png";
//
//	//有些用户没有设置用户头像，就是空的
//	bool bExist = PathFileExists(strOnlineAvatar.c_str());
//	if (bExist)
//	{
//		strLocalAvatar = strOnlineAvatar.c_str();
//	}
//	else
//	{
//		strLocalAvatar = DEF_AVATAR_NOR;
//	}	
//
//	return strLocalAvatar;
//}

//ECmdType CFileHelper::getFileType( string strType )
//{
//	std::transform(strType.begin(), strType.end(),strType.begin(), ::tolower);
//	if(strType.find("png") != -1)
//	{
//		return ECmdType::enmCmd_ImgReq;
//	}
//	else if (strType.find("bmp") != -1)
//	{
//		return ECmdType::enmCmd_ImgReq;
//	}
//	else if (strType.find("jpg") != -1)
//	{
//		return ECmdType::enmCmd_ImgReq;
//	}
//	else
//	{
//		return ECmdType::enmCmd_FileReq;
//	}
//}
