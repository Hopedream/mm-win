#include "StdAfx.h"
#include "UIDefine.h"
#include <CommDlg.h>
#include "mainDlg.h"

int getFileSize( const tstring& strPath )
{
	DWORD sz = 0;
	HANDLE handle = CreateFile(strPath.c_str(), FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (handle != INVALID_HANDLE_VALUE)
	{
		sz = GetFileSize(handle, NULL);
		//cout<<size<<endl;
		CloseHandle(handle);
	}
	return sz;
}

extern bool getSelectedFiles( vector<tstring> & lstFiles, char* lpFilter, bool bMulti)
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
	ofn.hwndOwner = mainDlg::getInstance()->GetHWND();
	ofn.lpstrFile = szOpenFileNames ;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szOpenFileNames );
	ofn.lpstrFilter = lpFilter;
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

extern tstring UrlEncode( const std::tstring& szToEncode )
{
	std::tstring src = szToEncode;  
	char hex[] = "0123456789ABCDEF";  
	tstring dst;  


	for (size_t i = 0; i < src.size(); ++i)  
	{  
		unsigned char cc = src[i];  
		if ( cc >= 'A' && cc <= 'Z'   
			|| cc >='a' && cc <= 'z'  
			|| cc >='0' && cc <= '9'  
			|| cc == '.'  
			|| cc == '_'  
			|| cc == '-'  
			|| cc == '*')  
		{  
			if (cc == ' ')  
			{  
				dst += "+";  
			}  
			else  
				dst += cc;  
		}  
		else  
		{  
			unsigned char c = static_cast<unsigned char>(src[i]);  
			dst += '%';  
			dst += hex[c / 16];  
			dst += hex[c % 16];  
		}  
	}  
	return dst;  
}

int g_iFileId = -1;
extern tstring getTmpFileId()
{
	g_iFileId ++;
	char buf[10];
	sprintf(buf, "%d", g_iFileId);
	tstring b = buf;
	return b;
}

//extern void processdirectory( const wchar_t* filespec )
//{
//	/*struct _wfinddata_t fileinfo;
//	intptr_t handle;
//	if ()*/
//	//    struct _wfinddata_t fileinfo;
//	//    intptr_t handle;
//	// 
//	//    if ((handle=_wfindfirst(filespec, &fileinfo)) == -1L)
//	//    {
//	//        perror("Files open error");
//	//    }
//	//    else
//	//    {
//	//        do
//	//        {
//	//            // 处理文件
//	//        } while (_wfindnext(handle, &fileinfo) == 0);
//	//        _findclose(handle);
//	//    }
//}
