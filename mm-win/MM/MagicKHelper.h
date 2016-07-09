#pragma once

//imagemagic++
#include <Magick++.h>
#include <string>
#include <iostream>
#include <windows.h>

using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

enum EImageType
{
	enmImage_PNG	= 0,
	enmImage_JPG	= 1,
	enmImage_GIF	= 2,
};

using namespace Magick;

class CMagicKHelper
{
public:
	static CMagicKHelper* Instance();
	~CMagicKHelper();

	/**
	* @strBase64		图片的base64字符串描述
	* @eType			生成图片的类型
	* @strOutPutPath	生成图片的路径
	*/
	bool GenerateImage(tstring& strBase64, tstring& strOutputPath, EImageType eType = enmImage_PNG);

	/**
	* @strBinary		经过decode后的头像字节
	* @strOutPutPath	生成图片的文件名(未加后缀[.png])
	*/
	bool GeneratePng(tstring& strBinary, tstring& strOutputPath);
	void GetPhotoBinval(const tstring& strPhotoName, tstring& strBinval); 


	bool ResizeImage(const tstring strSrcImg,const tstring strDestImg, unsigned int width, unsigned int height);

	/**
	* 头像灰化处理
	*/
	tstring GrayImage(tstring& strSrcPng);

	bool Composite(const vector<tstring>& vec_png, tstring& save_path);

protected:
	CMagicKHelper();
	tstring GetDesciption(EImageType eType);

protected:
	static CMagicKHelper* m_pInstance;
//	static tstring* p;
	static CRITICAL_SECTION* cs_;
};