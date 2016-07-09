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
	char chBuf[100];
	memset(chBuf, 0, 100);
	sprintf(chBuf, "%u-%02u-%02u %02u:%02u:%02u", 
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
		if(tstring::npos == str.find_first_not_of("0123456789"))
		{
			unsigned long dwValue = strtoul(str.c_str(), NULL, 10);
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
	return strAccount.Left(strAccount.Find("@"));
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
		strSize.Format("%.2f", dKB);
		strSize += _T("KB");
	}
	else if (nFileSize>= 1024*1024 && nFileSize<1024*1024*1024)
	{
		double dMB = double(nFileSize) / (1024.0*1024.0);
		strSize.Format("%.2f", dMB);
		strSize += _T("MB");
	}
	else
	{
		double dGB  = double(nFileSize)/(1024*1024*1024);
		strSize.Format("%.2f", dGB);
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
	if (strType.find("pdf") != -1)
	{
		strFileName = _T("CreateProject\\pdf.png");
	}
	else if(strType.find("png") != -1)
	{
		strFileName = _T("CreateProject\\png.png");
	}
	else if (strType.find("mp3") != -1)
	{
		strFileName = _T("CreateProject\\mp3.png");
	}
	else if (strType.find("zip") != -1)
	{
		strFileName = _T("CreateProject\\zip.png");
	}
	else if (strType.find("exe") != -1)
	{
		strFileName = _T("CreateProject\\exe.png");
	}
	else if (strType.find("bmp") != -1)
	{
		strFileName = _T("CreateProject\\bmp.png");
	}
	else if (strType.find("doc") != -1)
	{
		strFileName = _T("CreateProject\\doc.png");
	}
	else if (strType.find("docx") != -1)
	{
		strFileName = _T("CreateProject\\docx.png");
	}
	else if (strType.find("xls") != -1)
	{
		strFileName = _T("CreateProject\\xls.png");
	}
	else if (strType.find("xlsx") != -1)
	{
		strFileName = _T("CreateProject\\xlsx.png");
	}
	else if (strType.find("txt") != -1)
	{
		strFileName = _T("CreateProject\\txt.png");
	}
	else if (strType.find("mp4") != -1)
	{
		strFileName = _T("CreateProject\\mp4.png");
	}
	else if (strType.find("jpg") != -1)
	{
		strFileName = _T("CreateProject\\jpg.png");
	}
	else if (strType.find("html") != -1)
	{
		strFileName = _T("CreateProject\\html.png");
	}
	else if (strType.find("gif") != -1)
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
	if (strType.find("pdf") != -1)
	{
		strFileName = _T("ProjectFile\\pdf.png");
	}
	else if(strType.find("png") != -1)
	{
		strFileName = _T("ProjectFile\\image.png");
	}
	else if (strType.find("mp3") != -1)
	{
		strFileName = _T("ProjectFile\\audio.png");
	}
	else if (strType.find("bmp") != -1)
	{
		strFileName = _T("ProjectFile\\png.png");
	}
	else if (strType.find("doc") != -1)
	{
		strFileName = _T("ProjectFile\\doc.png");
	}
	else if (strType.find("docx") != -1)
	{
		strFileName = _T("ProjectFile\\doc.png");
	}
	else if (strType.find("xls") != -1)
	{
		strFileName = _T("ProjectFile\\excel.png");
	}
	else if (strType.find("xlsx") != -1)
	{
		strFileName = _T("ProjectFile\\excel.png");
	}
	else if (strType.find("mp4") != -1)
	{
		strFileName = _T("ProjectFile\\video.png");
	}
	else if (strType.find("jpg") != -1)
	{
		strFileName = _T("ProjectFile\\image.png");
	}
	else if (strType.find("gif") != -1)
	{
		strFileName = _T("ProjectFile\\image.png");
	}

	return strFileName;
}

bool IsQingyuePack(tstring& strProperty)
{
	if (_tcsicmp(strProperty.c_str(), "���۵�") == 0
		|| _tcsicmp(strProperty.c_str(), "��ͬ��������") == 0
		|| _tcsicmp(strProperty.c_str(), "��Լ��������") == 0
		|| _tcsicmp(strProperty.c_str(), "��Լ�����޸����滻") == 0
		|| _tcsicmp(strProperty.c_str(), "��Լ�й涨�Ľ����ĵ�") == 0
		|| _tcsicmp(strProperty.c_str(), "��Լ�ļ�") == 0)
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
		strDesc = "������";
		break;
	case 2:
		strDesc = "ѯ�۵�";
		break;
	case 3:
		strDesc = "���۵�";
		break;
	case 4:
		strDesc = "��ͬ��������";
		break;
	case 5:
		strDesc = "��Լ�����޸����滻";
		break;
	case 6:
		strDesc = "��Լ�й涨�Ľ����ĵ�";
		break;
	case 7:
		strDesc = "����֪ͨ��";
		break;
	case 8:
		strDesc = "���˸�֪��";
		break;
	case 9:
		strDesc = "װ�䵥";
		break;
	case 10:
		strDesc = "�ջ���";
		break;
	case 11:
		strDesc = "����֪ͨ��";
		break;
	case 12:
		strDesc = "����ƾ֤";
		break;
	case 13:
		strDesc = "��Լ�ļ�";
	default:
		break;
	}

	return strDesc;
}

tstring GetAttibuteValue(tstring& strDesc)
{
	tstring strValue;
	if (_tcsicmp(strDesc.c_str(), "������") == 0)
	{
		strValue = "1";
	}
	else if (_tcsicmp(strDesc.c_str(), "ѯ�۵�") == 0)
	{
		strValue = "2";
	}
	else if (_tcsicmp(strDesc.c_str(), "���۵�") == 0)
	{
		strValue = "3";
	}
	else if (_tcsicmp(strDesc.c_str(), "��ͬ��������") == 0
		||_tcsicmp(strDesc.c_str(), "��Լ��������") == 0)
	{
		strValue = "4";
	}
	else if (_tcsicmp(strDesc.c_str(), "��Լ�����޸����滻") == 0)
	{
		strValue = "5";
	}
	else if (_tcsicmp(strDesc.c_str(), "��Լ�й涨�Ľ����ĵ�") == 0)
	{
		strValue = "6";
	}
	else if (_tcsicmp(strDesc.c_str(), "����֪ͨ��") == 0)
	{
		strValue = "7";
	}
	else if (_tcsicmp(strDesc.c_str(), "���˸�֪��") == 0)
	{
		strValue = "8";
	}
	else if (_tcsicmp(strDesc.c_str(), "װ�䵥") == 0)
	{
		strValue = "9";
	}
	else if (_tcsicmp(strDesc.c_str(), "�ջ���") == 0)
	{
		strValue = "10";
	}
	else if (_tcsicmp(strDesc.c_str(), "����֪ͨ��") == 0)
	{
		strValue = "11";
	}
	else if (_tcsicmp(strDesc.c_str(), "����ƾ֤") == 0)
	{
		strValue = "12";
	}
	else if (_tcsicmp(strDesc.c_str(), "��Լ�ļ�") == 0)
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
		//0:Ĭ��״̬ 1:�ղ� 2:�շ� 3��ͬ�� 4���ܾ�
		//#COMMAND#type=groupcontract,0 1 2 3 4,packid,roomname,from,packname,attribute
		//1.����Ƿ���Ⱥ���ļ���Ϣ
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
					strStatus = "��ǩԼ";
				}
				else if (_tcsicmp(strStatus.c_str(), "4") == 0)
				{
					strStatus = "�Ѿܾ�";
				}

				bGroupFileMsg = true;
			}
		}
	}

	return bGroupFileMsg;
}

//ҵ��������ʱȡ����������ʱ���ؿ�vector;
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
			(a>=0)?vecProperty.push_back("����"):NULL;
			(b>=0)?vecProperty.push_back("��Լ"):NULL;
			(c>=0)?vecProperty.push_back("ִ��"):NULL;
			(d>=0)?vecProperty.push_back("�ۺ�"):NULL;
		}
		else
		{
			ostringstream ost;
			ost << strHttpResponse << "�в�����" << "duty" << "�ַ�����������ж��Ƿ���ȷ";
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


//@brief ��ȡͼƬ�ļ��ı��뷽ʽ��֧��bmp��jpg��jpeg��gif��tiff��png�ȸ�ʽͼƬ
//@date   1-13-2009  
//@param [in]  format ͼƬ��ʽ ֵ����Ϊ���¼���
//@"image/bmp"
//@"image/jpeg"
//@"image/gif"
//@"image/tiff"
//@"image/png"
//@param [in]  pClsid
//@return  �ɹ��򷵻�ֵ >= 0��ʧ���򷵻�ֵΪ-1
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

//@brief ��GDI+��BitmapͼƬ����Ϊ�ļ�
//@date   1-13-2009  
//@param [in]  hBitmap hBitmap�ľ����handle��
//@param [in]  lpszFileName ����ͼƬ������·��
//@return  �ɹ�����TRUE��ʧ�ܷ���FALSE

BOOL SaveBitmapToFile(HBITMAP hBitmap, LPWSTR lpszFileName)
{
	 CLSID pngClsid;

	 Bitmap bmp(hBitmap,NULL);

	 //��ȡBMP�ļ��ı��뷽ʽ(���ϣ����ȡJPEG�ı��뷽ʽ��

	 //��ô����һҪ����Ϊ��_TEXT("image/jpeg")������֧�ֵ�ͼƬ��ʽ����)
	 int nResult = GetEncoderClsid(L"image/png",&pngClsid);

	 if(nResult >= 0)
	 {
		 //��������ȡ����ĻͼƬ
		 bmp.Save(lpszFileName,&pngClsid);
	 }
	 else
	 {
		 return FALSE;
	 }
	   
	 return TRUE;
}

//ר��Ϊ ���� + ������Ƶ��ƶ������� ���� ����Ԫ ���� ��Լ ִ�� �ۺ�
//���󣬶�Լ��ִ�У��ۺ�İ�ť��С ǿ��Լ��Ϊ��30����16�����2
//��ǰ�����ּ��4
//���ܸ߶�
//�������������return
#define PROPERTY_WIDTH 30
#define PROPERTY_HIGHT 16
#define PROPERTY_SPACE 2
#define PROPERTY_SPACE_TO_HEAD 4
bool MoveControllers(CLabelUI* head, RECT head_pos, SIZE head_size, vector<CLabelUI*> following, RECT total_pos)
{
	return true;
	int nTotalSize = 0;
	nTotalSize += head_size.cx;//�����ֳ���
	nTotalSize += PROPERTY_SPACE_TO_HEAD;//�����������Եļ��
	nTotalSize += (following.size()*(PROPERTY_WIDTH + PROPERTY_SPACE));//���������ԵĿ��

	//�����㹻
	if (nTotalSize <= total_pos.right - total_pos.left)
	{
		head_pos.right = head_pos.left + head_size.cx;

	}
	else//���Ȳ���
	{
		int property_width = (following.size()*PROPERTY_WIDTH+PROPERTY_SPACE)+PROPERTY_SPACE_TO_HEAD;
		head_pos.right = total_pos.right - property_width;
	}

	head->SetPos(head_pos);
	//��ɵ�һ�����Կ�λ�õĳ�ʼ��
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

//ר��Ϊ ���� + Ĭ�Ͻ�������Ƶ��ƶ������� ���� ����Ԫ Ĭ�Ͻ�����
//��ǰ�����ּ��4
//���ܸ߶�
#define RECIPIENT_SPACE_TO_HEAD 4
bool MoveControllers( CLabelUI* head, RECT head_pos, SIZE head_size, 
	CLabelUI* recipient, RECT recipient_pos, SIZE recipient_size, 
	RECT total_pos )
{
	int nTotalSize = 0;
	nTotalSize += head_size.cx;//�����ֳ���
	nTotalSize += RECIPIENT_SPACE_TO_HEAD;//�����������Եļ��
	nTotalSize += recipient_size.cx;//��Ĭ�Ͻ����˵Ŀ��

	//�����㹻
	if (nTotalSize <= total_pos.right - total_pos.left)
	{
		head_pos.right = head_pos.left + head_size.cx;

	}
	else//���Ȳ���
	{
		head_pos.right = total_pos.right - recipient_size.cx - RECIPIENT_SPACE_TO_HEAD;
	}

	head->SetPos(head_pos);
	//��ɵ�һ�����Կ�λ�õĳ�ʼ��
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