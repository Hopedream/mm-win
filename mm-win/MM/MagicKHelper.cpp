#include "MagicKHelper.h"
#include "base64.h"
#include "FileHelper.h"
#include "StringHelper.h"
#include "../Common/Logger.h"


CMagicKHelper* CMagicKHelper::m_pInstance = NULL;
CRITICAL_SECTION* CMagicKHelper::cs_ = NULL;

CMagicKHelper* CMagicKHelper::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CMagicKHelper;
		cs_ = new CRITICAL_SECTION;
		InitializeCriticalSection(cs_);
	}	
	return m_pInstance;
}

CMagicKHelper::CMagicKHelper()
{
	try{
		tstring strApp = CFileHelper::GetCurrentAppPath().GetData();
		CLogger::GetInstance()->PrintErrLog("GetCurrentAppPath  %s", strApp.c_str());
		//Init image magic++
		Magick::InitializeMagick(CChineseCode::EncodeUTF8(strApp).c_str());
		CLogger::GetInstance()->PrintErrLog("InitializeMagick ");
	}
	
	catch (std::exception &ex)
	{
		CLogger::GetInstance()->PrintErrLog("GetInstance failure  %s", ex.what());
	}
}

CMagicKHelper::~CMagicKHelper()
{
	DeleteCriticalSection(cs_);
}

//strBase64 = "data:image/png;base64,ab2380da890qji..."
bool CMagicKHelper::GenerateImage(tstring& strBase64, 
				tstring& strOutputPath, EImageType eType /*= enmImage_PNG*/)
{
	bool bSucc = false;

	//是否存在字符串前缀，如果有，就去除。
	tstring strBase64Raw;
	tstring strTmp = strBase64;
	int index = strTmp.find("base64");
	if (index != -1)
	{
		int nBaseLen = tstring("base64,").length();
		strBase64Raw = strTmp.substr(index + nBaseLen, strTmp.length()-index-nBaseLen);
	}

	if (!strBase64Raw.empty())
	{
		int nLen = strBase64Raw.length();
		std::vector<BYTE> bytes = base64_decode(strBase64Raw);
		Magick::Blob input_blob((void*)&bytes[0],bytes.size());
		Magick::Image input_image;
		input_image.magick("INLINE");

//		EnterCriticalSection(cs_);

		input_image.read(input_blob);
		input_image.magick(GetDesciption(eType).c_str());
		input_image.write(strOutputPath.c_str());

//		LeaveCriticalSection(cs_);
		bSucc = true;
	}
	
	return bSucc;
}

bool CMagicKHelper::GeneratePng(tstring& strBinary, tstring& strOutputPath)
{
	bool bSucc = false;
	if (!strBinary.empty())
	{
		tstring strFileName = strOutputPath + _T(".png");

		Magick::Blob input_blob((void*)strBinary.c_str(),strBinary.length());
		Magick::Image input_image;
		input_image.magick("INLINE");

//		EnterCriticalSection(cs_);

		input_image.read(input_blob);
		CLogger::GetInstance()->PrintErrLog("read successed");
		input_image.magick("PNG");
		input_image.write(strFileName.c_str());

//		LeaveCriticalSection(cs_);
		bSucc = true;
	}

	return bSucc;
}

tstring CMagicKHelper::GrayImage(tstring& strSrcPng)
{
	Magick::Image imgSrc(strSrcPng.c_str());
	tstring strGrayName = CFileHelper::GetGrayImageName(strSrcPng);
	imgSrc.grayscale(UndefinedPixelIntensityMethod);

//	EnterCriticalSection(cs_);
	imgSrc.write(strGrayName);
//	LeaveCriticalSection(cs_);

	return strGrayName;
}

tstring CMagicKHelper::GetDesciption(EImageType eType)
{
	tstring strDes;
	if (eType == enmImage_PNG)
	{
		strDes = "PNG";
	}
	else if (eType == enmImage_JPG)
	{
		strDes = "JPG";
	}
	else if (eType == enmImage_GIF)
	{
		strDes = "GIF";
	}

	return strDes;
}

bool CMagicKHelper::ResizeImage( const tstring strSrcImg,const tstring strDestImg, unsigned int width, unsigned int height )
{
	CLogger::GetInstance()->PrintErrLog("begin resize image srcimg = %s, strDestImg = %s", strSrcImg.c_str(), strDestImg.c_str());
	Magick::Image imgSrc;//(strSrcImg);
	try { 

//			EnterCriticalSection(cs_);
			imgSrc.read(strSrcImg);	
			CLogger::GetInstance()->PrintErrLog("read successed");
			imgSrc.sample(Geometry(width,height));  
			CLogger::GetInstance()->PrintErrLog("sample successed");
			imgSrc.write(strDestImg);
//			LeaveCriticalSection(cs_);
			CLogger::GetInstance()->PrintErrLog("write successed");
	} 
	catch(std::exception &ex) 
	{ 
		//文件读取失败
		CLogger::GetInstance()->PrintErrLog("resize image failure  %s", ex.what());
		return false;
	} 
	return true;
}

void CMagicKHelper::GetPhotoBinval( const tstring& strPhotoName, tstring& strBinval )
{
	try
	{
		CLogger::GetInstance()->PrintErrLog("begin getphotoBinval");
		Magick::Image imgSrc(strPhotoName);
		CLogger::GetInstance()->PrintErrLog("imgSrc()");
		Magick::Blob blob;

//		EnterCriticalSection(cs_);
		imgSrc.write(&blob);
//		LeaveCriticalSection(cs_);

		CLogger::GetInstance()->PrintErrLog("write()");
		unsigned int len = blob.length();
		strBinval.assign((const char*)blob.data(), blob.length());
		CLogger::GetInstance()->PrintErrLog("write()");
	}
	catch (std::exception &ex)
	{
		CLogger::GetInstance()->PrintErrLog("resize image failure  %s", ex.what());
	}
}


//以九宫格的形式合并图像
/************************************************************************/
/* 

1.   1

2.  1 2

3.   1
    2 3

4.  1 2
    3 4

5.  1 2
    345

6.  123
    456

7.   1
    234
	567

8.  
*/
/************************************************************************/
bool CMagicKHelper::Composite(const vector<tstring>& vec_png, tstring& save_path)
{
	list<Magick::Image> source_image_list;
	Magick::Image image;

	int num = (vec_png.size()<=9)? vec_png.size(): 9;//注意，这里最多用九宫格显示，因此最多取9个
	for (int i=0;i!=num;i++)
	{
//		EnterCriticalSection(cs_);
//		FILE *file2 = fopen(vec_png[i],"r+");
		char file_path[MAX_PATH];
//		strcpy(file_path, vec_png[i]);
		FILE* file;
		file = fopen(vec_png[i].c_str(), "r+");
		if (file)
		{
			fclose(file);
			image.read(vec_png[i]);
			source_image_list.push_back(image);
		}
		else
		{
			Sleep(100);
			i = i-1;
			continue;
		}

		
//		LeaveCriticalSection(cs_);

		
	}

	Geometry size = image.size();
// 	Magick::Color color("rgba(0,0,0,0)");
// 
 	Montage montageSettings;
 	montageSettings.geometry("50x50+2+2>");
// 	montageSettings.shadow(true);
// 	montageSettings.backgroundColor(color);

	tstring tile;
	switch (num)
	{
	case 1:
		tile = "1x1";
		break;
	case 2:
	case 3:
	case 4:
		tile = "2x2";
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		tile = "3x3";
		break;
	default:
		break;
	}
 	montageSettings.tile(tile);

	list<Magick::Image> montagelist;
	Magick::montageImages( &montagelist, source_image_list.begin(), source_image_list.end(), montageSettings);

	// This will give the expected result????
//	EnterCriticalSection(cs_);
	Magick::writeImages(montagelist.begin(), montagelist.end(), save_path);
//	LeaveCriticalSection(cs_);

//	Magick::Blob *b = new Magick::Blob();
	// This will throw an exception mentioned above
//	Magick::writeImages(montagelist.begin(), montagelist.end(), b);
//	Magick::Image imageFromBlob(*b);
//	imageFromBlob.write("d:\\imtest\\Montage\\outBlob.png");

	return true;
//	Magick
}

