
#include "StringHelper.h"
#include "Utility.h"

void CChineseCode::UTF_8ToUnicode(wchar_t* pOut,const char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

void CChineseCode::UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

void CChineseCode::UnicodeToGB2312(char* pOut,wchar_t uData)
{
	::WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
	return;
}     

void CChineseCode::Gb2312ToUnicode(wchar_t* pOut, const char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return ;
}

void CChineseCode::GB2312ToUTF_8(string& pOut,const char *pText, int pLen)
{
	if (pLen ==0)
		return;
	char buf[4];
	int nLength = pLen* 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		//如果是英文直接复制就可以
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);

			UnicodeToUTF_8(buf,&pbuffer);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    

			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	//返回结果
	pOut = rst;             
	//delete []rst;   

	return;
}

void CChineseCode::UTF_8ToGB2312(string &pOut, const char *pText, int pLen)
{
	char * newBuf = new char[pLen+1];
	memset(newBuf, 0, pLen+1);
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Ctemp,Wtemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = '\0';

	pOut = newBuf;
	delete []newBuf;

	return; 
}  


tstring CChineseCode::EncodeUTF8(tstring strInput)
{
#ifdef UNICODE
	string input_temp = WStringToString(strInput);
	string out_temp;
	GB2312ToUTF_8(out_temp, input_temp.c_str(), strInput.length());
	wstring sOut =  StringToWString(out_temp);
	return sOut;
#else
	tstring strOut;
	GB2312ToUTF_8(strOut, strInput.c_str(), strInput.length());
	return strOut;
#endif	
}

std::tstring CChineseCode::DecodeUTF8(tstring strUtf8 )
{
#ifdef UNICODE
	string input_temp = WStringToString(strUtf8);
	string out_temp;
	UTF_8ToGB2312(out_temp, input_temp.c_str(), input_temp.length());
	wstring sOut =  StringToWString(out_temp);
	return sOut;
#else
	tstring strOut;
	UTF_8ToGB2312(strOut, strUtf8.c_str(), strUtf8.length());
	return strOut;
#endif
}

//std::tstring CChineseCode::str2HexStr( tstring str )
//{
//	char[] chars = "0123456789ABCDEF".toCharArray();      
//	StringBuilder sb = new StringBuilder(_T(""));    
//	byte[] bs = str.getBytes();      
//	int bit;      
//
//	for (int i = 0; i < bs.length; i++)    
//	{      
//		bit = (bs[i] & 0x0f0) >> 4;      
//		sb.append(chars[bit]);      
//		bit = bs[i] & 0x0f;      
//		sb.append(chars[bit]);    
//		sb.append(' ');    
//	}      
//	return sb.toString().trim();  
//}
//
//std::tstring CChineseCode::hexStr2Str( tstring hexStr )
//{
//	tstring str = "0123456789ABCDEF";      
//	char[] hexs = hexStr.toCharArray();      
//	byte[] bytes = new byte[hexStr.length() / 2];      
//	int n;      
//
//	for (int i = 0; i < bytes.length; i++)    
//	{      
//		n = str.indexOf(hexs[2 * i]) * 16;      
//		n += str.indexOf(hexs[2 * i + 1]);      
//		bytes[i] = (byte) (n & 0xff);      
//	}      
//	return new tstring(bytes);     
//}
