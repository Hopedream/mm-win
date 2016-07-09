#include "Utility.h"
#include <time.h>
#include <algorithm>
#include "HttpClient.h"
#include "..\Common\Logger.h"
#include <sstream>
#include "../include/Global_Def.h"
#include "Controller.h"

static int g_ListItemID = 0;

std::tstring GetFullTimeString(time_t t)
{
	if (t == 0)
	{
		return tstring(_T(""));
	}

	std::tstring strTime;
	struct tm * now = localtime( & t );
	TCHAR chBuf[100];
	memset(chBuf, 0, 100);
	_stprintf(chBuf, _T("%u-%02u-%02u %02u:%02u:%02u"), 
			now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
			now->tm_hour, now->tm_min, now->tm_sec);

	strTime = chBuf;

	return strTime;
}

std::tstring GetYMDTimeString(time_t t)
{
	if (t == 0)
	{
		return tstring(_T(""));
	}

	std::tstring strTime;
	struct tm * now = localtime( & t );
	char chBuf[100];
	memset(chBuf, 0, 100);
	sprintf(chBuf, "%u-%02u-%02u", 
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

	strTime = chBuf;

	return strTime;
}

std::tstring GetHMSTimeString(time_t t)
{
	if (t == 0)
	{
		return tstring(_T(""));
	}

	std::tstring strTime;
	struct tm * now = localtime( & t );
	char chBuf[100];
	memset(chBuf, 0, 100);
	sprintf(chBuf, "%02u:%02u:%02u", now->tm_hour, now->tm_min, now->tm_sec);

	strTime = chBuf;

	return strTime;
}

int GetNextItemID()
{
	return g_ListItemID++;
}

//check if account is number.
bool IsLegalAccount(tstring& str)
{
	bool bSucc = false;
	if (str.length() == 11)
	{
		bSucc = true;
	}
	if (bSucc)
	{
		if(tstring::npos == str.find_first_not_of(_T("0123456789")))
		{
			unsigned long dwValue = _tcstoul(str.c_str(), NULL, 10);
			if (dwValue>0)
			{
				return true;
			}
		}
	}
	
	return false;
}


tstring ExtractPhoneNo(const tstring strAccount)
{
	tstring str = strAccount.substr(0, strAccount.find('@'));
	return str;
}

CDuiString ExtractPhoneNo(const CDuiString strAccount)
{
	return strAccount.Left(strAccount.Find(_T("@")));
}

tstring GetFileSizeFormat(int nFileSize)
{
	tstring strSizeFormat;
	CDuiString strSize;
	if(nFileSize == 0)
	{
		strSize = _T("0KB");
	}
	else if (nFileSize>0 && nFileSize<1024)
	{
		strSize = _T("1KB");
		//strSize.Format("%d", nFileSize);
		//strSize += _T("B");
	}
	else if (nFileSize>=1024 && nFileSize<1024*1024)
	{
		double dKB = double(nFileSize) / (1024.0);
		strSize.Format(_T("%.2f"), dKB);
		strSize += _T("KB");
	}
	else if (nFileSize>= 1024*1024 && nFileSize<1024*1024*1024)
	{
		double dMB = double(nFileSize) / (1024.0*1024.0);
		strSize.Format(_T("%.2f"), dMB);
		strSize += _T("MB");
	}
	else
	{
		double dGB  = double(nFileSize)/(1024*1024*1024);
		strSize.Format(_T("%.2f"), dGB);
		strSize += _T("GB");
	}
	
	strSizeFormat = strSize.GetData();
	
	return strSizeFormat;
}

tstring GetDefaultFileIcon(tstring& strFileType)
{
	tstring strType = strFileType;
	tstring strFileName = _T("CreateProject\\006.png");
	std::transform(strType.begin(), strType.end(),strType.begin(), ::tolower);
	if (strType.find(_T("pdf")) != -1)
	{
		strFileName = _T("CreateProject\\pdf.png");
	}
	else if(strType.find(_T("png")) != -1)
	{
		strFileName = _T("CreateProject\\png.png");
	}
	else if (strType.find(_T("mp3")) != -1)
	{
		strFileName = _T("CreateProject\\mp3.png");
	}
	else if (strType.find(_T("zip")) != -1)
	{
		strFileName = _T("CreateProject\\zip.png");
	}
	else if (strType.find(_T("exe")) != -1)
	{
		strFileName = _T("CreateProject\\exe.png");
	}
	else if (strType.find(_T("bmp")) != -1)
	{
		strFileName = _T("CreateProject\\bmp.png");
	}
	else if (strType.find(_T("doc")) != -1)
	{
		strFileName = _T("CreateProject\\doc.png");
	}
	else if (strType.find(_T("docx")) != -1)
	{
		strFileName = _T("CreateProject\\docx.png");
	}
	else if (strType.find(_T("xls")) != -1)
	{
		strFileName = _T("CreateProject\\xls.png");
	}
	else if (strType.find(_T("xlsx")) != -1)
	{
		strFileName = _T("CreateProject\\xlsx.png");
	}
	else if (strType.find(_T("txt")) != -1)
	{
		strFileName = _T("CreateProject\\txt.png");
	}
	else if (strType.find(_T("mp4")) != -1)
	{
		strFileName = _T("CreateProject\\mp4.png");
	}
	else if (strType.find(_T("jpg")) != -1)
	{
		strFileName = _T("CreateProject\\jpg.png");
	}
	else if (strType.find(_T("html")) != -1)
	{
		strFileName = _T("CreateProject\\html.png");
	}
	else if (strType.find(_T("gif")) != -1)
	{
		strFileName = _T("CreateProject\\gif.png");
	}

	return strFileName;
}

tstring GetPackFileIcon(tstring& strFileType)
{
	tstring strType = strFileType;
	tstring strFileName = _T("ProjectFile\\file.png");
	std::transform(strType.begin(), strType.end(),strType.begin(), ::tolower);
	if (strType.find(_T("pdf")) != -1)
	{
		strFileName = _T("ProjectFile\\pdf.png");
	}
	else if(strType.find(_T("png")) != -1)
	{
		strFileName = _T("ProjectFile\\image.png");
	}
	else if (strType.find(_T("mp3")) != -1)
	{
		strFileName = _T("ProjectFile\\audio.png");
	}
	else if (strType.find(_T("bmp")) != -1)
	{
		strFileName = _T("ProjectFile\\png.png");
	}
	else if (strType.find(_T("doc")) != -1)
	{
		strFileName = _T("ProjectFile\\doc.png");
	}
	else if (strType.find(_T("docx")) != -1)
	{
		strFileName = _T("ProjectFile\\doc.png");
	}
	else if (strType.find(_T("xls")) != -1)
	{
		strFileName = _T("ProjectFile\\excel.png");
	}
	else if (strType.find(_T("xlsx")) != -1)
	{
		strFileName = _T("ProjectFile\\excel.png");
	}
	else if (strType.find(_T("mp4")) != -1)
	{
		strFileName = _T("ProjectFile\\video.png");
	}
	else if (strType.find(_T("jpg")) != -1)
	{
		strFileName = _T("ProjectFile\\image.png");
	}
	else if (strType.find(_T("gif")) != -1)
	{
		strFileName = _T("ProjectFile\\image.png");
	}

	return strFileName;
}

bool IsQingyuePack(tstring& strProperty)
{
	if (_tcsicmp(strProperty.c_str(), _T("报价单")) == 0
		|| _tcsicmp(strProperty.c_str(), _T("合同补充条款")) == 0
		|| _tcsicmp(strProperty.c_str(), _T("合约补充条款")) == 0
		|| _tcsicmp(strProperty.c_str(), _T("合约条款修改与替换")) == 0
		|| _tcsicmp(strProperty.c_str(), _T("合约中规定的交付文档")) == 0
		|| _tcsicmp(strProperty.c_str(), _T("合约文件")) == 0)
	{
		return true;
	}

	return false;
}
tstring GetAttibuteDescription(int nValue)
{
	tstring strDesc;
	switch(nValue)
	{
	case 1:
		strDesc = "无属性";
		break;
	case 2:
		strDesc = "询价单";
		break;
	case 3:
		strDesc = "报价单";
		break;
	case 4:
		strDesc = "合同补充条款";
		break;
	case 5:
		strDesc = "合约条款修改与替换";
		break;
	case 6:
		strDesc = "合约中规定的交付文档";
		break;
	case 7:
		strDesc = "发货通知单";
		break;
	case 8:
		strDesc = "发运告知单";
		break;
	case 9:
		strDesc = "装箱单";
		break;
	case 10:
		strDesc = "收货单";
		break;
	case 11:
		strDesc = "验收通知单";
		break;
	case 12:
		strDesc = "付款凭证";
		break;
	case 13:
		strDesc = "合约文件";
	default:
		break;
	}

	return strDesc;
}

tstring GetAttibuteValue(tstring& strDesc)
{
	tstring strValue;
	if (_tcsicmp(strDesc.c_str(), "无属性") == 0)
	{
		strValue = "1";
	}
	else if (_tcsicmp(strDesc.c_str(), "询价单") == 0)
	{
		strValue = "2";
	}
	else if (_tcsicmp(strDesc.c_str(), "报价单") == 0)
	{
		strValue = "3";
	}
	else if (_tcsicmp(strDesc.c_str(), "合同补充条款") == 0
		||_tcsicmp(strDesc.c_str(), "合约补充条款") == 0)
	{
		strValue = "4";
	}
	else if (_tcsicmp(strDesc.c_str(), "合约条款修改与替换") == 0)
	{
		strValue = "5";
	}
	else if (_tcsicmp(strDesc.c_str(), "合约中规定的交付文档") == 0)
	{
		strValue = "6";
	}
	else if (_tcsicmp(strDesc.c_str(), "发货通知单") == 0)
	{
		strValue = "7";
	}
	else if (_tcsicmp(strDesc.c_str(), "发运告知单") == 0)
	{
		strValue = "8";
	}
	else if (_tcsicmp(strDesc.c_str(), "装箱单") == 0)
	{
		strValue = "9";
	}
	else if (_tcsicmp(strDesc.c_str(), "收货单") == 0)
	{
		strValue = "10";
	}
	else if (_tcsicmp(strDesc.c_str(), "验收通知单") == 0)
	{
		strValue = "11";
	}
	else if (_tcsicmp(strDesc.c_str(), "付款凭证") == 0)
	{
		strValue = "12";
	}
	else if (_tcsicmp(strDesc.c_str(), "合约文件") == 0)
	{
		strValue = "13";
	}


	return strValue;
}


bool CheckIfGroupQianyuePack(tstring& strMsg, tstring& strPackID, tstring& strRoomID, tstring&strStatus)
{
	bool bGroupFileMsg = false;
	if (strMsg.find("#COMMAND#") != -1)
	{
		//0:默认状态 1:收藏 2:收发 3：同意 4：拒绝
		//#COMMAND#type=groupcontract,0 1 2 3 4,packid,roomname,from,packname,attribute
		//1.如果是发送群组文件消息
		if (strMsg.find("groupcontract") != -1)
		{
			int index = strMsg.find(",");
			if (index > 0)
			{
				int nContractStatusIndex = strMsg.find(',', index+1);
				int nPackIDIndex = strMsg.find(',', nContractStatusIndex+1);
				int nRoomIDIndex = strMsg.find(',', nPackIDIndex+1);
				int nSenderIndex = strMsg.find(',', nRoomIDIndex+1);
				int nPackNameIdx = strMsg.find(',', nSenderIndex+1);

				strStatus = strMsg.substr(index+1, nContractStatusIndex-index-1);
				strPackID = strMsg.substr(nContractStatusIndex+1, nPackIDIndex-nContractStatusIndex-1);
				strRoomID = strMsg.substr(nPackIDIndex+1, nRoomIDIndex-nPackIDIndex-1);

				if (_tcsicmp(strStatus.c_str(), "3") == 0)
				{
					strStatus = "已签约";
				}
				else if (_tcsicmp(strStatus.c_str(), "4") == 0)
				{
					strStatus = "已拒绝";
				}

				bGroupFileMsg = true;
			}
		}
	}

	return bGroupFileMsg;
}

//业务属性临时取消，这里暂时返回空vector;
bool GetDuty(const tstring& strOrgID, const tstring& strAccount, vector<tstring>& vecProperty)
{
	vecProperty.clear();
	return true;

	tstring strPhone = ExtractPhoneNo(strAccount);

	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-duty?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += strOrgID;
	strPost += _T("&cell_phone=");
	strPost += strPhone;
	strUrl += strPost;

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		int pos = strHttpResponse.find("duty");
		if (pos >= 0)
		{
			tstring strDuty = strHttpResponse.substr(pos + 6, strHttpResponse.length());
			int a = strDuty.find('1');
			int b = strDuty.find('2');
			int c = strDuty.find('3');
			int d = strDuty.find('4');
			(a>=0)?vecProperty.push_back("需求"):NULL;
			(b>=0)?vecProperty.push_back("订约"):NULL;
			(c>=0)?vecProperty.push_back("执行"):NULL;
			(d>=0)?vecProperty.push_back("售后"):NULL;
		}
		else
		{
			ostringstream ost;
			ost << strHttpResponse << "中不包含" << "duty" << "字符串，请检查该判断是否正确";
			tstring strPost = ost.str().c_str();
			CLogger::GetInstance()->PrintLog(LOG_INFO, strPost.c_str());
		}

		return true;
	}
}

bool GetRecipient(const tstring& organization_id, tstring& recipient)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-default?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += organization_id;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		//todo
		recipient = strHttpResponse.substr(strHttpResponse.find("\"cell_phone\":\"") + 14, strHttpResponse.find("\",\"") - (strHttpResponse.find("\"cell_phone\":\"") + 14));
		return true;
	}
}


//@brief 获取图片文件的编码方式，支持bmp、jpg、jpeg、gif、tiff和png等格式图片
//@date   1-13-2009  
//@param [in]  format 图片格式 值可以为以下几种
//@"image/bmp"
//@"image/jpeg"
//@"image/gif"
//@"image/tiff"
//@"image/png"
//@param [in]  pClsid
//@return  成功则返回值 >= 0，失败则返回值为-1
int GetEncoderClsid(const WCHAR* format, CLSID *pClsid)
{
	 int nRet = -1;
	 ImageCodecInfo* pCodecInfo = NULL;
	 UINT nNum = 0,nSize = 0;
	 GetImageEncodersSize(&nNum,&nSize);

	 if (nSize<0)
	 {
			 return nRet;
	 }

	 pCodecInfo= new ImageCodecInfo[nSize];

	 if (pCodecInfo==NULL)
	 {
	  return nRet;
	 }

	 GetImageEncoders(nNum,nSize,pCodecInfo);

	 for (UINT i=0; i<nNum;i++)
	 {
		if (wcscmp(pCodecInfo[i].MimeType,format)==0)
		{
			*pClsid= pCodecInfo[i].Clsid;
			nRet = i;
			delete[] pCodecInfo;
			return nRet;
		}
		else
		{
			continue;
		}
	 }

	 delete[] pCodecInfo;
	 return nRet;
}

//@brief 用GDI+将Bitmap图片保存为文件
//@date   1-13-2009  
//@param [in]  hBitmap hBitmap的句柄（handle）
//@param [in]  lpszFileName 保存图片的完整路径
//@return  成功返回TRUE，失败返回FALSE

BOOL SaveBitmapToFile(HBITMAP hBitmap, LPWSTR lpszFileName)
{
	 CLSID pngClsid;

	 Bitmap bmp(hBitmap,NULL);

	 //获取BMP文件的编码方式(如果希望获取JPEG的编码方式，

	 //那么参数一要设置为：_TEXT("image/jpeg")，其他支持的图片格式类似)
	 int nResult = GetEncoderClsid(L"image/png",&pngClsid);

	 if(nResult >= 0)
	 {
		 //保存所截取的屏幕图片
		 bmp.Save(lpszFileName,&pngClsid);
	 }
	 else
	 {
		 return FALSE;
	 }
	   
	 return TRUE;
}

//专门为 名字 + 属性设计的移动方法， 比如 赵有元 需求 订约 执行 售后
//需求，订约，执行，售后的按钮大小 强制约定为宽30，高16，间隔2
//与前面名字间隔4
//不管高度
//这个函数现在先return
#define PROPERTY_WIDTH 30
#define PROPERTY_HIGHT 16
#define PROPERTY_SPACE 2
#define PROPERTY_SPACE_TO_HEAD 4
bool MoveControllers(CLabelUI* head, RECT head_pos, SIZE head_size, vector<CLabelUI*> following, RECT total_pos)
{
	return true;
	int nTotalSize = 0;
	nTotalSize += head_size.cx;//加名字长度
	nTotalSize += PROPERTY_SPACE_TO_HEAD;//加名字与属性的间隔
	nTotalSize += (following.size()*(PROPERTY_WIDTH + PROPERTY_SPACE));//加数个属性的宽度

	//长度足够
	if (nTotalSize <= total_pos.right - total_pos.left)
	{
		head_pos.right = head_pos.left + head_size.cx;

	}
	else//长度不够
	{
		int property_width = (following.size()*PROPERTY_WIDTH+PROPERTY_SPACE)+PROPERTY_SPACE_TO_HEAD;
		head_pos.right = total_pos.right - property_width;
	}

	head->SetPos(head_pos);
	//完成第一个属性框位置的初始化
	RECT rect,rc;
	rect = head_pos;
	rc = head_pos;
	rect.left = rect.right + PROPERTY_SPACE_TO_HEAD;
	rect.right = rect.left + PROPERTY_WIDTH;
	//		rect.top = (rect.bottom-rect.top - PROPERTY_HIGHT)/2 + rect.top;
	rect.bottom = rect.top + PROPERTY_HIGHT;

	for (int i=0;i!=following.size();i++)
	{
		rc.left = rect.left + i*(PROPERTY_WIDTH+PROPERTY_SPACE);
		rc.right = rc.left + PROPERTY_WIDTH;
		following[i]->SetPos(rc);
	}

	return TRUE;
}

//专门为 名字 + 默认接收人设计的移动方法， 比如 赵有元 默认接收人
//与前面名字间隔4
//不管高度
#define RECIPIENT_SPACE_TO_HEAD 4
bool MoveControllers( CLabelUI* head, RECT head_pos, SIZE head_size, 
	CLabelUI* recipient, RECT recipient_pos, SIZE recipient_size, 
	RECT total_pos )
{
	int nTotalSize = 0;
	nTotalSize += head_size.cx;//加名字长度
	nTotalSize += RECIPIENT_SPACE_TO_HEAD;//加名字与属性的间隔
	nTotalSize += recipient_size.cx;//加默认接收人的宽度

	//长度足够
	if (nTotalSize <= total_pos.right - total_pos.left)
	{
		head_pos.right = head_pos.left + head_size.cx;

	}
	else//长度不够
	{
		head_pos.right = total_pos.right - recipient_size.cx - RECIPIENT_SPACE_TO_HEAD;
	}

	head->SetPos(head_pos);
	//完成第一个属性框位置的初始化
	RECT rect;
	rect = head_pos;
	rect.left = rect.right + RECIPIENT_SPACE_TO_HEAD;
	rect.right = rect.left + recipient_size.cx;
	//		rect.top = (rect.bottom-rect.top - PROPERTY_HIGHT)/2 + rect.top;
	rect.bottom = rect.top + recipient_size.cy;
	recipient->SetPos(rect);	

	return TRUE;
}

void ReplaceAll(tstring& str, const tstring& old_value, const tstring& new_value)   
{   
	while(true)   
	{   
		tstring::size_type  pos(0);   
		if((pos=str.find(old_value))!=tstring::npos)  
		{
			str.replace(pos,old_value.length(),new_value); 
		}
		else
		{
			break;   
		}
	}     
} 

void ReplaceAllDistinct(tstring& str, const tstring& old_value, const tstring& new_value)  
{   
	for(tstring::size_type pos(0);pos!=tstring::npos;pos+=new_value.length())   
	{   
		if((pos=str.find(old_value,pos))!=tstring::npos)   
		{
			str.replace(pos,old_value.length(),new_value);  
		}
		else
		{
			break;  
		}
	}    
}

std::tstring StringToTstring(const string &str)
{
#ifdef UNICODE
	std::wstring wstr(str.length(),L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr; 
#else
	std::string str_result(str);
	return str_result;
#endif
	
}

//只拷贝低字节至string中
std::tstring WStringToTString(const wstring &wstr)
{
#ifdef UNICODE
	std::wstring str_result(wstr);
	return str_result;
#else
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str; 
#endif
	
}

std::wstring StringToWString(const string &str)
{
	std::wstring wstr(str.length(),L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr; 
}

//只拷贝低字节至string中
std::string WStringToString(const wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str; 
}