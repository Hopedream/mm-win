#pragma once
/*#include "StdAfx.h"*/
#include <vector>
#include <map>
#include <string>
#include <list>
#include <tchar.h>
using namespace std;

typedef struct tagFileInfo{
	tstring strFilePath;
	tstring strFileName;
	tstring strFileType;
	tstring strFileFullName;
	tstring strPackID;
	tstring strPackName;
	tstring strFileUrl;
	int   dwFileSize;
	tstring strKey;
	tstring stropposite;           //发给谁，或者由谁发来的
	tstring strFileSz;
	tstring strDuration;
	tagFileInfo()
	{
		strFilePath = _T("");
		strFileName= _T("");
		strFileType= _T("");
		strFileFullName= _T("");
		strPackID= _T("");
		strFileUrl= _T("");
		dwFileSize= 0;
		strKey= _T("");
		stropposite= _T("");   
	}
} fileInfo;

typedef struct{
	tstring strId;             //机构id			
	tstring strName;           //机构名字
	tstring strPhotoBinary;    //机构头像
	tstring strPhotoType;      //头像类型
	tstring strCellPhone;      //机构手机号
	tstring strTelephone;      //机构电话
	tstring strAddress;        //机构地址省,市,区
	tstring strDetailAddr;     //机构详细地址

	//新增加的内容
	tstring strProperty;		  //机构属性
	tstring strMemberId;		  //会员Id	
} orgInfo;




//
struct sPackFileItem
{
	tstring strPackID;
	tstring strPackName;
	tstring strFileName;
	tstring strFileType;
	tstring strFileSizeShow;		//显示到界面上的字符串：1.3MB
	tstring strFileUrl;
	tstring strFileSizeAccurate;	//真实字节大小：13245 Byte
};

struct sPackItem
{
	tstring strPackID;
	tstring strPackName;
	tstring strPackSize;
	tstring strPackTime;
	tstring strPackSender;//发送人昵称
	tstring strPackProp;
	tstring strPackCreaterPhone;//发送人手机号
	tstring strOrgID;
	tstring strOrgName;
//	tstring strAccount;//added by zyy
	int    nPackStatus;

	//tstring strPackStatus;
	list<sPackFileItem> listFile;
};

typedef map<tstring, sPackItem> MapPackID2Packs;

//#packid : pack id; packname: pack name; from：mobile number that download the pack; attribute: pack attribute; time: download time
struct sPackReadReceipt
{
	tstring strPackID;
	tstring strRoomName;
	tstring strFrom; //回执的发送者
	tstring strPackName;
	tstring strAttribute;
	tstring strTime;
};

extern int  getFileSize(const tstring& strPath);

extern bool getSelectedFiles(vector<tstring> &, char* lpFilter = "All\0*.*\0Text\0*.TXT\0", bool bMulti=true);
extern tstring UrlEncode(const std::tstring& szToEncode);

extern tstring getTmpFileId();
//extern void processdirectory(const wchar_t* filespec);
//extern void process_directory(const wchar_t * filespec); // 分析目录，遍历所有满足条件的文件
//{
//    struct _wfinddata_t fileinfo;
//    intptr_t handle;
// 
//    if ((handle=_wfindfirst(filespec, &fileinfo)) == -1L)
//    {
//        perror("Files open error");
//    }
//    else
//    {
//        do
//        {
//            // 处理文件
//        } while (_wfindnext(handle, &fileinfo) == 0);
//        _findclose(handle);
//    }
//}

