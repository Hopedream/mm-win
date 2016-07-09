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
	* @strBase64		ͼƬ��base64�ַ�������
	* @eType			����ͼƬ������
	* @strOutPutPath	����ͼƬ��·��
	*/
	bool GenerateImage(tstring& strBase64, tstring& strOutputPath, EImageType eType = enmImage_PNG);

	/**
	* @strBinary		����decode���ͷ���ֽ�
	* @strOutPutPath	����ͼƬ���ļ���(δ�Ӻ�׺[.png])
	*/
	bool GeneratePng(tstring& strBinary, tstring& strOutputPath);
	void GetPhotoBinval(const tstring& strPhotoName, tstring& strBinval); 


	bool ResizeImage(const tstring strSrcImg,const tstring strDestImg, unsigned int width, unsigned int height);

	/**
	* ͷ��һ�����
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